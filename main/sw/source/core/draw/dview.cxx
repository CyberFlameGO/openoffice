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

#include "hintids.hxx"
#include <editeng/protitem.hxx>
#include <svx/svdpagv.hxx>
#include <svx/fmmodel.hxx>
#include <sot/exchange.hxx>

#include "swtypes.hxx"
#include "pagefrm.hxx"
#include "rootfrm.hxx"
#include "cntfrm.hxx"
#include "flyfrm.hxx"
#include "frmfmt.hxx"
#include "dflyobj.hxx"
#include "dcontact.hxx"
#include "frmatr.hxx"
#include "viewsh.hxx"
#include "viewimp.hxx"
#include "dview.hxx"
#include "dpage.hxx"
#include "doc.hxx"
#include "mdiexp.hxx"
#include <ndole.hxx>
#include <fmtanchr.hxx>
#include "shellres.hxx"
#include <IDocumentUndoRedo.hxx>

// #i7672#
#include <editeng/outliner.hxx>

#include <com/sun/star/embed/EmbedMisc.hpp>

// OD 18.06.2003 #108784#
//#ifndef _SVDVMARK_HXX //autogen
//#include <svx/svdvmark.hxx>
//#endif
#include <vector>
// --> OD 2004-06-24 #i28701#
#include <sortedobjs.hxx>
#include <flyfrms.hxx>
// <--


using namespace com::sun::star;


class SwSdrHdl : public SdrHdl
{
public:
    SwSdrHdl(const Point& rPnt, bool bTopRight ) :
        SdrHdl( rPnt, bTopRight ? HDL_ANCHOR_TR : HDL_ANCHOR ) {}
    virtual sal_Bool IsFocusHdl() const;
};

sal_Bool SwSdrHdl::IsFocusHdl() const
{
    if( HDL_ANCHOR == eKind || HDL_ANCHOR_TR == eKind )
        return sal_True;
    return SdrHdl::IsFocusHdl();
}

const SwFrm *lcl_FindAnchor( const SdrObject *pObj, sal_Bool bAll )
{
	const SwVirtFlyDrawObj *pVirt = pObj->ISA(SwVirtFlyDrawObj) ?
											(SwVirtFlyDrawObj*)pObj : 0;
	if ( pVirt )
	{
		if ( bAll || !pVirt->GetFlyFrm()->IsFlyInCntFrm() )
            return pVirt->GetFlyFrm()->GetAnchorFrm();
	}
	else
	{
        const SwDrawContact *pCont = (const SwDrawContact*)GetUserCall(pObj);
		if ( pCont )
            return pCont->GetAnchorFrm( pObj );
	}
	return 0;
}

/*************************************************************************
|*
|*	SwDrawView::Ctor
|*
|*	Ersterstellung		OK 18.11.94
|*	Letzte Aenderung	MA 22. Jul. 96
|*
*************************************************************************/



SwDrawView::SwDrawView( SwViewImp &rI, SdrModel *pMd, OutputDevice *pOutDev) :
	FmFormView( (FmFormModel*)pMd, pOutDev ),
	rImp( rI )
{
	SetPageVisible( sal_False );
	SetBordVisible( sal_False );
	SetGridVisible( sal_False );
	SetHlplVisible( sal_False );
	SetGlueVisible( sal_False );
	SetFrameDragSingles( sal_True );
	SetVirtualObjectBundling( sal_True );
	SetSwapAsynchron( sal_True );

	EnableExtendedKeyInputDispatcher( sal_False );
	EnableExtendedMouseEventDispatcher( sal_False );
	EnableExtendedCommandEventDispatcher( sal_False );

	SetHitTolerancePixel( GetMarkHdlSizePixel()/2 );

	SetPrintPreview( rI.GetShell()->IsPreView() );

	// #i73602# Use default from the configuration
	SetBufferedOverlayAllowed(getOptionsDrawinglayer().IsOverlayBuffer_Writer());

	// #i74769#, #i75172# Use default from the configuration
	SetBufferedOutputAllowed(getOptionsDrawinglayer().IsPaintBuffer_Writer());
}

// --> OD 2009-03-05 #i99665#
sal_Bool SwDrawView::IsAntiAliasing() const
{
    return getOptionsDrawinglayer().IsAntiAliasing();
}
// <--

//////////////////////////////////////////////////////////////////////////////

SdrObject* impLocalHitCorrection(SdrObject* pRetval, const Point& rPnt, sal_uInt16 nTol, const SdrMarkList &rMrkList)
{
    if(!nTol)
    {
        // the old method forced back to outer bounds test when nTol == 0, so
        // do not try to correct when nTol is not set (used from HelpContent)
    }
    else
    {
        // rebuild logic from former SwVirtFlyDrawObj::CheckSdrObjectHit. This is needed since
        // the SdrObject-specific CheckHit implementations are now replaced with primitives and
        // 'tricks' like in the old implementation (e.g. using a view from a model-data class to
        // detect if object is selected) are no longer valid.
        // The standard primitive hit-test for SwVirtFlyDrawObj now is the outer bound. The old
        // implementation reduced this excluding the inner bound when the object was not selected.
        SwVirtFlyDrawObj* pSwVirtFlyDrawObj = dynamic_cast< SwVirtFlyDrawObj* >(pRetval);

        if(pSwVirtFlyDrawObj)
        {
            if(pSwVirtFlyDrawObj->GetFlyFrm()->Lower() && pSwVirtFlyDrawObj->GetFlyFrm()->Lower()->IsNoTxtFrm())
			{
                // the old method used IsNoTxtFrm (should be for SW's own OLE and
                // graphic's) to accept hit only based on outer bounds; nothing to do
            }
            else
            {
                // check if the object is selected in this view
                const sal_uInt32 nMarkCount(rMrkList.GetMarkCount());
                bool bSelected(false);

                for(sal_uInt32 a(0); !bSelected && a < nMarkCount; a++)
                {
                    if(pSwVirtFlyDrawObj == rMrkList.GetMark(a)->GetMarkedSdrObj())
                    {
                        bSelected = true;
                    }
                }

                if(!bSelected)
                {
                    // when not selected, the object is not hit when hit position is inside
                    // inner range. Get and shrink inner range
                    basegfx::B2DRange aInnerBound(pSwVirtFlyDrawObj->getInnerBound());

                    aInnerBound.grow(-1.0 * nTol);

                    if(aInnerBound.isInside(basegfx::B2DPoint(rPnt.X(), rPnt.Y())))
                    {
                        // exclude this hit
                        pRetval = 0;
                    }
                }
            }
        }
    }

    return pRetval;
}

SdrObject* SwDrawView::CheckSingleSdrObjectHit(const Point& rPnt, sal_uInt16 nTol, SdrObject* pObj, SdrPageView* pPV, sal_uLong nOptions, const SetOfByte* pMVisLay) const
{
    // call parent
    SdrObject* pRetval = FmFormView::CheckSingleSdrObjectHit(rPnt, nTol, pObj, pPV, nOptions, pMVisLay);

    if(pRetval)
    {
        // overloaded to allow extra handling when picking SwVirtFlyDrawObj's
        pRetval = impLocalHitCorrection(pRetval, rPnt, nTol, GetMarkedObjectList());
    }

    return pRetval;
}

/*************************************************************************
|*
|*	SwDrawView::AddCustomHdl()
|*
|*  Gets called every time the handles need to be build
|*
|*	Ersterstellung		AW 06. Sep. 99
|*	Letzte Aenderung	AW 06. Sep. 99
|*
*************************************************************************/

void SwDrawView::AddCustomHdl()
{
	const SdrMarkList &rMrkList = GetMarkedObjectList();

	if(rMrkList.GetMarkCount() != 1 || !GetUserCall(rMrkList.GetMark( 0 )->GetMarkedSdrObj()))
		return;

	SdrObject *pObj = rMrkList.GetMark(0)->GetMarkedSdrObj();
    // --> OD 2006-11-06 #130889# - make code robust
//    const SwFmtAnchor &rAnchor = ::FindFrmFmt(pObj)->GetAnchor();
    SwFrmFmt* pFrmFmt( ::FindFrmFmt( pObj ) );
    if ( !pFrmFmt )
    {
        ASSERT( false, "<SwDrawView::AddCustomHdl()> - missing frame format!" );
        return;
    }
    const SwFmtAnchor &rAnchor = pFrmFmt->GetAnchor();
    // <--

    if (FLY_AS_CHAR == rAnchor.GetAnchorId())
		return;

	const SwFrm* pAnch;
	if(0 == (pAnch = CalcAnchor()))
		return;

    Point aPos(aAnchorPoint);

    if ( FLY_AT_CHAR == rAnchor.GetAnchorId() )
	{
        // --> OD 2004-06-24 #i28701# - use last character rectangle saved at object
        // in order to avoid a format of the anchor frame
        SwAnchoredObject* pAnchoredObj = ::GetUserCall( pObj )->GetAnchoredObj( pObj );
        SwRect aAutoPos = pAnchoredObj->GetLastCharRect();
        if ( aAutoPos.Height() )
        {
            aPos = aAutoPos.Pos();
        }
	}

	// add anchor handle:
    //Badaa: 2008-04-18 * Support for Classical Mongolian Script (SCMS) joint with Jiayanmin
    aHdl.AddHdl( new SwSdrHdl( aPos, ( pAnch->IsVertical() && !pAnch->IsVertLR() ) ||
                                     pAnch->IsRightToLeft() ) );	
}

/*************************************************************************
|*
|*	SwDrawView::GetMaxToTopObj(), _GetMaxToTopObj()
|*
|*	Ersterstellung		MA 13. Jan. 95
|*	Letzte Aenderung	MA 18. Mar. 97
|*
*************************************************************************/


SdrObject* SwDrawView::GetMaxToTopObj( SdrObject* pObj ) const
{
	if ( GetUserCall(pObj) )
	{
		const SwFrm *pAnch = ::lcl_FindAnchor( pObj, sal_False );
		if ( pAnch )
		{
			//Das oberste Obj innerhalb des Ankers darf nicht ueberholt
			//werden.
			const SwFlyFrm *pFly = pAnch->FindFlyFrm();
			if ( pFly )
			{
				const SwPageFrm *pPage = pFly->FindPageFrm();
				if ( pPage->GetSortedObjs() )
				{
					sal_uInt32 nOrdNum = 0;
					for ( sal_uInt16 i = 0; i < pPage->GetSortedObjs()->Count(); ++i )
					{
                        const SdrObject *pO =
                                    (*pPage->GetSortedObjs())[i]->GetDrawObj();

                        if ( pO->GetOrdNumDirect() > nOrdNum )
						{
                            const SwFrm *pTmpAnch = ::lcl_FindAnchor( pO, sal_False );
                            if ( pFly->IsAnLower( pTmpAnch ) )
							{
								nOrdNum = pO->GetOrdNumDirect();
							}
						}
					}
					if ( nOrdNum )
					{
                        SdrPage *pTmpPage = GetModel()->GetPage( 0 );
						++nOrdNum;
                        if ( nOrdNum < pTmpPage->GetObjCount() )
						{
                            return pTmpPage->GetObj( nOrdNum );
						}
					}
				}
			}
		}
	}
	return 0;
}

/*************************************************************************
|*
|*	SwDrawView::GetMaxToBtmObj()
|*
|*	Ersterstellung		MA 13. Jan. 95
|*	Letzte Aenderung	MA 05. Sep. 96
|*
*************************************************************************/


SdrObject* SwDrawView::GetMaxToBtmObj(SdrObject* pObj) const
{
	if ( GetUserCall(pObj) )
	{
		const SwFrm *pAnch = ::lcl_FindAnchor( pObj, sal_False );
		if ( pAnch )
		{
			//Der Fly des Ankers darf nicht "unterflogen" werden.
			const SwFlyFrm *pFly = pAnch->FindFlyFrm();
			if ( pFly )
			{
				SdrObject *pRet = (SdrObject*)pFly->GetVirtDrawObj();
				return pRet != pObj ? pRet : 0;
			}
		}
	}
	return 0;
}

/*************************************************************************
|*
|*	SwDrawView::ObjOrderChanged()
|*
|*	Ersterstellung		MA 31. Jul. 95
|*	Letzte Aenderung	MA 18. Mar. 97
|*
*************************************************************************/

inline sal_Bool lcl_IsChild( SdrObject *pParent, SdrObject *pChild )
{
	if ( pParent->ISA(SwVirtFlyDrawObj) )
	{
		const SwFrm *pAnch = lcl_FindAnchor( pChild, sal_False );
		if ( pAnch && ((SwVirtFlyDrawObj*)pParent)->GetFlyFrm()->IsAnLower( pAnch ))
		{
			return sal_True;
		}
	}
	return sal_False;
}

inline SdrObject *lcl_FindParent( SdrObject *pObj )
{
	const SwFrm *pAnch = lcl_FindAnchor( pObj, sal_False );
	if ( pAnch && pAnch->IsInFly() )
		return (SdrObject*)pAnch->FindFlyFrm()->GetVirtDrawObj();
	return 0;
}

/** determine maximal order number for a 'child' object of given 'parent' object

    OD 2004-08-20 #110810#

    @author OD
*/
sal_uInt32 SwDrawView::_GetMaxChildOrdNum( const SwFlyFrm& _rParentObj,
                                           const SdrObject* _pExclChildObj ) const
{
    sal_uInt32 nMaxChildOrdNum = _rParentObj.GetDrawObj()->GetOrdNum();

    const SdrPage* pDrawPage = _rParentObj.GetDrawObj()->GetPage();
    ASSERT( pDrawPage,
            "<SwDrawView::_GetMaxChildOrdNum(..) - missing drawing page at parent object - crash!" );

    sal_uInt32 nObjCount = pDrawPage->GetObjCount();
    for ( sal_uInt32 i = nObjCount-1; i > _rParentObj.GetDrawObj()->GetOrdNum() ; --i )
    {
        const SdrObject* pObj = pDrawPage->GetObj( i );

        // Don't consider 'child' object <_pExclChildObj>
        if ( pObj == _pExclChildObj )
        {
            continue;
        }

        if ( pObj->GetOrdNum() > nMaxChildOrdNum &&
             _rParentObj.IsAnLower( lcl_FindAnchor( pObj, sal_True ) ) )
        {
            nMaxChildOrdNum = pObj->GetOrdNum();
            break;
        }
    }

    return nMaxChildOrdNum;
}

/** method to move 'repeated' objects of the given moved object to the
    according level

    OD 2004-08-23 #110810#

    @author OD
*/
void SwDrawView::_MoveRepeatedObjs( const SwAnchoredObject& _rMovedAnchoredObj,
                                    const std::vector<SdrObject*>& _rMovedChildObjs ) const
{
    // determine 'repeated' objects of already moved object <_rMovedAnchoredObj>
    std::list<SwAnchoredObject*> aAnchoredObjs;
    {
        const SwContact* pContact = ::GetUserCall( _rMovedAnchoredObj.GetDrawObj() );
        ASSERT( pContact,
                "SwDrawView::_MoveRepeatedObjs(..) - missing contact object -> crash." );
        pContact->GetAnchoredObjs( aAnchoredObjs );
    }

    // check, if 'repeated' objects exists.
    if ( aAnchoredObjs.size() > 1 )
    {
        SdrPage* pDrawPage = GetModel()->GetPage( 0 );

        // move 'repeated' ones to the same order number as the already moved one.
        sal_uInt32 nNewPos = _rMovedAnchoredObj.GetDrawObj()->GetOrdNum();
        while ( !aAnchoredObjs.empty() )
        {
            SwAnchoredObject* pAnchoredObj = aAnchoredObjs.back();
            if ( pAnchoredObj != &_rMovedAnchoredObj )
            {
                pDrawPage->SetObjectOrdNum( pAnchoredObj->GetDrawObj()->GetOrdNum(),
                                            nNewPos );
                pDrawPage->RecalcObjOrdNums();
                // adjustments for accessibility API
                if ( pAnchoredObj->ISA(SwFlyFrm) )
                {
                    const SwFlyFrm *pTmpFlyFrm = static_cast<SwFlyFrm*>(pAnchoredObj);
                    rImp.DisposeAccessibleFrm( pTmpFlyFrm );
                    rImp.AddAccessibleFrm( pTmpFlyFrm );
                }
                else
                {
                    rImp.DisposeAccessibleObj( pAnchoredObj->GetDrawObj() );
                    rImp.AddAccessibleObj( pAnchoredObj->GetDrawObj() );
                }
            }
            aAnchoredObjs.pop_back();
        }

        // move 'repeated' ones of 'child' objects
        for ( std::vector<SdrObject*>::const_iterator aObjIter = _rMovedChildObjs.begin();
              aObjIter != _rMovedChildObjs.end(); ++aObjIter )
        {
            SdrObject* pChildObj = (*aObjIter);
            {
                const SwContact* pContact = ::GetUserCall( pChildObj );
                ASSERT( pContact,
                        "SwDrawView::_MoveRepeatedObjs(..) - missing contact object -> crash." );
                pContact->GetAnchoredObjs( aAnchoredObjs );
            }
            // move 'repeated' ones to the same order number as the already moved one.
            const sal_uInt32 nTmpNewPos = pChildObj->GetOrdNum();
            while ( !aAnchoredObjs.empty() )
            {
                SwAnchoredObject* pAnchoredObj = aAnchoredObjs.back();
                if ( pAnchoredObj->GetDrawObj() != pChildObj )
                {
                    pDrawPage->SetObjectOrdNum( pAnchoredObj->GetDrawObj()->GetOrdNum(),
                                                nTmpNewPos );
                    pDrawPage->RecalcObjOrdNums();
                    // adjustments for accessibility API
                    if ( pAnchoredObj->ISA(SwFlyFrm) )
                    {
                        const SwFlyFrm *pTmpFlyFrm = static_cast<SwFlyFrm*>(pAnchoredObj);
                        rImp.DisposeAccessibleFrm( pTmpFlyFrm );
                        rImp.AddAccessibleFrm( pTmpFlyFrm );
                    }
                    else
                    {
                        rImp.DisposeAccessibleObj( pAnchoredObj->GetDrawObj() );
                        rImp.AddAccessibleObj( pAnchoredObj->GetDrawObj() );
                    }
                }
                aAnchoredObjs.pop_back();
            }
        }
    }
}

// --> OD 2004-08-20 #110810# - adjustment and re-factoring of method
void SwDrawView::ObjOrderChanged( SdrObject* pObj, sal_uLong nOldPos,
										  sal_uLong nNewPos )
{
    // --> OD 2004-08-17 #110810# - nothing to do for group members
    if ( pObj->GetUpGroup() )
    {
        return;
    }
    // <--

    // determine drawing page and assure that the order numbers are correct.
    SdrPage* pDrawPage = GetModel()->GetPage( 0 );
    if ( pDrawPage->IsObjOrdNumsDirty() )
        pDrawPage->RecalcObjOrdNums();
    const sal_uInt32 nObjCount = pDrawPage->GetObjCount();

    SwAnchoredObject* pMovedAnchoredObj =
                                ::GetUserCall( pObj )->GetAnchoredObj( pObj );
    const SwFlyFrm* pParentAnchoredObj =
                                pMovedAnchoredObj->GetAnchorFrm()->FindFlyFrm();

    const bool bMovedForward = nOldPos < nNewPos;

    // assure for a 'child' object, that it doesn't exceed the limits of its 'parent'
    if ( pParentAnchoredObj )
    {
        if ( bMovedForward )
        {
            sal_uInt32 nMaxChildOrdNumWithoutMoved =
                    _GetMaxChildOrdNum( *pParentAnchoredObj, pMovedAnchoredObj->GetDrawObj() );
            if ( nNewPos > nMaxChildOrdNumWithoutMoved+1 )
            {
                // set position to the top of the 'child' object group
                pDrawPage->SetObjectOrdNum( nNewPos, nMaxChildOrdNumWithoutMoved+1 );
                nNewPos = nMaxChildOrdNumWithoutMoved+1;
            }
        }
        else
        {
            const sal_uInt32 nParentOrdNum = pParentAnchoredObj->GetDrawObj()->GetOrdNum();
            if ( nNewPos < nParentOrdNum )
            {
                // set position to the bottom of the 'child' object group
                pDrawPage->SetObjectOrdNum( nNewPos, nParentOrdNum );
                nNewPos = nParentOrdNum;
            }
        }
        if ( pDrawPage->IsObjOrdNumsDirty() )
            pDrawPage->RecalcObjOrdNums();
    }

    // Assure, that object isn't positioned between 'repeated' ones
    if ( ( bMovedForward && nNewPos < nObjCount - 1 ) ||
         ( !bMovedForward && nNewPos > 0 ) )
    {
        const SdrObject* pTmpObj =
                pDrawPage->GetObj( bMovedForward ? nNewPos - 1 : nNewPos + 1 );
        if ( pTmpObj )
        {
            sal_uInt32 nTmpNewPos( nNewPos );
            if ( bMovedForward )
            {
                // move before the top 'repeated' object
                const sal_uInt32 nTmpMaxOrdNum =
                                    ::GetUserCall( pTmpObj )->GetMaxOrdNum();
                if ( nTmpMaxOrdNum > nNewPos )
                    nTmpNewPos = nTmpMaxOrdNum;
            }
            else
            {
                // move behind the bottom 'repeated' object
                const sal_uInt32 nTmpMinOrdNum =
                                    ::GetUserCall( pTmpObj )->GetMinOrdNum();
                if ( nTmpMinOrdNum < nNewPos )
                    nTmpNewPos = nTmpMinOrdNum;
            }
            if ( nTmpNewPos != nNewPos )
            {
                pDrawPage->SetObjectOrdNum( nNewPos, nTmpNewPos );
                nNewPos = nTmpNewPos;
                pDrawPage->RecalcObjOrdNums();
            }
        }
    }

    // On move forward, assure that object is moved before its own childs.
    // Only Writer fly frames can have childs.
    if ( pMovedAnchoredObj->ISA(SwFlyFrm) &&
         bMovedForward && nNewPos < nObjCount - 1 )
    {
        sal_uInt32 nMaxChildOrdNum =
                    _GetMaxChildOrdNum( *(static_cast<const SwFlyFrm*>(pMovedAnchoredObj)) );
        if ( nNewPos < nMaxChildOrdNum )
        {
            // determine position before the object before its top 'child' object
            const SdrObject* pTmpObj = pDrawPage->GetObj( nMaxChildOrdNum );
            sal_uInt32 nTmpNewPos = ::GetUserCall( pTmpObj )->GetMaxOrdNum() + 1;
            if ( nTmpNewPos >= nObjCount )
            {
                --nTmpNewPos;
            }
            // assure, that determined position isn't between 'repeated' objects
            pTmpObj = pDrawPage->GetObj( nTmpNewPos );
            nTmpNewPos = ::GetUserCall( pTmpObj )->GetMaxOrdNum();
            // apply new position
            pDrawPage->SetObjectOrdNum( nNewPos, nTmpNewPos );
            nNewPos = nTmpNewPos;
            pDrawPage->RecalcObjOrdNums();
        }
    }

    // Assure, that object isn't positioned between nested objects
    if ( ( bMovedForward && nNewPos < nObjCount - 1 ) ||
         ( !bMovedForward && nNewPos > 0 ) )
    {
        sal_uInt32 nTmpNewPos( nNewPos );
        const SwFrmFmt* pParentFrmFmt =
                pParentAnchoredObj ? &(pParentAnchoredObj->GetFrmFmt()) : 0L;
        const SdrObject* pTmpObj = pDrawPage->GetObj( nNewPos + 1 );
        while ( pTmpObj )
        {
            // --> OD 2004-12-07 #i38563# - assure, that anchor frame exists.
            // If object is anchored inside a invisible part of the document
            // (e.g. page header, whose page style isn't applied, or hidden
            // section), no anchor frame exists.
            const SwFrm* pTmpAnchorFrm = lcl_FindAnchor( pTmpObj, sal_True );
            const SwFlyFrm* pTmpParentObj = pTmpAnchorFrm
                                            ? pTmpAnchorFrm->FindFlyFrm() : 0L;
            // <--
            if ( pTmpParentObj &&
                 &(pTmpParentObj->GetFrmFmt()) != pParentFrmFmt )
            {
                if ( bMovedForward )
                {
                    nTmpNewPos = ::GetUserCall( pTmpObj )->GetMaxOrdNum();
                    pTmpObj = pDrawPage->GetObj( nTmpNewPos + 1 );
                }
                else
                {
                    nTmpNewPos = ::GetUserCall( pTmpParentObj->GetDrawObj() )
                                                            ->GetMinOrdNum();
                    pTmpObj = pTmpParentObj->GetDrawObj();
                }
            }
            else
                break;
        }
        if ( nTmpNewPos != nNewPos )
        {
            pDrawPage->SetObjectOrdNum( nNewPos, nTmpNewPos );
            nNewPos = nTmpNewPos;
            pDrawPage->RecalcObjOrdNums();
        }
    }

    // setup collection of moved 'child' objects to move its 'repeated' objects.
    std::vector< SdrObject* > aMovedChildObjs;

    // move 'childs' accordingly
    if ( pMovedAnchoredObj->ISA(SwFlyFrm) )
    {
        const SwFlyFrm* pFlyFrm = static_cast<SwFlyFrm*>(pMovedAnchoredObj);

        // adjustments for accessibility API
        rImp.DisposeAccessibleFrm( pFlyFrm );
        rImp.AddAccessibleFrm( pFlyFrm );

        const sal_uInt32 nChildNewPos = bMovedForward ? nNewPos : nNewPos+1;
        sal_uInt32 i = bMovedForward ? nOldPos : nObjCount-1;
        do
        {
            SdrObject* pTmpObj = pDrawPage->GetObj( i );
            if ( pTmpObj == pObj )
                break;

            // --> OD 2004-12-07 #i38563# - assure, that anchor frame exists.
            // If object is anchored inside a invisible part of the document
            // (e.g. page header, whose page style isn't applied, or hidden
            // section), no anchor frame exists.
            const SwFrm* pTmpAnchorFrm = lcl_FindAnchor( pTmpObj, sal_True );
            const SwFlyFrm* pTmpParentObj = pTmpAnchorFrm
                                            ? pTmpAnchorFrm->FindFlyFrm() : 0L;
            // <--
            if ( pTmpParentObj &&
                 ( ( pTmpParentObj == pFlyFrm ) ||
                   ( pFlyFrm->IsUpperOf( *pTmpParentObj ) ) ) )
            {
                // move child object.,
                pDrawPage->SetObjectOrdNum( i, nChildNewPos );
                pDrawPage->RecalcObjOrdNums();
                // collect 'child' object
                aMovedChildObjs.push_back( pTmpObj );
                // adjustments for accessibility API
                if ( pTmpObj->ISA(SwVirtFlyDrawObj) )
                {
                    const SwFlyFrm *pTmpFlyFrm =
                        static_cast<SwVirtFlyDrawObj*>(pTmpObj)->GetFlyFrm();
                    rImp.DisposeAccessibleFrm( pTmpFlyFrm );
                    rImp.AddAccessibleFrm( pTmpFlyFrm );
                }
                else
                {
                    rImp.DisposeAccessibleObj( pTmpObj );
                    rImp.AddAccessibleObj( pTmpObj );
                }
            }
            else
            {
                // adjust loop counter
                if ( bMovedForward )
                    ++i;
                else if ( !bMovedForward && i > 0 )
                    --i;
            }

        } while ( ( bMovedForward && i < ( nObjCount - aMovedChildObjs.size() ) ) ||
                  ( !bMovedForward && i > ( nNewPos + aMovedChildObjs.size() ) ) );
    }
	else
	{
        // adjustments for accessibility API
		rImp.DisposeAccessibleObj( pObj );
		rImp.AddAccessibleObj( pObj );
	}

    _MoveRepeatedObjs( *pMovedAnchoredObj, aMovedChildObjs );
}
// <--

/*************************************************************************
|*
|*	SwDrawView::TakeDragLimit()
|*
|*	Ersterstellung		AMA 26. Apr. 96
|*	Letzte Aenderung	MA 03. May. 96
|*
*************************************************************************/


sal_Bool SwDrawView::TakeDragLimit( SdrDragMode eMode,
											Rectangle& rRect ) const
{
	const SdrMarkList &rMrkList = GetMarkedObjectList();
	sal_Bool bRet = sal_False;
	if( 1 == rMrkList.GetMarkCount() )
	{
		const SdrObject *pObj = rMrkList.GetMark( 0 )->GetMarkedSdrObj();
		SwRect aRect;
		if( ::CalcClipRect( pObj, aRect, eMode == SDRDRAG_MOVE ) )
		{
			rRect = aRect.SVRect();
		 	bRet = sal_True;
		}
	}
	return bRet;
}

/*************************************************************************
|*
|*	SwDrawView::CalcAnchor()
|*
|*	Ersterstellung		MA 13. Jan. 95
|*	Letzte Aenderung	MA 08. Nov. 96
|*
*************************************************************************/


const SwFrm* SwDrawView::CalcAnchor()
{
	const SdrMarkList &rMrkList = GetMarkedObjectList();
	if ( rMrkList.GetMarkCount() != 1 )
		return NULL;

    SdrObject* pObj = rMrkList.GetMark( 0 )->GetMarkedSdrObj();

	//Fuer Absatzgebundene Objekte suchen, andernfalls einfach nur
	//der aktuelle Anker. Nur suchen wenn wir gerade draggen.
    const SwFrm* pAnch;
    Rectangle aMyRect;
	const sal_Bool bFly = pObj->ISA(SwVirtFlyDrawObj);
	if ( bFly )
	{
        pAnch = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm()->GetAnchorFrm();
        aMyRect = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm()->Frm().SVRect();
	}
	else
	{
        SwDrawContact *pC = (SwDrawContact*)GetUserCall(pObj);
        // OD 17.06.2003 #108784# - determine correct anchor position for
        // 'virtual' drawing objects.
        // OD 2004-03-25 #i26791#
        pAnch = pC->GetAnchorFrm( pObj );
		if( !pAnch )
		{
			pC->ConnectToLayout();
            // OD 17.06.2003 #108784# - determine correct anchor position for
            // 'virtual' drawing objects.
            // OD 2004-03-25 #i26791#
            pAnch = pC->GetAnchorFrm( pObj );
		}
        aMyRect = pObj->GetSnapRect();
	}

	//Badaa: 2008-04-18 * Support for Classical Mongolian Script (SCMS) joint with Jiayanmin
    const sal_Bool bTopRight = pAnch && ( ( pAnch->IsVertical() && 
    										!pAnch->IsVertLR() ) ||
                                         	pAnch->IsRightToLeft() );
    const Point aMyPt = bTopRight ? aMyRect.TopRight() : aMyRect.TopLeft();

    Point aPt;
    if ( IsAction() )
    {
        if ( !TakeDragObjAnchorPos( aPt, bTopRight ) )
            return NULL;
    }
    else
    {
        Rectangle aRect = pObj->GetSnapRect();
        aPt = bTopRight ? aRect.TopRight() : aRect.TopLeft();
    }

    if ( aPt != aMyPt )
	{
		if ( pAnch->IsCntntFrm() )
        {
            // OD 26.06.2003 #108784# - allow drawing objects in header/footer,
            // but exclude control objects.
            bool bBodyOnly = CheckControlLayer( pObj );
            pAnch = ::FindAnchor( (SwCntntFrm*)pAnch, aPt, bBodyOnly );
        }
		else if ( !bFly )
        {
            const SwRect aRect( aPt.X(), aPt.Y(), 1, 1 );

            SwDrawContact* pContact = (SwDrawContact*)GetUserCall(pObj);
            if ( pContact->GetAnchorFrm( pObj ) &&
                 pContact->GetAnchorFrm( pObj )->IsPageFrm() )
                pAnch = pContact->GetPageFrm();
			else
				pAnch = pContact->FindPage( aRect );
		}
	}
	if( pAnch && !pAnch->IsProtected() )
        aAnchorPoint = pAnch->GetFrmAnchorPos( ::HasWrap( pObj ) );
	else
		pAnch = 0;
	return pAnch;
}

/*************************************************************************
|*
|*	SwDrawView::ShowDragXor(), HideDragXor()
|*
|*	Ersterstellung		MA 17. Jan. 95
|*	Letzte Aenderung	MA 27. Jan. 95
|*
*************************************************************************/


void SwDrawView::ShowDragAnchor()
{
	SdrHdl* pHdl = aHdl.GetHdl(HDL_ANCHOR);
	if ( ! pHdl )
		pHdl = aHdl.GetHdl(HDL_ANCHOR_TR);

	if(pHdl)
	{
		CalcAnchor();
		pHdl->SetPos(aAnchorPoint);
		//OLMRefreshAllIAOManagers();
	}
}

/*************************************************************************
|*
|*	SwDrawView::MarkListHasChanged()
|*
|*	Ersterstellung		OM 02. Feb. 95
|*	Letzte Aenderung	OM 07. Jul. 95
|*
*************************************************************************/


void SwDrawView::MarkListHasChanged()
{
    Imp().GetShell()->DrawSelChanged();
	FmFormView::MarkListHasChanged();
}

// #i7672#
void SwDrawView::ModelHasChanged()
{
	// The ModelHasChanged() call in DrawingLayer also updates
	// a eventually active text edit view (OutlinerView). This also leads
	// to newly setting the background color for that edit view. Thus,
	// this method rescues the current background color if a OutlinerView
	// exists and re-establishes it then. To be more safe, the OutlinerView
	// will be fetched again (maybe textedit has ended).
	OutlinerView* pView = GetTextEditOutlinerView();
	Color aBackColor;
	sal_Bool bColorWasSaved(sal_False);

	if(pView)
	{
		aBackColor = pView->GetBackgroundColor();
		bColorWasSaved = sal_True;
	}

	// call parent
	FmFormView::ModelHasChanged();

	if(bColorWasSaved)
	{
		pView = GetTextEditOutlinerView();

		if(pView)
		{
			pView->SetBackgroundColor(aBackColor);
		}
	}
}

void SwDrawView::MakeVisible( const Rectangle &rRect, Window & )
{
    ASSERT( rImp.GetShell()->GetWin(), "MakeVisible, unknown Window");
	rImp.GetShell()->MakeVisible( SwRect( rRect ) );
}

void SwDrawView::CheckPossibilities()
{
	FmFormView::CheckPossibilities();

	//Zusaetzlich zu den bestehenden Flags der Objekte selbst, die von der
	//DrawingEngine ausgewertet werden, koennen weitere Umstaende zu einem
	//Schutz fuehren.
	//Objekte, die in Rahmen verankert sind, muessen genau dann geschuetzt
	//sein, wenn der Inhalt des Rahmens geschuetzt ist.
	//OLE-Objekte konnen selbst einen Resize-Schutz wuenschen (StarMath)

	const SdrMarkList &rMrkList = GetMarkedObjectList();
	sal_Bool bProtect = sal_False,
			 bSzProtect = sal_False;
	for ( sal_uInt16 i = 0; !bProtect && i < rMrkList.GetMarkCount(); ++i )
	{
		const SdrObject *pObj = rMrkList.GetMark( i )->GetMarkedSdrObj();
		const SwFrm *pFrm = NULL;
		if ( pObj->ISA(SwVirtFlyDrawObj) )
		{
			const SwFlyFrm *pFly = ((SwVirtFlyDrawObj*)pObj)->GetFlyFrm();
			if ( pFly  )
			{
                pFrm = pFly->GetAnchorFrm();
				if ( pFly->Lower() && pFly->Lower()->IsNoTxtFrm() )
				{
					SwOLENode *pNd = ((SwCntntFrm*)pFly->Lower())->GetNode()->GetOLENode();
					if ( pNd )
					{
                        uno::Reference < embed::XEmbeddedObject > xObj = pNd->GetOLEObj().GetOleRef();
                        if ( xObj.is() )
						{
                            // --> OD 2004-08-16 #110810# - improvement for
                            // the future, when more than one Writer fly frame
                            // can be selected.

                            // TODO/LATER: retrieve Aspect - from where?!
                            bSzProtect |= ( embed::EmbedMisc::EMBED_NEVERRESIZE & xObj->getStatus( embed::Aspects::MSOLE_CONTENT ) ) ? sal_True : sal_False;

                            // <--

                            // #i972: protect position if it is a Math object anchored 'as char' and baseline alignment is activated
                        	SwDoc* pDoc = Imp().GetShell()->GetDoc();
                            const bool bProtectMathPos = SotExchange::IsMath( xObj->getClassID() ) 
                                    && FLY_AS_CHAR == pFly->GetFmt()->GetAnchor().GetAnchorId() 
                                    && pDoc->get( IDocumentSettingAccess::MATH_BASELINE_ALIGNMENT );
                            if (bProtectMathPos)
                                bMoveProtect = true;
                        }
					}
				}
			}
		}
		else
		{
			SwDrawContact *pC = (SwDrawContact*)GetUserCall(pObj);
            if ( pC )
                pFrm = pC->GetAnchorFrm( pObj );
		}
		if ( pFrm )
			bProtect = pFrm->IsProtected(); //Rahmen, Bereiche usw.
        {
            SwFrmFmt* pFrmFmt( ::FindFrmFmt( const_cast<SdrObject*>(pObj) ) );
            if ( !pFrmFmt )
            {
                ASSERT( false,
                        "<SwDrawView::CheckPossibilities()> - missing frame format" );
                bProtect = sal_True;
            }
            else if ((FLY_AS_CHAR == pFrmFmt->GetAnchor().GetAnchorId()) &&
                      rMrkList.GetMarkCount() > 1 )
            {
                bProtect = sal_True;
            }
        }
	}
	bMoveProtect	|= bProtect;
	bResizeProtect	|= bProtect | bSzProtect;
}

/** replace marked <SwDrawVirtObj>-objects by its reference object for delete
    marked objects.

    OD 18.06.2003 #108784#

    @author OD
*/
void SwDrawView::ReplaceMarkedDrawVirtObjs( SdrMarkView& _rMarkView )
{
    SdrPageView* pDrawPageView = _rMarkView.GetSdrPageView();
    const SdrMarkList& rMarkList = _rMarkView.GetMarkedObjectList();

    if( rMarkList.GetMarkCount() )
    {
        // collect marked objects in a local data structure
        std::vector<SdrObject*> aMarkedObjs;
        for( sal_uInt32 i = 0; i < rMarkList.GetMarkCount(); ++i )
        {
            SdrObject* pMarkedObj = rMarkList.GetMark( i )->GetMarkedSdrObj();
            aMarkedObjs.push_back( pMarkedObj );
        }
        // unmark all objects
        _rMarkView.UnmarkAllObj();
        // re-mark objects, but for marked <SwDrawVirtObj>-objects marked its
        // reference object.
        while ( !aMarkedObjs.empty() )
        {
            SdrObject* pMarkObj = aMarkedObjs.back();
            if ( pMarkObj->ISA(SwDrawVirtObj) )
            {
                SdrObject* pRefObj = &(static_cast<SwDrawVirtObj*>(pMarkObj)->ReferencedObj());
                if ( !_rMarkView.IsObjMarked( pRefObj )  )
                {
                    _rMarkView.MarkObj( pRefObj, pDrawPageView );
                }
            }
            else
            {
                _rMarkView.MarkObj( pMarkObj, pDrawPageView );
            }

            aMarkedObjs.pop_back();
        }
        // sort marked list in order to assure consistent state in drawing layer
        _rMarkView.SortMarkedObjects();
    }
}

void SwDrawView::DeleteMarked()
{
	SwDoc* pDoc = Imp().GetShell()->GetDoc();
	SwRootFrm *pTmpRoot = pDoc->GetCurrentLayout();//swmod 080317
    if ( pTmpRoot )
		pTmpRoot->StartAllAction();	
    pDoc->GetIDocumentUndoRedo().StartUndo(UNDO_EMPTY, NULL);
    // OD 18.06.2003 #108784# - replace marked <SwDrawVirtObj>-objects by its
    // reference objects.
    {
        SdrPageView* pDrawPageView = rImp.GetPageView();
        if ( pDrawPageView )
        {
            SdrMarkView* pMarkView = PTR_CAST( SdrMarkView, &(pDrawPageView->GetView()) );
            if ( pMarkView )
            {
                ReplaceMarkedDrawVirtObjs( *pMarkView );
            }
        }
    }
    if ( pDoc->DeleteSelection( *this ) )
	{
		FmFormView::DeleteMarked();
		::FrameNotify( Imp().GetShell(), FLY_DRAG_END );
	}
    pDoc->GetIDocumentUndoRedo().EndUndo(UNDO_EMPTY, NULL);
	if( pTmpRoot )
		pTmpRoot->EndAllAction();	//swmod 080218
}

