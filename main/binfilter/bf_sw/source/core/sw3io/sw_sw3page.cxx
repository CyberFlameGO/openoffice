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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <bf_svtools/bf_solar.h>

#include "swerror.h"

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "doc.hxx"

#ifndef _TOOLS_TENCCVT_HXX //autogen
#include <tools/tenccvt.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _FMTCOL_HXX //autogen
#include <fmtcol.hxx>
#endif

#ifndef _FMTHDFT_HXX //autogen
#include <fmthdft.hxx>
#endif
#ifndef _FMTPDSC_HXX //autogen
#include <fmtpdsc.hxx>
#endif
#ifndef _HINTS_HXX
#include <hints.hxx>
#endif
#include "sw3imp.hxx"
#include "pagedesc.hxx"
#include "poolfmt.hxx"
namespace binfilter {


////////////////////////////////////////////////////////////////////////////

// Fussnoten-Info einlesen

/*N*/ void Sw3IoImp::InPageFtnInfo( SwPageFtnInfo& rFtn )
/*N*/ {
/*N*/ 	INT32 nHeight, nTopDist, nBottomDist, nNum, nDenom;
/*N*/ 	INT16 nAdjust, nPenWidth;
/*N*/ 	Color aPenColor;
/*N*/ 	BYTE cType = Peek();
/*N*/ 	if( cType == SWG_FOOTINFO || cType == SWG_PAGEFOOTINFO )
/*N*/ 	{
/*N*/ 		OpenRec( cType );
/*N*/ 		*pStrm >> nHeight
/*N*/ 			   >> nTopDist
/*N*/ 			   >> nBottomDist
/*N*/ 			   >> nAdjust
/*N*/ 			   >> nNum >> nDenom
/*N*/ 			   >> nPenWidth
/*N*/ 			   >> aPenColor;
/*N*/ 		CloseRec( cType );
/*N*/ 		rFtn.SetHeight( (SwTwips) nHeight );
/*N*/ 		rFtn.SetTopDist( (SwTwips) nTopDist );
/*N*/ 		rFtn.SetBottomDist( (SwTwips) nBottomDist );
/*N*/ 		rFtn.SetAdj( (SwFtnAdj) nAdjust );
/*N*/ 		Fraction f( nNum, nDenom );
/*N*/ 		rFtn.SetWidth( f );
/*N*/ 		rFtn.SetLineColor( aPenColor );
/*N*/ 		rFtn.SetLineWidth( nPenWidth );
/*N*/ 	}
/*N*/ 	else
/*N*/ 		Error();
/*N*/ }

// Fussnoten-Info ausgeben

/*N*/ void Sw3IoImp::OutPageFtnInfo( const SwPageFtnInfo& rFtn )
/*N*/ {
/*N*/ 	OpenRec( SWG_PAGEFOOTINFO );
/*N*/ 	*pStrm << (INT32) rFtn.GetHeight()
/*N*/ 		   << (INT32) rFtn.GetTopDist()
/*N*/ 		   << (INT32) rFtn.GetBottomDist()
/*N*/ 		   << (INT16) rFtn.GetAdj()
/*N*/ 		   << (INT32) rFtn.GetWidth().GetNumerator()
/*N*/ 		   << (INT32) rFtn.GetWidth().GetDenominator()
/*N*/ 		   << (INT16) rFtn.GetLineWidth()
/*N*/ 		   << rFtn.GetLineColor();
/*N*/ 	CloseRec( SWG_PAGEFOOTINFO );
/*N*/ }

/*N*/ void Sw3IoImp::InPageDescs()
/*N*/ {
/*N*/ 	if( Peek() != SWG_STRINGPOOL )
/*N*/ 	{
/*N*/ 		InHeader( TRUE );
/*N*/ 		if( !Good() )
/*N*/ 			return;
/*N*/ 	}
/*N*/ 	else
/*N*/ 		nVersion = SWG_FRSTVERSION;
/*N*/ 	// Der Stream koennte auch leer sein, falls eine leere Textbaustein-
/*N*/ 	// datei eingelesen wird.
/*N*/ 	if( Peek() == SWG_STRINGPOOL )
/*N*/ 	{
/*N*/ 		InStringPool( SWG_STRINGPOOL, aStringPool );
/*N*/ 
/*N*/ 		while( SWG_FIELDTYPE == Peek() )
/*N*/ 			InFieldType();
/*N*/ 
/*N*/ 		if( SWG_BOOKMARKS == Peek() )
/*N*/ 			InBookmarks();
/*N*/ 
/*N*/ 		if( SWG_REDLINES == Peek() )
                InRedlines(); //SW50.SDW 
/*N*/ 
/*N*/ 		// ggfs. dem Numberformatter lesen
/*N*/ 		if( SWG_NUMBERFORMATTER == Peek() )
/*N*/ 			InNumberFormatter();
/*N*/ 
/*N*/ 		OpenRec( SWG_PAGEDESCS );
/*N*/ 		OpenFlagRec();
/*N*/ 		USHORT nDesc;
/*N*/ 		*pStrm >> nDesc;
/*N*/ 		CloseFlagRec();
/*N*/ 		if( nDesc > 0 )
/*N*/ 		{
/*N*/ 			USHORT* pFollows = new USHORT[ nDesc ];
/*N*/ 			SwPageDesc** pDescs = new SwPageDesc*[ nDesc ];
				USHORT i=0;
/*N*/ 			for( i = 0; i < nDesc; i++ )
/*N*/ 				pDescs[ i ] = InPageDesc( pFollows[ i ] );
/*N*/ 			// Follows aufloesen
/*N*/ 			for( i = 0; i < nDesc; i++ )
/*N*/ 			{
/*N*/ 				USHORT nFollow = pFollows[ i ];
/*N*/ 				SwPageDesc* pDesc = pDescs[ i ];
/*N*/ 				if( pDesc && nFollow < IDX_SPEC_VALUE )
/*N*/ 					pDesc->SetFollow( FindPageDesc( nFollow ) );
/*N*/ 			}
/*N*/             delete []pDescs;
/*N*/             delete []pFollows;
/*N*/ 		}
/*N*/ 		else if( nDesc )
/*N*/ 			Error();
/*N*/ 
/*N*/ #ifdef TEST_HUGE_DOCS
/*N*/ 		if( SWG_TESTHUGEDOCS == Peek() )
/*N*/ 			InHugeRecord();
/*N*/ #endif
/*N*/ 
/*N*/ 		CloseRec( SWG_PAGEDESCS );
/*N*/ 
/*N*/ 		// Aufraeumen, wenn Seitenvorlagen geladen wurden
/*N*/ 		if( bPageDescs )
/*N*/ 		{
/*?*/ 			SwDBData aOldData(pDoc->_GetDBDesc());
/*?*/ 
/*?*/ 			// fuer Versionen ohne mehrfache Datenbanken wird jetzt der
/*?*/ 			// Datenbankname gelesen.
/*?*/ 			// if( !nRes && nVersion < SWG_MULTIDB )
/*?*/ 			if( !nRes &&
/*?*/ 				!IsVersion( SWG_MULTIDB, SWG_EXPORT31, SWG_DESKTOP40 ) )
                    SetDBName();
/*?*/ 			Cleanup();
/*?*/ 
/*?*/ 			pDoc->ChgDBData( aOldData );
/*N*/ 		}
/*N*/ 		else if( bOrganizer )
/*N*/ 		{
/*N*/ 			//JP 18.08.98: Bug 55115 - PageDescAttribute mit ihren Vorlagen
/*N*/ 			//				verbinden
/*?*/ 			ConnectPageDescAttrs();
/*?*/ 			if( gsl_getSystemTextEncoding() != eSrcSet )
/*?*/ 				ChangeFontItemCharSet();
/*?*/ 
/*?*/ 			// Temporaere Namenserweiterungen entfernen
/*?*/ 			aStringPool.RemoveExtensions( *pDoc );
/*?*/ 			ConvertFmtsToStarSymbol();
/*N*/ 		}
/*N*/ 		// TODO: unicode: is this required really?
/*N*/ 		eSrcSet = GetSOLoadTextEncoding( gsl_getSystemTextEncoding(),
/*N*/ 										 pStrm->GetVersion() );
/*N*/ 	}
/*N*/ }

void Sw3IoImp::SetDBName()
{
    SvStream* pOld = pStrm;
    pContents->Seek( 0L );
    pContents->SetBufferSize( SW3_BSR_CONTENTS );
    pStrm = pContents;

    // Erstmal den Header lesen
    BYTE cLen, cSet;
    INT8 nLCompatVer, nDummy8;
    USHORT nLVersion, nLFileFlags;
    INT32 nDummy32;
    sal_Char cHdrSign[ 8 ];

    Reset2();
    OutputMode( FALSE );

    if( pStrm->Read( cHdrSign, 7 ) != 7 || !CheckHeader(cHdrSign) )
    {
        Error( ERR_SW6_NOWRITER_FILE );
        return;
    }
    *pStrm >> cLen;

    // nRecSizesPos braucht hier nicht gelesen zu werden, da die Methode
    // nicht fuer eine 5.0 ff aufgerufen wird.
    ULONG nOld = pStrm->Tell();
    *pStrm >> nLVersion >> nLFileFlags >> nDummy32 >> nDummy32 >> nDummy32
           >> nDummy8 >> nDummy8 >> nDummy8 >> nLCompatVer;
    if( nLCompatVer > SWG_CVERSION )
    {
        Error( ERR_SWG_NEW_VERSION );
        return;
    }
    if( nLFileFlags & SWGF_BAD_FILE )
    {
        Error( ERR_SWG_READ_ERROR );
        return;
    }

    pStrm->Read( cPasswd, 16L );
    *pStrm >> cSet >> nDummy8 >> nDummy32 >> nDummy32;
    if( nLFileFlags & SWGF_BLOCKNAME )
    {
        BYTE cBuf[ 64 ];
        if( pStrm->Read( cBuf, 64 ) != 64 )
            pStrm->SetError( SVSTREAM_FILEFORMAT_ERROR );
    }

    ULONG nNew = pStrm->Tell();
    nOld += cLen;
    if( nOld != nNew )
        pStrm->Seek( nOld );

    BOOL bDone = FALSE;

    // Normales Lesen?
    while( !bDone )
    {
        BYTE cType = Peek();
        if( !Good() || pStrm->IsEof() )
            bDone = TRUE;
        else switch( cType )
        {
            case SWG_EOF:
                bDone = TRUE;
                break;

            case SWG_DBNAME:
            {
                String sDBName;
                ByteString s8;
                OpenRec( SWG_DBNAME );

                pStrm->ReadByteString( s8 );
                rtl_TextEncoding eEnc = GetSOLoadTextEncoding(
                                (rtl_TextEncoding)cSet, pStrm->GetVersion() );
                sDBName = ConvertStringNoDbDelim( s8, eEnc );
                SwDBData aData;
                aData.sDataSource = sDBName.GetToken(0, DB_DELIM);
                aData.sCommand = sDBName.GetToken(1, DB_DELIM);
                pDoc->ChgDBData( aData );

                ULONG nSaveWarn = nWarn;
                CloseRec( SWG_DBNAME );
                nWarn = nSaveWarn;
            }
            break;

            default:
                SkipRec();
        }
    }

    pStrm = pOld;
    CheckIoError( pContents );
    pContents->SetBufferSize( 0 );
}

/*N*/ void Sw3IoImp::OutPageDescs( BOOL bUsed )
/*N*/ {
/*N*/ 	OutHeader();
/*N*/ 	CollectFlyFrms( NULL );
/*N*/ 	// Stringpool fuellen, Namen im Doc erweitern
/*N*/ 	aStringPool.Setup( *pDoc, pStrm->GetVersion(), pExportInfo );
/*N*/ 	OutStringPool( SWG_STRINGPOOL, aStringPool );
/*N*/ 
/*N*/ 	// Nicht-Systemfeldtypen
/*N*/ 	if( !nRes ) OutFieldTypes();
/*N*/ 
/*N*/ 	// Bookmarks (nicht, wenn ein SW2-TextBlockDoc konvertiert wird)
/*N*/ 	// Wenn nicht nach 3.1 exportiert wird, werden nur die Bookmarks
/*N*/ 	// der Seitenvorlagen geschrieben
/*N*/ 	if( !nRes && !( nGblFlags & SW3F_CONVBLOCK ) )
/*N*/ 		OutBookmarks( TRUE );
/*N*/ 
/*N*/ 	if( !nRes && !IsSw31Or40Export() )
/*N*/ 		OutRedlines( TRUE );
/*N*/ 
/*N*/ 	// Numberformatter schreiben bei normalen Dokumenten. Bei Textbausteinen
/*N*/ 	// erfolgt es im SaveDocContents
/*N*/ 	if( !nRes && !IsSw31Export() && !bBlock )
/*N*/ 		OutNumberFormatter();
/*N*/ 
/*N*/ 	USHORT nArrLen = pDoc->GetPageDescCnt();
/*N*/ 	USHORT nCnt = 0;
/*N*/ 	if( bUsed )
/*N*/ 	{
/*?*/ 		for( USHORT n = 0; n < nArrLen; n++ )
/*?*/ 		{
/*?*/ 			const SwPageDesc& rDesc = pDoc->GetPageDesc( n );
/*?*/ 			if( pDoc->IsUsed( rDesc ) )
/*?*/ 				nCnt++;
/*?*/ 		}
/*N*/ 	}
/*N*/ 	else
/*N*/ 		nCnt = nArrLen;
/*N*/ 
/*N*/ 	OpenRec( SWG_PAGEDESCS );
/*N*/ 	*pStrm << (BYTE)  0x02
/*N*/ 		   << (INT16) nCnt;
/*N*/ 	if( bUsed )
/*N*/ 	{
/*?*/ 		for( USHORT n = 0; n < nArrLen; n++ )
/*?*/ 		{
/*?*/ 			const SwPageDesc& rDesc = pDoc->GetPageDesc( n );
/*?*/ 			if( pDoc->IsUsed( rDesc ) )
/*?*/ 				OutPageDesc( rDesc );
/*?*/ 		}
/*N*/ 	}
/*N*/ 	else for( USHORT n = 0; n < nArrLen; n++ )
/*N*/ 	  OutPageDesc( pDoc->GetPageDesc( n ) );
/*N*/ 
/*N*/ #ifdef TEST_HUGE_DOCS
/*N*/ 	BOOL b = FALSE;
/*N*/ 	if( b )
/*N*/ 		OutHugeRecord( 1024, 32*1024 );
/*N*/ #endif
/*N*/ 
/*N*/ 	CloseRec( SWG_PAGEDESCS );
/*N*/ 
/*N*/ 	// Temporaere Namenserweiterungen entfernen
/*N*/ 	aStringPool.RemoveExtensions( *pDoc );
/*N*/ 
/*N*/ 	// Die Redlines der Seiten-Vorlagen werden jetzt nicht mehr gebraucht
/*N*/ 	// und muessen sogar geloescht werden, weil die Indizierung im
/*N*/ 	// Content-Bereich wieder mit 0 anfaengt.
/*N*/ 	if( pRedlines && nCntntRedlineStart )
/*N*/ 	{
/*?*/ 		pRedlines->Remove( 0, nCntntRedlineStart );
/*?*/ 		nCntntRedlineStart = 0;
/*N*/ 	}
/*N*/ }

// Seitenvorlage einlesen

/*N*/ SwPageDesc* Sw3IoImp::InPageDesc( USHORT& nFollow )
/*N*/ {
/*N*/ 	OpenRec( SWG_PAGEDESC );
/*N*/ 	short nUsedOn;
/*N*/ 	// 0x10 - Landscape mode
/*N*/ 	BYTE cFlags = OpenFlagRec();
/*N*/ 	USHORT nIdx = 0, nPoolId = 0;
/*N*/ 	USHORT nRegCollIdx = IDX_NO_VALUE;
/*N*/ 	BYTE nNumType;
/*N*/ 	*pStrm >> nIdx >> nFollow >> nPoolId >> nNumType >> nUsedOn;
/*N*/ 	if( IsVersion( SWG_REGISTER, SWG_EXPORT31, SWG_DESKTOP40 ) )
/*N*/ 		*pStrm >> nRegCollIdx;
/*N*/ 	CloseFlagRec();
/*N*/ 	if( nRes )
/*N*/ 		return NULL;
/*N*/ 
/*N*/ 	const String& rName = aStringPool.Find( nIdx );
/*N*/ 	SwPageDesc* pDesc = NULL;
/*N*/ 	// Gibt es diese Vorlage bereits?
/*N*/ 	BOOL bPresent = FALSE;
/*N*/ 	USHORT nPos=0;
/*N*/ 	if( bInsert )
/*N*/ 	{
/*?*/ 		USHORT nArrLen = pDoc->GetPageDescCnt();
/*?*/ 		for( nPos = 0; nPos < nArrLen; nPos++ )
/*?*/ 		{
/*?*/ 			pDesc = &pDoc->_GetPageDesc( nPos );
/*?*/ 			if( pDesc->GetName() == rName )
/*?*/ 			{
/*?*/ 				bPresent = TRUE; break;
/*?*/ 			}
/*?*/ 		}
/*?*/ 		if( bPresent && bAdditive )
/*?*/ 		{
/*?*/ 			// Nur hinzufuegen: lass die Vorlage in Ruhe!
/*?*/ 			// Dabei geht nichts verloren, also nWarn retten
/*?*/ 			ULONG nWarnSave = nWarn;
/*?*/ 			CloseRec( SWG_PAGEDESC );
/*?*/ 			nWarn = nWarnSave;
/*?*/ 			return NULL;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	if( bPresent )
/*N*/ 		// Ueberplaetten im Insert Mode: wir brauchen eine Kopie
/*?*/ 		pDesc = new SwPageDesc( *pDesc );
/*N*/ 	else
/*N*/ 	{
/*N*/ 		// Sonst bauen wir uns eine
/*N*/ 		if( nPoolId != IDX_NO_VALUE )
/*N*/ 		{
/*N*/ 			// Fehlerfall: unbekannte Poolvorlage -> neu anlegen
/*N*/ 			if( RES_POOLPAGE_BEGIN > nPoolId ||  nPoolId >= RES_POOLPAGE_END )
/*N*/ 			{
/*?*/ 				ASSERT( !this, "ungueltige Id" );
/*?*/ 				nPoolId = IDX_NO_VALUE;
/*N*/ 			}
/*N*/ 		}
/*N*/ 
/*N*/ 		if( nPoolId != IDX_NO_VALUE )
/*N*/ 			pDesc = pDoc->GetPageDescFromPool( nPoolId );
/*N*/ 		else
/*N*/ 			pDesc = &pDoc->_GetPageDesc( pDoc->MakePageDesc( rName ) );
/*N*/ 	}
/*N*/ 
/*N*/ 	SvxNumberType aType;
/*N*/ 	aType.SetNumberingType(nNumType);
/*N*/ 	pDesc->SetNumType( aType );
/*N*/ 	pDesc->WriteUseOn( UseOnPage( nUsedOn ) );
/*N*/ 	pDesc->SetLandscape( BOOL( ( cFlags & 0x10 ) != 0 ) );
/*N*/ 	if( nPoolId != IDX_NO_VALUE )
/*N*/ 		pDesc->SetPoolFmtId( nPoolId );
/*N*/ 
/*N*/ 	if( nRegCollIdx != IDX_NO_VALUE )
/*N*/ 		pDesc->SetRegisterFmtColl( FindTxtColl( nRegCollIdx ) );
/*N*/ 
/*N*/ 	pDesc->ResetAllMasterAttr();
/*N*/ 	pDesc->ResetAllLeftAttr();
/*N*/ 	// Header und Footer sind Shared-Formate
/*N*/ 	nGblFlags |= SW3F_SHAREDFMT;
/*N*/ 	short nAttrSet = 0;
/*N*/ 	while( BytesLeft() )
/*N*/ 	{
/*N*/ 		BYTE cType = Peek();
/*N*/ 		switch( cType )
/*N*/ 		{
/*N*/ 			case SWG_FOOTINFO:
/*N*/ 			case SWG_PAGEFOOTINFO:
/*N*/ 				InPageFtnInfo( pDesc->GetFtnInfo() ); break;
/*N*/ 			case SWG_ATTRSET:
/*N*/ 				switch( ++nAttrSet )
/*N*/ 				{
/*N*/ 					case 1:	InAttrSet( (SwAttrSet&) pDesc->GetMaster().GetAttrSet() );
/*N*/ 							break;
/*N*/ 					case 2: InAttrSet( (SwAttrSet&) pDesc->GetLeft().GetAttrSet() );
/*N*/ 							break;
/*?*/ 					default: Error(); OpenRec( cType ); CloseRec( cType );
/*N*/ 				} break;
/*N*/ 			default:
/*?*/ 				SkipRec();
/*N*/ 		}
/*N*/ 	}
/*N*/ 	CloseRec( SWG_PAGEDESC );
/*N*/ 	// Hdr/Ftr-Formate umsetzen:
/*N*/ 	if( pDesc->IsHeaderShared() )
/*N*/ 	{
/*N*/ 		const SwFmtHeader& rFmtM = pDesc->GetMaster().GetHeader();
/*N*/ 		pDesc->GetLeft().SetAttr( rFmtM );
/*N*/ 	}
/*N*/ 	if( pDesc->IsFooterShared() )
/*N*/ 	{
/*N*/ 		const SwFmtFooter& rFmtM = pDesc->GetMaster().GetFooter();
/*N*/ 		pDesc->GetLeft().SetAttr( rFmtM );
/*N*/ 	}
/*N*/ 	nGblFlags &= ~SW3F_SHAREDFMT;
/*N*/ 
/*N*/ 	if( bPresent )
/*N*/ 	{
/*N*/ 		// Kopie uebernehmen
/*?*/ 		pDoc->ChgPageDesc( nPos, *pDesc );
/*?*/ 		delete pDesc;
/*?*/ 		pDesc = &pDoc->_GetPageDesc( nPos );
/*N*/ 	}
/*N*/ 
/*N*/ 	return pDesc;
/*N*/ }

/*N*/ void Sw3IoImp::OutPageDesc( const SwPageDesc& rPg )
/*N*/ {
/*N*/ 	const SwPageDesc* p;
/*N*/ 	// 0x10 - Landscape mode
/*N*/ 	BYTE cFlags = IsSw31Export() ? 0x09: 0x0b; 	// diverse Daten
/*N*/ 	if( rPg.GetLandscape() )
/*N*/ 		cFlags |= 0x10;
/*N*/ 	USHORT nIdx, nFollow = IDX_NO_VALUE, nPoolId = rPg.GetPoolFmtId();
/*N*/ 	USHORT nRegCollIdx = IDX_NO_VALUE;
/*N*/ 	nIdx = aStringPool.Add( rPg.GetName(), nPoolId );
/*N*/ 	p = rPg.GetFollow();
/*N*/ 	if( p )
/*N*/ 		nFollow = aStringPool.Add( p->GetName(), p->GetPoolFmtId() );
/*N*/ 
/*N*/ 	const SwTxtFmtColl *pRegFmtColl = rPg.GetRegisterFmtColl();
/*N*/ 	if( pRegFmtColl )
/*N*/ 		nRegCollIdx = aStringPool.Add( pRegFmtColl->GetName(),
/*N*/ 									   pRegFmtColl->GetPoolFmtId() );
/*N*/ 
/*N*/ 
/*N*/ 	OpenRec( SWG_PAGEDESC );
/*N*/ 	*pStrm << (BYTE)   cFlags
/*N*/ 		   << (UINT16) nIdx
/*N*/ 		   << (UINT16) nFollow
/*N*/ 		   << (UINT16) nPoolId
/*N*/ 		   << (BYTE)   rPg.GetNumType().GetNumberingType()
/*N*/ 		   << (UINT16) rPg.ReadUseOn();
/*N*/ 	if( !IsSw31Export() )
/*N*/ 		*pStrm << (UINT16) nRegCollIdx;
/*N*/ 
/*N*/ 	OutPageFtnInfo( rPg.GetFtnInfo() );
/*N*/ 	// Formate (evtl. mit Unterdrueckung von Hdr/Ftr-Formaten)
/*N*/ 	// Diese Unterdrueckung wird auch in den Attr-Schreiberoutinen
/*N*/ 	// verwendet!
/*N*/ 	OutAttrSet( rPg.GetMaster().GetAttrSet() );
/*N*/ 	USHORT nOldFlags = nGblFlags;
/*N*/ 	if( rPg.IsHeaderShared() ) nGblFlags |= SW3F_NOHDRFMT;
/*N*/ 	if( rPg.IsFooterShared() ) nGblFlags |= SW3F_NOFTRFMT;
/*N*/ 	OutAttrSet( rPg.GetLeft().GetAttrSet() );
/*N*/ 	nGblFlags = nOldFlags;
/*N*/ 	CloseRec( SWG_PAGEDESC );
/*N*/ }

// PageDesc-Attribute koennen in einer Absatzvorlage vorkommen; diese
// werden vor den Seitenvorlagen eingelesen, so dass eine Vorwaerts-
// Referenz entsteht. Die Einleseroutine legt den Stringpool-Index
// des Vorlagennamens im Attribut ab. Nach dem Einlesen muss die Vorlage
// dann noch verbunden werden.

// Aufloesen aller Seitenbeschreibungs-Attribute
// Die Attribute werden aus dem Pool entnommen und die
// korrekten Seitenvorlagen werden eingetragen.

/*N*/ void Sw3IoImp::ConnectPageDescAttrs()
/*N*/ {
/*N*/ 	SfxItemPool& rPool = pDoc->GetAttrPool();
/*N*/ 	USHORT nArrLen = rPool.GetItemCount( RES_PAGEDESC );
/*N*/ 	for( USHORT n = 0; n < nArrLen; n++ )
/*N*/ 	{
/*N*/ 		SwFmtPageDesc* pAttr =
/*N*/ 			(SwFmtPageDesc*) rPool.GetItem( RES_PAGEDESC, n );
/*N*/ 		if( pAttr && pAttr->GetDescNameIdx() != IDX_NO_VALUE )
/*N*/ 		{
/*N*/ 			SwPageDesc* pDesc = FindPageDesc( pAttr->GetDescNameIdx() );
/*N*/ 			pAttr->SetDescNameIdx( IDX_NO_VALUE );
/*N*/ 			ASSERT( pDesc, "Unbekannte Seitenvorlage fuer PageDesc-Attribut" );
/*N*/ 			if( pDesc )
/*N*/ 			{
/*N*/ 				pDesc->Add( pAttr );
/*N*/ 				if( bInsert && pAttr->GetDefinedIn() )
/*N*/ 				{
/*N*/ 					// dann sollte das Layout auch etwas davon mitbekommen.
/*N*/ 					if( pAttr->GetDefinedIn()->ISA( SwTxtFmtColl ) )
/*N*/ 					{
/*N*/ 						const SwTxtFmtColl *pColl =
/*N*/ 							static_cast< const SwTxtFmtColl * >( pAttr->GetDefinedIn() );
/*N*/ 						SwAttrSet aChgSet( *pColl->GetAttrSet().GetPool(), RES_PAGEDESC, RES_PAGEDESC );
/*N*/ 						aChgSet.Put( *pAttr );
/*N*/ 						SwAttrSetChg aOld( pColl->GetAttrSet(), aChgSet );
/*N*/ 						SwAttrSetChg aNew( pColl->GetAttrSet(), aChgSet );
/*N*/ 
/*N*/ 						const_cast < SwTxtFmtColl *>( pColl )->Modify( &aOld, &aNew );
/*N*/ 
/*N*/ 					}
/*N*/ 					else
/*N*/ 					{
/*?*/ 					((SwModify*)pAttr->GetDefinedIn())->SwModify::Modify( pAttr, pAttr );
/*N*/ 					}
/*N*/ 				}
/*N*/ 			}
/*N*/		}
/*N*/ 	}
/*N*/ }



}
