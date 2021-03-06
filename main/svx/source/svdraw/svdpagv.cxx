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
#include "precompiled_svx.hxx"
#include <svx/svdpagv.hxx>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/PosSize.hpp>
#include <comphelper/processfactory.hxx>
#include <svx/svdoutl.hxx>
#include <svx/xpoly.hxx>
#include <svx/svdouno.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdview.hxx>

#include <svx/svdedxv.hxx>
#include <svx/svdpagv.hxx>
#include <svx/svdoutl.hxx>
#include <svx/svdpagv.hxx>
#include <editeng/outliner.hxx>
#include <svx/svdetc.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdouno.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdview.hxx>
#include "svx/svditer.hxx"
#include <svx/svdogrp.hxx>
#include <svx/svdtypes.hxx>

#include <svx/svdotext.hxx> // fuer PaintOutlinerView
#include <svx/svdoole2.hxx>

// #110094#
#include <svx/sdr/contact/objectcontactofpageview.hxx>
#include <svx/svdogrp.hxx>
#include <svx/sdr/contact/viewobjectcontactredirector.hxx>
#include <svx/fmview.hxx>

// for search on vector
#include <algorithm>

using namespace ::rtl;
using namespace ::com::sun::star;
#include <svx/sdrpagewindow.hxx>
#include <svx/sdrpaintwindow.hxx>

TYPEINIT1(SdrPageView, SfxListener);
DBG_NAME(SdrPageView);

////////////////////////////////////////////////////////////////////////////////////////////////////
// interface to SdrPageWindow

SdrPageWindow* SdrPageView::FindPageWindow(SdrPaintWindow& rPaintWindow) const
{
	for(SdrPageWindowVector::const_iterator a = maPageWindows.begin(); a != maPageWindows.end(); a++)
	{
		if(&((*a)->GetPaintWindow()) == &rPaintWindow)
		{
			return *a;
		}
	}

	return 0L;
}

const SdrPageWindow* SdrPageView::FindPatchedPageWindow( const OutputDevice& _rOutDev ) const
{
	for (   SdrPageWindowVector::const_iterator loop = maPageWindows.begin();
            loop != maPageWindows.end();
            ++loop
        )
	{
        const SdrPageWindow& rPageWindow( *(*loop) );
        const SdrPaintWindow& rPaintWindow( rPageWindow.GetOriginalPaintWindow() ? *rPageWindow.GetOriginalPaintWindow() : rPageWindow.GetPaintWindow() );
        if ( &rPaintWindow.GetOutputDevice() == &_rOutDev )
        {
            return &rPageWindow;
        }
	}

	return NULL;
}

SdrPageWindow* SdrPageView::FindPageWindow(const OutputDevice& rOutDev) const
{
	for(SdrPageWindowVector::const_iterator a = maPageWindows.begin(); a != maPageWindows.end(); a++)
	{
		if(&((*a)->GetPaintWindow().GetOutputDevice()) == &rOutDev)
		{
			return *a;
		}
	}

	return 0L;
}

SdrPageWindow* SdrPageView::GetPageWindow(sal_uInt32 nIndex) const
{
	// #126416#
	if(nIndex < maPageWindows.size())
	{
		return maPageWindows[nIndex];
	}

	return 0L;
}

void SdrPageView::ClearPageWindows()
{
	// #126416#
	for(SdrPageWindowVector::const_iterator a = maPageWindows.begin(); a != maPageWindows.end(); a++)
	{
		delete *a;
	}

	maPageWindows.clear();
}

void SdrPageView::AppendPageWindow(SdrPageWindow& rNew)
{
	maPageWindows.push_back(&rNew);
}

SdrPageWindow* SdrPageView::RemovePageWindow(sal_uInt32 nPos)
{
	if(nPos < maPageWindows.size())
	{
		SdrPageWindowVector::iterator aAccess = maPageWindows.begin() + nPos;
		// #114376# remember return value
		SdrPageWindow* pErasedSdrPageWindow = *aAccess;
		maPageWindows.erase(aAccess);
		return pErasedSdrPageWindow;
	}

	return 0L;
}

SdrPageWindow* SdrPageView::RemovePageWindow(SdrPageWindow& rOld)
{
	const SdrPageWindowVector::iterator aFindResult = ::std::find(maPageWindows.begin(), maPageWindows.end(), &rOld);

	if(aFindResult != maPageWindows.end())
	{
		// #114376# remember return value
		SdrPageWindow* pSdrPageWindow = *aFindResult;
		maPageWindows.erase(aFindResult);
		return pSdrPageWindow;
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

SdrPageView::SdrPageView(SdrPage* pPage1, SdrView& rNewView)
:	mrView(rNewView),
	// #103911# col_auto color lets the view takes the default SvxColorConfig entry
	maDocumentColor( COL_AUTO ),
	maBackgroundColor(COL_AUTO ), // #i48367# also react on autocolor
	mpPreparedPageWindow(0) // #i72752#
{
	DBG_CTOR(SdrPageView,NULL);
	mpPage = pPage1;

	if(mpPage)
	{
		aPgOrg.X()=mpPage->GetLftBorder();
		aPgOrg.Y()=mpPage->GetUppBorder();
	}
	mbHasMarked = sal_False;
	aLayerVisi.SetAll();
	aLayerPrn.SetAll();

	mbVisible = sal_False;
	pAktList = NULL;
	pAktGroup = NULL;
	SetAktGroupAndList(NULL, mpPage);

	StartListening(*rNewView.GetModel());

	for(sal_uInt32 a(0L); a < rNewView.PaintWindowCount(); a++)
	{
		AddPaintWindowToPageView(*rNewView.GetPaintWindow(a));
	}
}

SdrPageView::~SdrPageView()
{
	DBG_DTOR(SdrPageView,NULL);

	// cleanup window vector
	ClearPageWindows();
}

SdrPageWindow& SdrPageView::CreateNewPageWindowEntry(SdrPaintWindow& rPaintWindow)
{
	// MIB 3.7.08: Das WinRec muss sofort in die Liste eingetragen werden,
	// weil sich das InsertControlContainer darauf verlaesst
	//SdrPageViewWinRec* pRec = new SdrPageViewWinRec( *this, pOut );
	//pWinList->Insert(pRec);
	SdrPageWindow& rWindow = *(new SdrPageWindow(*this, rPaintWindow));
	AppendPageWindow(rWindow);

	return rWindow;
}

void SdrPageView::AddPaintWindowToPageView(SdrPaintWindow& rPaintWindow)
{
	if(!FindPageWindow(rPaintWindow))
	{
		CreateNewPageWindowEntry(rPaintWindow);
	}
}

void SdrPageView::RemovePaintWindowFromPageView(SdrPaintWindow& rPaintWindow)
{
	SdrPageWindow* pCandidate = FindPageWindow(rPaintWindow);

	if(pCandidate)
	{
		pCandidate = RemovePageWindow(*pCandidate);

		if(pCandidate)
		{
			delete pCandidate;
		}
	}
}

::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > SdrPageView::GetControlContainer( const OutputDevice& _rDevice ) const
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > xReturn;
	const SdrPageWindow* pCandidate = FindPatchedPageWindow( _rDevice );

	if ( pCandidate )
        xReturn = pCandidate->GetControlContainer( true );

	return xReturn;
}

void __EXPORT SdrPageView::Notify(SfxBroadcaster& /*rBC*/, const SfxHint& /*rHint*/)
{
    // not really interested in
}

void SdrPageView::ModelHasChanged()
{
	if (GetAktGroup()!=NULL) CheckAktGroup();
}

sal_Bool SdrPageView::IsReadOnly() const
{
	return (0L == GetPage() || GetView().GetModel()->IsReadOnly() || GetPage()->IsReadOnly() || GetObjList()->IsReadOnly());
}

void SdrPageView::Show()
{
	if(!IsVisible())
	{
		mbVisible = sal_True;
		InvalidateAllWin();

		for(sal_uInt32 a(0L); a < GetView().PaintWindowCount(); a++)
		{
			AddPaintWindowToPageView(*GetView().GetPaintWindow(a));
		}
	}
}

void SdrPageView::Hide()
{
	if(IsVisible())
	{
		InvalidateAllWin();
		mbVisible = sal_False;
		ClearPageWindows();
	}
}

Rectangle SdrPageView::GetPageRect() const
{
	if (GetPage()==NULL) return Rectangle();
	return Rectangle(Point(),Size(GetPage()->GetWdt()+1,GetPage()->GetHgt()+1));
}

void SdrPageView::InvalidateAllWin()
{
	if(IsVisible() && GetPage())
	{
		Rectangle aRect(Point(0,0),Size(GetPage()->GetWdt()+1,GetPage()->GetHgt()+1));
		aRect.Union(GetPage()->GetAllObjBoundRect());
		GetView().InvalidateAllWin(aRect);
	}
}

void SdrPageView::InvalidateAllWin(const Rectangle& rRect, sal_Bool bPlus1Pix)
{
	if(IsVisible())
	{
		GetView().InvalidateAllWin(rRect, bPlus1Pix);
	}
}

void SdrPageView::PaintOutlinerView(OutputDevice* pOut, const Rectangle& rRect) const
{
	if (GetView().pTextEditOutliner==NULL) return;
	//const SdrObject* pTextObjTmp=GetView().GetTextEditObject();
	//const SdrTextObj* pText=PTR_CAST(SdrTextObj,pTextObjTmp);
	//FASTBOOL bTextFrame=pText!=NULL && pText->IsTextFrame();
	sal_uIntPtr nViewAnz=GetView().pTextEditOutliner->GetViewCount();
	for (sal_uIntPtr i=0; i<nViewAnz; i++) {
		OutlinerView* pOLV=GetView().pTextEditOutliner->GetView(i);
		if (pOLV->GetWindow()==pOut) {
			GetView().ImpPaintOutlinerView(*pOLV, rRect);
			return;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SdrPageView::PrePaint()
{
	const sal_uInt32 nCount(PageWindowCount());

    for(sal_uInt32 a(0); a < nCount; a++)
    {
        SdrPageWindow* pCandidate = GetPageWindow(a);
        
        if(pCandidate)
        {
            pCandidate->PrePaint();
        }
    }
}

void SdrPageView::PostPaint()
{
	const sal_uInt32 nCount(PageWindowCount());

    for(sal_uInt32 a(0); a < nCount; a++)
    {
        SdrPageWindow* pCandidate = GetPageWindow(a);
        
        if(pCandidate)
        {
            pCandidate->PostPaint();
        }
    }
}

void SdrPageView::CompleteRedraw(SdrPaintWindow& rPaintWindow, const Region& rReg, sdr::contact::ViewObjectContactRedirector* pRedirector) const
{
	if(GetPage())
	{
		SdrPageWindow* pPageWindow = FindPageWindow(rPaintWindow);
		sal_Bool bIsTempTarget(sal_False);

		if(!pPageWindow)
		{
			// create temp PageWindow
			pPageWindow = new SdrPageWindow(*((SdrPageView*)this), rPaintWindow);
			bIsTempTarget = sal_True;
		}

		// do the redraw
		pPageWindow->PrepareRedraw(rReg);
		pPageWindow->RedrawAll(pRedirector);

		// get rid of temp PageWindow
		if(bIsTempTarget)
		{
			delete pPageWindow;
			pPageWindow = 0L;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// #i74769# use SdrPaintWindow directly

void SdrPageView::setPreparedPageWindow(SdrPageWindow* pKnownTarget)
{
	// #i72752# remember prepared SdrPageWindow
	mpPreparedPageWindow = pKnownTarget;
}

void SdrPageView::DrawLayer(SdrLayerID nID, OutputDevice* pGivenTarget, sdr::contact::ViewObjectContactRedirector* pRedirector) const
{
	if(GetPage())
	{
		if(pGivenTarget)
		{
			const SdrPageWindow* pKnownTarget = FindPageWindow(*pGivenTarget);

			if(pKnownTarget)
			{
				// paint known target
				pKnownTarget->RedrawLayer(&nID, pRedirector);
			}
			else
			{
				// #i72752# DrawLayer() uses a OutputDevice different from BeginDrawLayer. This happens
				// e.g. when SW paints a single text line in text edit mode. Try to use it
				SdrPageWindow* pPreparedTarget = mpPreparedPageWindow;

				if(pPreparedTarget)
				{
					// if we have a prepared target, do not use a new SdrPageWindow since this 
					// works but is expensive. Just use a temporary PaintWindow
					SdrPaintWindow aTemporaryPaintWindow(mrView, *pGivenTarget);

					// Copy existing paint region to use the same as prepared in BeginDrawLayer
					SdrPaintWindow& rExistingPaintWindow = pPreparedTarget->GetPaintWindow();
					const Region& rExistingRegion = rExistingPaintWindow.GetRedrawRegion();
					aTemporaryPaintWindow.SetRedrawRegion(rExistingRegion);

					// patch the ExistingPageWindow
					pPreparedTarget->patchPaintWindow(aTemporaryPaintWindow);

					// redraw the layer
					pPreparedTarget->RedrawLayer(&nID, pRedirector);

					// restore the ExistingPageWindow
					pPreparedTarget->unpatchPaintWindow();
				}
				else
				{
					OSL_ENSURE(false, "SdrPageView::DrawLayer: Creating temporary SdrPageWindow (ObjectContact), this should never be needed (!)");

					// None of the known OutputDevices is the target of this paint, use
					// a temporary SdrPageWindow for this Redraw.
					SdrPaintWindow aTemporaryPaintWindow(mrView, *pGivenTarget);
					SdrPageWindow aTemporaryPageWindow(*((SdrPageView*)this), aTemporaryPaintWindow);

					// #i72752#
					// Copy existing paint region if other PageWindows exist, this was created by
					// PrepareRedraw() from BeginDrawLayer(). Needs to be used e.g. when suddenly SW
					// paints into an unknown device other than the view was created for (e.g. VirtualDevice)
					if(PageWindowCount())
					{
						SdrPageWindow* pExistingPageWindow = GetPageWindow(0L);
						SdrPaintWindow& rExistingPaintWindow = pExistingPageWindow->GetPaintWindow();
						const Region& rExistingRegion = rExistingPaintWindow.GetRedrawRegion();
						aTemporaryPaintWindow.SetRedrawRegion(rExistingRegion);
					}

					aTemporaryPageWindow.RedrawLayer(&nID, pRedirector);
				}
			}
		}
		else
		{
			// paint in all known windows
			for(sal_uInt32 a(0L); a < PageWindowCount(); a++)
			{
				SdrPageWindow* pTarget = GetPageWindow(a);
				pTarget->RedrawLayer(&nID, pRedirector);
			}
		}
	}
}

void SdrPageView::SetDesignMode( bool _bDesignMode ) const
{
    for ( sal_uInt32 i = 0L; i < PageWindowCount(); ++i )
    {
        const SdrPageWindow& rPageViewWindow = *GetPageWindow(i);
        rPageViewWindow.SetDesignMode( _bDesignMode );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef OS2
#define RGBCOLOR(r,g,b) ((sal_uIntPtr)(((sal_uInt8)(b) | ((sal_uInt16)(g)<<8)) | (((sal_uIntPtr)(sal_uInt8)(r))<<16)))
#endif

void SdrPageView::DrawPageViewGrid(OutputDevice& rOut, const Rectangle& rRect, Color aColor)
{
	if (GetPage()==NULL)
		return;

	long nx1=GetView().aGridBig.Width();
	long nx2=GetView().aGridFin.Width();
	long ny1=GetView().aGridBig.Height();
	long ny2=GetView().aGridFin.Height();

	if (nx1==0) nx1=nx2;
	if (nx2==0) nx2=nx1;
	if (ny1==0) ny1=ny2;
	if (ny2==0) ny2=ny1;
	if (nx1==0) { nx1=ny1; nx2=ny2; }
	if (ny1==0) { ny1=nx1; ny2=nx2; }
	if (nx1<0) nx1=-nx1;
	if (nx2<0) nx2=-nx2;
	if (ny1<0) ny1=-ny1;
	if (ny2<0) ny2=-ny2;

	if (nx1!=0)
	{
		// no more global output size, use window size instead to decide grid sizes
		long nScreenWdt = rOut.GetOutputSizePixel().Width();
		// old: long nScreenWdt=System::GetDesktopRectPixel().GetWidth();

		// Grid bei kleinen Zoomstufen etwas erweitern
		//Size a1PixSiz(rOut.PixelToLogic(Size(1,1)));
		long nMinDotPix=2;
		long nMinLinPix=4;

		if (nScreenWdt>=1600)
		{
			nMinDotPix=4;
			nMinLinPix=8;
		}
		else if (nScreenWdt>=1024)
		{
			nMinDotPix=3;
			nMinLinPix=6;
		}
		else
		{ // z.B. 640x480
			nMinDotPix=2;
			nMinLinPix=4;
		}
		Size aMinDotDist(rOut.PixelToLogic(Size(nMinDotPix,nMinDotPix)));
		//Size a3PixSiz(rOut.PixelToLogic(Size(2,2)));
		Size aMinLinDist(rOut.PixelToLogic(Size(nMinLinPix,nMinLinPix)));
		FASTBOOL bHoriSolid=nx2<aMinDotDist.Width();
		FASTBOOL bVertSolid=ny2<aMinDotDist.Height();
		// Linienabstand vergroessern (mind. 4 Pixel)
		// Vergroesserung: *2 *5 *10 *20 *50 *100 ...
		int nTgl=0;
		long nVal0=nx1;
		while (nx1<aMinLinDist.Width())
		{
			long a=nx1;

			if (nTgl==0) nx1*=2;
			if (nTgl==1) nx1=nVal0*5; // => nx1*=2.5
			if (nTgl==2) nx1*=2;

			nVal0=a;
			nTgl++; if (nTgl>=3) nTgl=0;
		}
		nTgl=0;
		nVal0=ny1;
		while (ny1<aMinLinDist.Height())
		{
			long a=ny1;

			if (nTgl==0) ny1*=2;
			if (nTgl==1) ny1=nVal0*5; // => ny1*=2.5
			if (nTgl==2) ny1*=2;

			nVal0=a;
			nTgl++;

			if (nTgl>=3) nTgl=0;
		}
		// Keine Zwischenpunkte, wenn...
		//if (nx2<a2PixSiz.Width()) nx2=nx1;
		//if (ny2<a2PixSiz.Height()) ny2=ny1;

		FASTBOOL bHoriFine=nx2<nx1;
		FASTBOOL bVertFine=ny2<ny1;
		FASTBOOL bHoriLines=bHoriSolid || bHoriFine || !bVertFine;
		FASTBOOL bVertLines=bVertSolid || bVertFine;

		Color aColorMerk( rOut.GetLineColor() );
		rOut.SetLineColor( aColor );

		bool bMap0=rOut.IsMapModeEnabled();

		long nWrX=0;//aWriterPageOffset.X();
		long nWrY=0;//aWriterPageOffset.Y();
		Point aOrg(aPgOrg);
		long x1=GetPage()->GetLftBorder()+1+nWrX;
		long x2=GetPage()->GetWdt()-GetPage()->GetRgtBorder()-1+nWrY;
		long y1=GetPage()->GetUppBorder()+1+nWrX;
		long y2=GetPage()->GetHgt()-GetPage()->GetLwrBorder()-1+nWrY;
		const SdrPageGridFrameList* pFrames=GetPage()->GetGridFrameList(this,NULL);
		//sal_uInt16 nBufSiz=1024; // 4k Buffer = max. 512 Punkte
		// #90353# long* pBuf = NULL;
		sal_uInt16 nGridPaintAnz=1;
		if (pFrames!=NULL) nGridPaintAnz=pFrames->GetCount();
		for (sal_uInt16 nGridPaintNum=0; nGridPaintNum<nGridPaintAnz; nGridPaintNum++) {
			if (pFrames!=NULL) {
				const SdrPageGridFrame& rGF=(*pFrames)[nGridPaintNum];
				nWrX=rGF.GetPaperRect().Left();
				nWrY=rGF.GetPaperRect().Top();
				x1=rGF.GetUserArea().Left();
				x2=rGF.GetUserArea().Right();
				y1=rGF.GetUserArea().Top();
				y2=rGF.GetUserArea().Bottom();
				aOrg=rGF.GetUserArea().TopLeft();
				aOrg-=rGF.GetPaperRect().TopLeft();
			}
			if (!rRect.IsEmpty()) {
				Size a1PixSiz(rOut.PixelToLogic(Size(1,1)));
				long nX1Pix=a1PixSiz.Width();  // 1 Pixel Toleranz drauf
				long nY1Pix=a1PixSiz.Height();
				if (x1<rRect.Left()  -nX1Pix) x1=rRect.Left()  -nX1Pix;
				if (x2>rRect.Right() +nX1Pix) x2=rRect.Right() +nX1Pix;
				if (y1<rRect.Top()	 -nY1Pix) y1=rRect.Top()   -nY1Pix;
				if (y2>rRect.Bottom()+nY1Pix) y2=rRect.Bottom()+nY1Pix;
			}
			Point aPnt;

			long xBigOrg=aOrg.X()+nWrX;
			while (xBigOrg>=x1) xBigOrg-=nx1;
			while (xBigOrg<x1) xBigOrg+=nx1;
			long xFinOrg=xBigOrg;
			while (xFinOrg>=x1) xFinOrg-=nx2;
			while (xFinOrg<x1) xFinOrg+=nx2;

			long yBigOrg=aOrg.Y()+nWrY;
			while (yBigOrg>=y1) yBigOrg-=ny1;
			while (yBigOrg<y1) yBigOrg+=ny1;
			long yFinOrg=yBigOrg;
			while (yFinOrg>=y1) yFinOrg-=ny2;
			while (yFinOrg<y1) yFinOrg+=ny2;

			if( x1 <= x2 && y1 <= y2 )
			{
				if( bHoriLines )
				{
					sal_uIntPtr nGridFlags = ( bHoriSolid ? GRID_HORZLINES : GRID_DOTS );
					sal_uInt16 nSteps = sal_uInt16(nx1 / nx2);
					sal_uInt32 nRestPerStepMul1000 = nSteps ? ( ((nx1 * 1000L)/ nSteps) - (nx2 * 1000L) ) : 0;
					sal_uInt32 nStepOffset = 0;
					sal_uInt16 nPointOffset = 0;

					for(sal_uInt16 a=0;a<nSteps;a++)
					{
						// Zeichnen
						rOut.DrawGrid(
							Rectangle( xFinOrg + (a * nx2) + nPointOffset, yBigOrg, x2, y2 ),
							Size( nx1, ny1 ), nGridFlags );

						// Schritt machen
						nStepOffset += nRestPerStepMul1000;
						while(nStepOffset >= 1000)
						{
							nStepOffset -= 1000;
							nPointOffset++;
						}
					}
				}

				if( bVertLines )
				{
					sal_uIntPtr nGridFlags = ( bVertSolid ? GRID_VERTLINES : GRID_DOTS );
					sal_uInt16 nSteps = sal_uInt16(ny1 / ny2);
					sal_uInt32 nRestPerStepMul1000 = nSteps ? ( ((ny1 * 1000L)/ nSteps) - (ny2 * 1000L) ) : 0;
					sal_uInt32 nStepOffset = 0;
					sal_uInt16 nPointOffset = 0;

					for(sal_uInt16 a=0;a<nSteps;a++)
					{
						// Zeichnen
						rOut.DrawGrid(
							Rectangle( xBigOrg, yFinOrg + (a * ny2) + nPointOffset, x2, y2 ),
							Size( nx1, ny1 ), nGridFlags );

						// Schritt machen
						nStepOffset += nRestPerStepMul1000;
						while(nStepOffset >= 1000)
						{
							nStepOffset -= 1000;
							nPointOffset++;
						}
					}

					// rOut.DrawGrid( Rectangle( xo + xBigOrg, yo + yFinOrg, x2, y2 ), Size( nx1, ny2 ), nGridFlags );
				}
			}
		}

		rOut.EnableMapMode(bMap0);
		rOut.SetLineColor(aColorMerk);
	}
}

void SdrPageView::AdjHdl()
{
	GetView().AdjustMarkHdl();
}

void SdrPageView::SetLayer(const XubString& rName, SetOfByte& rBS, sal_Bool bJa)
{
	if(!GetPage())
		return;

	SdrLayerID nID = GetPage()->GetLayerAdmin().GetLayerID(rName, sal_True);

	if(SDRLAYER_NOTFOUND != nID)
		rBS.Set(nID, bJa);
}

sal_Bool SdrPageView::IsLayer(const XubString& rName, const SetOfByte& rBS) const
{
	if(!GetPage())
		return sal_False;

	sal_Bool bRet(sal_False);

	if(rName.Len())
	{
		SdrLayerID nId = GetPage()->GetLayerAdmin().GetLayerID(rName, sal_True);

		if(SDRLAYER_NOTFOUND != nId)
		{
			bRet = rBS.IsSet(nId);
		}
	}

	return bRet;
}

void SdrPageView::SetAllLayers(SetOfByte& rB, sal_Bool bJa)
{
	if(bJa)
	{
		rB.SetAll();
		rB.Clear(SDRLAYER_NOTFOUND);
	}
	else
	{
		rB.ClearAll();
	}
}

sal_Bool SdrPageView::IsObjMarkable(SdrObject* pObj) const
{
	if(pObj)
	{
		// Vom Markieren ausgeschlossen?
		if(pObj->IsMarkProtect())
		{
			return sal_False;
		}

		// only visible are markable
		if( !pObj->IsVisible() )
		{
			return sal_False;
		}

		// #112440#
		if(pObj->ISA(SdrObjGroup))
		{
			// If object is a Group object, visibility depends evtl. on
			// multiple layers. If one object is markable, Group is markable.
			SdrObjList* pObjList = ((SdrObjGroup*)pObj)->GetSubList();

			if(pObjList && pObjList->GetObjCount())
			{
				sal_Bool bGroupIsMarkable(sal_False);

				for(sal_uInt32 a(0L); !bGroupIsMarkable && a < pObjList->GetObjCount(); a++)
				{
					SdrObject* pCandidate = pObjList->GetObj(a);

					// call recursively
					if(IsObjMarkable(pCandidate))
					{
						bGroupIsMarkable = sal_True;
					}
				}

				return bGroupIsMarkable;
			}
			else
			{
				// #i43302#
				// Allow empty groups to be selected to be able to delete them
				return sal_True;
			}
		}
		else
		{
			// Der Layer muss sichtbar und darf nicht gesperrt sein
			SdrLayerID nL = pObj->GetLayer();
			return (aLayerVisi.IsSet(sal_uInt8(nL)) && !aLayerLock.IsSet(sal_uInt8(nL)));
		}
	}

	return sal_False;
}

void SdrPageView::SetPageOrigin(const Point& rOrg)
{
	if (rOrg!=aPgOrg) {
		aPgOrg=rOrg;
		if (GetView().IsGridVisible()) {
			InvalidateAllWin();
		}
	}
}

void SdrPageView::ImpInvalidateHelpLineArea(sal_uInt16 nNum) const
{
	if (GetView().IsHlplVisible() && nNum<aHelpLines.GetCount()) {
		const SdrHelpLine& rHL=aHelpLines[nNum];

		for(sal_uInt32 a(0L); a < GetView().PaintWindowCount(); a++)
		{
			SdrPaintWindow* pCandidate = GetView().GetPaintWindow(a);

			if(pCandidate->OutputToWindow())
			{
				OutputDevice& rOutDev = pCandidate->GetOutputDevice();
				Rectangle aR(rHL.GetBoundRect(rOutDev));
				Size aSiz(rOutDev.PixelToLogic(Size(1,1)));
				aR.Left() -= aSiz.Width();
				aR.Right() += aSiz.Width();
				aR.Top() -= aSiz.Height();
				aR.Bottom() += aSiz.Height();
				((SdrView&)GetView()).InvalidateOneWin((Window&)rOutDev, aR);
			}
		}
	}
}

void SdrPageView::SetHelpLines(const SdrHelpLineList& rHLL)
{
	aHelpLines=rHLL;
	InvalidateAllWin();
}

void SdrPageView::SetHelpLine(sal_uInt16 nNum, const SdrHelpLine& rNewHelpLine)
{
	if (nNum<aHelpLines.GetCount() && aHelpLines[nNum]!=rNewHelpLine) {
		FASTBOOL bNeedRedraw=sal_True;
		if (aHelpLines[nNum].GetKind()==rNewHelpLine.GetKind()) {
			switch (rNewHelpLine.GetKind()) {
				case SDRHELPLINE_VERTICAL  : if (aHelpLines[nNum].GetPos().X()==rNewHelpLine.GetPos().X()) bNeedRedraw=sal_False; break;
				case SDRHELPLINE_HORIZONTAL: if (aHelpLines[nNum].GetPos().Y()==rNewHelpLine.GetPos().Y()) bNeedRedraw=sal_False; break;
				default: break;
			} // switch
		}
		if (bNeedRedraw) ImpInvalidateHelpLineArea(nNum);
		aHelpLines[nNum]=rNewHelpLine;
		if (bNeedRedraw) ImpInvalidateHelpLineArea(nNum);
	}
}

void SdrPageView::DeleteHelpLine(sal_uInt16 nNum)
{
	if (nNum<aHelpLines.GetCount()) {
		ImpInvalidateHelpLineArea(nNum);
		aHelpLines.Delete(nNum);
	}
}

void SdrPageView::InsertHelpLine(const SdrHelpLine& rHL, sal_uInt16 nNum)
{
	if (nNum>aHelpLines.GetCount()) nNum=aHelpLines.GetCount();
	aHelpLines.Insert(rHL,nNum);
	if (GetView().IsHlplVisible()) {
		if (GetView().IsHlplFront()) {
			// Hier optimieren ...
			ImpInvalidateHelpLineArea(nNum);
		 } else {
			ImpInvalidateHelpLineArea(nNum);
		}
	}
}

// Betretene Gruppe und Liste setzen
void SdrPageView::SetAktGroupAndList(SdrObject* pNewGroup, SdrObjList* pNewList)
{
	if(pAktGroup != pNewGroup)
	{
		pAktGroup = pNewGroup;
	}
	if(pAktList != pNewList)
	{
		pAktList = pNewList;
	}
}

sal_Bool SdrPageView::EnterGroup(SdrObject* pObj)
{
	sal_Bool bRet(sal_False);

	if(pObj && pObj->IsGroupObject())
	{
		sal_Bool bGlueInvalidate(GetView().ImpIsGlueVisible());

		if(bGlueInvalidate)
		{
			GetView().GlueInvalidate();
		}

		// deselect all
		GetView().UnmarkAll();

		// set current group and list
		SdrObjList* pNewObjList = pObj->GetSubList();
		SetAktGroupAndList(pObj, pNewObjList);

		// select contained object if only one object is contained,
		// else select nothing and let the user decide what to do next
		if(pNewObjList && pNewObjList->GetObjCount() == 1)
		{
			SdrObject* pFirstObject = pNewObjList->GetObj(0L);

			if(GetView().GetSdrPageView())
			{
				GetView().MarkObj(pFirstObject, GetView().GetSdrPageView());
			}
		}

		// build new handles
		GetView().AdjustMarkHdl();

		// invalidate only when view wants to visualize group entering
		if(GetView().DoVisualizeEnteredGroup())
		{
			InvalidateAllWin();
		}

		if (bGlueInvalidate)
		{
			GetView().GlueInvalidate();
		}

		bRet = sal_True;
	}

	return bRet;
}

void SdrPageView::LeaveOneGroup()
{
	if(GetAktGroup())
	{
		sal_Bool bGlueInvalidate = (GetView().ImpIsGlueVisible());

		if(bGlueInvalidate)
			GetView().GlueInvalidate();

		SdrObject* pLastGroup = GetAktGroup();
		SdrObject* pParentGroup = GetAktGroup()->GetUpGroup();
		SdrObjList* pParentList = GetPage();

		if(pParentGroup)
			pParentList = pParentGroup->GetSubList();

		// Alles deselektieren
		GetView().UnmarkAll();

		// Zuweisungen, pAktGroup und pAktList muessen gesetzt sein
		SetAktGroupAndList(pParentGroup, pParentList);

		// gerade verlassene Gruppe selektieren
		if(pLastGroup)
			if(GetView().GetSdrPageView())
				GetView().MarkObj(pLastGroup, GetView().GetSdrPageView());

		GetView().AdjustMarkHdl();

		// invalidate only when view wants to visualize group entering
		if(GetView().DoVisualizeEnteredGroup())
			InvalidateAllWin();

		if(bGlueInvalidate)
			GetView().GlueInvalidate();
	}
}

void SdrPageView::LeaveAllGroup()
{
	if(GetAktGroup())
	{
		sal_Bool bGlueInvalidate = (GetView().ImpIsGlueVisible());

		if(bGlueInvalidate)
			GetView().GlueInvalidate();

		SdrObject* pLastGroup = GetAktGroup();

		// Alles deselektieren
		GetView().UnmarkAll();

		// Zuweisungen, pAktGroup und pAktList muessen gesetzt sein
		SetAktGroupAndList(NULL, GetPage());

		// Oberste letzte Gruppe finden und selektieren
		if(pLastGroup)
		{
			while(pLastGroup->GetUpGroup())
				pLastGroup = pLastGroup->GetUpGroup();

			if(GetView().GetSdrPageView())
				GetView().MarkObj(pLastGroup, GetView().GetSdrPageView());
		}

		GetView().AdjustMarkHdl();

		// invalidate only when view wants to visualize group entering
		if(GetView().DoVisualizeEnteredGroup())
			InvalidateAllWin();

		if(bGlueInvalidate)
			GetView().GlueInvalidate();
	}
}

sal_uInt16 SdrPageView::GetEnteredLevel() const
{
	sal_uInt16 nAnz=0;
	SdrObject* pGrp=GetAktGroup();
	while (pGrp!=NULL) {
		nAnz++;
		pGrp=pGrp->GetUpGroup();
	}
	return nAnz;
}

XubString SdrPageView::GetActualGroupName() const
{
	if(GetAktGroup())
	{
		XubString aStr(GetAktGroup()->GetName());

		if(!aStr.Len())
			aStr += sal_Unicode('?');

		return aStr;
	}
	else
		return String();
}

XubString SdrPageView::GetActualPathName(sal_Unicode cSep) const
{
	XubString aStr;
	sal_Bool bNamFnd(sal_False);
	SdrObject* pGrp = GetAktGroup();

	while(pGrp)
	{
		XubString aStr1(pGrp->GetName());

		if(!aStr1.Len())
			aStr1 += sal_Unicode('?');
		else
			bNamFnd = sal_True;

		aStr += aStr1;
		pGrp = pGrp->GetUpGroup();

		if(pGrp)
			aStr += cSep;
	}

	if(!bNamFnd && GetAktGroup())
	{
		aStr = String();
		aStr += sal_Unicode('(');
		aStr += String::CreateFromInt32( GetEnteredLevel() );
		aStr += sal_Unicode(')');
	}

	return aStr;
}

void SdrPageView::CheckAktGroup()
{
	SdrObject* pGrp=GetAktGroup();
	while (pGrp!=NULL &&
		   (!pGrp->IsInserted() || pGrp->GetObjList()==NULL ||
			pGrp->GetPage()==NULL || pGrp->GetModel()==NULL)) { // irgendwas daneben?
		pGrp=pGrp->GetUpGroup();
	}
	if (pGrp!=GetAktGroup()) {
		if (pGrp!=NULL) EnterGroup(pGrp);
		else LeaveAllGroup();
	}
}

// #103834# Set background color for svx at SdrPageViews
void SdrPageView::SetApplicationBackgroundColor(Color aBackgroundColor)
{
	maBackgroundColor = aBackgroundColor;
}

// #109585#
Color SdrPageView::GetApplicationBackgroundColor() const
{
	return maBackgroundColor;
}

// #103911# Set document color for svx at SdrPageViews
void SdrPageView::SetApplicationDocumentColor(Color aDocumentColor)
{
	maDocumentColor = aDocumentColor;
}

Color SdrPageView::GetApplicationDocumentColor() const
{
	return maDocumentColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// eof
