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
#include "precompiled_vcl.hxx"

#include <tools/debug.hxx>

#include <vcl/settings.hxx>
#include <vcl/svapp.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/virdev.hxx>

#include <salinst.hxx>
#include <salgdi.hxx>
#include <salframe.hxx>
#include <salvd.hxx>
#include <outdev.h>
#include <svdata.hxx>

using namespace ::com::sun::star::uno;

// =======================================================================

void VirtualDevice::ImplInitVirDev( const OutputDevice* pOutDev,
									long nDX, long nDY, sal_uInt16 nBitCount, const SystemGraphicsData *pData )
{
	DBG_ASSERT( nBitCount <= 1,
				"VirtualDevice::VirtualDevice(): Only 0 or 1 is for BitCount allowed" );

	if ( nDX < 1 )
		nDX = 1;

	if ( nDY < 1 )
		nDY = 1;

	ImplSVData* pSVData = ImplGetSVData();

	if ( !pOutDev )
		pOutDev = ImplGetDefaultWindow();
    if( !pOutDev )
        return;

	SalGraphics* pGraphics;
	if ( !pOutDev->mpGraphics )
		((OutputDevice*)pOutDev)->ImplGetGraphics();
	pGraphics = pOutDev->mpGraphics;
	if ( pGraphics )
		mpVirDev = pSVData->mpDefInst->CreateVirtualDevice( pGraphics, nDX, nDY, nBitCount, pData );
	else
		mpVirDev = NULL;
	if ( !mpVirDev )
    {
        // do not abort but throw an exception, may be the current thread terminates anyway (plugin-scenario)
        throw ::com::sun::star::uno::RuntimeException( 
            OUString( RTL_CONSTASCII_USTRINGPARAM( "Could not create system bitmap!" ) ),
            ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >() );
		//GetpApp()->Exception( EXC_SYSOBJNOTCREATED );
    }

	mnBitCount		= ( nBitCount ? nBitCount : pOutDev->GetBitCount() );
	mnOutWidth		= nDX;
	mnOutHeight 	= nDY;
	mbScreenComp	= sal_True;
    mnAlphaDepth	= -1;

    // #i59315# init vdev size from system object, when passed a
    // SystemGraphicsData. Otherwise, output size will always
    // incorrectly stay at (1,1)
    if( pData && mpVirDev )
        mpVirDev->GetSize(mnOutWidth,mnOutHeight);

    if( mnBitCount < 8 )
        SetAntialiasing( ANTIALIASING_DISABLE_TEXT );

	if ( pOutDev->GetOutDevType() == OUTDEV_PRINTER )
		mbScreenComp = sal_False;
	else if ( pOutDev->GetOutDevType() == OUTDEV_VIRDEV )
		mbScreenComp = ((VirtualDevice*)pOutDev)->mbScreenComp;

	meOutDevType	= OUTDEV_VIRDEV;
	mbDevOutput 	= sal_True;
	mpFontList		= pSVData->maGDIData.mpScreenFontList;
	mpFontCache 	= pSVData->maGDIData.mpScreenFontCache;
	mnDPIX			= pOutDev->mnDPIX;
	mnDPIY			= pOutDev->mnDPIY;
	maFont			= pOutDev->maFont;

    if( maTextColor != pOutDev->maTextColor )
    {
        maTextColor	= pOutDev->maTextColor;
        mbInitTextColor = true;
    }

	// Virtuelle Devices haben defaultmaessig einen weissen Hintergrund
	SetBackground( Wallpaper( Color( COL_WHITE ) ) );

    // #i59283# don't erase user-provided surface
    if( !pData )
        Erase();

	// VirDev in Liste eintragen
	mpNext = pSVData->maGDIData.mpFirstVirDev;
	mpPrev = NULL;
	if ( mpNext )
		mpNext->mpPrev = this;
	else
		pSVData->maGDIData.mpLastVirDev = this;
	pSVData->maGDIData.mpFirstVirDev = this;
}

// -----------------------------------------------------------------------

VirtualDevice::VirtualDevice( sal_uInt16 nBitCount )
:   mpVirDev( NULL ),
    meRefDevMode( REFDEV_NONE )
{
	DBG_TRACE1( "VirtualDevice::VirtualDevice( %hu )", nBitCount );

	ImplInitVirDev( Application::GetDefaultDevice(), 1, 1, nBitCount );
}

// -----------------------------------------------------------------------

VirtualDevice::VirtualDevice( const OutputDevice& rCompDev, sal_uInt16 nBitCount )
	: mpVirDev( NULL ),
    meRefDevMode( REFDEV_NONE )
{
	DBG_TRACE1( "VirtualDevice::VirtualDevice( %hu )", nBitCount );

	ImplInitVirDev( &rCompDev, 1, 1, nBitCount );
}

// -----------------------------------------------------------------------

VirtualDevice::VirtualDevice( const OutputDevice& rCompDev, sal_uInt16 nBitCount, sal_uInt16 nAlphaBitCount )
	: mpVirDev( NULL ),
    meRefDevMode( REFDEV_NONE )
{
	DBG_TRACE1( "VirtualDevice::VirtualDevice( %hu )", nBitCount );

	ImplInitVirDev( &rCompDev, 1, 1, nBitCount );

    // #110958# Enable alpha channel
    mnAlphaDepth = sal::static_int_cast<sal_Int8>(nAlphaBitCount);
}

// -----------------------------------------------------------------------

VirtualDevice::VirtualDevice( const SystemGraphicsData *pData, sal_uInt16 nBitCount )
:   mpVirDev( NULL ),
    meRefDevMode( REFDEV_NONE )
{
	DBG_TRACE1( "VirtualDevice::VirtualDevice( %hu )", nBitCount );

	ImplInitVirDev( Application::GetDefaultDevice(), 1, 1, nBitCount, pData );
}

// -----------------------------------------------------------------------

VirtualDevice::~VirtualDevice()
{
	DBG_TRACE( "VirtualDevice::~VirtualDevice()" );

    ImplSVData* pSVData = ImplGetSVData();

	ImplReleaseGraphics();

	if ( mpVirDev )
		pSVData->mpDefInst->DestroyVirtualDevice( mpVirDev );

	// remove this VirtualDevice from the double-linked global list
	if( mpPrev )
		mpPrev->mpNext = mpNext;
	else
		pSVData->maGDIData.mpFirstVirDev = mpNext;

	if( mpNext )
		mpNext->mpPrev = mpPrev;
	else
		pSVData->maGDIData.mpLastVirDev = mpPrev;
}

// -----------------------------------------------------------------------

sal_Bool VirtualDevice::ImplSetOutputSizePixel( const Size& rNewSize, sal_Bool bErase )
{
	DBG_TRACE3( "VirtualDevice::ImplSetOutputSizePixel( %ld, %ld, %d )", rNewSize.Width(), rNewSize.Height(), (int)bErase );

	if ( !mpVirDev )
		return sal_False;
	else if ( rNewSize == GetOutputSizePixel() )
	{
		if ( bErase )
			Erase();
		return sal_True;
	}

	sal_Bool bRet;
	long nNewWidth = rNewSize.Width(), nNewHeight = rNewSize.Height();

	if ( nNewWidth < 1 )
		nNewWidth = 1;

	if ( nNewHeight < 1 )
		nNewHeight = 1;

	if ( bErase )
	{
		bRet = mpVirDev->SetSize( nNewWidth, nNewHeight );

		if ( bRet )
		{
			mnOutWidth	= rNewSize.Width();
			mnOutHeight = rNewSize.Height();
			Erase();
		}
	}
	else
	{
		SalVirtualDevice*	pNewVirDev;
		ImplSVData* 		pSVData = ImplGetSVData();

		// we need a graphics
		if ( !mpGraphics )
		{
			if ( !ImplGetGraphics() )
				return sal_False;
		}

		pNewVirDev = pSVData->mpDefInst->CreateVirtualDevice( mpGraphics, nNewWidth, nNewHeight, mnBitCount );
		if ( pNewVirDev )
		{
			SalGraphics* pGraphics = pNewVirDev->GetGraphics();
			if ( pGraphics )
			{
				SalTwoRect aPosAry;
				long nWidth;
				long nHeight;
				if ( mnOutWidth < nNewWidth )
					nWidth = mnOutWidth;
				else
					nWidth = nNewWidth;
				if ( mnOutHeight < nNewHeight )
					nHeight = mnOutHeight;
				else
					nHeight = nNewHeight;
				aPosAry.mnSrcX		 = 0;
				aPosAry.mnSrcY		 = 0;
				aPosAry.mnSrcWidth	 = nWidth;
				aPosAry.mnSrcHeight  = nHeight;
				aPosAry.mnDestX 	 = 0;
				aPosAry.mnDestY 	 = 0;
				aPosAry.mnDestWidth  = nWidth;
				aPosAry.mnDestHeight = nHeight;

				pGraphics->CopyBits( &aPosAry, mpGraphics, this, this );
				pNewVirDev->ReleaseGraphics( pGraphics );
				ImplReleaseGraphics();
				pSVData->mpDefInst->DestroyVirtualDevice( mpVirDev );
				mpVirDev = pNewVirDev;
				mnOutWidth	= rNewSize.Width();
				mnOutHeight = rNewSize.Height();
				bRet = sal_True;
			}
			else
			{
				bRet = sal_False;
				pSVData->mpDefInst->DestroyVirtualDevice( pNewVirDev );
			}
		}
		else
			bRet = sal_False;
	}

	return bRet;
}

// -----------------------------------------------------------------------

// #i32109#: Fill opaque areas correctly (without relying on 
// fill/linecolor state)
void VirtualDevice::ImplFillOpaqueRectangle( const Rectangle& rRect )
{
    // Set line and fill color to black (->opaque), 
    // fill rect with that (linecolor, too, because of
    // those pesky missing pixel problems)
    Push( PUSH_LINECOLOR | PUSH_FILLCOLOR );
    SetLineColor( COL_BLACK );
    SetFillColor( COL_BLACK );
    DrawRect( rRect );
    Pop();
}

// -----------------------------------------------------------------------

sal_Bool VirtualDevice::SetOutputSizePixel( const Size& rNewSize, sal_Bool bErase )
{
    if( ImplSetOutputSizePixel(rNewSize, bErase) )
    {
        if( mnAlphaDepth != -1 )
        {
            // #110958# Setup alpha bitmap
			if(mpAlphaVDev && mpAlphaVDev->GetOutputSizePixel() != rNewSize)
			{
				delete mpAlphaVDev;
				mpAlphaVDev = 0L;
			}

			if( !mpAlphaVDev )
            {
                mpAlphaVDev = new VirtualDevice( *this, mnAlphaDepth );
                mpAlphaVDev->ImplSetOutputSizePixel(rNewSize, bErase);
            }

            // TODO: copy full outdev state to new one, here. Also needed in outdev2.cxx:DrawOutDev
            if( GetLineColor() != Color( COL_TRANSPARENT ) )
                mpAlphaVDev->SetLineColor( COL_BLACK );

            if( GetFillColor() != Color( COL_TRANSPARENT ) )
                mpAlphaVDev->SetFillColor( COL_BLACK );

            mpAlphaVDev->SetMapMode( GetMapMode() );
        }

        return sal_True;
    }

    return sal_False;
}

// -----------------------------------------------------------------------

void VirtualDevice::SetReferenceDevice( RefDevMode i_eRefDevMode )
{
    sal_Int32 nDPIX = 600, nDPIY = 600;
    switch( i_eRefDevMode )
    {
    case REFDEV_NONE:
    default:
        DBG_ASSERT( sal_False, "VDev::SetRefDev illegal argument!" );
        break;
    case REFDEV_MODE06:
        nDPIX = nDPIY = 600;
        break;
    case REFDEV_MODE48:
        nDPIX = nDPIY = 4800;
        break;
    case REFDEV_MODE_MSO1:
        nDPIX = nDPIY = 6*1440;
        break;
    case REFDEV_MODE_PDF1:
        nDPIX = nDPIY = 720;
        break;
    }
    ImplSetReferenceDevice( i_eRefDevMode, nDPIX, nDPIY );
}

void VirtualDevice::SetReferenceDevice( sal_Int32 i_nDPIX, sal_Int32 i_nDPIY )
{
    ImplSetReferenceDevice( REFDEV_CUSTOM, i_nDPIX, i_nDPIY );
}

void VirtualDevice::ImplSetReferenceDevice( RefDevMode i_eRefDevMode, sal_Int32 i_nDPIX, sal_Int32 i_nDPIY )
{
    mnDPIX = i_nDPIX;
    mnDPIY = i_nDPIY;

    EnableOutput( sal_False );  // prevent output on reference device
    mbScreenComp = sal_False;

    // invalidate currently selected fonts
    mbInitFont = sal_True;
    mbNewFont = sal_True;

    // avoid adjusting font lists when already in refdev mode
    sal_uInt8 nOldRefDevMode = meRefDevMode;
    sal_uInt8 nOldCompatFlag = (sal_uInt8)meRefDevMode & REFDEV_FORCE_ZERO_EXTLEAD;
    meRefDevMode = (sal_uInt8)(i_eRefDevMode | nOldCompatFlag);
    if( (nOldRefDevMode ^ nOldCompatFlag) != REFDEV_NONE )
        return;

    // the reference device should have only scalable fonts
    // => clean up the original font lists before getting new ones
    if ( mpFontEntry )
    {
        mpFontCache->Release( mpFontEntry );
        mpFontEntry = NULL;
    }
    if ( mpGetDevFontList )
    {
        delete mpGetDevFontList;
        mpGetDevFontList = NULL;
    }
    if ( mpGetDevSizeList )
    {
        delete mpGetDevSizeList;
        mpGetDevSizeList = NULL;
    }

    // preserve global font lists
    ImplSVData* pSVData = ImplGetSVData();
    if( mpFontList && (mpFontList != pSVData->maGDIData.mpScreenFontList) )
        delete mpFontList;
    if( mpFontCache && (mpFontCache != pSVData->maGDIData.mpScreenFontCache) )
        delete mpFontCache;

    // get font list with scalable fonts only
    ImplGetGraphics();
    mpFontList = pSVData->maGDIData.mpScreenFontList->Clone( true, false );

    // prepare to use new font lists
    mpFontCache = new ImplFontCache( false );
}

// -----------------------------------------------------------------------

void VirtualDevice::Compat_ZeroExtleadBug()
{
	meRefDevMode = (sal_uInt8)meRefDevMode | REFDEV_FORCE_ZERO_EXTLEAD; 
}

// -----------------------------------------------------------------------

