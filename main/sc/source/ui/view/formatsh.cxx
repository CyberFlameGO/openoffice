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
#include "precompiled_sc.hxx"


#include <com/sun/star/style/XStyleFamiliesSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameAccess.hpp>

//------------------------------------------------------------------

//svdraw.hxx
#define _SDR_NOITEMS
#define _SDR_NOTOUCH
#define _SDR_NOTRANSFORM
#define _SI_NOSBXCONTROLS
#define _VCONT_HXX
#define _SI_NOOTHERFORMS
#define _VCTRLS_HXX
#define _SI_NOCONTROL
#define _SETBRW_HXX
#define _VCBRW_HXX
#define _SI_NOSBXCONTROLS

//------------------------------------------------------------------

#include "scitems.hxx"
#include <editeng/eeitem.hxx>

#include <sfx2/app.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/request.hxx>
#include <svl/whiter.hxx>
#include <vcl/msgbox.hxx>

#define _ZFORLIST_DECLARE_TABLE
#include <svl/stritem.hxx>
#include <svl/zformat.hxx>
#include <svl/languageoptions.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/langitem.hxx>
#include <svx/numinf.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/templdlg.hxx>
#include <sfx2/tplpitem.hxx>
#include <editeng/svxenum.hxx>
#include <svx/algitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/bolnitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/brshitem.hxx>
#include <editeng/frmdiritem.hxx>
#include <editeng/scripttypeitem.hxx>
#include <svtools/colorcfg.hxx>
#include <editeng/shaditem.hxx>

#include "formatsh.hxx"
#include "sc.hrc"
#include "globstr.hrc"
#include "docsh.hxx"
#include "patattr.hxx"
#include "scmod.hxx"
//CHINA001 #include "styledlg.hxx"
#include "attrdlg.hrc"
#include "stlpool.hxx"
#include "stlsheet.hxx"
#include "printfun.hxx"
#include "docpool.hxx"
#include "scresid.hxx"
#include "tabvwsh.hxx"
#include "undostyl.hxx"


#define ScFormatShell
#define	TableFont
#define FormatForSelection
#include "scslots.hxx"

#include "scabstdlg.hxx" //CHINA001

namespace {

SvxCellHorJustify lclConvertSlotToHAlign( sal_uInt16 nSlot )
{
    SvxCellHorJustify eHJustify = SVX_HOR_JUSTIFY_STANDARD;
    switch( nSlot )
    {
        case SID_ALIGN_ANY_HDEFAULT:    eHJustify = SVX_HOR_JUSTIFY_STANDARD;   break;
        case SID_ALIGN_ANY_LEFT:        eHJustify = SVX_HOR_JUSTIFY_LEFT;       break;
        case SID_ALIGN_ANY_HCENTER:     eHJustify = SVX_HOR_JUSTIFY_CENTER;     break;
        case SID_ALIGN_ANY_RIGHT:       eHJustify = SVX_HOR_JUSTIFY_RIGHT;      break;
        case SID_ALIGN_ANY_JUSTIFIED:   eHJustify = SVX_HOR_JUSTIFY_BLOCK;      break;
        default:    DBG_ERRORFILE( "lclConvertSlotToHAlign - invalid slot" );
    }
    return eHJustify;
}

SvxCellVerJustify lclConvertSlotToVAlign( sal_uInt16 nSlot )
{
    SvxCellVerJustify eVJustify = SVX_VER_JUSTIFY_STANDARD;
    switch( nSlot )
    {
        case SID_ALIGN_ANY_VDEFAULT:    eVJustify = SVX_VER_JUSTIFY_STANDARD;   break;
        case SID_ALIGN_ANY_TOP:         eVJustify = SVX_VER_JUSTIFY_TOP;        break;
        case SID_ALIGN_ANY_VCENTER:     eVJustify = SVX_VER_JUSTIFY_CENTER;     break;
        case SID_ALIGN_ANY_BOTTOM:      eVJustify = SVX_VER_JUSTIFY_BOTTOM;     break;
        default:    DBG_ERRORFILE( "lclConvertSlotToVAlign - invalid slot" );
    }
    return eVJustify;
}

} // namespace

TYPEINIT1( ScFormatShell, SfxShell );

SFX_IMPL_INTERFACE(ScFormatShell, SfxShell, ScResId(SCSTR_FORMATSHELL) )
{
	SFX_OBJECTBAR_REGISTRATION( SFX_OBJECTBAR_OBJECT | SFX_VISIBILITY_STANDARD |
								SFX_VISIBILITY_SERVER,
								ScResId(RID_OBJECTBAR_FORMAT));

}


ScFormatShell::ScFormatShell(ScViewData* pData) :
	SfxShell(pData->GetViewShell()),
	pViewData(pData)
{
	ScTabViewShell*	pTabViewShell = GetViewData()->GetViewShell();

	SetPool( &pTabViewShell->GetPool() );
    ::svl::IUndoManager* pMgr = pViewData->GetSfxDocShell()->GetUndoManager();
    SetUndoManager( pMgr );
    if ( !pViewData->GetDocument()->IsUndoEnabled() )
    {
        pMgr->SetMaxUndoActionCount( 0 );
    }
	SetHelpId(HID_SCSHELL_FORMATSH);
	SetName(String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("Format")));
}

ScFormatShell::~ScFormatShell()
{
}

//------------------------------------------------------------------

void __EXPORT ScFormatShell::GetStyleState( SfxItemSet& rSet )
{
	ScDocument*				pDoc  		= GetViewData()->GetDocument();
	ScTabViewShell*	pTabViewShell  		= GetViewData()->GetViewShell();
	SfxStyleSheetBasePool*	pStylePool	= pDoc->GetStyleSheetPool();

	sal_Bool bProtected = sal_False;
	SCTAB nTabCount = pDoc->GetTableCount();
	for (SCTAB i=0; i<nTabCount; i++)
		if (pDoc->IsTabProtected(i))				// ueberhaupt eine Tabelle geschuetzt?
			bProtected = sal_True;

	SfxWhichIter	aIter(rSet);
	sal_uInt16			nWhich = aIter.FirstWhich();
	sal_uInt16			nSlotId = 0;

	while ( nWhich )
	{
		nSlotId = SfxItemPool::IsWhich( nWhich )
					? GetPool().GetSlotId( nWhich )
					: nWhich;

		switch ( nSlotId )
		{
			case SID_STYLE_APPLY:
				if ( !pStylePool )
					rSet.DisableItem( nSlotId );
				break;

			case SID_STYLE_FAMILY2:		// Zellvorlagen
			{
				SfxStyleSheet* pStyleSheet = (SfxStyleSheet*)
											 pTabViewShell->GetStyleSheetFromMarked();

				if ( pStyleSheet )
					rSet.Put( SfxTemplateItem( nSlotId, pStyleSheet->GetName() ) );
				else
					rSet.Put( SfxTemplateItem( nSlotId, String() ) );
			}
			break;

			case SID_STYLE_FAMILY4:		// Seitenvorlagen
			{
				SCTAB			nCurTab		= GetViewData()->GetTabNo();
				String			aPageStyle  = pDoc->GetPageStyle( nCurTab );
				SfxStyleSheet*	pStyleSheet = (SfxStyleSheet*)pStylePool->
									Find( aPageStyle, SFX_STYLE_FAMILY_PAGE );

				if ( pStyleSheet )
					rSet.Put( SfxTemplateItem( nSlotId, aPageStyle ) );
				else
					rSet.Put( SfxTemplateItem( nSlotId, String() ) );
			}
			break;

			case SID_STYLE_WATERCAN:
			{
				rSet.Put( SfxBoolItem( nSlotId, SC_MOD()->GetIsWaterCan() ) );
			}
			break;

			case SID_STYLE_UPDATE_BY_EXAMPLE:
			{
				ISfxTemplateCommon* pDesigner = SFX_APP()->
						GetCurrentTemplateCommon(pTabViewShell->GetViewFrame()->GetBindings());
				sal_Bool bPage = pDesigner && SFX_STYLE_FAMILY_PAGE == pDesigner->GetActualFamily();

				if ( bProtected || bPage )
					rSet.DisableItem( nSlotId );
			}
			break;

			case SID_STYLE_EDIT:
			case SID_STYLE_DELETE:
			{
				ISfxTemplateCommon* pDesigner = SFX_APP()->
						GetCurrentTemplateCommon(pTabViewShell->GetViewFrame()->GetBindings());
				sal_Bool bPage = pDesigner && SFX_STYLE_FAMILY_PAGE == pDesigner->GetActualFamily();

				if ( bProtected && !bPage )
					rSet.DisableItem( nSlotId );
			}
			break;

			default:
				break;
		}

		nWhich = aIter.NextWhich();
	}
}

//------------------------------------------------------------------

void __EXPORT ScFormatShell::ExecuteStyle( SfxRequest& rReq )
{
	// Wenn ToolBar vertikal :
	if ( !rReq.GetArgs() )
	{
		pViewData->GetDispatcher().Execute( SID_STYLE_DESIGNER, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD );
		return;
	}

	//--------------------------------------------------------------------
	SfxBindings&		rBindings	= pViewData->GetBindings();
	const SfxItemSet*   pArgs   	= rReq.GetArgs();
	const sal_uInt16        nSlotId 	= rReq.GetSlot();
	const SCTAB		    nCurTab		= GetViewData()->GetTabNo();
	ScDocShell*			pDocSh		= GetViewData()->GetDocShell();
	ScTabViewShell*		pTabViewShell= GetViewData()->GetViewShell();
	ScDocument*         pDoc    	= pDocSh->GetDocument();
	ScMarkData&			rMark		= GetViewData()->GetMarkData();
	ScModule*			pScMod		= SC_MOD();
	String				aRefName;
	sal_Bool				bUndo		= pDoc->IsUndoEnabled();

	if (   (nSlotId == SID_STYLE_NEW)
		|| (nSlotId == SID_STYLE_EDIT)
		|| (nSlotId == SID_STYLE_DELETE)
		|| (nSlotId == SID_STYLE_APPLY)
		|| (nSlotId == SID_STYLE_WATERCAN)
		|| (nSlotId == SID_STYLE_FAMILY)
		|| (nSlotId == SID_STYLE_NEW_BY_EXAMPLE)
		|| (nSlotId == SID_STYLE_UPDATE_BY_EXAMPLE) )
	{
		SfxStyleSheetBasePool*  pStylePool	= pDoc->GetStyleSheetPool();
		SfxStyleSheetBase*      pStyleSheet = NULL;

		sal_Bool bStyleToMarked = sal_False;
		sal_Bool bListAction = sal_False;
		sal_Bool bAddUndo = sal_False;			// add ScUndoModifyStyle (style modified)
		ScStyleSaveData aOldData;		// for undo/redo
		ScStyleSaveData aNewData;

		SfxStyleFamily eFamily = SFX_STYLE_FAMILY_PARA;
		const SfxPoolItem* pFamItem;
		if ( pArgs && SFX_ITEM_SET == pArgs->GetItemState( SID_STYLE_FAMILY, sal_True, &pFamItem ) )
			eFamily = (SfxStyleFamily)((const SfxUInt16Item*)pFamItem)->GetValue();
        else
		if ( pArgs && SFX_ITEM_SET == pArgs->GetItemState( SID_STYLE_FAMILYNAME, sal_True, &pFamItem ) )
        {
            String sFamily = ((const SfxStringItem*)pFamItem)->GetValue();
            if (sFamily.CompareToAscii("CellStyles") == COMPARE_EQUAL)
                eFamily = SFX_STYLE_FAMILY_PARA;
            else
            if (sFamily.CompareToAscii("PageStyles") == COMPARE_EQUAL)
                eFamily = SFX_STYLE_FAMILY_PAGE;
        }

		String                  aStyleName;
		sal_uInt16                  nRetMask = 0xffff;
//      #96983# only stylist sends focus to sheet
//        sal_Bool                    bGrabFocus = ( SID_STYLE_APPLY == nSlotId );

		pStylePool->SetSearchMask( eFamily, SFXSTYLEBIT_ALL );

		switch ( nSlotId )
		{
			case SID_STYLE_NEW:
				{
					const SfxPoolItem* pNameItem;
					if (pArgs && SFX_ITEM_SET == pArgs->GetItemState( nSlotId, sal_True, &pNameItem ))
						aStyleName  = ((const SfxStringItem*)pNameItem)->GetValue();

					const SfxPoolItem* pRefItem=NULL;
					if (pArgs && SFX_ITEM_SET == pArgs->GetItemState( SID_STYLE_REFERENCE, sal_True, &pRefItem ))
					{
						if(pRefItem!=NULL)
							aRefName  = ((const SfxStringItem*)pRefItem)->GetValue();
					}

					pStyleSheet = &(pStylePool->Make( aStyleName, eFamily,
													  SFXSTYLEBIT_USERDEF ) );

					if ( pStyleSheet && pStyleSheet->HasParentSupport() )
						pStyleSheet->SetParent(aRefName);
				}
				break;

			case SID_STYLE_APPLY:
            {
                SFX_REQUEST_ARG( rReq, pNameItem, SfxStringItem, SID_APPLY_STYLE, sal_False );
                SFX_REQUEST_ARG( rReq, pFamilyItem, SfxStringItem, SID_STYLE_FAMILYNAME, sal_False );
                if ( pFamilyItem && pNameItem )
                {
                    com::sun::star::uno::Reference< com::sun::star::style::XStyleFamiliesSupplier > xModel(pDocSh->GetModel(), com::sun::star::uno::UNO_QUERY);
                    try
                    {
                        com::sun::star::uno::Reference< com::sun::star::container::XNameAccess > xStyles;
                        com::sun::star::uno::Reference< com::sun::star::container::XNameAccess > xCont = xModel->getStyleFamilies();
                        xCont->getByName(pFamilyItem->GetValue()) >>= xStyles;
                        com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet > xInfo;
                        xStyles->getByName( pNameItem->GetValue() ) >>= xInfo;
                        ::rtl::OUString aUIName;
                        xInfo->getPropertyValue( ::rtl::OUString::createFromAscii("DisplayName") ) >>= aUIName;
                        if ( aUIName.getLength() )
                            rReq.AppendItem( SfxStringItem( SID_STYLE_APPLY, aUIName ) );
                    }
                    catch( com::sun::star::uno::Exception& )
                    {
                    }
                }
            }
			case SID_STYLE_EDIT:
			case SID_STYLE_DELETE:
			case SID_STYLE_NEW_BY_EXAMPLE:
				{
					const SfxPoolItem* pNameItem;
					if (pArgs && SFX_ITEM_SET == pArgs->GetItemState( nSlotId, sal_True, &pNameItem ))
						aStyleName  = ((const SfxStringItem*)pNameItem)->GetValue();
					pStyleSheet = pStylePool->Find( aStyleName, eFamily );

					aOldData.InitFromStyle( pStyleSheet );
				}
				break;

			case SID_STYLE_WATERCAN:
			{
				sal_Bool bWaterCan = pScMod->GetIsWaterCan();

				if( !bWaterCan )
				{
					const SfxPoolItem* pItem;

					if ( SFX_ITEM_SET ==
						 pArgs->GetItemState( nSlotId, sal_True, &pItem ) )
					{
						const SfxStringItem* pStrItem = PTR_CAST(SfxStringItem,pItem);
						if ( pStrItem )
						{
							aStyleName  = pStrItem->GetValue();
							pStyleSheet = pStylePool->Find( aStyleName, eFamily );

							if ( pStyleSheet )
							{
								((ScStyleSheetPool*)pStylePool)->
										SetActualStyleSheet( pStyleSheet );
								rReq.Done();
							}
						}
					}
				}

				if ( !bWaterCan && pStyleSheet )
				{
					pScMod->SetWaterCan( sal_True );
					pTabViewShell->SetActivePointer( Pointer(POINTER_FILL) );
					rReq.Done();
				}
				else
				{
					pScMod->SetWaterCan( sal_False );
					pTabViewShell->SetActivePointer( Pointer(POINTER_ARROW) );
					rReq.Done();
				}
			}
			break;

			default:
				break;
		}

		// Neuen Style fuer WaterCan-Mode setzen
		if ( nSlotId == SID_STYLE_APPLY && pScMod->GetIsWaterCan() && pStyleSheet )
			((ScStyleSheetPool*)pStylePool)->SetActualStyleSheet( pStyleSheet );

		switch ( eFamily )
		{
			case SFX_STYLE_FAMILY_PARA:
			{
				switch ( nSlotId )
				{
					case SID_STYLE_DELETE:
					{
						if ( pStyleSheet )
						{
                            pTabViewShell->RemoveStyleSheetInUse( pStyleSheet );
							pStylePool->Remove( pStyleSheet );
							pTabViewShell->InvalidateAttribs();
							nRetMask = sal_True;
							bAddUndo = sal_True;
							rReq.Done();
						}
						else
							nRetMask = sal_False;
					}
					break;

					case SID_STYLE_APPLY:
					{
						if ( pStyleSheet && !pScMod->GetIsWaterCan() )
						{
							// Anwenden der Vorlage auf das Dokument
							pTabViewShell->SetStyleSheetToMarked( (SfxStyleSheet*)pStyleSheet );
							pTabViewShell->InvalidateAttribs();
							rReq.Done();
						}
					}
					break;

					case SID_STYLE_NEW_BY_EXAMPLE:
					case SID_STYLE_UPDATE_BY_EXAMPLE:
					{
						// Vorlage erzeugen/ersetzen durch Attribute
						// an der Cursor-Position:

						const ScPatternAttr* pAttrItem = NULL;

						// Die Abfrage, ob markiert ist, war hier immer falsch,
						// darum jetzt gar nicht mehr, und einfach vom Cursor.
						// Wenn Attribute aus der Selektion genommen werden sollen,
						// muss noch darauf geachtet werden, Items aus Vorlagen nicht
						// zu uebernehmen (GetSelectionPattern sammelt auch Items aus
						// Vorlagen zusammen) (#44748#)
						//		pAttrItem = GetSelectionPattern();

                        // ScViewData* pViewData = GetViewData();
						SCCOL		nCol = pViewData->GetCurX();
						SCROW		nRow = pViewData->GetCurY();
						pAttrItem = pDoc->GetPattern( nCol, nRow, nCurTab );

						SfxItemSet aAttrSet = pAttrItem->GetItemSet();
						aAttrSet.ClearItem( ATTR_MERGE );
						aAttrSet.ClearItem( ATTR_MERGE_FLAG );
						//	bedingte Formatierung und Gueltigkeit nicht uebernehmen,
						//	weil sie in der Vorlage nicht editiert werden koennen
						aAttrSet.ClearItem( ATTR_VALIDDATA );
						aAttrSet.ClearItem( ATTR_CONDITIONAL );

						if ( SID_STYLE_NEW_BY_EXAMPLE == nSlotId )
						{
							if ( bUndo )
							{
								String aUndo = ScGlobal::GetRscString( STR_UNDO_EDITCELLSTYLE );
								pDocSh->GetUndoManager()->EnterListAction( aUndo, aUndo );
								bListAction = sal_True;
							}

							sal_Bool			bConvertBack = sal_False;
							SfxStyleSheet*	pSheetInUse = (SfxStyleSheet*)
														  pTabViewShell->GetStyleSheetFromMarked();

							// wenn neuer Style vorhanden und in der Selektion
							// verwendet wird, so darf der Parent nicht uebernommen
							// werden:

							if ( pStyleSheet && pSheetInUse && pStyleSheet == pSheetInUse )
								pSheetInUse = NULL;

							// wenn bereits vorhanden, erstmal entfernen...
							if ( pStyleSheet )
							{
								// Style-Pointer zu Namen vor Erase,
								// weil Zellen sonst ungueltige Pointer
								// enthalten.
								//!!! bei Gelenheit mal eine Methode, die
								//    das fuer einen bestimmten Style macht
								pDoc->StylesToNames();
								bConvertBack = sal_True;
								pStylePool->Remove(pStyleSheet);
							}

							// ...und neu anlegen
							pStyleSheet = &pStylePool->Make( aStyleName, eFamily,
															 SFXSTYLEBIT_USERDEF );

							// wenn ein Style vorhanden ist, so wird dieser
							// Parent der neuen Vorlage:
							if ( pSheetInUse && pStyleSheet->HasParentSupport() )
								pStyleSheet->SetParent( pSheetInUse->GetName() );

							if ( bConvertBack )
								// Namen zu Style-Pointer
								pDoc->UpdStlShtPtrsFrmNms();
                            else
                                pDoc->GetPool()->CellStyleCreated( aStyleName );

							// Attribute uebernehmen und Style anwenden
							pStyleSheet->GetItemSet().Put( aAttrSet );
                            pTabViewShell->UpdateStyleSheetInUse( pStyleSheet );

							//	call SetStyleSheetToMarked after adding the ScUndoModifyStyle
							//	(pStyleSheet pointer is used!)
							bStyleToMarked = sal_True;
						}
						else // ( nSlotId == SID_STYLE_UPDATE_BY_EXAMPLE )
						{
							pStyleSheet = (SfxStyleSheet*)pTabViewShell->GetStyleSheetFromMarked();

							if ( pStyleSheet )
							{
								aOldData.InitFromStyle( pStyleSheet );

								if ( bUndo )
								{
									String aUndo = ScGlobal::GetRscString( STR_UNDO_EDITCELLSTYLE );
									pDocSh->GetUndoManager()->EnterListAction( aUndo, aUndo );
									bListAction = sal_True;
								}

								pStyleSheet->GetItemSet().Put( aAttrSet );
                                pTabViewShell->UpdateStyleSheetInUse( pStyleSheet );

								//	call SetStyleSheetToMarked after adding the ScUndoModifyStyle
								//	(pStyleSheet pointer is used!)
								bStyleToMarked = sal_True;
							}
						}

						aNewData.InitFromStyle( pStyleSheet );
						bAddUndo = sal_True;
						rReq.Done();
					}
					break;

					default:
						break;
				}
			} // case SFX_STYLE_FAMILY_PARA:
			break;

			case SFX_STYLE_FAMILY_PAGE:
			{
				switch ( nSlotId )
				{
					case SID_STYLE_DELETE:
					{
						nRetMask = ( NULL != pStyleSheet );
						if ( pStyleSheet )
						{
							if ( pDoc->RemovePageStyleInUse( pStyleSheet->GetName() ) )
							{
								ScPrintFunc( pDocSh, pTabViewShell->GetPrinter(sal_True), nCurTab ).UpdatePages();
								rBindings.Invalidate( SID_STATUS_PAGESTYLE );
								rBindings.Invalidate( FID_RESET_PRINTZOOM );
							}
							pStylePool->Remove( pStyleSheet );
							rBindings.Invalidate( SID_STYLE_FAMILY4 );
							pDocSh->SetDocumentModified();
							bAddUndo = sal_True;
							rReq.Done();
						}
					}
					break;

					case SID_STYLE_APPLY:
					{
						nRetMask = ( NULL != pStyleSheet );
						if ( pStyleSheet && !pScMod->GetIsWaterCan() )
						{
                            ScUndoApplyPageStyle* pUndoAction = 0;
                            for( SCTAB nTab = 0, nTabCount = pDoc->GetTableCount(); nTab < nTabCount; ++nTab )
                            {
                                if( rMark.GetTableSelect( nTab ) )
                                {
                                    String aOldName = pDoc->GetPageStyle( nTab );
                                    if ( aOldName != aStyleName )
                                    {
                                        pDoc->SetPageStyle( nTab, aStyleName );
                                        ScPrintFunc( pDocSh, pTabViewShell->GetPrinter(sal_True), nTab ).UpdatePages();
                                        if( !pUndoAction )
                                            pUndoAction = new ScUndoApplyPageStyle( pDocSh, aStyleName );
                                        pUndoAction->AddSheetAction( nTab, aOldName );
                                    }
                                }
                            }
                            if( pUndoAction )
                            {
                                pDocSh->GetUndoManager()->AddUndoAction( pUndoAction );
                                pDocSh->SetDocumentModified();
                                rBindings.Invalidate( SID_STYLE_FAMILY4 );
                                rBindings.Invalidate( SID_STATUS_PAGESTYLE );
                                rBindings.Invalidate( FID_RESET_PRINTZOOM );
                            }
							rReq.Done();
						}
					}
					break;

					case SID_STYLE_NEW_BY_EXAMPLE:
					{
						const String& rStrCurStyle = pDoc->GetPageStyle( nCurTab );

						if ( rStrCurStyle != aStyleName )
						{
							SfxStyleSheetBase*	pCurStyle = pStylePool->Find( rStrCurStyle, eFamily );
							SfxItemSet			aAttrSet  = pCurStyle->GetItemSet();
							SCTAB				nInTab;
							sal_Bool				bUsed = pDoc->IsPageStyleInUse( aStyleName, &nInTab );

							// wenn bereits vorhanden, erstmal entfernen...
							if ( pStyleSheet )
								pStylePool->Remove( pStyleSheet );

							// ...und neu anlegen
							pStyleSheet = &pStylePool->Make( aStyleName, eFamily,
															 SFXSTYLEBIT_USERDEF );

							// Attribute uebernehmen
							pStyleSheet->GetItemSet().Put( aAttrSet );
							pDocSh->SetDocumentModified();

							// wenn in Verwendung -> Update
							if ( bUsed )
								ScPrintFunc( pDocSh, pTabViewShell->GetPrinter(sal_True), nInTab ).UpdatePages();

							aNewData.InitFromStyle( pStyleSheet );
							bAddUndo = sal_True;
							rReq.Done();
							nRetMask = sal_True;
						}
					}
					break;

					default:
						break;
				} // switch ( nSlotId )
			} // case SFX_STYLE_FAMILY_PAGE:
			break;

			default:
				break;
		} // switch ( eFamily )

		// Neu anlegen oder bearbeiten ueber Dialog:
		if ( nSlotId == SID_STYLE_NEW || nSlotId == SID_STYLE_EDIT )
		{
			if ( pStyleSheet )
			{
				SvxNumberInfoItem* pNumberInfoItem = NULL;

				SfxStyleFamily	eFam    = pStyleSheet->GetFamily();
                // ScDocument*     pDoc    = GetViewData()->GetDocument();
                // ScDocShell*     pDocSh  = GetViewData()->GetDocShell();
				//CHINA001 ScStyleDlg*		pDlg    = NULL;
				SfxAbstractTabDialog* pDlg    = NULL; //CHINA001
				sal_uInt16			nRsc	= 0;

				//	#37034#/#37245# alte Items aus der Vorlage merken
				SfxItemSet aOldSet = pStyleSheet->GetItemSet();
				String aOldName = pStyleSheet->GetName();

				switch ( eFam )
				{
					case SFX_STYLE_FAMILY_PAGE:
						nRsc = RID_SCDLG_STYLES_PAGE;
						break;

					case SFX_STYLE_FAMILY_PARA:
					default:
						{
							SfxItemSet& rSet = pStyleSheet->GetItemSet();

							const SfxPoolItem* pItem;
							if ( rSet.GetItemState( ATTR_VALUE_FORMAT,
									sal_False, &pItem ) == SFX_ITEM_SET )
							{
								// NumberFormat Value aus Value und Language
								// erzeugen und eintueten
								sal_uLong nFormat =
									((SfxUInt32Item*)pItem)->GetValue();
								LanguageType eLang =
									((SvxLanguageItem*)&rSet.Get(
									ATTR_LANGUAGE_FORMAT ))->GetLanguage();
								sal_uLong nLangFormat = pDoc->GetFormatTable()->
									GetFormatForLanguageIfBuiltIn( nFormat, eLang );
								if ( nLangFormat != nFormat )
								{
									SfxUInt32Item aNewItem( ATTR_VALUE_FORMAT, nLangFormat );
									rSet.Put( aNewItem );
									aOldSet.Put( aNewItem );
									// auch in aOldSet fuer Vergleich nach dem Dialog,
									// sonst geht evtl. eine Aenderung der Sprache verloren
								}
							}

							pTabViewShell->MakeNumberInfoItem( pDoc, GetViewData(), &pNumberInfoItem );
							pDocSh->PutItem( *pNumberInfoItem );
							nRsc = RID_SCDLG_STYLES_PAR;

							//	auf jeden Fall ein SvxBoxInfoItem mit Table = sal_False im Set:
							//	(wenn gar kein Item da ist, loescht der Dialog auch das
							//	 BORDER_OUTER SvxBoxItem aus dem Vorlagen-Set)

							if ( rSet.GetItemState( ATTR_BORDER_INNER, sal_False ) != SFX_ITEM_SET )
							{
								SvxBoxInfoItem aBoxInfoItem( ATTR_BORDER_INNER );
								aBoxInfoItem.SetTable(sal_False);		// keine inneren Linien
                                aBoxInfoItem.SetDist(sal_True);
                                aBoxInfoItem.SetMinDist(sal_False);
								rSet.Put( aBoxInfoItem );
							}
						}
						break;
				}

				//	If GetDefDialogParent is a dialog, it must be used
				//	(style catalog)

				Window* pParent = Application::GetDefDialogParent();
				if ( !pParent || !pParent->IsDialog() )
				{
					//	#107256# GetDefDialogParent currently doesn't return the window
					//	that was set with SetDefDialogParent (but dynamically finds the
					//	topmost parent of the focus window), so IsDialog above is FALSE
					//	even if called from the style catalog.
					//	-> Use NULL if a modal dialog is open, to enable the Dialog's
					//	default parent handling.
					if ( Application::IsInModalMode() )
						pParent = NULL;
					else
						pParent = pTabViewShell->GetDialogParent();
				}

				pTabViewShell->SetInFormatDialog(sal_True);

				//CHINA001 pDlg = new ScStyleDlg( pParent, *pStyleSheet, nRsc );
				ScAbstractDialogFactory* pFact = ScAbstractDialogFactory::Create();
				DBG_ASSERT(pFact, "ScAbstractFactory create fail!");//CHINA001

				pDlg = pFact->CreateScStyleDlg( pParent, *pStyleSheet, nRsc, nRsc );
				DBG_ASSERT(pDlg, "Dialog create fail!");//CHINA001
				short nResult = pDlg->Execute();
				pTabViewShell->SetInFormatDialog(sal_False);

				if ( nResult == RET_OK )
				{
					const SfxItemSet* pOutSet = pDlg->GetOutputItemSet();

					if ( pOutSet )
					{
						nRetMask = pStyleSheet->GetMask();

						//	#37034#/#37245# Attribut-Vergleiche (frueher in ModifyStyleSheet)
						//	jetzt hier mit den alten Werten (Style ist schon veraendert)

						if ( SFX_STYLE_FAMILY_PARA == eFam )
						{
//							pDoc->CellStyleChanged( *pStyleSheet, aOldSet );

							SfxItemSet& rNewSet = pStyleSheet->GetItemSet();
							sal_Bool bNumFormatChanged;
							if ( ScGlobal::CheckWidthInvalidate(
												bNumFormatChanged, aOldSet, rNewSet ) )
								pDoc->InvalidateTextWidth( NULL, NULL, bNumFormatChanged );

                            SCTAB nTabCount = pDoc->GetTableCount();
                            for (SCTAB nTab=0; nTab<nTabCount; nTab++)
                                if (pDoc->IsStreamValid(nTab))
                                    pDoc->SetStreamValid(nTab, sal_False);

							sal_uLong nOldFormat = ((const SfxUInt32Item&)aOldSet.
													Get( ATTR_VALUE_FORMAT )).GetValue();
							sal_uLong nNewFormat = ((const SfxUInt32Item&)rNewSet.
													Get( ATTR_VALUE_FORMAT )).GetValue();
							if ( nNewFormat != nOldFormat )
							{
								SvNumberFormatter* pFormatter = pDoc->GetFormatTable();
								const SvNumberformat* pOld = pFormatter->GetEntry( nOldFormat );
								const SvNumberformat* pNew = pFormatter->GetEntry( nNewFormat );
								if ( pOld && pNew && pOld->GetLanguage() != pNew->GetLanguage() )
									rNewSet.Put( SvxLanguageItem(
													pNew->GetLanguage(), ATTR_LANGUAGE_FORMAT ) );
							}

                            pDoc->GetPool()->CellStyleCreated( pStyleSheet->GetName() );
						}
						else
						{
							//!	auch fuer Seitenvorlagen die Abfragen hier

							String aNewName = pStyleSheet->GetName();
							if ( aNewName != aOldName &&
									pDoc->RenamePageStyleInUse( aOldName, aNewName ) )
							{
								rBindings.Invalidate( SID_STATUS_PAGESTYLE );
								rBindings.Invalidate( FID_RESET_PRINTZOOM );
							}

							pDoc->ModifyStyleSheet( *pStyleSheet, *pOutSet );
							rBindings.Invalidate( FID_RESET_PRINTZOOM );
						}

						pDocSh->SetDocumentModified();

						if ( SFX_STYLE_FAMILY_PARA == eFam )
						{
							pTabViewShell->UpdateNumberFormatter( pDoc,
								(const SvxNumberInfoItem&)
									*(pDocSh->GetItem(SID_ATTR_NUMBERFORMAT_INFO)) );

                            pTabViewShell->UpdateStyleSheetInUse( pStyleSheet );
							pTabViewShell->InvalidateAttribs();
						}

						aNewData.InitFromStyle( pStyleSheet );
						bAddUndo = sal_True;
					}
				}
				else
				{
					if ( nSlotId == SID_STYLE_NEW )
						pStylePool->Remove( pStyleSheet );
					else
					{
						//	falls zwischendurch etwas mit dem temporaer geaenderten
						//	ItemSet gepainted wurde:
						pDocSh->PostPaintGridAll();
					}
				}
				delete pDlg;
			}
		}

//		if ( nRetMask != 0xffff )// Irgendein Wert MUSS geliefert werden JN
			rReq.SetReturnValue( SfxUInt16Item( nSlotId, nRetMask ) );

//      #96983# only stylist sends focus to sheet
//        if ( bGrabFocus )
//            pTabViewShell->GetActiveWin()->GrabFocus();

		if ( bAddUndo && bUndo)
			pDocSh->GetUndoManager()->AddUndoAction(
						new ScUndoModifyStyle( pDocSh, eFamily, aOldData, aNewData ) );

		if ( bStyleToMarked )
		{
			//	call SetStyleSheetToMarked after adding the ScUndoModifyStyle,
			//	so redo will find the modified style
			pTabViewShell->SetStyleSheetToMarked( (SfxStyleSheet*)pStyleSheet );
			pTabViewShell->InvalidateAttribs();
		}

		if ( bListAction )
			pDocSh->GetUndoManager()->LeaveListAction();
	}
	else
	{
		DBG_ERROR( "Unknown slot (ScViewShell::ExecuteStyle)" );
	}
}

void ScFormatShell::ExecuteNumFormat( SfxRequest& rReq )
{
	ScModule*			pScMod		= SC_MOD();
	ScTabViewShell*	pTabViewShell  	= GetViewData()->GetViewShell();
	const SfxItemSet*	pReqArgs	= rReq.GetArgs();
	sal_uInt16				nSlot		= rReq.GetSlot();

	pTabViewShell->HideListBox();					// Autofilter-DropDown-Listbox

									// Eingabe beenden
	if ( GetViewData()->HasEditView( GetViewData()->GetActivePart() ) )
	{
		switch ( nSlot )
		{
			case SID_NUMBER_TWODEC:
			case SID_NUMBER_SCIENTIFIC:
			case SID_NUMBER_DATE:
			case SID_NUMBER_CURRENCY:
			case SID_NUMBER_PERCENT:
			case SID_NUMBER_STANDARD:
			case SID_NUMBER_FORMAT:
			case SID_NUMBER_INCDEC:
			case SID_NUMBER_DECDEC:
			case FID_DEFINE_NAME:
			case FID_USE_NAME:
			case FID_INSERT_NAME:
            case SID_SPELL_DIALOG:
            case SID_HANGUL_HANJA_CONVERSION:

			pScMod->InputEnterHandler();
			pTabViewShell->UpdateInputHandler();
			break;

			default:
			break;
		}
	}

	switch ( nSlot )
	{
		case SID_NUMBER_TWODEC:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_NUMBER, 4 );		// Standard+4 = #.##0,00
			rReq.Done();
			break;
		case SID_NUMBER_SCIENTIFIC:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_SCIENTIFIC );
			rReq.Done();
			break;
		case SID_NUMBER_DATE:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_DATE );
			rReq.Done();
			break;
		case SID_NUMBER_TIME:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_TIME );
			rReq.Done();
			break;
		case SID_NUMBER_CURRENCY:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_CURRENCY );
			rReq.Done();
			break;
		case SID_NUMBER_PERCENT:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_PERCENT );
			rReq.Done();
			break;
		case SID_NUMBER_STANDARD:
			pTabViewShell->SetNumberFormat( NUMBERFORMAT_NUMBER );
			rReq.Done();
			break;
		case SID_NUMBER_INCDEC:
			pTabViewShell->ChangeNumFmtDecimals( sal_True );
			rReq.Done();
			break;
		case SID_NUMBER_DECDEC:
			pTabViewShell->ChangeNumFmtDecimals( sal_False );
			rReq.Done();
			break;

		case SID_NUMBER_FORMAT:
			if ( pReqArgs )
			{
				const SfxPoolItem* pItem;
				if(pReqArgs->GetItemState(nSlot, sal_True, &pItem) == SFX_ITEM_SET)
				{
					String aCode = ((const SfxStringItem*)pItem)->GetValue();
					pTabViewShell->SetNumFmtByStr( aCode );
				}
			}
			break;

        case SID_ATTR_NUMBERFORMAT_VALUE:
			if ( pReqArgs )
			{
				const SfxPoolItem* pItem;
				if ( pReqArgs->GetItemState( ATTR_VALUE_FORMAT, sal_True, &pItem ) == SFX_ITEM_SET )
				{
                    // We have to accomplish this using ApplyAttributes()
                    // because we also need the language information to be
                    // considered.
                    const SfxItemSet& rOldSet =
                        pTabViewShell->GetSelectionPattern()->GetItemSet();
                    SfxItemPool* pDocPool = GetViewData()->GetDocument()->GetPool();
                    SfxItemSet aNewSet( *pDocPool, ATTR_PATTERN_START, ATTR_PATTERN_END );
                    aNewSet.Put( *pItem );
                    pTabViewShell->ApplyAttributes( &aNewSet, &rOldSet, sal_True );
				}
			}
			break;

		default:
			DBG_ERROR("falscher Slot bei ExecuteEdit");
			break;
	}
}


//------------------------------------------------------------------

#define APPLY_HOR_JUSTIFY(j) \
	{																		\
		if ( !pHorJustify || (eHorJustify != (j) ) )                        \
			pTabViewShell->ApplyAttr( SvxHorJustifyItem( (j) ) );                          \
		else                                                                \
			pTabViewShell->ApplyAttr( SvxHorJustifyItem( SVX_HOR_JUSTIFY_STANDARD ) );     \
	}

#define APPLY_VER_JUSTIFY(j) \
	{																		\
		if ( !pVerJustify || (eVerJustify != (j) ) )                        \
			pTabViewShell->ApplyAttr( SvxVerJustifyItem( (j) ) );                          \
		else                                                                \
			pTabViewShell->ApplyAttr( SvxVerJustifyItem( SVX_VER_JUSTIFY_STANDARD ) );     \
	}

void ScFormatShell::ExecuteAlignment( SfxRequest& rReq )
{
	ScTabViewShell*	pTabViewShell  		= GetViewData()->GetViewShell();
	SfxBindings&			rBindings	= pViewData->GetBindings();
	const SfxItemSet*       pSet		= rReq.GetArgs();
	sal_uInt16					nSlot		= rReq.GetSlot();

	pTabViewShell->HideListBox();	// Autofilter-DropDown-Listbox

    switch( nSlot )
    {
        // pseudo slots for Format menu
        case SID_ALIGN_ANY_HDEFAULT:
        case SID_ALIGN_ANY_LEFT:
        case SID_ALIGN_ANY_HCENTER:
        case SID_ALIGN_ANY_RIGHT:
        case SID_ALIGN_ANY_JUSTIFIED:
            pTabViewShell->ApplyAttr( SvxHorJustifyItem( lclConvertSlotToHAlign( nSlot ), ATTR_HOR_JUSTIFY ) );
        break;
        case SID_ALIGN_ANY_VDEFAULT:
        case SID_ALIGN_ANY_TOP:
        case SID_ALIGN_ANY_VCENTER:
        case SID_ALIGN_ANY_BOTTOM:
            pTabViewShell->ApplyAttr( SvxVerJustifyItem( lclConvertSlotToVAlign( nSlot ), ATTR_VER_JUSTIFY ) );
        break;

        default:
            if( pSet )
            {
                const SfxPoolItem* pItem = NULL;
                if( pSet->GetItemState(GetPool().GetWhich(nSlot), sal_True, &pItem  ) == SFX_ITEM_SET )
                {

                    switch ( nSlot )
                    {
                        case SID_ATTR_ALIGN_HOR_JUSTIFY:
                        case SID_ATTR_ALIGN_VER_JUSTIFY:
                        case SID_ATTR_ALIGN_INDENT:
                        case SID_ATTR_ALIGN_HYPHENATION:
                        case SID_ATTR_ALIGN_DEGREES:
                        case SID_ATTR_ALIGN_LOCKPOS:
                        case SID_ATTR_ALIGN_MARGIN:
                        case SID_ATTR_ALIGN_STACKED:
                            pTabViewShell->ApplyAttr( *pItem );
                        break;

                        case SID_H_ALIGNCELL:
                        {
                            SvxCellHorJustify eJust = (SvxCellHorJustify)((const SvxHorJustifyItem*)pItem)->GetValue();
                            // #i78476# update alignment of text in cell edit mode
                            pTabViewShell->UpdateInputHandlerCellAdjust( eJust );
                            pTabViewShell->ApplyAttr( SvxHorJustifyItem( eJust, ATTR_HOR_JUSTIFY ) );
                        }
                        break;
                        case SID_V_ALIGNCELL:
                            pTabViewShell->ApplyAttr( SvxVerJustifyItem( (SvxCellVerJustify)((const SvxVerJustifyItem*)pItem)->GetValue(), ATTR_VER_JUSTIFY ) );
                        break;
                        default:
                            DBG_ERROR( "ExecuteAlignment: invalid slot" );
                            return;
                    }
                }
            }
    }

    rBindings.Invalidate( SID_ALIGNLEFT );
    rBindings.Invalidate( SID_ALIGNRIGHT );
    rBindings.Invalidate( SID_ALIGNCENTERHOR );
    rBindings.Invalidate( SID_ALIGNBLOCK );
    rBindings.Invalidate( SID_ALIGNTOP );
    rBindings.Invalidate( SID_ALIGNBOTTOM );
    rBindings.Invalidate( SID_ALIGNCENTERVER );
    rBindings.Invalidate( SID_V_ALIGNCELL );
    rBindings.Invalidate( SID_H_ALIGNCELL );
    // pseudo slots for Format menu
    rBindings.Invalidate( SID_ALIGN_ANY_HDEFAULT );
    rBindings.Invalidate( SID_ALIGN_ANY_LEFT );
    rBindings.Invalidate( SID_ALIGN_ANY_HCENTER );
    rBindings.Invalidate( SID_ALIGN_ANY_RIGHT );
    rBindings.Invalidate( SID_ALIGN_ANY_JUSTIFIED );
    rBindings.Invalidate( SID_ALIGN_ANY_VDEFAULT );
    rBindings.Invalidate( SID_ALIGN_ANY_TOP );
    rBindings.Invalidate( SID_ALIGN_ANY_VCENTER );
    rBindings.Invalidate( SID_ALIGN_ANY_BOTTOM );
    rBindings.Update();

    if( ! rReq.IsAPI() )
        rReq.Done();
}

void ScFormatShell::ExecuteTextAttr( SfxRequest& rReq )
{
	ScTabViewShell*	pTabViewShell  		= GetViewData()->GetViewShell();
	SfxBindings&			rBindings	= pViewData->GetBindings();
	const ScPatternAttr*    pAttrs		= pTabViewShell->GetSelectionPattern();
	const SfxItemSet*       pSet		= rReq.GetArgs();
	sal_uInt16					nSlot		= rReq.GetSlot();
	SfxAllItemSet*			pNewSet = 0;

	pTabViewShell->HideListBox();					// Autofilter-DropDown-Listbox

	if (  (nSlot == SID_ATTR_CHAR_WEIGHT)
		||(nSlot == SID_ATTR_CHAR_POSTURE)
		||(nSlot == SID_ATTR_CHAR_UNDERLINE)
		||(nSlot == SID_ULINE_VAL_NONE)
		||(nSlot == SID_ULINE_VAL_SINGLE)
		||(nSlot == SID_ULINE_VAL_DOUBLE)
		||(nSlot == SID_ULINE_VAL_DOTTED) )
	{
		pNewSet = new SfxAllItemSet( GetPool() );

		switch ( nSlot )
		{
			case SID_ATTR_CHAR_WEIGHT:
			{
				// #i78017 establish the same behaviour as in Writer
				sal_uInt8 nScript = SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN | SCRIPTTYPE_COMPLEX;

				SfxItemPool& rPool = GetPool();
				SvxScriptSetItem aSetItem( nSlot, rPool );
				if ( pSet )
					aSetItem.PutItemForScriptType( nScript, pSet->Get( ATTR_FONT_WEIGHT ) );
				else
				{
					//	toggle manually

					FontWeight eWeight = WEIGHT_BOLD;
					SvxScriptSetItem aOldSetItem( nSlot, rPool );
					aOldSetItem.GetItemSet().Put( pAttrs->GetItemSet(), sal_False );
					const SfxPoolItem* pCore = aOldSetItem.GetItemOfScript( nScript );
					if ( pCore && ((const SvxWeightItem*)pCore)->GetWeight() == WEIGHT_BOLD )
						eWeight = WEIGHT_NORMAL;

                    aSetItem.PutItemForScriptType( nScript, SvxWeightItem( eWeight, ATTR_FONT_WEIGHT ) );
				}
				pTabViewShell->ApplyUserItemSet( aSetItem.GetItemSet() );
				pNewSet->Put( aSetItem.GetItemSet(), sal_False );
			}
			break;

			case SID_ATTR_CHAR_POSTURE:
			{
				// #i78017 establish the same behaviour as in Writer
				sal_uInt8 nScript = SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN | SCRIPTTYPE_COMPLEX;

				SfxItemPool& rPool = GetPool();
				SvxScriptSetItem aSetItem( nSlot, rPool );
				if ( pSet )
					aSetItem.PutItemForScriptType( nScript, pSet->Get( ATTR_FONT_POSTURE ) );
				else
				{
					//	toggle manually

					FontItalic eItalic = ITALIC_NORMAL;
					SvxScriptSetItem aOldSetItem( nSlot, rPool );
					aOldSetItem.GetItemSet().Put( pAttrs->GetItemSet(), sal_False );
					const SfxPoolItem* pCore = aOldSetItem.GetItemOfScript( nScript );
					if ( pCore && ((const SvxPostureItem*)pCore)->GetPosture() == ITALIC_NORMAL )
						eItalic = ITALIC_NONE;

                    aSetItem.PutItemForScriptType( nScript, SvxPostureItem( eItalic, ATTR_FONT_POSTURE ) );
				}
				pTabViewShell->ApplyUserItemSet( aSetItem.GetItemSet() );
				pNewSet->Put( aSetItem.GetItemSet(), sal_False );
			}
			break;

			case SID_ATTR_CHAR_UNDERLINE:
				{
					FontUnderline       eUnderline;

					if( pSet )
					{
                        const SfxPoolItem& rUnderline = pSet->Get( ATTR_FONT_UNDERLINE );

						if( rUnderline.ISA(SvxUnderlineItem) )
						{
							pTabViewShell->ApplyAttr( rUnderline );
							pNewSet->Put( rUnderline,rUnderline.Which() );
						}
                        else if ( rUnderline.ISA(SvxTextLineItem) )
                        {
                            // #i106580# also allow SvxTextLineItem (base class of SvxUnderlineItem)
                            const SvxTextLineItem& rTextLineItem = static_cast<const SvxTextLineItem&>(rUnderline);
                            SvxUnderlineItem aNewItem( rTextLineItem.GetLineStyle(), rTextLineItem.Which() );
                            aNewItem.SetColor( rTextLineItem.GetColor() );
                            pTabViewShell->ApplyAttr( aNewItem );
                            pNewSet->Put( aNewItem, aNewItem.Which() );
                        }
					}
					else
					{
						SvxUnderlineItem aUnderline( (const SvxUnderlineItem&)
														pAttrs->GetItem(
															ATTR_FONT_UNDERLINE ) );
						eUnderline = (UNDERLINE_NONE != aUnderline.GetLineStyle())
									? UNDERLINE_NONE
									: UNDERLINE_SINGLE;
						aUnderline.SetLineStyle( eUnderline );
						pTabViewShell->ApplyAttr( aUnderline );
						pNewSet->Put( aUnderline,aUnderline.Which() );
					}
				}
				break;

			case SID_ULINE_VAL_NONE:
				pTabViewShell->ApplyAttr( SvxUnderlineItem( UNDERLINE_NONE, ATTR_FONT_UNDERLINE ) );
				break;
			case SID_ULINE_VAL_SINGLE:		// Toggles
			case SID_ULINE_VAL_DOUBLE:
			case SID_ULINE_VAL_DOTTED:
				{
					FontUnderline eOld = ((const SvxUnderlineItem&)
											pAttrs->GetItem(ATTR_FONT_UNDERLINE)).GetLineStyle();
					FontUnderline eNew = eOld;
					switch (nSlot)
					{
						case SID_ULINE_VAL_SINGLE:
							eNew = ( eOld == UNDERLINE_SINGLE ) ? UNDERLINE_NONE : UNDERLINE_SINGLE;
							break;
						case SID_ULINE_VAL_DOUBLE:
							eNew = ( eOld == UNDERLINE_DOUBLE ) ? UNDERLINE_NONE : UNDERLINE_DOUBLE;
							break;
						case SID_ULINE_VAL_DOTTED:
							eNew = ( eOld == UNDERLINE_DOTTED ) ? UNDERLINE_NONE : UNDERLINE_DOTTED;
							break;
					}
					pTabViewShell->ApplyAttr( SvxUnderlineItem( eNew, ATTR_FONT_UNDERLINE ) );
				}
				break;

			default:
				break;
		}
		rBindings.Invalidate( nSlot );
	}
	else
	{
		/*
		 * "Selbstgemachte" RadioButton-Funktionalitaet
		 * Beim Toggle gibt es den Standard-State, d.h. kein
		 * Button ist gedrueckt
		 */

		const SfxItemSet&		 rAttrSet	= pTabViewShell->GetSelectionPattern()->GetItemSet();
		const SfxPoolItem*       pItem       = NULL;
		const SvxHorJustifyItem* pHorJustify = NULL;
		const SvxVerJustifyItem* pVerJustify = NULL;
		SvxCellHorJustify        eHorJustify = SVX_HOR_JUSTIFY_STANDARD;
		SvxCellVerJustify        eVerJustify = SVX_VER_JUSTIFY_STANDARD;

		if (rAttrSet.GetItemState(ATTR_HOR_JUSTIFY, sal_True,&pItem ) == SFX_ITEM_SET)
		{
			pHorJustify = (const SvxHorJustifyItem*)pItem;
			eHorJustify = SvxCellHorJustify( pHorJustify->GetValue() );
		}
		if (rAttrSet.GetItemState(ATTR_VER_JUSTIFY, sal_True,&pItem ) == SFX_ITEM_SET)
		{
			pVerJustify = (const SvxVerJustifyItem*)pItem;
			eVerJustify = SvxCellVerJustify( pVerJustify->GetValue() );
		}

		switch ( nSlot )
		{
			case SID_ALIGNLEFT:
				rReq.SetSlot( SID_H_ALIGNCELL );
				rReq.AppendItem( SvxHorJustifyItem(
					!pHorJustify || (eHorJustify != SVX_HOR_JUSTIFY_LEFT) ?
					SVX_HOR_JUSTIFY_LEFT : SVX_HOR_JUSTIFY_STANDARD, SID_H_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_HOR_JUSTIFY( SVX_HOR_JUSTIFY_LEFT );
                //break;

			case SID_ALIGNRIGHT:
				rReq.SetSlot( SID_H_ALIGNCELL );
				rReq.AppendItem( SvxHorJustifyItem(
					!pHorJustify || (eHorJustify != SVX_HOR_JUSTIFY_RIGHT) ?
					SVX_HOR_JUSTIFY_RIGHT : SVX_HOR_JUSTIFY_STANDARD, SID_H_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_HOR_JUSTIFY( SVX_HOR_JUSTIFY_RIGHT );
                //break;

			case SID_ALIGNCENTERHOR:
				rReq.SetSlot( SID_H_ALIGNCELL );
				rReq.AppendItem( SvxHorJustifyItem(
					!pHorJustify || (eHorJustify != SVX_HOR_JUSTIFY_CENTER) ?
					SVX_HOR_JUSTIFY_CENTER : SVX_HOR_JUSTIFY_STANDARD, SID_H_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_HOR_JUSTIFY( SVX_HOR_JUSTIFY_CENTER );
                //break;

			case SID_ALIGNBLOCK:
				rReq.SetSlot( SID_H_ALIGNCELL );
				rReq.AppendItem( SvxHorJustifyItem(
					!pHorJustify || (eHorJustify != SVX_HOR_JUSTIFY_BLOCK) ?
					SVX_HOR_JUSTIFY_BLOCK : SVX_HOR_JUSTIFY_STANDARD, SID_H_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_HOR_JUSTIFY( SVX_HOR_JUSTIFY_BLOCK );
                //break;

			case SID_ALIGNTOP:
				rReq.SetSlot( SID_V_ALIGNCELL );
				rReq.AppendItem( SvxVerJustifyItem(
					!pVerJustify || (eVerJustify != SVX_VER_JUSTIFY_TOP) ?
					SVX_VER_JUSTIFY_TOP : SVX_VER_JUSTIFY_STANDARD, SID_V_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_VER_JUSTIFY( SVX_VER_JUSTIFY_TOP );
                //break;

			case SID_ALIGNBOTTOM:
				rReq.SetSlot( SID_V_ALIGNCELL );
				rReq.AppendItem( SvxVerJustifyItem(
					!pVerJustify || (eVerJustify != SVX_VER_JUSTIFY_BOTTOM) ?
					SVX_VER_JUSTIFY_BOTTOM : SVX_VER_JUSTIFY_STANDARD, SID_V_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_VER_JUSTIFY( SVX_VER_JUSTIFY_BOTTOM );
                //break;

			case SID_ALIGNCENTERVER:
				rReq.SetSlot( SID_V_ALIGNCELL );
				rReq.AppendItem( SvxVerJustifyItem(
					!pVerJustify || (eVerJustify != SVX_VER_JUSTIFY_CENTER) ?
					SVX_VER_JUSTIFY_CENTER : SVX_VER_JUSTIFY_STANDARD, SID_V_ALIGNCELL ) );
				ExecuteSlot( rReq, GetInterface() );
				return;
//				APPLY_VER_JUSTIFY( SVX_VER_JUSTIFY_CENTER );
                //break;

			default:
			break;
		}

	}

	rBindings.Update();
//	rReq.Done();

	if( pNewSet )
	{
		rReq.Done( *pNewSet );
		delete pNewSet;
	}
	else
	{
		rReq.Done();
	}

}

#undef APPLY_HOR_JUSTIFY
#undef APPLY_VER_JUSTIFY

//------------------------------------------------------------------

void ScFormatShell::ExecuteAttr( SfxRequest& rReq )
{
	ScTabViewShell*	pTabViewShell  		= GetViewData()->GetViewShell();
	SfxBindings&		rBindings = pViewData->GetBindings();
	const SfxItemSet*	pNewAttrs = rReq.GetArgs();

	pTabViewShell->HideListBox();					// Autofilter-DropDown-Listbox

	if ( !pNewAttrs )
	{
		sal_uInt16 nSlot = rReq.GetSlot();

		switch ( nSlot )
		{
			case SID_ATTR_CHAR_FONT:
			case SID_ATTR_CHAR_FONTHEIGHT:
				pTabViewShell->ExecuteCellFormatDlg( rReq, TP_FONT );	    // wenn ToolBar vertikal
				break;

			case SID_ATTR_ALIGN_LINEBREAK:					// ohne Parameter als Toggle
				{
					const ScPatternAttr* pAttrs = pTabViewShell->GetSelectionPattern();
					sal_Bool bOld = ((const SfxBoolItem&)pAttrs->GetItem(ATTR_LINEBREAK)).GetValue();
					SfxBoolItem aBreakItem( ATTR_LINEBREAK, !bOld );
					pTabViewShell->ApplyAttr( aBreakItem );

					SfxAllItemSet aNewSet( GetPool() );
					aNewSet.Put( aBreakItem,aBreakItem.Which() );
					rReq.Done( aNewSet );

					rBindings.Invalidate( nSlot );
				}
				break;

			case SID_BACKGROUND_COLOR:
				{
					//	SID_BACKGROUND_COLOR without arguments -> set transparent background

					SvxBrushItem        aBrushItem( (const SvxBrushItem&)
											pTabViewShell->GetSelectionPattern()->
												GetItem( ATTR_BACKGROUND ) );

					aBrushItem.SetColor( COL_TRANSPARENT );

					pTabViewShell->ApplyAttr( aBrushItem );
				}
				break;
		}
	}
	else
	{
		sal_uInt16 nSlot = rReq.GetSlot();

		switch ( nSlot )
		{
            case SID_ATTR_CHAR_OVERLINE:
            case SID_ATTR_CHAR_STRIKEOUT:
			case SID_ATTR_ALIGN_LINEBREAK:
			case SID_ATTR_CHAR_COLOR:
            case SID_ATTR_CHAR_CONTOUR:
            case SID_ATTR_CHAR_SHADOWED:
            case SID_ATTR_CHAR_RELIEF:
            case SID_SCATTR_PROTECTION :
				pTabViewShell->ApplyAttr( pNewAttrs->Get( pNewAttrs->GetPool()->GetWhich( nSlot ) ) );
				rBindings.Invalidate( nSlot );
				rBindings.Update( nSlot );
				break;

			case SID_ATTR_CHAR_FONT:
			case SID_ATTR_CHAR_FONTHEIGHT:
				{
					// #i78017 establish the same behaviour as in Writer
					sal_uInt8 nScript = SCRIPTTYPE_LATIN | SCRIPTTYPE_ASIAN | SCRIPTTYPE_COMPLEX;
					if (nSlot == SID_ATTR_CHAR_FONT)
						nScript = pTabViewShell->GetSelectionScriptType();

					SfxItemPool& rPool = GetPool();
					SvxScriptSetItem aSetItem( nSlot, rPool );
					sal_uInt16 nWhich = rPool.GetWhich( nSlot );
					aSetItem.PutItemForScriptType( nScript, pNewAttrs->Get( nWhich ) );

					pTabViewShell->ApplyUserItemSet( aSetItem.GetItemSet() );

					rBindings.Invalidate( nSlot );
					rBindings.Update( nSlot );
				}
				break;

			case SID_FRAME_LINESTYLE:
				{
					// Default-Linie aktualisieren
					const SvxBorderLine* pLine =
							((const SvxLineItem&)
								pNewAttrs->Get( SID_FRAME_LINESTYLE )).
								GetLine();

					if ( pLine )
					{
						SvxBorderLine* pDefLine = pTabViewShell->GetDefaultFrameLine();

						if ( pDefLine )
						{
							pDefLine->SetOutWidth( pLine->GetOutWidth() );
							pDefLine->SetInWidth ( pLine->GetInWidth() );
							pDefLine->SetDistance( pLine->GetDistance() );
							pTabViewShell->SetSelectionFrameLines( pDefLine, sal_False );
						}
						else
						{
							pTabViewShell->SetDefaultFrameLine( pLine );
							pTabViewShell->GetDefaultFrameLine()->SetColor( COL_BLACK );
							pTabViewShell->SetSelectionFrameLines( pLine, sal_False );
						}
					}
					else
					{
						Color           aColorBlack( COL_BLACK );
						SvxBorderLine   aDefLine( &aColorBlack, 20, 0, 0 );
						pTabViewShell->SetDefaultFrameLine( &aDefLine );
						pTabViewShell->SetSelectionFrameLines( NULL, sal_False );
					}
				}
				break;

			case SID_FRAME_LINECOLOR:
				{
					SvxBorderLine*  pDefLine = pTabViewShell->GetDefaultFrameLine();
					const Color&    rColor = ((const SvxColorItem&)
										pNewAttrs->Get( SID_FRAME_LINECOLOR )).
											GetValue();

					// Default-Linie aktualisieren
					if ( pDefLine )
					{
						pDefLine->SetColor( rColor );
						pTabViewShell->SetSelectionFrameLines( pDefLine, sal_True );
					}
					else
					{
						SvxBorderLine   aDefLine( &rColor, 20, 0, 0 );
						pTabViewShell->SetDefaultFrameLine( &aDefLine );
						pTabViewShell->SetSelectionFrameLines( &aDefLine, sal_False );
					}
				}
				break;

			case SID_ATTR_BORDER_OUTER:
			case SID_ATTR_BORDER:
				{
					SvxBorderLine*          pDefLine = pTabViewShell->GetDefaultFrameLine();
					const ScPatternAttr*    pOldAttrs = pTabViewShell->GetSelectionPattern();
					ScDocument*             pDoc = GetViewData()->GetDocument();
					SfxItemSet*             pOldSet =
												new SfxItemSet(
														*(pDoc->GetPool()),
														ATTR_PATTERN_START,
														ATTR_PATTERN_END );
					SfxItemSet*             pNewSet =
												new SfxItemSet(
														*(pDoc->GetPool()),
														ATTR_PATTERN_START,
														ATTR_PATTERN_END );
					const SfxPoolItem&      rBorderAttr =
												pOldAttrs->GetItemSet().
													Get( ATTR_BORDER );

					// Border-Items vom Controller auswerten:
					const SfxPoolItem* pItem = 0;

					if ( pNewAttrs->GetItemState( ATTR_BORDER, sal_True, &pItem )
						 == SFX_ITEM_SET )
					{
						//	#100959# The SvxFrameToolBoxControl toolbox controller uses a default
						//	SvxBorderLine (all widths 0) to mark the lines that should be set.
						//	Macro recording uses a SvxBoxItem with the real values (OutWidth > 0)
						//	or NULL pointers for no lines.
						//	-> Substitute existing lines with pDefLine only if widths are 0.
						SvxBoxItem aBoxItem ( *(const SvxBoxItem*)pItem );
						if ( aBoxItem.GetTop() && aBoxItem.GetTop()->GetOutWidth() == 0 )
							aBoxItem.SetLine( pDefLine, BOX_LINE_TOP );
						if ( aBoxItem.GetBottom() && aBoxItem.GetBottom()->GetOutWidth() == 0 )
							aBoxItem.SetLine( pDefLine, BOX_LINE_BOTTOM );
						if ( aBoxItem.GetLeft() && aBoxItem.GetLeft()->GetOutWidth() == 0 )
							aBoxItem.SetLine( pDefLine, BOX_LINE_LEFT );
						if ( aBoxItem.GetRight() && aBoxItem.GetRight()->GetOutWidth() == 0 )
							aBoxItem.SetLine( pDefLine, BOX_LINE_RIGHT );
						pNewSet->Put( aBoxItem );
                        rReq.AppendItem( aBoxItem );
					}

					if ( pNewAttrs->GetItemState( ATTR_BORDER_INNER, sal_True, &pItem )
						 == SFX_ITEM_SET )
					{
						SvxBoxInfoItem aBoxInfoItem( *(const SvxBoxInfoItem*)pItem );
						if ( aBoxInfoItem.GetHori() && aBoxInfoItem.GetHori()->GetOutWidth() == 0 )
							aBoxInfoItem.SetLine( pDefLine, BOXINFO_LINE_HORI );
						if ( aBoxInfoItem.GetVert() && aBoxInfoItem.GetVert()->GetOutWidth() == 0 )
							aBoxInfoItem.SetLine( pDefLine, BOXINFO_LINE_VERT );
						pNewSet->Put( aBoxInfoItem );
                        rReq.AppendItem( aBoxInfoItem );
					}
					else
					{
						SvxBoxInfoItem aBoxInfoItem( ATTR_BORDER_INNER );
						aBoxInfoItem.SetLine( NULL, BOXINFO_LINE_HORI );
						aBoxInfoItem.SetLine( NULL, BOXINFO_LINE_VERT );
						pNewSet->Put( aBoxInfoItem );
					}

					pOldSet->Put( rBorderAttr );
					pTabViewShell->ApplyAttributes( pNewSet, pOldSet );

					delete pOldSet;
					delete pNewSet;
				}
				break;

			// ATTR_BACKGROUND (=SID_ATTR_BRUSH) muss ueber zwei IDs
			// gesetzt werden:
			case SID_BACKGROUND_COLOR:
				{
					const SvxColorItem  rNewColorItem = (const SvxColorItem&)
											pNewAttrs->Get( SID_BACKGROUND_COLOR );

					SvxBrushItem        aBrushItem( (const SvxBrushItem&)
											pTabViewShell->GetSelectionPattern()->
												GetItem( ATTR_BACKGROUND ) );

					aBrushItem.SetColor( rNewColorItem.GetValue() );

					pTabViewShell->ApplyAttr( aBrushItem );
				}
				break;

				case SID_ATTR_BRUSH:
				{
					SvxBrushItem        aBrushItem( (const SvxBrushItem&)
											pTabViewShell->GetSelectionPattern()->
												GetItem( ATTR_BACKGROUND ) );
					const SvxBrushItem& rNewBrushItem = (const SvxBrushItem&)
											pNewAttrs->Get( GetPool().GetWhich(nSlot) );
					aBrushItem.SetColor(rNewBrushItem.GetColor());
					pTabViewShell->ApplyAttr( aBrushItem );
				}
				break;

			case SID_ATTR_BORDER_SHADOW:
				{
					const SvxShadowItem& rNewShadowItem = (const SvxShadowItem&)
											pNewAttrs->Get( ATTR_SHADOW );
					pTabViewShell->ApplyAttr( rNewShadowItem );
				}
				break;

			default:
			break;
		}

		if( ! rReq.IsAPI() )
			if( ! rReq.IsDone() )
				rReq.Done();
	}
}

void ScFormatShell::GetAttrState( SfxItemSet& rSet )
{
	ScTabViewShell*	pTabViewShell  	= GetViewData()->GetViewShell();
	const SfxItemSet&	 rAttrSet	= pTabViewShell->GetSelectionPattern()->GetItemSet();
	const SvxBorderLine* pLine      = pTabViewShell->GetDefaultFrameLine();
	const SvxBrushItem&  rBrushItem = (const SvxBrushItem&)rAttrSet.Get( ATTR_BACKGROUND );
	SfxWhichIter aIter( rSet );
	sal_uInt16 nWhich = aIter.FirstWhich();

	rSet.Put( rAttrSet, sal_False );

	//	choose font info according to selection script type
	sal_uInt8 nScript = 0;		// GetSelectionScriptType never returns 0
	if ( rSet.GetItemState( ATTR_FONT ) != SFX_ITEM_UNKNOWN )
	{
		if (!nScript) nScript = pTabViewShell->GetSelectionScriptType();
		ScViewUtil::PutItemScript( rSet, rAttrSet, ATTR_FONT, nScript );
	}
	if ( rSet.GetItemState( ATTR_FONT_HEIGHT ) != SFX_ITEM_UNKNOWN )
	{
		if (!nScript) nScript = pTabViewShell->GetSelectionScriptType();
		ScViewUtil::PutItemScript( rSet, rAttrSet, ATTR_FONT_HEIGHT, nScript );
	}

	while ( nWhich )
	{
		switch(nWhich)
		{
			case SID_BACKGROUND_COLOR:
			{
                rSet.Put( SvxColorItem( rBrushItem.GetColor(), SID_BACKGROUND_COLOR ) );
			}
			break;
			case SID_FRAME_LINECOLOR:
			{
                rSet.Put( SvxColorItem( pLine ? pLine->GetColor() : Color(), SID_FRAME_LINECOLOR ) );
			}
			break;
			case SID_ATTR_BRUSH:
			{
				rSet.Put( rBrushItem, GetPool().GetWhich(nWhich) );
			}
			break;
/*			case SID_ATTR_ALIGN_LINEBREAK:
			{
				const SfxBoolItem& rBreakItem = (const SfxBoolItem&)rAttrSet.Get( ATTR_LINEBREAK );
				rSet.Put( rBreakItem, GetPool().GetWhich(nWhich) );
			}
			break;
*/
		}
		nWhich = aIter.NextWhich();
	}
}

//------------------------------------------------------------------

void ScFormatShell::GetTextAttrState( SfxItemSet& rSet )
{
	ScTabViewShell*	pTabViewShell  	= GetViewData()->GetViewShell();
	const SfxItemSet& rAttrSet	= pTabViewShell->GetSelectionPattern()->GetItemSet();
	rSet.Put( rAttrSet, sal_False ); // ItemStates mitkopieren

	//	choose font info according to selection script type
	sal_uInt8 nScript = 0;		// GetSelectionScriptType never returns 0
	if ( rSet.GetItemState( ATTR_FONT_WEIGHT ) != SFX_ITEM_UNKNOWN )
	{
		if (!nScript) nScript = pTabViewShell->GetSelectionScriptType();
		ScViewUtil::PutItemScript( rSet, rAttrSet, ATTR_FONT_WEIGHT, nScript );
	}
	if ( rSet.GetItemState( ATTR_FONT_POSTURE ) != SFX_ITEM_UNKNOWN )
	{
		if (!nScript) nScript = pTabViewShell->GetSelectionScriptType();
		ScViewUtil::PutItemScript( rSet, rAttrSet, ATTR_FONT_POSTURE, nScript );
	}

	SfxItemState eState;
//	const SfxPoolItem* pItem;

	//--------------------------------------------------------------------
	// eigene Kontrolle ueber RadioButton-Funktionalitaet:
	//--------------------------------------------------------------------
	// Unterstreichung
	//------------------------

	eState = rAttrSet.GetItemState( ATTR_FONT_UNDERLINE, sal_True );
	if ( eState == SFX_ITEM_DONTCARE )
	{
		rSet.InvalidateItem( SID_ULINE_VAL_NONE );
		rSet.InvalidateItem( SID_ULINE_VAL_SINGLE );
		rSet.InvalidateItem( SID_ULINE_VAL_DOUBLE );
		rSet.InvalidateItem( SID_ULINE_VAL_DOTTED );
	}
	else
	{
		FontUnderline eUnderline = ((const SvxUnderlineItem&)
					rAttrSet.Get(ATTR_FONT_UNDERLINE)).GetLineStyle();
		sal_uInt16 nId = SID_ULINE_VAL_NONE;
		switch (eUnderline)
		{
			case UNDERLINE_SINGLE:	nId = SID_ULINE_VAL_SINGLE;	break;
			case UNDERLINE_DOUBLE:	nId = SID_ULINE_VAL_DOUBLE;	break;
			case UNDERLINE_DOTTED:	nId = SID_ULINE_VAL_DOTTED;	break;
			default:
				break;
		}
		rSet.Put( SfxBoolItem( nId, sal_True ) );
	}

	//------------------------
	// horizontale Ausrichtung
	//------------------------

	const SvxHorJustifyItem* pHorJustify = NULL;
	const SvxVerJustifyItem* pVerJustify = NULL;
	SvxCellHorJustify        eHorJustify = SVX_HOR_JUSTIFY_STANDARD;
	SvxCellVerJustify        eVerJustify = SVX_VER_JUSTIFY_STANDARD;
	sal_uInt16		 			 nWhich		 = 0;
	sal_Bool					 bJustifyStd = sal_False;
	SfxBoolItem				 aBoolItem	 ( 0, sal_True );

	eState 	 = rAttrSet.GetItemState( ATTR_HOR_JUSTIFY, sal_True,
										(const SfxPoolItem**)&pHorJustify );
	switch ( eState )
	{
		case SFX_ITEM_SET:
			{
				eHorJustify = SvxCellHorJustify( pHorJustify->GetValue() );

				switch ( SvxCellHorJustify( pHorJustify->GetValue() ) )
				{
					case SVX_HOR_JUSTIFY_STANDARD:
						break;

					case SVX_HOR_JUSTIFY_LEFT:
						nWhich = SID_ALIGNLEFT;
						break;

					case SVX_HOR_JUSTIFY_RIGHT:
						nWhich = SID_ALIGNRIGHT;
						break;

					case SVX_HOR_JUSTIFY_CENTER:
						nWhich = SID_ALIGNCENTERHOR;
						break;

					case SVX_HOR_JUSTIFY_BLOCK:
						nWhich = SID_ALIGNBLOCK;
						break;

					case SVX_HOR_JUSTIFY_REPEAT:
					default:
						bJustifyStd = sal_True;
						break;
				}
			}
			break;

		case SFX_ITEM_DONTCARE:
			rSet.InvalidateItem( SID_ALIGNLEFT );
			rSet.InvalidateItem( SID_ALIGNRIGHT );
			rSet.InvalidateItem( SID_ALIGNCENTERHOR );
			rSet.InvalidateItem( SID_ALIGNBLOCK );
			break;

		default:
			bJustifyStd = sal_True;
			break;
	}

	if ( nWhich )
	{
		aBoolItem.SetWhich( nWhich );
		rSet.Put( aBoolItem );
	}
	else if ( bJustifyStd )
	{
		aBoolItem.SetValue( sal_False );
		aBoolItem.SetWhich( SID_ALIGNLEFT );	  rSet.Put( aBoolItem );
		aBoolItem.SetWhich( SID_ALIGNRIGHT );	  rSet.Put( aBoolItem );
		aBoolItem.SetWhich( SID_ALIGNCENTERHOR ); rSet.Put( aBoolItem );
		aBoolItem.SetWhich( SID_ALIGNBLOCK );	  rSet.Put( aBoolItem );
		bJustifyStd = sal_False;
	}

	//------------------------
	// vertikale Ausrichtung
	//------------------------

	nWhich = 0;
	aBoolItem.SetValue( sal_True );

	eState = rAttrSet.GetItemState( ATTR_VER_JUSTIFY, sal_True,
									(const SfxPoolItem**)&pVerJustify );

	switch ( eState )
	{
		case SFX_ITEM_SET:
			{
				eVerJustify = SvxCellVerJustify( pVerJustify->GetValue() );

				switch ( eVerJustify )
				{
					case SVX_VER_JUSTIFY_TOP:
						nWhich = SID_ALIGNTOP;
						break;

					case SVX_VER_JUSTIFY_BOTTOM:
						nWhich = SID_ALIGNBOTTOM;
						break;

					case SVX_VER_JUSTIFY_CENTER:
						nWhich = SID_ALIGNCENTERVER;
						break;

					case SVX_VER_JUSTIFY_STANDARD:
					default:
						bJustifyStd = sal_True;
						break;
				}
			}
			break;

		case SFX_ITEM_DONTCARE:
			rSet.InvalidateItem( SID_ALIGNTOP );
			rSet.InvalidateItem( SID_ALIGNBOTTOM );
			rSet.InvalidateItem( SID_ALIGNCENTERVER );
			break;

		default:
			bJustifyStd = sal_True;
			break;
	}

	if ( nWhich )
	{
		aBoolItem.SetWhich( nWhich );
		rSet.Put( aBoolItem );
	}
	else if ( bJustifyStd )
	{
		aBoolItem.SetValue( sal_False );
		aBoolItem.SetWhich( SID_ALIGNTOP );	  	  rSet.Put( aBoolItem );
		aBoolItem.SetWhich( SID_ALIGNBOTTOM );	  rSet.Put( aBoolItem );
		aBoolItem.SetWhich( SID_ALIGNCENTERVER ); rSet.Put( aBoolItem );
	}
}


//------------------------------------------------------------------

void ScFormatShell::GetBorderState( SfxItemSet& rSet )
{
	ScTabViewShell*	pTabViewShell  	= GetViewData()->GetViewShell();
	SvxBoxItem		aBoxItem( ATTR_BORDER );
	SvxBoxInfoItem	aInfoItem( ATTR_BORDER_INNER );

	pTabViewShell->GetSelectionFrame( aBoxItem, aInfoItem );

	if ( rSet.GetItemState( ATTR_BORDER ) != SFX_ITEM_UNKNOWN )
		rSet.Put( aBoxItem );
	if ( rSet.GetItemState( ATTR_BORDER_INNER ) != SFX_ITEM_UNKNOWN )
		rSet.Put( aInfoItem );
}

//------------------------------------------------------------------

void ScFormatShell::GetAlignState( SfxItemSet& rSet )
{
	ScTabViewShell*	pTabViewShell  	= GetViewData()->GetViewShell();
	const SfxItemSet& rAttrSet    = pTabViewShell->GetSelectionPattern()->GetItemSet();
	SfxWhichIter	aIter(rSet);
	sal_uInt16			nWhich = aIter.FirstWhich();

    SvxCellHorJustify eHAlign = SVX_HOR_JUSTIFY_STANDARD;
    bool bHasHAlign = rAttrSet.GetItemState( ATTR_HOR_JUSTIFY ) != SFX_ITEM_DONTCARE;
    if( bHasHAlign )
        eHAlign = (SvxCellHorJustify)((const SvxHorJustifyItem&) rAttrSet.Get( ATTR_HOR_JUSTIFY )).GetValue();

    SvxCellVerJustify eVAlign = SVX_VER_JUSTIFY_STANDARD;
    bool bHasVAlign = rAttrSet.GetItemState( ATTR_VER_JUSTIFY ) != SFX_ITEM_DONTCARE;
    if( bHasVAlign )
        eVAlign = (SvxCellVerJustify)((const SvxVerJustifyItem&) rAttrSet.Get( ATTR_VER_JUSTIFY )).GetValue();

	while ( nWhich )
	{
		switch ( nWhich )
		{
			case SID_H_ALIGNCELL:
                if ( bHasHAlign )
                    rSet.Put( SvxHorJustifyItem( eHAlign, nWhich ));
			break;
			case SID_V_ALIGNCELL:
                if ( bHasVAlign )
                    rSet.Put( SvxVerJustifyItem( eVAlign, nWhich ));
			break;

            // pseudo slots for Format menu
            case SID_ALIGN_ANY_HDEFAULT:
            case SID_ALIGN_ANY_LEFT:
            case SID_ALIGN_ANY_HCENTER:
            case SID_ALIGN_ANY_RIGHT:
            case SID_ALIGN_ANY_JUSTIFIED:
                rSet.Put( SfxBoolItem( nWhich, bHasHAlign && (eHAlign == lclConvertSlotToHAlign( nWhich )) ) );
            break;
            case SID_ALIGN_ANY_VDEFAULT:
            case SID_ALIGN_ANY_TOP:
            case SID_ALIGN_ANY_VCENTER:
            case SID_ALIGN_ANY_BOTTOM:
                rSet.Put( SfxBoolItem( nWhich, bHasVAlign && (eVAlign == lclConvertSlotToVAlign( nWhich )) ) );
            break;
		}
		nWhich = aIter.NextWhich();
	}
}

void ScFormatShell::GetNumFormatState( SfxItemSet& rSet )
{
	ScTabViewShell*	pTabViewShell  	= GetViewData()->GetViewShell();

    // ScViewData* pViewData   = GetViewData();
	ScDocument* pDoc		= pViewData->GetDocument();

	SfxWhichIter aIter(rSet);
	sal_uInt16 nWhich = aIter.FirstWhich();
	while ( nWhich )
	{
		switch ( nWhich )
		{
			case SID_NUMBER_FORMAT:
				{
					String aFormatCode;			// bleibt leer, wenn dont-care

					const SfxItemSet& rAttrSet  = pTabViewShell->GetSelectionPattern()->GetItemSet();
					if ( rAttrSet.GetItemState( ATTR_VALUE_FORMAT ) != SFX_ITEM_DONTCARE )
					{
						sal_uLong nNumberFormat = ((const SfxUInt32Item&)rAttrSet.Get(
													ATTR_VALUE_FORMAT )).GetValue();

						SvNumberFormatter* pFormatter = pDoc->GetFormatTable();
						const SvNumberformat* pFormatEntry = pFormatter->GetEntry( nNumberFormat );
						if ( pFormatEntry )
							aFormatCode = pFormatEntry->GetFormatstring();
					}

					rSet.Put( SfxStringItem( nWhich, aFormatCode ) );
				}
				break;

		}
		nWhich = aIter.NextWhich();
	}
}


void ScFormatShell::ExecuteTextDirection( SfxRequest& rReq )
{
    ScTabViewShell* pTabViewShell = GetViewData()->GetViewShell();
    pTabViewShell->HideListBox();               // Autofilter-DropDown-Listbox
    if ( GetViewData()->HasEditView( GetViewData()->GetActivePart() ) )
    {
        SC_MOD()->InputEnterHandler();
        pTabViewShell->UpdateInputHandler();
    }

    sal_uInt16 nSlot = rReq.GetSlot();
    switch( nSlot )
    {
        case SID_TEXTDIRECTION_LEFT_TO_RIGHT:
        case SID_TEXTDIRECTION_TOP_TO_BOTTOM:
        {
            sal_Bool bVert = (nSlot == SID_TEXTDIRECTION_TOP_TO_BOTTOM);
            ScPatternAttr aAttr( GetViewData()->GetDocument()->GetPool() );
            SfxItemSet& rItemSet = aAttr.GetItemSet();
            rItemSet.Put( SfxBoolItem( ATTR_STACKED, bVert ) );
            rItemSet.Put( SfxBoolItem( ATTR_VERTICAL_ASIAN, bVert ) );
            pTabViewShell->ApplySelectionPattern( aAttr );
            pTabViewShell->AdjustBlockHeight();
        }
        break;

        case SID_ATTR_PARA_LEFT_TO_RIGHT:
        case SID_ATTR_PARA_RIGHT_TO_LEFT:
        {
            SvxFrameDirection eDirection = ( nSlot == SID_ATTR_PARA_LEFT_TO_RIGHT ) ?
                                                FRMDIR_HORI_LEFT_TOP : FRMDIR_HORI_RIGHT_TOP;
            pTabViewShell->ApplyAttr( SvxFrameDirectionItem( eDirection, ATTR_WRITINGDIR ) );
        }
        break;
    }
}

void ScFormatShell::GetTextDirectionState( SfxItemSet& rSet )
{
    ScTabViewShell* pTabViewShell = GetViewData()->GetViewShell();
    const SfxItemSet& rAttrSet = pTabViewShell->GetSelectionPattern()->GetItemSet();

    sal_Bool bVertDontCare =
        (rAttrSet.GetItemState( ATTR_VERTICAL_ASIAN ) == SFX_ITEM_DONTCARE) ||
        (rAttrSet.GetItemState( ATTR_STACKED ) == SFX_ITEM_DONTCARE);
    sal_Bool bLeftRight = !bVertDontCare &&
        !((const SfxBoolItem&) rAttrSet.Get( ATTR_STACKED )).GetValue();
    sal_Bool bTopBottom = !bVertDontCare && !bLeftRight &&
        ((const SfxBoolItem&) rAttrSet.Get( ATTR_VERTICAL_ASIAN )).GetValue();

    sal_Bool bBidiDontCare = (rAttrSet.GetItemState( ATTR_WRITINGDIR ) == SFX_ITEM_DONTCARE);
    EEHorizontalTextDirection eBidiDir = EE_HTEXTDIR_DEFAULT;
    if ( !bBidiDontCare )
    {
        SvxFrameDirection eCellDir = (SvxFrameDirection)((const SvxFrameDirectionItem&)
                                        rAttrSet.Get( ATTR_WRITINGDIR )).GetValue();
        if ( eCellDir == FRMDIR_ENVIRONMENT )
            eBidiDir = (EEHorizontalTextDirection)GetViewData()->GetDocument()->
                                GetEditTextDirection( GetViewData()->GetTabNo() );
        else if ( eCellDir == FRMDIR_HORI_RIGHT_TOP )
            eBidiDir = EE_HTEXTDIR_R2L;
        else
            eBidiDir = EE_HTEXTDIR_L2R;
    }

	SvtLanguageOptions	aLangOpt;
	sal_Bool bDisableCTLFont = !aLangOpt.IsCTLFontEnabled();
	sal_Bool bDisableVerticalText = !aLangOpt.IsVerticalTextEnabled();

    SfxWhichIter aIter( rSet );
    sal_uInt16 nWhich = aIter.FirstWhich();
    while( nWhich )
    {
        switch( nWhich )
        {
            case SID_TEXTDIRECTION_LEFT_TO_RIGHT:
            case SID_TEXTDIRECTION_TOP_TO_BOTTOM:
                if ( bDisableVerticalText )
					rSet.DisableItem( nWhich );
				else
				{
					if( bVertDontCare )
						rSet.InvalidateItem( nWhich );
					else if ( nWhich == SID_TEXTDIRECTION_LEFT_TO_RIGHT )
						rSet.Put( SfxBoolItem( nWhich, bLeftRight ) );
					else
						rSet.Put( SfxBoolItem( nWhich, bTopBottom ) );
				}
            break;

            case SID_ATTR_PARA_LEFT_TO_RIGHT:
            case SID_ATTR_PARA_RIGHT_TO_LEFT:
				if ( bDisableCTLFont )
					rSet.DisableItem( nWhich );
				else
				{
					if ( bTopBottom )
						rSet.DisableItem( nWhich );
					else if ( bBidiDontCare )
						rSet.InvalidateItem( nWhich );
					else if ( nWhich == SID_ATTR_PARA_LEFT_TO_RIGHT )
						rSet.Put( SfxBoolItem( nWhich, eBidiDir == EE_HTEXTDIR_L2R ) );
					else
						rSet.Put( SfxBoolItem( nWhich, eBidiDir == EE_HTEXTDIR_R2L ) );
				}
        }
        nWhich = aIter.NextWhich();
    }
}

void ScFormatShell::ExecFormatPaintbrush( SfxRequest& rReq )
{
    ScViewFunc* pView = pViewData->GetView();
    if ( pView->HasPaintBrush() )
    {
        // cancel paintbrush mode
        pView->ResetBrushDocument();
    }
    else
    {
        sal_Bool bLock = sal_False;
        const SfxItemSet *pArgs = rReq.GetArgs();
        if( pArgs && pArgs->Count() >= 1 )
            bLock = static_cast<const SfxBoolItem&>(pArgs->Get(SID_FORMATPAINTBRUSH)).GetValue();

        // in case of multi selection, deselect all and use the cursor position
        ScRange aDummy;
        if ( pViewData->GetSimpleArea(aDummy) != SC_MARK_SIMPLE )
            pView->Unmark();

        ScDocument* pBrushDoc = new ScDocument( SCDOCMODE_CLIP );
        pView->CopyToClip( pBrushDoc, sal_False, sal_True );
        pView->SetBrushDocument( pBrushDoc, bLock );
    }
}

void ScFormatShell::StateFormatPaintbrush( SfxItemSet& rSet )
{
    if ( pViewData->HasEditView( pViewData->GetActivePart() ) )
        rSet.DisableItem( SID_FORMATPAINTBRUSH );
    else
        rSet.Put( SfxBoolItem( SID_FORMATPAINTBRUSH, pViewData->GetView()->HasPaintBrush() ) );
}

