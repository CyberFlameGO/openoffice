/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
#ifdef SW_DLLIMPLEMENTATION
#undef SW_DLLIMPLEMENTATION
#endif

#include "dbinsdlg.hxx"

#include <memory>

#include <float.h>

#include <hintids.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/sdbc/XDataSource.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#include <com/sun/star/sdb/XQueriesSupplier.hpp>
#include <com/sun/star/sdb/CommandType.hpp>
#include <com/sun/star/sdb/XColumn.hpp>
#include <com/sun/star/sdb/XDatabaseAccess.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/ResultSetType.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XNumberFormatter.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <comphelper/processfactory.hxx>
#include <editeng/langitem.hxx>
#include <svl/numuno.hxx>
#include <svl/stritem.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/svapp.hxx>
#include <vcl/mnemonic.hxx>
#include <svl/style.hxx>
#include <svl/zformat.hxx>
#include <svx/htmlmode.hxx>
#include <editeng/unolingu.hxx>
#include <sfx2/app.hxx>
#include <svl/itemset.hxx>
#include <editeng/brshitem.hxx>
#include <editeng/boxitem.hxx>
#include <svx/rulritem.hxx>
#include <swdbtoolsclient.hxx>
#include <tabledlg.hxx>
#include <fmtclds.hxx>
#include <tabcol.hxx>
#include <uiitems.hxx>
#include <viewopt.hxx>
#include <uitool.hxx>
#include <wrtsh.hxx>
#include <wview.hxx>
#include <docsh.hxx>
#include <dbmgr.hxx>
#include <tblafmt.hxx>
#include <cellatr.hxx>
#include <swtable.hxx>
#include <dbfld.hxx>
#include <fmtcol.hxx>
#include <section.hxx>
#include <swwait.hxx>
#include <modcfg.hxx>
#include <swmodule.hxx>
#include <poolfmt.hxx>
#include <crsskip.hxx>

#include <dbinsdlg.hrc>
#include <dbui.hrc>

#include <cmdid.h>
#include <helpid.h>
#include <cfgid.h>
#include <SwStyleNameMapper.hxx>
#include <comphelper/uno3.hxx>
#include "tabsh.hxx"
#include "swabstdlg.hxx"
#include "table.hrc"
#include <unomid.h>
#include <IDocumentMarkAccess.hxx>


namespace swui
{
	SwAbstractDialogFactory * GetFactory();
}

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;

// tblafmt.hxx
SV_IMPL_PTRARR( _SwTableAutoFmtTbl, SwTableAutoFmt* )

const char cDBFldStart	= '<';
const char cDBFldEnd 	= '>';

// Hilfsstruktur fuers einfuegen von Datenbankspalten als Felder oder Text
struct _DB_Column
{
	enum ColType { DB_FILLTEXT, DB_COL_FIELD, DB_COL_TEXT, DB_SPLITPARA } eColType;

	union {
		String* pText;
		SwField* pField;
		sal_uLong nFormat;
	} DB_ColumnData;
	const SwInsDBColumn* pColInfo;

	_DB_Column()
	{
		pColInfo = 0;
		DB_ColumnData.pText = 0;
		eColType = DB_SPLITPARA;
	}

	_DB_Column( const String& rTxt )
	{
		pColInfo = 0;
		DB_ColumnData.pText = new String( rTxt );
		eColType = DB_FILLTEXT;
	}

	_DB_Column( const SwInsDBColumn& rInfo, sal_uLong nFormat )
	{
		pColInfo = &rInfo;
		DB_ColumnData.nFormat = nFormat;
		eColType = DB_COL_TEXT;
	}

	_DB_Column( const SwInsDBColumn& rInfo, SwDBField& rFld )
	{
		pColInfo = &rInfo;
		DB_ColumnData.pField = &rFld;
		eColType = DB_COL_FIELD;
	}

	~_DB_Column()
	{
		if( DB_COL_FIELD == eColType )
			delete DB_ColumnData.pField;
		else if( DB_FILLTEXT == eColType )
			delete DB_ColumnData.pText;
	}
};

typedef _DB_Column* _DB_ColumnPtr;
SV_DECL_PTRARR_DEL( _DB_Columns, _DB_ColumnPtr, 32, 32 )
SV_IMPL_PTRARR( _DB_Columns, _DB_ColumnPtr )

SV_IMPL_OP_PTRARR_SORT( SwInsDBColumns, SwInsDBColumnPtr )

/*  */

#define DBCOLUMN_CONFIG_VERSION1 	1
#define DBCOLUMN_CONFIG_VERSION 	DBCOLUMN_CONFIG_VERSION1
#define DBCOLUMN_MAXDATA			5

struct _DB_ColumnConfigData
{
	SwInsDBColumns aDBColumns;
    rtl::OUString sSource, sTable, sEdit, sTblList, sTmplNm, sTAutoFmtNm;
	sal_Bool bIsTable : 1,
		 bIsField : 1,
		 bIsHeadlineOn : 1,
		 bIsEmptyHeadln : 1;

	_DB_ColumnConfigData()
	{
		bIsTable = bIsHeadlineOn = sal_True;
		bIsField = bIsEmptyHeadln = sal_False;
	}

	~_DB_ColumnConfigData();
private:
	_DB_ColumnConfigData( const _DB_ColumnConfigData& );
	_DB_ColumnConfigData& operator =( const _DB_ColumnConfigData& );
};

/*  */

int SwInsDBColumn::operator<( const SwInsDBColumn& rCmp ) const
{
	return 0 > GetAppCollator().compareString( sColumn, rCmp.sColumn );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwInsertDBColAutoPilot::SwInsertDBColAutoPilot( SwView& rView,
		Reference<XDataSource> xDataSource,
		Reference<sdbcx::XColumnsSupplier> xColSupp,
		const SwDBData& rData )
	: SfxModalDialog( rView.GetWindow(), SW_RES( DLG_AP_INSERT_DB_SEL )),
	ConfigItem(C2U("Office.Writer/InsertData/DataSet"), CONFIG_MODE_DELAYED_UPDATE),
	aFtInsertData( this, SW_RES( FT_INSERT_DATA )),
	aRbAsTable( this, SW_RES( RB_AS_TABLE )),
	aRbAsField( this, SW_RES( RB_AS_FIELD )),
    aRbAsText( this, SW_RES( RB_AS_TEXT )),

    aFlHead( this, SW_RES( FL_HEAD )),
    aFtDbColumn( this, SW_RES( FT_DB_COLUMN )),

    aLbTblDbColumn( this, SW_RES( LB_TBL_DB_COLUMN )),
    aLbTxtDbColumn( this, SW_RES( LB_TXT_DB_COLUMN )),

    aFlFormat( this, SW_RES( FL_FORMAT )),
    aRbDbFmtFromDb( this, SW_RES( RB_DBFMT_FROM_DB )),
    aRbDbFmtFromUsr( this, SW_RES( RB_DBFMT_FROM_USR )),
    aLbDbFmtFromUsr( this, &rView, SW_RES( LB_DBFMT_FROM_USR )),

    aIbDbcolToEdit( this, SW_RES( IB_DBCOL_TOEDIT )),
    aEdDbText( this, SW_RES( ED_DB_TEXT )),
    aFtDbParaColl( this, SW_RES( FT_DB_PARA_COLL )),
    aLbDbParaColl( this, SW_RES( LB_DB_PARA_COLL )),

    aIbDbcolAllTo( this, SW_RES( IB_DBCOL_ALL_TO )),
    aIbDbcolOneTo( this, SW_RES( IB_DBCOL_ONE_TO )),
    aIbDbcolOneFrom( this, SW_RES( IB_DBCOL_ONE_FROM )),
    aIbDbcolAllFrom( this, SW_RES( IB_DBCOL_ALL_FROM )),
    aFtTableCol( this, SW_RES( FT_TABLE_COL )),
    aLbTableCol( this, SW_RES( LB_TABLE_COL )),
    aCbTableHeadon( this, SW_RES( CB_TABLE_HEADON )),
    aRbHeadlColnms( this, SW_RES( RB_HEADL_COLNMS )),
    aRbHeadlEmpty( this, SW_RES( RB_HEADL_EMPTY )),
    aPbTblFormat( this, SW_RES( PB_TBL_FORMAT )),
    aPbTblAutofmt( this, SW_RES( PB_TBL_AUTOFMT )),

    aBtOk( this, SW_RES( BT_OK )),
    aBtCancel( this, SW_RES( BT_CANCEL )),
    aBtHelp( this, SW_RES( BT_HELP )),

    aFlBottom( this, SW_RES( FL_BOTTOM )),

    aDBData(rData),

    aOldNumFmtLnk( aLbDbFmtFromUsr.GetSelectHdl() ),
	sNoTmpl( SW_RES( STR_NOTEMPL )),
	pView( &rView ),
	pTAutoFmt( 0 ),
	pTblSet( 0 ),
    pRep( 0 )
{
	FreeResource();

    nGBFmtLen = aFlFormat.GetText().Len();

	if(xColSupp.is())
	{
		SwWrtShell& rSh = pView->GetWrtShell();
		Locale aDocLocale( SvxCreateLocale( rSh.GetCurLang() ));
		SvNumberFormatter* pNumFmtr = rSh.GetNumberFormatter();
		SvNumberFormatsSupplierObj* pNumFmt = new SvNumberFormatsSupplierObj( pNumFmtr );
		Reference< util::XNumberFormatsSupplier >  xDocNumFmtsSupplier = pNumFmt;
        Reference< util::XNumberFormats > xDocNumberFormats = xDocNumFmtsSupplier->getNumberFormats();
        Reference< util::XNumberFormatTypes > xDocNumberFormatTypes(xDocNumberFormats, UNO_QUERY);

		Reference<XPropertySet> xSourceProps(xDataSource, UNO_QUERY);
        Reference< util::XNumberFormats > xNumberFormats;
		if(xSourceProps.is())
		{
			Any aFormats = xSourceProps->getPropertyValue(C2U("NumberFormatsSupplier"));
			if(aFormats.hasValue())
			{
                Reference< util::XNumberFormatsSupplier> xSuppl;
                aFormats >>= xSuppl;
				if(xSuppl.is())
				{
					xNumberFormats = xSuppl->getNumberFormats(  );
				}
			}
		}
		Reference <XNameAccess> xCols = xColSupp->getColumns();
        Sequence<rtl::OUString> aColNames = xCols->getElementNames();
        const rtl::OUString* pColNames = aColNames.getConstArray();
		long nCount = aColNames.getLength();
		for (long n = 0; n < nCount; n++)
		{
			SwInsDBColumn* pNew = new SwInsDBColumn( pColNames[n], (sal_uInt16)n );
			Any aCol = xCols->getByName(pColNames[n]);
            Reference <XPropertySet> xCol;
            aCol >>= xCol;
			Any aType = xCol->getPropertyValue(C2S("Type"));
			sal_Int32 eDataType = 0;
			aType >>= eDataType;
			switch(eDataType)
			{
				case DataType::BIT:
				case DataType::BOOLEAN:
				case DataType::TINYINT:
				case DataType::SMALLINT:
				case DataType::INTEGER:
				case DataType::BIGINT:
				case DataType::FLOAT:
				case DataType::REAL:
				case DataType::DOUBLE:
				case DataType::NUMERIC:
				case DataType::DECIMAL:
				case DataType::DATE:
				case DataType::TIME:
				case DataType::TIMESTAMP:
				{
					pNew->bHasFmt = sal_True;
					Any aFormat = xCol->getPropertyValue(C2U("FormatKey"));
					if(aFormat.hasValue())
					{
						sal_Int32 nFmt = 0;
						aFormat >>= nFmt;
						if(xNumberFormats.is())
						{
							try
							{
								Reference<XPropertySet> xNumProps = xNumberFormats->getByKey( nFmt );
                                Any aFormatVal = xNumProps->getPropertyValue(C2U("FormatString"));
								Any aLocale = xNumProps->getPropertyValue(C2U("Locale"));
                                rtl::OUString sFormat;
                                aFormatVal >>= sFormat;
                                lang::Locale aLoc;
								aLocale >>= aLoc;
								long nKey = xDocNumberFormats->queryKey( sFormat, aLoc, sal_True);
								if(nKey < 0)
								{
									nKey = xDocNumberFormats->addNew( sFormat, aLoc );
								}
								pNew->nDBNumFmt = nKey;
							}
							catch(const Exception& )
							{
								DBG_ERROR("illegal number format key");
							}
						}
					}
					else
                    {
                        pNew->nDBNumFmt = SwNewDBMgr::GetDbtoolsClient().getDefaultNumberFormat(xCol,
                                                    xDocNumberFormatTypes, aDocLocale);
                    }

				}
				break;
			}
			if( !aDBColumns.Insert( pNew ))
			{
				ASSERT( !this, "Spaltenname mehrfach vergeben?" );
				delete pNew;
			}
		}
	}

	// Absatzvorlagen-ListBox fuellen
	{
		SfxStyleSheetBasePool* pPool = pView->GetDocShell()->GetStyleSheetPool();
		pPool->SetSearchMask( SFX_STYLE_FAMILY_PARA, SFXSTYLEBIT_ALL );
		aLbDbParaColl.InsertEntry( sNoTmpl );

		const SfxStyleSheetBase* pBase = pPool->First();
		while( pBase )
		{
			aLbDbParaColl.InsertEntry( pBase->GetName() );
			pBase = pPool->Next();
		}
		aLbDbParaColl.SelectEntryPos( 0 );
	}

	// steht der Cursor in einer Tabelle, darf NIE Tabelle auswaehlbar sein
	if( pView->GetWrtShell().GetTableFmt() )
	{
		aRbAsTable.Enable( sal_False );
		aRbAsField.Check( sal_True );
		aRbDbFmtFromDb.Check( sal_True );
	}
	else
	{
		aRbAsTable.Check( sal_True );
		aRbDbFmtFromDb.Check( sal_True );
        aIbDbcolOneFrom.Enable( sal_False );
        aIbDbcolAllFrom.Enable( sal_False );
	}

	aRbAsTable.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, PageHdl ));
	aRbAsField.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, PageHdl ));
	aRbAsText.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, PageHdl ));

	aRbDbFmtFromDb.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, DBFormatHdl ));
	aRbDbFmtFromUsr.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, DBFormatHdl ));

	aPbTblFormat.SetClickHdl(LINK(this, SwInsertDBColAutoPilot, TblFmtHdl ));
	aPbTblAutofmt.SetClickHdl(LINK(this, SwInsertDBColAutoPilot, AutoFmtHdl ));

    aIbDbcolAllTo.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, TblToFromHdl ));
    aIbDbcolOneTo.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, TblToFromHdl ));
    aIbDbcolOneFrom.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, TblToFromHdl ));
    aIbDbcolAllFrom.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, TblToFromHdl ));
    aIbDbcolToEdit.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, TblToFromHdl ));

	aCbTableHeadon.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, HeaderHdl ));
	aRbHeadlColnms.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, HeaderHdl ));
	aRbHeadlEmpty.SetClickHdl( LINK(this, SwInsertDBColAutoPilot, HeaderHdl ));

	aLbTxtDbColumn.SetSelectHdl( LINK( this, SwInsertDBColAutoPilot, SelectHdl ));
	aLbTblDbColumn.SetSelectHdl( LINK( this, SwInsertDBColAutoPilot, SelectHdl ));
	aLbDbFmtFromUsr.SetSelectHdl( LINK( this, SwInsertDBColAutoPilot, SelectHdl ));
	aLbTableCol.SetSelectHdl( LINK( this, SwInsertDBColAutoPilot, SelectHdl ));

	aLbTxtDbColumn.SetDoubleClickHdl( LINK( this, SwInsertDBColAutoPilot, DblClickHdl ));
	aLbTblDbColumn.SetDoubleClickHdl( LINK( this, SwInsertDBColAutoPilot, DblClickHdl ));
	aLbTableCol.SetDoubleClickHdl( LINK( this, SwInsertDBColAutoPilot, DblClickHdl ));

	for( sal_uInt16 n = 0; n < aDBColumns.Count(); ++n )
	{
		const String& rS = aDBColumns[ n ]->sColumn;
		aLbTblDbColumn.InsertEntry( rS, n );
		aLbTxtDbColumn.InsertEntry( rS, n );
	}
	aLbTxtDbColumn.SelectEntryPos( 0 );
	aLbTblDbColumn.SelectEntryPos( 0 );

	// read configuration
	Load();

	// Controls initialisieren:
	PageHdl( aRbAsTable.IsChecked() ? &aRbAsTable : &aRbAsField );
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
SwInsertDBColAutoPilot::~SwInsertDBColAutoPilot()
{
	delete pTblSet;
	delete pRep;

//	delete pConfig;
	delete pTAutoFmt;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, PageHdl, Button*, pButton )
{
	sal_Bool bShowTbl = pButton == &aRbAsTable;

	String sTxt( pButton->GetText() );
    aFlHead.SetText( MnemonicGenerator::EraseAllMnemonicChars( sTxt ) );

	aLbTxtDbColumn.Show( !bShowTbl );
    aIbDbcolToEdit.Show( !bShowTbl );
	aEdDbText.Show( !bShowTbl );
	aFtDbParaColl.Show( !bShowTbl );
	aLbDbParaColl.Show( !bShowTbl );

	aLbTblDbColumn.Show( bShowTbl );
    aIbDbcolAllTo.Show( bShowTbl );
    aIbDbcolOneTo.Show( bShowTbl );
    aIbDbcolOneFrom.Show( bShowTbl );
    aIbDbcolAllFrom.Show( bShowTbl );
	aFtTableCol.Show( bShowTbl );
	aLbTableCol.Show( bShowTbl );
	aCbTableHeadon.Show( bShowTbl );
	aRbHeadlColnms.Show( bShowTbl );
	aRbHeadlEmpty.Show( bShowTbl );
	aPbTblFormat.Show( bShowTbl );
	aPbTblAutofmt.Show( bShowTbl );

	if( bShowTbl )
		aPbTblFormat.Enable( 0 != aLbTableCol.GetEntryCount() );

	SelectHdl( bShowTbl ? &aLbTblDbColumn : &aLbTxtDbColumn );

	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, DBFormatHdl, Button*, pButton )
{
	sal_uInt16 nFndPos;
	ListBox& rBox = aRbAsTable.IsChecked()
						? ( 0 == aLbTableCol.GetEntryData( 0 )
							? aLbTblDbColumn
							: aLbTableCol )
						: aLbTxtDbColumn;

	SwInsDBColumn aSrch( rBox.GetSelectEntry(), 0 );
	aDBColumns.Seek_Entry( &aSrch, &nFndPos );

	sal_Bool bFromDB = &aRbDbFmtFromDb == pButton;
	aDBColumns[ nFndPos ]->bIsDBFmt = bFromDB;
	aLbDbFmtFromUsr.Enable( !bFromDB );

	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, TblToFromHdl, Button*, pButton )
{
	sal_Bool bChgEnable = sal_True, bEnableTo = sal_True, bEnableFrom = sal_True;
	aLbTblDbColumn.SetUpdateMode( sal_False );
	aLbTableCol.SetUpdateMode( sal_False );

    if( pButton == &aIbDbcolAllTo )
	{
		bEnableTo = sal_False;

		sal_uInt16 n, nInsPos = aLbTableCol.GetSelectEntryPos(),
			   nCnt = aLbTblDbColumn.GetEntryCount();
		if( LISTBOX_APPEND == nInsPos )
			for( n = 0; n < nCnt; ++n )
				aLbTableCol.InsertEntry( aLbTblDbColumn.GetEntry( n ),
											LISTBOX_APPEND );
		else
			for( n = 0; n < nCnt; ++n, ++nInsPos )
				aLbTableCol.InsertEntry( aLbTblDbColumn.GetEntry( n ), nInsPos );
		aLbTblDbColumn.Clear();
		aLbTableCol.SelectEntryPos( nInsPos );
		aLbTblDbColumn.SelectEntryPos( LISTBOX_APPEND );
	}
    else if( pButton == &aIbDbcolOneTo &&
			LISTBOX_ENTRY_NOTFOUND != aLbTblDbColumn.GetSelectEntryPos() )
	{
		sal_uInt16 nInsPos = aLbTableCol.GetSelectEntryPos(),
			   nDelPos = aLbTblDbColumn.GetSelectEntryPos(),
			   nTopPos = aLbTblDbColumn.GetTopEntry();
		aLbTableCol.InsertEntry( aLbTblDbColumn.GetEntry( nDelPos ), nInsPos );
		aLbTblDbColumn.RemoveEntry( nDelPos );

		aLbTableCol.SelectEntryPos( nInsPos );
		if( nDelPos >= aLbTblDbColumn.GetEntryCount() )
			nDelPos = aLbTblDbColumn.GetEntryCount() - 1;
		aLbTblDbColumn.SelectEntryPos( nDelPos );
		aLbTblDbColumn.SetTopEntry( nTopPos );

		bEnableTo = 0 != aLbTblDbColumn.GetEntryCount();
	}
    else if( pButton == &aIbDbcolOneFrom )
	{
		if( LISTBOX_ENTRY_NOTFOUND != aLbTableCol.GetSelectEntryPos() )
		{
			sal_uInt16 nFndPos, nInsPos,
					nDelPos = aLbTableCol.GetSelectEntryPos(),
					nTopPos = aLbTableCol.GetTopEntry();

			// die richtige InsertPos suchen!!
			SwInsDBColumn aSrch( aLbTableCol.GetEntry( nDelPos ), 0 );
			aDBColumns.Seek_Entry( &aSrch, &nFndPos );
			if( !nFndPos || nFndPos == aDBColumns.Count()-1 )
				nInsPos = nFndPos;
			else
			{
				nInsPos = LISTBOX_ENTRY_NOTFOUND;
				while( ++nFndPos < aDBColumns.Count() &&
						LISTBOX_ENTRY_NOTFOUND == (nInsPos = aLbTblDbColumn.
						GetEntryPos( String(aDBColumns[ nFndPos ]->sColumn ))) )
					;
			}

			aLbTblDbColumn.InsertEntry( aSrch.sColumn, nInsPos );
			aLbTableCol.RemoveEntry( nDelPos );

			if( nInsPos >= aLbTblDbColumn.GetEntryCount() )
				nInsPos = aLbTblDbColumn.GetEntryCount() - 1;
			aLbTblDbColumn.SelectEntryPos( nInsPos );

			if( nDelPos >= aLbTableCol.GetEntryCount() )
				nDelPos = aLbTableCol.GetEntryCount() - 1;
			aLbTableCol.SelectEntryPos( nDelPos );
			aLbTableCol.SetTopEntry( nTopPos );
		}
		else
			bEnableTo = 0 != aLbTblDbColumn.GetEntryCount();

		bEnableFrom = 0 != aLbTableCol.GetEntryCount();
	}
    else if( pButton == &aIbDbcolAllFrom )
	{
		bEnableFrom = sal_False;

		aLbTblDbColumn.Clear();
		aLbTableCol.Clear();
		for( sal_uInt16 n = 0; n < aDBColumns.Count(); ++n )
			aLbTblDbColumn.InsertEntry( aDBColumns[ n ]->sColumn, n );
		aLbTblDbColumn.SelectEntryPos( 0 );
	}
    else if( pButton == &aIbDbcolToEdit )
	{
		bChgEnable = sal_False;
		// Daten ins Edit moven:
		String aFld( aLbTxtDbColumn.GetSelectEntry() );
		if( aFld.Len() )
		{
			String aStr( aEdDbText.GetText() );
			sal_uInt16 nPos = (sal_uInt16)aEdDbText.GetSelection().Min();
			sal_uInt16 nSel = sal_uInt16(aEdDbText.GetSelection().Max()) - nPos;
			if( nSel )
				// dann loesche erstmal die bestehende Selektion
				aStr.Erase( nPos, nSel );

			aFld.Insert( cDBFldStart, 0 );
			aFld += cDBFldEnd;
			if( aStr.Len() )
			{
				if( nPos ) 							// ein Space davor
				{
					sal_Unicode c = aStr.GetChar( nPos-1 );
					if( '\n' != c && '\r' != c )
						aFld.Insert( ' ', 0 );
				}
				if( nPos < aStr.Len() )				// ein Space dahinter
				{
					sal_Unicode c = aStr.GetChar( nPos );
					if( '\n' != c && '\r' != c )
						aFld += ' ';
				}
			}

			aStr.Insert( aFld, nPos );
			aEdDbText.SetText( aStr );
            nPos = nPos + aFld.Len();
			aEdDbText.SetSelection( Selection( nPos ));
		}
	}

	if( bChgEnable )
	{
        aIbDbcolOneTo.Enable( bEnableTo );
        aIbDbcolAllTo.Enable( bEnableTo );
        aIbDbcolOneFrom.Enable( bEnableFrom );
        aIbDbcolAllFrom.Enable( bEnableFrom );

		aRbDbFmtFromDb.Enable( sal_False );
		aRbDbFmtFromUsr.Enable( sal_False );
		aLbDbFmtFromUsr.Enable( sal_False );

		aPbTblFormat.Enable( bEnableFrom );
	}
	aLbTblDbColumn.SetUpdateMode( sal_True );
	aLbTableCol.SetUpdateMode( sal_True );

	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, DblClickHdl, ListBox*, pBox )
{
	Button* pButton = 0;
	if( pBox == &aLbTxtDbColumn )
        pButton = &aIbDbcolToEdit;
    else if( pBox == &aLbTblDbColumn && aIbDbcolOneTo.IsEnabled() )
        pButton = &aIbDbcolOneTo;
    else if( pBox == &aLbTableCol && aIbDbcolOneFrom.IsEnabled() )
        pButton = &aIbDbcolOneFrom;

	if( pButton )
		TblToFromHdl( pButton );

	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, TblFmtHdl, PushButton*, pButton )
{
	SwWrtShell& rSh = pView->GetWrtShell();
	sal_Bool bNewSet = sal_False;
	if( !pTblSet )
	{
		bNewSet = sal_True;
		pTblSet = new SfxItemSet( rSh.GetAttrPool(), SwuiGetUITableAttrRange() );

		//Ersteinmal die einfachen Attribute besorgen.
		pTblSet->Put( SfxStringItem( FN_PARAM_TABLE_NAME, rSh.GetUniqueTblName() ));
		pTblSet->Put( SfxUInt16Item( FN_PARAM_TABLE_HEADLINE, 1 ) );

		pTblSet->Put( SfxUInt16Item( SID_BACKGRND_DESTINATION,
									rSh.GetViewOptions()->GetTblDest() ));

		SvxBrushItem aBrush( RES_BACKGROUND );
		pTblSet->Put( aBrush );
		pTblSet->Put( aBrush, SID_ATTR_BRUSH_ROW );
		pTblSet->Put( aBrush, SID_ATTR_BRUSH_TABLE );

        SvxBoxInfoItem aBoxInfo( SID_ATTR_BORDER_INNER );
			// Tabellenvariante, wenn mehrere Tabellenzellen selektiert
		aBoxInfo.SetTable( sal_True );
			// Abstandsfeld immer anzeigen
		aBoxInfo.SetDist( sal_True);
			// Minimalgroesse in Tabellen und Absaetzen setzen
		aBoxInfo.SetMinDist( sal_False );
			// Default-Abstand immer setzen
		aBoxInfo.SetDefDist( MIN_BORDER_DIST );
			// Einzelne Linien koennen nur in Tabellen DontCare-Status haben
		aBoxInfo.SetValid( VALID_DISABLE, sal_True );
		pTblSet->Put( aBoxInfo );

		SwGetCurColNumPara aPara;
        const sal_uInt16 nNum = rSh.GetCurColNum( &aPara );
		long nWidth;

		if( nNum )
		{
			nWidth = aPara.pPrtRect->Width();
			const SwFmtCol& rCol = aPara.pFrmFmt->GetCol();
			const SwColumns& rCols = rCol.GetColumns();

			//nStart und nEnd initialisieren fuer nNum == 0
			long nWidth1 = 0,
				nStart1 = 0,
				nEnd1 = nWidth;
            for( sal_uInt16 i = 0; i < nNum; ++i )
			{
				SwColumn* pCol = rCols[i];
				nStart1 = pCol->GetLeft() + nWidth1;
				nWidth1 += (long)rCol.CalcColWidth( i, (sal_uInt16)nWidth );
				nEnd1 = nWidth1 - pCol->GetRight();
			}
			if(nStart1 || nEnd1 != nWidth)
				nWidth = nEnd1 - nStart1;
		}
		else
			nWidth = rSh.GetAnyCurRect(
								FRMTYPE_FLY_ANY & rSh.GetFrmType( 0, sal_True )
											  ? RECT_FLY_PRT_EMBEDDED
											  : RECT_PAGE_PRT ).Width();

		SwTabCols aTabCols;
		aTabCols.SetRight( nWidth );
		aTabCols.SetRightMax( nWidth );
		pRep = new SwTableRep( aTabCols, sal_False );
        pRep->SetAlign( text::HoriOrientation::NONE );
		pRep->SetSpace( nWidth );
		pRep->SetWidth( nWidth );
		pRep->SetWidthPercent( 100 );
		pTblSet->Put( SwPtrItem( FN_TABLE_REP, pRep ));

		pTblSet->Put( SfxUInt16Item( SID_HTML_MODE,
					::GetHtmlMode( pView->GetDocShell() )));
	}

	if( aLbTableCol.GetEntryCount() != pRep->GetAllColCount() )
	{
		// Anzahl der Spalten hat sich geaendert: dann muessen die
		// TabCols angepasst werden
		long nWidth = pRep->GetWidth();
		sal_uInt16 nCols = aLbTableCol.GetEntryCount() - 1;
		SwTabCols aTabCols( nCols );
		aTabCols.SetRight( nWidth  );
		aTabCols.SetRightMax( nWidth );
		if( nCols )
			for( sal_uInt16 n = 0, nStep = (sal_uInt16)(nWidth / (nCols+1)), nW = nStep;
                    n < nCols; ++n, nW = nW + nStep )
			{
                aTabCols.Insert( nW, sal_False, n );
            }
		delete pRep;
		pRep = new SwTableRep( aTabCols, sal_False );
        pRep->SetAlign( text::HoriOrientation::NONE );
		pRep->SetSpace( nWidth );
		pRep->SetWidth( nWidth );
		pRep->SetWidthPercent( 100 );
		pTblSet->Put( SwPtrItem( FN_TABLE_REP, pRep ));
	}

    SwAbstractDialogFactory* pFact = swui::GetFactory();
    DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

    SfxAbstractTabDialog* pDlg = pFact->CreateSwTableTabDlg(  pButton, rSh.GetAttrPool(),pTblSet, &rSh, DLG_FORMAT_TABLE );
    DBG_ASSERT(pDlg, "Dialogdiet fail!");
	if( RET_OK == pDlg->Execute() )
		pTblSet->Put( *pDlg->GetOutputItemSet() );
	else if( bNewSet )
	{
		delete pTblSet, pTblSet = 0;
		delete pRep, pRep = 0;
	}
	delete pDlg;

	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, AutoFmtHdl, PushButton*, pButton )
{
    SwAbstractDialogFactory* pFact = swui::GetFactory();
    DBG_ASSERT(pFact, "SwAbstractDialogFactory fail!");

    AbstractSwAutoFormatDlg* pDlg = pFact->CreateSwAutoFormatDlg(pButton, pView->GetWrtShellPtr(),DLG_AUTOFMT_TABLE, sal_False, pTAutoFmt);
    DBG_ASSERT(pDlg, "Dialogdiet fail!");
    if( RET_OK == pDlg->Execute())
        pDlg->FillAutoFmtOfIndex( pTAutoFmt );
    delete pDlg;
	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, SelectHdl, ListBox*, pBox )
{
	ListBox* pGetBox = pBox == &aLbDbFmtFromUsr
							? ( aRbAsTable.IsChecked()
									? ( 0 == aLbTableCol.GetEntryData( 0 )
										? &aLbTblDbColumn
										: &aLbTableCol )
									: &aLbTxtDbColumn )
							: pBox;

	sal_uInt16 nFndPos;
	SwInsDBColumn aSrch( pGetBox->GetSelectEntry(), 0 );
	aDBColumns.Seek_Entry( &aSrch, &nFndPos );

	if( pBox == &aLbDbFmtFromUsr )
	{
		if( aSrch.sColumn.getLength() )
		{
			aOldNumFmtLnk.Call( pBox );
			aDBColumns[ nFndPos ]->nUsrNumFmt = aLbDbFmtFromUsr.GetFormat();
		}
	}
	else
	{
		// an der FormatGroupBox den ausgewaehlten FeldNamen setzen, damit
		// klar ist, welches Feld ueber das Format eingestellt wird!
        String sTxt( aFlFormat.GetText().Copy( 0, nGBFmtLen ));
		if( !aSrch.sColumn.getLength() )
		{
			aRbDbFmtFromDb.Enable( sal_False );
			aRbDbFmtFromUsr.Enable( sal_False );
			aLbDbFmtFromUsr.Enable( sal_False );
		}
		else
		{
			sal_Bool bEnableFmt = aDBColumns[ nFndPos ]->bHasFmt;
			aRbDbFmtFromDb.Enable( bEnableFmt );
			aRbDbFmtFromUsr.Enable( bEnableFmt );

			if( bEnableFmt )
			{
				(( sTxt += C2S(" (" )) += String(aSrch.sColumn) ) += (sal_Unicode)')';
			}

			sal_Bool bIsDBFmt = aDBColumns[ nFndPos ]->bIsDBFmt;
			aRbDbFmtFromDb.Check( bIsDBFmt );
			aRbDbFmtFromUsr.Check( !bIsDBFmt );
			aLbDbFmtFromUsr.Enable( !bIsDBFmt );
			if( !bIsDBFmt )
				aLbDbFmtFromUsr.SetDefFormat( aDBColumns[ nFndPos ]->nUsrNumFmt );
		}

        aFlFormat.SetText( sTxt );

		// um spaeter zu wissen, welche ListBox die "aktive" war, wird sich
		// im 1. Eintrag ein Flag gemerkt,
		void* pPtr = pBox == &aLbTableCol ? &aLbTableCol : 0;
		aLbTableCol.SetEntryData( 0, pPtr );
	}
	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
IMPL_LINK( SwInsertDBColAutoPilot, HeaderHdl, Button*, pButton )
{
	if( pButton == &aCbTableHeadon )
	{
		sal_Bool bEnable = aCbTableHeadon.IsChecked();

		aRbHeadlColnms.Enable( bEnable );
		aRbHeadlEmpty.Enable( bEnable );
	}
	return 0;
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
static void lcl_InsTextInArr( const String& rTxt, _DB_Columns& rColArr )
{
	_DB_Column* pNew;
	sal_uInt16 nSttPos = 0, nFndPos;
	while( STRING_NOTFOUND != ( nFndPos = rTxt.Search( '\x0A', nSttPos )) )
	{
		if( 1 < nFndPos )
		{
			pNew = new _DB_Column( rTxt.Copy( nSttPos, nFndPos -1 ) );
			rColArr.Insert( pNew, rColArr.Count() );
		}
		pNew = new _DB_Column;
		rColArr.Insert( pNew, rColArr.Count() );
		nSttPos = nFndPos + 1;
	}
	if( nSttPos < rTxt.Len() )
	{
		pNew = new _DB_Column( rTxt.Copy( nSttPos ) );
		rColArr.Insert( pNew, rColArr.Count() );
	}
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
sal_Bool SwInsertDBColAutoPilot::SplitTextToColArr( const String& rTxt,
								_DB_Columns& rColArr,
								sal_Bool bInsField )
{
	// aus dem Text wieder die einzelnen Datenbank - Spalten erzeugen
	// und dann in einem Array speichern
	// Die Datenbankspalten stehen in <> und muessen im Array der Spalten
	// vorhanden sein:
	String sTxt( rTxt );
	sal_uInt16 nFndPos, nEndPos, nSttPos = 0;

	while( STRING_NOTFOUND != ( nFndPos = sTxt.Search( cDBFldStart, nSttPos )))
	{
		nSttPos = nFndPos + 1;
		if( STRING_NOTFOUND != ( nEndPos = sTxt.Search( cDBFldEnd, nSttPos+1 )))
		{
			// Text in <> geklammert gefunden: was ist es denn:
			SwInsDBColumn aSrch( sTxt.Copy( nSttPos, nEndPos - nSttPos ), 0);
			if( aDBColumns.Seek_Entry( &aSrch, &nFndPos ) )
			{
				// das ist ein gueltiges Feld
				// also sicher den Text "davor":
				const SwInsDBColumn& rFndCol = *aDBColumns[ nFndPos ];

				_DB_Column* pNew;

				if( 1 < nSttPos )
				{
					::lcl_InsTextInArr( sTxt.Copy( 0, nSttPos-1 ), rColArr );
					sTxt.Erase( 0, nSttPos-1 );
				}

				sTxt.Erase( 0, (xub_StrLen)(rFndCol.sColumn.getLength() + 2) );
				nSttPos = 0;

				sal_uInt16 nSubType = 0;
				sal_uLong nFormat;
				if( rFndCol.bHasFmt )
				{
					if( rFndCol.bIsDBFmt )
						nFormat =  rFndCol.nDBNumFmt;
					else
					{
						nFormat = rFndCol.nUsrNumFmt;
						nSubType = nsSwExtendedSubType::SUB_OWN_FMT;
					}
				}
				else
					nFormat = 0;

				if( bInsField )
				{
					SwWrtShell& rSh = pView->GetWrtShell();
					SwDBFieldType aFldType( rSh.GetDoc(), aSrch.sColumn,
											aDBData );
					pNew = new _DB_Column( rFndCol, *new SwDBField(
							(SwDBFieldType*)rSh.InsertFldType( aFldType ),
															nFormat ) );
					if( nSubType )
						pNew->DB_ColumnData.pField->SetSubType( nSubType );
				}
				else
					pNew = new _DB_Column( rFndCol, nFormat );

				rColArr.Insert( pNew, rColArr.Count() );
			}
		}
	}

	// den letzten Text nicht vergessen
	if( sTxt.Len() )
		::lcl_InsTextInArr( sTxt, rColArr );

	return 0 != rColArr.Count();
}
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
void SwInsertDBColAutoPilot::DataToDoc( const Sequence<Any>& rSelection,
	Reference< XDataSource> xSource,
	Reference< XConnection> xConnection,
	Reference< sdbc::XResultSet > xResultSet )
{
    const Any* pSelection = rSelection.getLength() ? rSelection.getConstArray() : 0;
	SwWrtShell& rSh = pView->GetWrtShell();

	//with the drag and drop interface no result set is initially available
    sal_Bool bDisposeResultSet = sal_False;
	// we don't have a cursor, so we have to create our own RowSet
	if ( !xResultSet.is() )
	{
		xResultSet = SwNewDBMgr::createCursor(aDBData.sDataSource,aDBData.sCommand,aDBData.nCommandType,xConnection);
		bDisposeResultSet = xResultSet.is();
	}

	Reference< sdbc::XRow > xRow(xResultSet, UNO_QUERY);
	if ( !xRow.is() )
		return;

	rSh.StartAllAction();
	sal_Bool bUndo = rSh.DoesUndo();
	if( bUndo )
        rSh.StartUndo( UNDO_EMPTY );

	sal_Bool bAsTable = aRbAsTable.IsChecked();
	SvNumberFormatter& rNumFmtr = *rSh.GetNumberFormatter();

	if( rSh.HasSelection() )
		rSh.DelRight();

	::std::auto_ptr<SwWait> pWait;

	Reference< XColumnsSupplier > xColsSupp( xResultSet, UNO_QUERY );
	Reference <XNameAccess> xCols = xColsSupp->getColumns();

	do{                                 // middle checked loop!!
	if( bAsTable )			// Daten als Tabelle einfuegen
	{
		rSh.DoUndo( sal_False );

		sal_uInt16 n, nRows = 0, nCols = aLbTableCol.GetEntryCount();
		if( aCbTableHeadon.IsChecked() )
			nRows++;

		if( pSelection )
            nRows = nRows + (sal_uInt16)rSelection.getLength();
		else
			++nRows;

		// bereite das Array fuer die ausgewaehlten Spalten auf
		SwInsDBColumns_SAR aColFlds( 255 >= nCols ? (sal_uInt8)nCols : 255, 5 );
		for( n = 0; n < nCols; ++n )
		{
			sal_uInt16 nFndPos;
			SwInsDBColumn aSrch( aLbTableCol.GetEntry( n ), 0 );
			if( aDBColumns.Seek_Entry( &aSrch, &nFndPos ) )
				aColFlds.Insert( aDBColumns[ nFndPos ], n );
			else {
				ASSERT( !this, "Datenbankspalte nicht mehr gefunden" );
            }
		}

		if( nCols != aColFlds.Count() )
		{
			ASSERT( !this, "nicht alle Datenbankspalten gefunden" );
			nCols = aColFlds.Count();
		}

		if(!nRows || !nCols)
		{
			ASSERT( !this, "wrong parameters" );
			break;
		}

		const SwModuleOptions* pModOpt = SW_MOD()->GetModuleConfig();

		sal_Bool bHTML = 0 != (::GetHtmlMode( pView->GetDocShell() ) & HTMLMODE_ON);
        rSh.InsertTable(
            pModOpt->GetInsTblFlags(bHTML),
            nRows, nCols, text::HoriOrientation::FULL, (pSelection ? pTAutoFmt : 0) );
        rSh.MoveTable( GetfnTablePrev(), GetfnTableStart() );

		if( pSelection && pTblSet )
			SetTabSet();

		SfxItemSet aTblSet( rSh.GetAttrPool(), RES_BOXATR_FORMAT,
												RES_BOXATR_VALUE );
		sal_Bool bIsAutoUpdateCells = rSh.IsAutoUpdateCells();
		rSh.SetAutoUpdateCells( sal_False );


		if( aCbTableHeadon.IsChecked() )
		{
			for( n = 0; n < nCols; ++n )
			{
				if( aRbHeadlColnms.IsChecked() )
                {
                    rSh.SwEditShell::Insert2( aColFlds[ n ]->sColumn );
                }
				rSh.GoNextCell();
			}
		}
		else
            rSh.SetRowsToRepeat( 0 );

		for( sal_Int32 i = 0 ; ; ++i )
		{
			sal_Bool bBreak = sal_False;
			try
			{
				if(pSelection)
                {
                    sal_Int32 nPos = 0;
                    pSelection[i] >>= nPos;
                    bBreak = !xResultSet->absolute(nPos);
                }
				else if(!i)
					bBreak = !xResultSet->first();
			}
            catch(const Exception& )
			{
				bBreak = sal_True;
			}
			if(bBreak)
				break;

			for( n = 0; n < nCols; ++n )
			{
				// beim aller erstenmal KEIN GoNextCell, weil wir schon
				// drin stehen. Auch nicht nach dem Insert das GoNextCell,
				// weil am Ende eine leere Zeile einfuegt wird.
				if( i || n )
					rSh.GoNextCell();

				const SwInsDBColumn* pEntry = aColFlds[ n ];

				Reference< XColumn > xColumn;
				xCols->getByName(pEntry->sColumn) >>= xColumn;
                Reference< XPropertySet > xColumnProps( xColumn, UNO_QUERY );
                sal_Int32 eDataType = 0;
                if( xColumnProps.is() )
                {
                    Any aType = xColumnProps->getPropertyValue(C2U("Type"));
    	            aType >>= eDataType;
                }
				try
				{
					if( pEntry->bHasFmt )
					{
						SwTblBoxNumFormat aNumFmt(
										pEntry->bIsDBFmt ? pEntry->nDBNumFmt
													 	: pEntry->nUsrNumFmt );
						aTblSet.Put(aNumFmt);
						if( xColumn.is() )
						{
							double fVal = xColumn->getDouble();
							if(	xColumn->wasNull() )
								aTblSet.ClearItem( RES_BOXATR_VALUE );
							else
							{
								if(rNumFmtr.GetType(aNumFmt.GetValue()) & NUMBERFORMAT_DATE)
								{
									::Date aStandard(1,1,1900);
									if (*rNumFmtr.GetNullDate() != aStandard)
										fVal += (aStandard - *rNumFmtr.GetNullDate());
								}
								aTblSet.Put( SwTblBoxValue( fVal ));
							}
						}
						else
							aTblSet.ClearItem( RES_BOXATR_VALUE );
						rSh.SetTblBoxFormulaAttrs( aTblSet );
					}
                    //#i60207# don't insert binary data as string - creates a loop
                    else if( DataType::BINARY       == eDataType ||
                             DataType::VARBINARY    == eDataType ||
                             DataType::LONGVARBINARY== eDataType ||
                             DataType::SQLNULL      == eDataType ||
                             DataType::OTHER        == eDataType ||
                             DataType::OBJECT       == eDataType ||
                             DataType::DISTINCT     == eDataType ||
                             DataType::STRUCT       == eDataType ||
                             DataType::ARRAY        == eDataType ||
                             DataType::BLOB         == eDataType ||
                             DataType::CLOB         == eDataType ||
                             DataType::REF          == eDataType
                             )
                    {
                        // do nothing
                    }
                    else
					{
                        rtl::OUString sVal =  xColumn->getString();
						if(!xColumn->wasNull())
                        {
                            rSh.SwEditShell::Insert2( sVal );
                        }
					}
				}
                catch(Exception&
#ifdef DBG_UTIL
                            aExcept
#endif
                )
				{
					DBG_ERROR(ByteString(String(aExcept.Message), gsl_getSystemTextEncoding()).GetBuffer());
				}
			}

			if( !pSelection )
			{
				if ( !xResultSet->next() )
					break;
			}
			else if( i+1 >= rSelection.getLength() )
				break;

			if( 10 == i )
				pWait = ::std::auto_ptr<SwWait>(new SwWait( *pView->GetDocShell(), sal_True ));
		}

        rSh.MoveTable( GetfnTableCurr(), GetfnTableStart() );
		if( !pSelection && ( pTblSet || pTAutoFmt ))
		{
			if( pTblSet )
				SetTabSet();

			if( pTAutoFmt )
				rSh.SetTableAutoFmt( *pTAutoFmt );
		}
		rSh.SetAutoUpdateCells( bIsAutoUpdateCells );
	}
	else 							// Daten als Felder/Text einfuegen
	{
		_DB_Columns aColArr;
		if( SplitTextToColArr( aEdDbText.GetText(), aColArr, aRbAsField.IsChecked() ) )
		{
			// jetzt kann bei jedem Datensatz einfach ueber das Array iteriert
			// und die Daten eingefuegt werden

			if( !rSh.IsSttPara() )
				rSh.SwEditShell::SplitNode();
			if( !rSh.IsEndPara() )
			{
				rSh.SwEditShell::SplitNode();
				rSh.SwCrsrShell::Left(1,CRSR_SKIP_CHARS);
			}

			rSh.DoUndo( sal_False );

			SwTxtFmtColl* pColl = 0;
			{
				String sTmplNm( aLbDbParaColl.GetSelectEntry() );
				if( sNoTmpl != sTmplNm )
				{
					pColl = rSh.FindTxtFmtCollByName( sTmplNm );
					if( !pColl )
					{
						sal_uInt16 nId = SwStyleNameMapper::GetPoolIdFromUIName( sTmplNm, nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL );
						if( USHRT_MAX != nId )
							pColl = rSh.GetTxtCollFromPool( nId );
						else
							pColl = rSh.MakeTxtFmtColl( sTmplNm );
					}
					rSh.SetTxtFmtColl( pColl );
				}
			}

			// fuers Einfuegen als Felder -> nach jedem Datensatz ein
			// "NextField" einfuegen
			SwDBFormatData aDBFormatData;
			Reference< XMultiServiceFactory > xMgr( ::comphelper::getProcessServiceFactory() );
			if( xMgr.is() )
			{
				Reference<XInterface> xInstance = xMgr->createInstance( C2U( "com.sun.star.util.NumberFormatter" ));
				aDBFormatData.xFormatter = Reference<util::XNumberFormatter>(xInstance, UNO_QUERY) ;
			}

			Reference<XPropertySet> xSourceProps(xSource, UNO_QUERY);
			if(xSourceProps.is())
			{
              Any aFormats = xSourceProps->getPropertyValue(C2U("NumberFormatsSupplier"));
              if(aFormats.hasValue())
              {
                  Reference< util::XNumberFormatsSupplier> xSuppl;
                  aFormats >>= xSuppl;
                  if(xSuppl.is())
                  {
                        Reference< XPropertySet > xSettings = xSuppl->getNumberFormatSettings();
                        Any aNull = xSettings->getPropertyValue(C2U("NullDate"));
                        aNull >>= aDBFormatData.aNullDate;
                        if(aDBFormatData.xFormatter.is())
                            aDBFormatData.xFormatter->attachNumberFormatsSupplier(xSuppl);
                  }
              }
			}
			aDBFormatData.aLocale = SvxCreateLocale( rSh.GetCurLang() );
			SwDBNextSetField aNxtDBFld( (SwDBNextSetFieldType*)rSh.
										GetFldType( 0, RES_DBNEXTSETFLD ),
										C2S("1"), aEmptyStr, aDBData );


			sal_Bool bSetCrsr = sal_True;
            sal_uInt16 n = 0, nCols = aColArr.Count();
            ::sw::mark::IMark* pMark = NULL;
			for( sal_Int32 i = 0 ; ; ++i )
			{
				sal_Bool bBreak = sal_False;
				try
				{
					if(pSelection)
                    {
                        sal_Int32 nPos = 0;
                        pSelection[i] >>= nPos;
                        bBreak = !xResultSet->absolute(nPos);
                    }
					else if(!i)
						bBreak = !xResultSet->first();
				}
                catch(Exception&)
                {
					bBreak = sal_True;
				}

				if(bBreak)
					break;


				for( n = 0; n < nCols; ++n )
				{
					_DB_Column* pDBCol = aColArr[ n ];
					String sIns;
					switch( pDBCol->eColType )
					{
					case _DB_Column::DB_FILLTEXT:
						sIns =  *pDBCol->DB_ColumnData.pText;
						break;

					case _DB_Column::DB_SPLITPARA:
						rSh.SplitNode();
						// wenn nicht die gleiche Vorlage die Follow Vorlage
						// ist, dann muss die ausgewaehlte neu gesetzt werden
						if( pColl && &pColl->GetNextTxtFmtColl() != pColl )
							rSh.SetTxtFmtColl( pColl );
						break;

					case _DB_Column::DB_COL_FIELD:
						{
                            SwDBField *const pFld = static_cast<SwDBField *>(
                                pDBCol->DB_ColumnData.pField->CopyField());
							double nValue = DBL_MAX;

							Reference< XPropertySet > xColumnProps;
							xCols->getByName(pDBCol->pColInfo->sColumn) >>= xColumnProps;

							pFld->SetExpansion( SwNewDBMgr::GetDBField(
												xColumnProps,
												aDBFormatData,
												&nValue ) );
							if( DBL_MAX != nValue )
                            {
	                            Any aType = xColumnProps->getPropertyValue(C2U("Type"));
	                            sal_Int32 eDataType = 0;
	                            aType >>= eDataType;
		                        if(	DataType::DATE == eDataType  || DataType::TIME == eDataType  ||
                                    DataType::TIMESTAMP  == eDataType)

		                        {
                                    ::Date aStandard(1,1,1900);
                                    ::Date aCompare(aDBFormatData.aNullDate.Day ,
                                                    aDBFormatData.aNullDate.Month,
                                                    aDBFormatData.aNullDate.Year);
                                    if(aStandard != aCompare)
                                        nValue += (aStandard - aCompare);
	    	                    }
                                pFld->ChgValue( nValue, sal_True );
                            }
							pFld->SetInitialized();

							rSh.Insert( *pFld );
							delete pFld;
						}
						break;

					case _DB_Column::DB_COL_TEXT:
						{
							double nValue = DBL_MAX;
							Reference< XPropertySet > xColumnProps;
							xCols->getByName(pDBCol->pColInfo->sColumn) >>= xColumnProps;
							sIns = SwNewDBMgr::GetDBField(
												xColumnProps,
												aDBFormatData,
												&nValue );
							if( pDBCol->DB_ColumnData.nFormat &&
								DBL_MAX != nValue )
							{
								Color* pCol;
                                if(rNumFmtr.GetType(pDBCol->DB_ColumnData.nFormat) & NUMBERFORMAT_DATE)
                                {
                                    ::Date aStandard(1,1,1900);
                                    if (*rNumFmtr.GetNullDate() != aStandard)
                                        nValue += (aStandard - *rNumFmtr.GetNullDate());
                                }
                                rNumFmtr.GetOutputString( nValue,
											pDBCol->DB_ColumnData.nFormat,
											sIns, &pCol );
							}
						}
						break;
					}

					if( sIns.Len() )
						rSh.Insert( sIns );

					if( bSetCrsr && sIns.Len() )
					{
						// zum Anfang und eine Mark setzen, damit der
						// Cursor am Ende wieder auf Anfangsposition
						// gesetzt werden kann.

						// rSh.SwCrsrShell::MovePara( fnParaCurr, fnParaStart );
						rSh.SwCrsrShell::MovePara(
							GetfnParaCurr(), GetfnParaStart() );
						pMark = rSh.SetBookmark(
                            KeyCode(),
                            ::rtl::OUString(),
                            ::rtl::OUString(), IDocumentMarkAccess::UNO_BOOKMARK );
						// rSh.SwCrsrShell::MovePara( fnParaCurr, fnParaEnd );
						rSh.SwCrsrShell::MovePara(
							GetfnParaCurr(), GetfnParaEnd() );
						bSetCrsr = sal_False;
					}
				}

				if( !pSelection )
				{
					sal_Bool bNext = xResultSet->next();
					if(!bNext)
						break;
				}
				else if( i+1 >= rSelection.getLength() )
					break;

				if( aRbAsField.IsChecked() )
					rSh.Insert( aNxtDBFld );

				if( !rSh.IsSttPara() )
					rSh.SwEditShell::SplitNode();

				if( 10 == i )
					pWait = ::std::auto_ptr<SwWait>(new SwWait( *pView->GetDocShell(), sal_True ));
			}

			if( !bSetCrsr && pMark != NULL)
			{
                rSh.SetMark();
                rSh.GotoMark( pMark );
                rSh.getIDocumentMarkAccess()->deleteMark( pMark );
                break;
			}
		}
	}
	// write configuration
	Commit();
	}while( sal_False );                    // middle checked loop

	if( bUndo )
	{
		rSh.DoUndo( sal_True );
		rSh.AppendUndoForInsertFromDB( bAsTable );
        rSh.EndUndo( UNDO_EMPTY );
	}
	rSh.ClearMark();
	rSh.EndAllAction();

    if ( bDisposeResultSet )
		::comphelper::disposeComponent(xResultSet);
}
void SwInsertDBColAutoPilot::SetTabSet()
{
	SwWrtShell& rSh = pView->GetWrtShell();
	const SfxPoolItem* pItem;

	if( pTAutoFmt )
	{
		if( pTAutoFmt->IsFrame() )
		{
			// Umrandung kommt vom AutoFormat
			pTblSet->ClearItem( RES_BOX );
			pTblSet->ClearItem( SID_ATTR_BORDER_INNER );
		}
		if( pTAutoFmt->IsBackground() )
		{
			pTblSet->ClearItem( RES_BACKGROUND );
			pTblSet->ClearItem( SID_ATTR_BRUSH_ROW );
			pTblSet->ClearItem( SID_ATTR_BRUSH_TABLE );
		}
	}
	else
	{
		// die Defaults wieder entfernen, es macht keinen Sinn sie zu setzen
		SvxBrushItem aBrush( RES_BACKGROUND );
		static sal_uInt16 __READONLY_DATA aIds[3] =
			{ RES_BACKGROUND, SID_ATTR_BRUSH_ROW, SID_ATTR_BRUSH_TABLE };
		for( int i = 0; i < 3; ++i )
			if( SFX_ITEM_SET == pTblSet->GetItemState( aIds[ i ],
				sal_False, &pItem ) && *pItem == aBrush )
				pTblSet->ClearItem( aIds[ i ] );
	}

	if( SFX_ITEM_SET == pTblSet->GetItemState( FN_PARAM_TABLE_NAME, sal_False,
		&pItem ) && ((const SfxStringItem*)pItem)->GetValue() ==
					rSh.GetTableFmt()->GetName() )
		pTblSet->ClearItem( FN_PARAM_TABLE_NAME );

    rSh.MoveTable( GetfnTableCurr(), GetfnTableStart() );
	rSh.SetMark();
    rSh.MoveTable( GetfnTableCurr(), GetfnTableEnd() );

	ItemSetToTableParam( *pTblSet, rSh );

	rSh.ClearMark();
    rSh.MoveTable( GetfnTableCurr(), GetfnTableStart() );
}

/*  */


_DB_ColumnConfigData::~_DB_ColumnConfigData() {}

/* -----------------------------05.12.00 16:15--------------------------------

 ---------------------------------------------------------------------------*/
static Sequence<rtl::OUString> lcl_createSourceNames(const String& rNodeName)
{
    Sequence<rtl::OUString> aSourceNames(11);
    rtl::OUString* pNames = aSourceNames.getArray();

	String sTmp( rNodeName );
	const xub_StrLen nPos = sTmp.Len();
	pNames[0] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/DataSource" ));
	pNames[1] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/Command" ));
	pNames[2] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/CommandType" ));
	pNames[3] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/ColumnsToText" ));
	pNames[4] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/ColumnsToTable" ));
	pNames[5] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/ParaStyle" ));
	pNames[6] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/TableAutoFormat" ));
	pNames[7] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/IsTable" ));
	pNames[8] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/IsField" ));
	pNames[9] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/IsHeadlineOn" ));
	pNames[10] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/IsEmptyHeadline" ));
	return aSourceNames;
}
/* -----------------------------05.12.00 16:25--------------------------------

 ---------------------------------------------------------------------------*/
static Sequence<rtl::OUString> lcl_CreateSubNames( const String& rSubNodeName )
{
    Sequence<rtl::OUString> aSubSourceNames(6);
    rtl::OUString* pNames = aSubSourceNames.getArray();
	String sTmp( rSubNodeName );
	const xub_StrLen nPos = sTmp.Len();
	pNames[0] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/ColumnName" ));
	pNames[1] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/ColumnIndex" ));
	pNames[2] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/IsNumberFormat" ));
	pNames[3] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/IsNumberFormatFromDataBase" ));
	pNames[4] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/NumberFormat" ));
	pNames[5] = sTmp.ReplaceAscii( nPos, STRING_MAXLEN,
							RTL_CONSTASCII_STRINGPARAM( "/NumberFormatLocale" ));
	return aSubSourceNames;
}
/* -----------------------------06.12.00 13:03--------------------------------

 ---------------------------------------------------------------------------*/
static rtl::OUString lcl_CreateUniqueName(const Sequence<rtl::OUString>& aNames)
{
	sal_Int32 nIdx = aNames.getLength();
    const rtl::OUString* pNames = aNames.getConstArray();
    rtl::OUString sTest(C2U("_"));
    rtl::OUString sRet;
	while(sal_True)
	{
        sRet = sTest; sRet += rtl::OUString::valueOf(nIdx++);
		sal_Bool bFound = sal_False;
		for(sal_Int32 i = 0; i < aNames.getLength(); i++)
		{
			if(pNames[i] == sRet)
			{
				bFound = sal_True;
				break;
			}
		}
		if(!bFound)
			break;
	}
	return sRet;
}
/* -----------------------------05.12.00 15:00--------------------------------

 ---------------------------------------------------------------------------*/
void SwInsertDBColAutoPilot::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >&  ) {}

void SwInsertDBColAutoPilot::Commit()
{
    Sequence <rtl::OUString> aNames = GetNodeNames(rtl::OUString());
    const rtl::OUString* pNames = aNames.getArray();
	//remove entries that contain this data source + table at first
	for(sal_Int32 nNode = 0; nNode < aNames.getLength(); nNode++)
	{
        Sequence<rtl::OUString> aSourceNames(2);
        rtl::OUString* pSourceNames = aSourceNames.getArray();
		pSourceNames[0] = pNames[nNode];
		pSourceNames[0] += C2U("/DataSource");
		pSourceNames[1] = pNames[nNode];
		pSourceNames[1] += C2U("/Command");
		Sequence<Any> aSourceProperties = GetProperties(aSourceNames);
		const Any* pSourceProps = aSourceProperties.getArray();
        rtl::OUString sSource, sCommand;
		pSourceProps[0] >>= sSource;
		pSourceProps[1] >>= sCommand;
		if(sSource.equals(aDBData.sDataSource) && sCommand.equals(aDBData.sCommand))
		{
            Sequence<rtl::OUString> aElements(1);
			aElements.getArray()[0] = pNames[nNode];
            ClearNodeElements(rtl::OUString(), aElements);
		}
	}

    aNames = GetNodeNames(rtl::OUString());
    rtl::OUString sNewNode = lcl_CreateUniqueName(aNames);
    Sequence<rtl::OUString> aNodeNames = lcl_createSourceNames(sNewNode);
	Sequence<PropertyValue> aValues(aNodeNames.getLength());
	PropertyValue* pValues = aValues.getArray();
    const rtl::OUString* pNodeNames = aNodeNames.getConstArray();
    rtl::OUString sSlash(C2U("/"));
	for(sal_Int32 i = 0; i < aNodeNames.getLength(); i++)
	{
		pValues[i].Name = sSlash;
		pValues[i].Name += pNodeNames[i];
	}

    pValues[0].Value <<= rtl::OUString(aDBData.sDataSource);
    pValues[1].Value <<= rtl::OUString(aDBData.sCommand);
	pValues[2].Value <<= aDBData.nCommandType;
    pValues[3].Value <<= rtl::OUString(aEdDbText.GetText());

	String sTmp;
	for( sal_uInt16 n = 0, nCnt = aLbTableCol.GetEntryCount(); n < nCnt; ++n )
		( sTmp += aLbTableCol.GetEntry( n ) ) += '\x0a';

	if( sTmp.Len() )
        pValues[4].Value <<= rtl::OUString(sTmp);

	if( sNoTmpl != (sTmp = aLbDbParaColl.GetSelectEntry()) )
        pValues[5].Value <<= rtl::OUString(sTmp);

	if( pTAutoFmt )
        pValues[6].Value <<= rtl::OUString(pTAutoFmt->GetName());

	const Type& rBoolType = ::getBooleanCppuType();
	sal_Bool bTmp = aRbAsTable.IsChecked();
	pValues[7].Value.setValue(&bTmp, rBoolType);

	bTmp = aRbAsField.IsChecked();
	pValues[8].Value.setValue(&bTmp, rBoolType);

	bTmp = aCbTableHeadon.IsChecked();
	pValues[9].Value.setValue(&bTmp, rBoolType);

	bTmp = aRbHeadlEmpty.IsChecked();
	pValues[10].Value.setValue(&bTmp, rBoolType);

    SetSetProperties(rtl::OUString(), aValues);

	sNewNode += C2U("/ColumnSet");
	String sDelim( String::CreateFromAscii( "/__" ));

	LanguageType ePrevLang = (LanguageType)-1;
    rtl::OUString sPrevLang;

	SvNumberFormatter& rNFmtr = *pView->GetWrtShell().GetNumberFormatter();
	for(sal_uInt16 nCol = 0; nCol < aDBColumns.Count(); nCol++)
	{
        rtl::OUString sColumnNode = sNewNode;
 		SwInsDBColumn* pColumn = aDBColumns[nCol];
		String sColumnInsertNode(sColumnNode);
		sColumnInsertNode += sDelim;
		if( nCol < 100 )
			sColumnInsertNode += '0';
		if( nCol < 10 )
			sColumnInsertNode += '0';
		sColumnInsertNode += String::CreateFromInt32(  nCol );

        Sequence <rtl::OUString> aSubNodeNames = lcl_CreateSubNames(sColumnInsertNode);
		Sequence<PropertyValue> aSubValues(aSubNodeNames.getLength());
		PropertyValue* pSubValues = aSubValues.getArray();
        const rtl::OUString* pSubNodeNames = aSubNodeNames.getConstArray();
		sal_Int32 i;

		for( i = 0; i < aSubNodeNames.getLength(); i++)
			pSubValues[i].Name = pSubNodeNames[i];
		pSubValues[0].Value <<= pColumn->sColumn;
		pSubValues[1].Value <<= i;

		sal_Bool bVal = pColumn->bHasFmt;
		pSubValues[2].Value.setValue(&bVal, rBoolType);
		bVal = pColumn->bIsDBFmt;
		pSubValues[3].Value.setValue(&bVal, rBoolType);

		SwStyleNameMapper::FillUIName( RES_POOLCOLL_STANDARD, sTmp );
		const SvNumberformat* pNF = rNFmtr.GetEntry( pColumn->nUsrNumFmt );
		LanguageType eLang;
		if( pNF )
		{
            pSubValues[4].Value <<= rtl::OUString(pNF->GetFormatstring());
			eLang = pNF->GetLanguage();
		}
		else
		{
            pSubValues[4].Value <<= rtl::OUString(sTmp);
			eLang = (LanguageType)GetAppLanguage();
		}

		if( eLang != ePrevLang )
		{
			Locale aLocale;
			aLocale = SvxLanguageToLocale( aLocale, eLang );
            (( sPrevLang = aLocale.Country ) += rtl::OUString( '-' )) += aLocale.Language;
			ePrevLang = eLang;
		}

		pSubValues[5].Value <<=  sPrevLang;
		SetSetProperties(sColumnNode, aSubValues);
	}
}
/* -----------------------------05.12.00 15:00--------------------------------

 ---------------------------------------------------------------------------*/
void SwInsertDBColAutoPilot::Load()
{
    Sequence <rtl::OUString> aNames = GetNodeNames(rtl::OUString());
    const rtl::OUString* pNames = aNames.getArray();
	SvNumberFormatter& rNFmtr = *pView->GetWrtShell().GetNumberFormatter();
	for(sal_Int32 nNode = 0; nNode < aNames.getLength(); nNode++)
	{
		//search for entries with the appropriate data source and table
        Sequence<rtl::OUString> aSourceNames = lcl_createSourceNames(pNames[nNode]);

		Sequence< Any> aDataSourceProps = GetProperties(aSourceNames);
		const Any* pDataSourceProps = aDataSourceProps.getConstArray();
        rtl::OUString sSource, sCommand;
		sal_Int16 nCommandType;
		pDataSourceProps[0] >>= sSource;
		pDataSourceProps[1] >>= sCommand;
		pDataSourceProps[2] >>= nCommandType;
		if(sSource.equals(aDBData.sDataSource) && sCommand.equals(aDBData.sCommand))
		{
			_DB_ColumnConfigData* pNewData = new _DB_ColumnConfigData;
			pNewData->sSource = sSource;
			pNewData->sTable = sCommand;

			pDataSourceProps[3] >>= pNewData->sEdit;
			pDataSourceProps[4] >>= pNewData->sTblList;
			pDataSourceProps[5] >>= pNewData->sTmplNm;
			pDataSourceProps[6] >>= pNewData->sTAutoFmtNm;
			if(pDataSourceProps[7].hasValue())
				pNewData->bIsTable = *(sal_Bool*)pDataSourceProps[7].getValue();
			if(pDataSourceProps[8].hasValue())
		 		pNewData->bIsField = *(sal_Bool*)pDataSourceProps[8].getValue();
			if(pDataSourceProps[9].hasValue())
		 		pNewData->bIsHeadlineOn = *(sal_Bool*)pDataSourceProps[9].getValue();
			if(pDataSourceProps[10].hasValue())
		 		pNewData->bIsEmptyHeadln = *(sal_Bool*)pDataSourceProps[10].getValue();

            rtl::OUString sSubNodeName(pNames[nNode]);
			sSubNodeName += C2U("/ColumnSet/");
            Sequence <rtl::OUString> aSubNames = GetNodeNames(sSubNodeName);
            const rtl::OUString* pSubNames = aSubNames.getConstArray();
			for(sal_Int32 nSub = 0; nSub < aSubNames.getLength(); nSub++)
			{
                rtl::OUString sSubSubNodeName(sSubNodeName);
				sSubSubNodeName += pSubNames[nSub];
                Sequence <rtl::OUString> aSubNodeNames = lcl_CreateSubNames(sSubSubNodeName);
				Sequence< Any> aSubProps = GetProperties(aSubNodeNames);
				const Any* pSubProps = aSubProps.getConstArray();

                rtl::OUString sColumn;
				pSubProps[0] >>= sColumn;
				//check for existance of the loaded column name
				sal_Bool bFound = sal_False;
				for(sal_Int32 nRealColumn = 0; nRealColumn < aDBColumns.Count(); nRealColumn++)
				{
                    if(aDBColumns[(sal_uInt16)nRealColumn]->sColumn == sColumn)
					{
						bFound = sal_True;
						break;
					}
				}
				if(!bFound)
					continue;
				sal_Int16 nIndex = 0;
				pSubProps[1] >>= nIndex;
				SwInsDBColumnPtr pInsDBColumn = new SwInsDBColumn(sColumn, nIndex);
				if(pSubProps[2].hasValue())
					pInsDBColumn->bHasFmt = *(sal_Bool*)pSubProps[2].getValue();
				if(pSubProps[3].hasValue())
					pInsDBColumn->bIsDBFmt = *(sal_Bool*)pSubProps[3].getValue();

				pSubProps[4] >>= pInsDBColumn->sUsrNumFmt;
                rtl::OUString sNumberFormatLocale;
				pSubProps[5] >>= sNumberFormatLocale;

				Locale aLocale;
				aLocale.Language = sNumberFormatLocale.copy(0, 2);
				aLocale.Country = sNumberFormatLocale.copy(3, 2);
				pInsDBColumn->eUsrNumFmtLng = SvxLocaleToLanguage( aLocale );

				pInsDBColumn->nUsrNumFmt = rNFmtr.GetEntryKey( pInsDBColumn->sUsrNumFmt,
														pInsDBColumn->eUsrNumFmtLng );

//				pInsDBColumn->nDBNumFmt

				pNewData->aDBColumns.Insert(pInsDBColumn);
			}
			sal_uInt16 n = 0;
			String sTmp( pNewData->sTblList );
			if( sTmp.Len() )
			{
				do {
					String sEntry( sTmp.GetToken( 0, '\x0a', n ) );
					//preselect column - if they still exist!
                    if(aLbTblDbColumn.GetEntryPos(sEntry) != LISTBOX_ENTRY_NOTFOUND)
                    {
						aLbTableCol.InsertEntry( sEntry );
						aLbTblDbColumn.RemoveEntry( sEntry );
					}
				} while( n < sTmp.Len() );

				if( !aLbTblDbColumn.GetEntryCount() )
				{
                    aIbDbcolAllTo.Enable( sal_False );
                    aIbDbcolOneTo.Enable( sal_False );
				}
                aIbDbcolOneFrom.Enable( sal_True );
                aIbDbcolAllFrom.Enable( sal_True );
			}
			aEdDbText.SetText( pNewData->sEdit );

			sTmp = pNewData->sTmplNm;
			if( sTmp.Len() )
				aLbDbParaColl.SelectEntry( sTmp );
			else
				aLbDbParaColl.SelectEntryPos( 0 );

			if( pTAutoFmt )
				delete pTAutoFmt, pTAutoFmt = 0;
			sTmp = pNewData->sTAutoFmtNm;
			if( sTmp.Len() )
			{
				// dann erstmal die AutoFmt-Datei laden und das Autoformat suchen
				SwTableAutoFmtTbl aAutoFmtTbl;
				aAutoFmtTbl.Load();
                for( sal_uInt16 nAutoFmt = aAutoFmtTbl.Count(); nAutoFmt; )
                    if( sTmp == aAutoFmtTbl[ --nAutoFmt ]->GetName() )
					{
                        pTAutoFmt = new SwTableAutoFmt( *aAutoFmtTbl[ nAutoFmt ] );
						break;
					}
			}

			aRbAsTable.Check( pNewData->bIsTable );
			aRbAsField.Check( pNewData->bIsField );
			aRbAsText.Check( !pNewData->bIsTable && !pNewData->bIsField );

			aCbTableHeadon.Check( pNewData->bIsHeadlineOn );
			aRbHeadlColnms.Check( !pNewData->bIsEmptyHeadln );
			aRbHeadlEmpty.Check( pNewData->bIsEmptyHeadln );
			HeaderHdl(&aCbTableHeadon);

			// jetzt noch die benutzerdefinierten Numberformat Strings in die
			// Shell kopieren. Nur diese sind dann als ID verfuegbar
            for( n = 0; n < aDBColumns.Count() ; ++n )
			{
				SwInsDBColumn& rSet = *aDBColumns[ n ];
                for( sal_uInt16 m = 0; m < pNewData->aDBColumns.Count() ; ++m )
                {
                    SwInsDBColumn& rGet = *pNewData->aDBColumns[ m ];
                    if(rGet.sColumn == rSet.sColumn)
                    {
                        if( rGet.bHasFmt && !rGet.bIsDBFmt )
                        {
                            rSet.bIsDBFmt = sal_False;
                            rSet.nUsrNumFmt = rNFmtr.GetEntryKey( rGet.sUsrNumFmt,
                                                                    rGet.eUsrNumFmtLng );
                            if( NUMBERFORMAT_ENTRY_NOT_FOUND == rSet.nUsrNumFmt )
                            {
                                xub_StrLen nCheckPos;
                                short nType;
                                String sTmpFmt = rGet.sUsrNumFmt;
                                rNFmtr.PutEntry( sTmpFmt, nCheckPos, nType,
                                                rSet.nUsrNumFmt, rGet.eUsrNumFmtLng );
                                rGet.sUsrNumFmt = sTmpFmt;
                            }
                        }
                        break;
                    }
                }
			}

			// steht der Cursor in einer Tabelle, darf NIE Tabelle auswaehlbar sein
			if( !aRbAsTable.IsEnabled() && aRbAsTable.IsChecked() )
				aRbAsField.Check( sal_True );
			delete pNewData;
			break;
		}
	}
}

