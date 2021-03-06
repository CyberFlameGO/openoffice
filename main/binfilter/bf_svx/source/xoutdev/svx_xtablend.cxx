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



// include ---------------------------------------------------------------

#ifndef SVX_LIGHT

#ifndef _SVX_XPROPERTYTABLE_HXX
#include "XPropertyTable.hxx"
#endif
#ifndef _UNTOOLS_UCBSTREAMHELPER_HXX 
#include <unotools/ucbstreamhelper.hxx>
#endif

#include "xmlxtimp.hxx"

#endif

#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _SV_VIRDEV_HXX
#include <vcl/virdev.hxx>
#endif

#ifndef _SV_APP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif

#ifndef _SFXDOCFILE_HXX
#include <bf_sfx2/docfile.hxx>
#endif

#include "dialogs.hrc"
#include "dialmgr.hxx"

#include "xtable.hxx"
#include "xiocomp.hxx"
#include "xpool.hxx"
#include "xoutx.hxx"

#ifndef SVX_XFILLIT0_HXX //autogen
#include <xfillit0.hxx>
#endif

#ifndef _SVX_XFLCLIT_HXX //autogen
#include <xflclit.hxx>
#endif

#ifndef _SVX_XLNSTWIT_HXX //autogen
#include <xlnstwit.hxx>
#endif

#ifndef _SVX_XLNEDWIT_HXX //autogen
#include <xlnedwit.hxx>
#endif

#ifndef _SVX_XLNCLIT_HXX //autogen
#include <xlnclit.hxx>
#endif

#ifndef _SVX_XLINEIT0_HXX //autogen
#include <xlineit0.hxx>
#endif

#ifndef _SVX_XLNSTIT_HXX //autogen
#include <xlnstit.hxx>
#endif

#ifndef _SVX_XLNEDIT_HXX //autogen
#include <xlnedit.hxx>
#endif
namespace binfilter {

#define GLOBALOVERFLOW

using namespace ::com::sun::star;
using namespace rtl;

sal_Unicode const pszExtLineEnd[]	= {'s','o','e'};

static char const aChckLEnd[]  = { 0x04, 0x00, 'S','O','E','L'};	// < 5.2
static char const aChckLEnd0[] = { 0x04, 0x00, 'S','O','E','0'};	// = 5.2
static char const aChckXML[]   = { '<', '?', 'x', 'm', 'l' };		// = 6.0

/************************************************************************/

/*N*/ XLineEndTable::~XLineEndTable()
/*N*/ {
/*N*/ }

/************************************************************************/

/*N*/ XLineEndEntry* XLineEndTable::Replace(long nIndex, XLineEndEntry* pEntry )
/*N*/ {
/*N*/ 	return (XLineEndEntry*) XPropertyTable::Replace(nIndex, pEntry);
/*N*/ }

/************************************************************************/

/*N*/ XLineEndEntry* XLineEndTable::Remove(long nIndex)
/*N*/ {
/*N*/ 	return (XLineEndEntry*) XPropertyTable::Remove(nIndex, 0);
/*N*/ }

/************************************************************************/

/*N*/ XLineEndEntry* XLineEndTable::Get(long nIndex) const
/*N*/ {
/*N*/ 	return (XLineEndEntry*) XPropertyTable::Get(nIndex, 0);
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndTable::Load()
/*N*/ {
/*N*/ 	return( FALSE );
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndTable::Save()
/*N*/ {
/*N*/ 	return( FALSE );
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndTable::Create()
/*N*/ {
/*N*/ 	return( FALSE );
/*N*/ }

/************************************************************************/

/*N*/ Bitmap* XLineEndTable::CreateBitmapForUI( long nIndex, BOOL bDelete )
/*N*/ {
/*N*/ 	return( NULL );
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndTable::CreateBitmapsForUI()
/*N*/ {
/*N*/ 	return( FALSE );
/*N*/ }

// --------------------
// class XLineEndList
// --------------------

/*************************************************************************
|*
|* XLineEndList::XLineEndList()
|*
*************************************************************************/

/*N*/ XLineEndList::XLineEndList( const String& rPath,
/*N*/ 							XOutdevItemPool* pInPool,
/*N*/ 							USHORT nInitSize, USHORT nReSize ) :
/*N*/ 				XPropertyList( rPath, pInPool, nInitSize, nReSize),
/*N*/ 				pVD				( NULL ),
/*N*/ 				pXOut			( NULL ),
/*N*/ 				pXFSet			( NULL ),
/*N*/ 				pXLSet			( NULL )
/*N*/ {
/*N*/ 	pBmpList = new List( nInitSize, nReSize );
/*N*/ }

/************************************************************************/

/*N*/ XLineEndList::~XLineEndList()
/*N*/ {
/*N*/ 	if( pVD )    delete pVD;
/*N*/ 	if( pXOut )  delete pXOut;
/*N*/ 	if( pXFSet ) delete pXFSet;
/*N*/ 	if( pXLSet ) delete pXLSet;
/*N*/ }

/************************************************************************/

/*N*/ XLineEndEntry* XLineEndList::Replace(XLineEndEntry* pEntry, long nIndex )
/*N*/ {
/*N*/ 	return (XLineEndEntry*) XPropertyList::Replace(pEntry, nIndex);
/*N*/ }

/************************************************************************/

/*N*/ XLineEndEntry* XLineEndList::Remove(long nIndex)
/*N*/ {
/*N*/ 	return (XLineEndEntry*) XPropertyList::Remove(nIndex, 0);
/*N*/ }

/************************************************************************/

/*N*/ XLineEndEntry* XLineEndList::Get(long nIndex) const
/*N*/ {
/*N*/ 	return (XLineEndEntry*) XPropertyList::Get(nIndex, 0);
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndList::Load()
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	if( bListDirty )
/*N*/ 	{
/*N*/ 		bListDirty = FALSE;
/*N*/ 
/*N*/ 		INetURLObject aURL( aPath );
/*N*/ 
/*N*/ 		if( INET_PROT_NOT_VALID == aURL.GetProtocol() )
/*N*/ 		{
/*N*/ 			DBG_ASSERT( !aPath.Len(), "invalid URL" );
/*N*/ 			return FALSE;
/*N*/ 		}
/*N*/ 
/*N*/ 		aURL.Append( aName );
/*N*/ 
/*N*/ 		if( !aURL.getExtension().getLength() )
/*N*/ 			aURL.setExtension( rtl::OUString( pszExtLineEnd, 3 ) );
/*N*/ 
/*N*/ 		// check if file exists, SfxMedium shows an errorbox else
/*N*/ 		{
/*N*/ 			::com::sun::star::uno::Reference < ::com::sun::star::task::XInteractionHandler > xHandler;
/*N*/ 			SvStream* pIStm = ::utl::UcbStreamHelper::CreateStream( aURL.GetMainURL( INetURLObject::NO_DECODE ), STREAM_READ, xHandler );
/*N*/ 
/*N*/ 			sal_Bool bOk = pIStm && ( pIStm->GetError() == 0);
/*N*/ 
/*N*/ 			if( pIStm )
/*N*/ 				delete pIStm;
/*N*/ 
/*N*/ 			if( !bOk )
/*N*/ 				return sal_False;
/*N*/ 		}
/*N*/ 
/*N*/ 		{
/*N*/ 			SfxMedium aMedium( aURL.GetMainURL( INetURLObject::NO_DECODE ), STREAM_READ | STREAM_NOCREATE, TRUE );
/*N*/ 			SvStream* pStream = aMedium.GetInStream();
/*N*/ 			if( !pStream )
/*N*/ 				return( FALSE );
/*N*/ 
/*N*/ 			char aCheck[6];
/*N*/ 			pStream->Read( aCheck, 6 );
/*N*/ 
/*N*/ 			// Handelt es sich um die gew'unschte Tabelle?
/*N*/ 			if( memcmp( aCheck, aChckLEnd, sizeof( aChckLEnd ) ) == 0 ||
/*N*/ 				memcmp( aCheck, aChckLEnd0, sizeof( aChckLEnd0 ) ) == 0 )
/*N*/ 			{
/*N*/ 				ImpRead( *pStream );
/*N*/ 				return( pStream->GetError() == SVSTREAM_OK );
/*N*/ 			}
/*N*/ 			else if( memcmp( aCheck, aChckXML, sizeof( aChckXML ) ) != 0 )
/*N*/ 			{
/*N*/ 				return FALSE;
/*N*/ 			}
/*N*/ 		}
/*N*/ 
/*N*/ 		uno::Reference< container::XNameContainer > xTable( SvxUnoXLineEndTable_createInstance( this ), uno::UNO_QUERY );
/*N*/ 		return SvxXMLXTableImport::load( aURL.GetMainURL( INetURLObject::NO_DECODE ), xTable );
/*N*/ 	}
/*N*/ #endif
/*N*/ 	return( FALSE );
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndList::Save()
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); return false;
/*
	SfxMedium aMedium( aURL.GetMainURL( INetURLObject::NO_DECODE ), STREAM_WRITE | STREAM_TRUNC, TRUE );
	aMedium.IsRemote();

	SvStream* pStream = aMedium.GetOutStream();
	if( !pStream )
		return( FALSE );

	// UNICODE: *pStream << String( pszChckLineEnd0, 4 );
	pStream->WriteByteString(String( pszChckLineEnd0, 4 ));

	ImpStore( *pStream );

	aMedium.Close();
	aMedium.Commit();

	return( aMedium.GetError() == 0 );
*/
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndList::Create()
/*N*/ {
/*N*/ 	XPolygon aTriangle(3);
/*N*/ 	aTriangle[0].X()=10; aTriangle[0].Y()= 0;
/*N*/ 	aTriangle[1].X()= 0; aTriangle[1].Y()=30;
/*N*/ 	aTriangle[2].X()=20; aTriangle[2].Y()=30;
/*N*/ 	Insert( new XLineEndEntry( aTriangle, SVX_RESSTR( RID_SVXSTR_ARROW ) ) );
/*N*/ 
/*N*/ 	XPolygon aSquare(4);
/*N*/ 	aSquare[0].X()= 0; aSquare[0].Y()= 0;
/*N*/ 	aSquare[1].X()=10; aSquare[1].Y()= 0;
/*N*/ 	aSquare[2].X()=10; aSquare[2].Y()=10;
/*N*/ 	aSquare[3].X()= 0; aSquare[3].Y()=10;
/*N*/ 	Insert( new XLineEndEntry( aSquare, SVX_RESSTR( RID_SVXSTR_SQUARE ) ) );
/*N*/ 
/*N*/ 	XPolygon aCircle(Point(0,0),100,100);
/*N*/ 	Insert( new XLineEndEntry( aCircle, SVX_RESSTR( RID_SVXSTR_CIRCLE ) ) );
/*N*/ 
/*N*/ 	return( TRUE );
/*N*/ }

/************************************************************************/

/*N*/ BOOL XLineEndList::CreateBitmapsForUI()
/*N*/ {
/*N*/ 	for( long i = 0; i < Count(); i++)
/*N*/ 	{
/*N*/ 		Bitmap* pBmp = CreateBitmapForUI( i, FALSE );
/*N*/ 		DBG_ASSERT( pBmp, "XLineEndList: Bitmap(UI) konnte nicht erzeugt werden!" );
/*N*/ 
/*N*/ 		if( pBmp )
/*N*/ 			pBmpList->Insert( pBmp, i );
/*N*/ 	}
/*N*/ 	// Loeschen, da JOE den Pool vorm Dtor entfernt!
/*N*/ 	if( pVD )	{ delete pVD;	pVD = NULL;     }
/*N*/ 	if( pXOut ) { delete pXOut;	pXOut = NULL;   }
/*N*/ 	if( pXFSet ){ delete pXFSet; pXFSet = NULL; }
/*N*/ 	if( pXLSet ){ delete pXLSet; pXLSet = NULL; }
/*N*/ 
/*N*/ 	return( TRUE );
/*N*/ }

/************************************************************************/

/*N*/ Bitmap* XLineEndList::CreateBitmapForUI( long nIndex, BOOL bDelete )
/*N*/ {
/*N*/ 	Point	aZero;
/*N*/ 	Size	aVDSize;
/*N*/ 
/*N*/ 	if( !pVD ) // und pXOut und pXFSet und pXLSet
/*N*/ 	{
/*N*/ 		pVD = new VirtualDevice;
/*N*/ 		DBG_ASSERT( pVD, "XLineEndList: Konnte kein VirtualDevice erzeugen!" );
/*N*/ 		pVD->SetMapMode( MAP_100TH_MM );
/*N*/ 		aVDSize = pVD->PixelToLogic( Size( BITMAP_WIDTH * 2, BITMAP_HEIGHT ) );
/*N*/ 		pVD->SetOutputSize( aVDSize );
/*N*/ 
/*N*/ 		const StyleSettings& rStyles = Application::GetSettings().GetStyleSettings();
/*N*/ 		pVD->SetFillColor( rStyles.GetFieldColor() );
/*N*/ 		pVD->SetLineColor( rStyles.GetFieldColor() );
/*N*/ 
/*N*/ 		pXOut = new XOutputDevice( pVD );
/*N*/ 		DBG_ASSERT( pVD, "XLineEndList: Konnte kein XOutDevice erzeugen!" );
/*N*/ 
/*N*/ 		pXFSet = new XFillAttrSetItem( pXPool );
/*N*/ 		DBG_ASSERT( pVD, "XLineEndList: Konnte kein XFillAttrSetItem erzeugen!" );
/*N*/ 		pXFSet->GetItemSet().Put( XFillStyleItem( XFILL_SOLID ) );
/*N*/ 		const StyleSettings& rStyleSettings = pVD->GetSettings().GetStyleSettings(); 
/*N*/ 		pXFSet->GetItemSet().Put( XFillColorItem( String(), rStyleSettings.GetFieldColor() ) );
/*N*/ 
/*N*/ 		pXLSet = new XLineAttrSetItem( pXPool );
/*N*/ 		DBG_ASSERT( pVD, "XLineEndList: Konnte kein XLineAttrSetItem erzeugen!" );
/*N*/ 		pXLSet->GetItemSet().Put( XLineStartWidthItem( aVDSize.Height() ) );
/*N*/ 		pXLSet->GetItemSet().Put( XLineEndWidthItem( aVDSize.Height() ) );
/*N*/ 		pXLSet->GetItemSet().Put( XLineColorItem( String(), RGB_Color( rStyles.GetFieldTextColor().GetColor() ) ) );
/*N*/ 
/*N*/ 	}
/*N*/ 	else
/*N*/ 		aVDSize = pVD->GetOutputSize();
/*N*/ 
/*N*/ 	pXLSet->GetItemSet().Put( XLineStyleItem( XLINE_NONE ) );
/*N*/ //-/	pXOut->SetLineAttr( *pXLSet );
/*N*/ 
/*N*/ 	pXOut->SetLineAttr( pXLSet->GetItemSet() );
/*N*/ //-/	pXOut->SetFillAttr( *pXFSet );
/*N*/ 	
/*N*/ 	pXOut->SetFillAttr( pXFSet->GetItemSet() );
/*N*/ 	pXOut->DrawRect( Rectangle( aZero, aVDSize ) );
/*N*/ 
/*N*/ 	pXLSet->GetItemSet().Put( XLineStyleItem( XLINE_SOLID ) );
/*N*/ 	pXLSet->GetItemSet().Put( XLineStartItem( String(), Get( nIndex )->GetLineEnd() ) );
/*N*/ 	pXLSet->GetItemSet().Put( XLineEndItem( String(), Get( nIndex )->GetLineEnd() ) );
/*N*/ 
/*N*/ //-/	pXOut->SetLineAttr( *pXLSet );
/*N*/ 	pXOut->SetLineAttr( pXLSet->GetItemSet() );
/*N*/ 	
/*N*/ 	pXOut->DrawLine( Point( 0, aVDSize.Height() / 2 ),
/*N*/ 					 Point( aVDSize.Width(), aVDSize.Height() / 2 ) );
/*N*/ 
/*N*/ 	Bitmap* pBitmap = new Bitmap( pVD->GetBitmap( aZero, aVDSize ) );
/*N*/ 
/*N*/ 	// Loeschen, da JOE den Pool vorm Dtor entfernt!
/*N*/ 	if( bDelete )
/*N*/ 	{
/*N*/ 		if( pVD )	{ delete pVD;	pVD = NULL;     }
/*N*/ 		if( pXOut ) { delete pXOut;	pXOut = NULL;   }
/*N*/ 		if( pXFSet ){ delete pXFSet; pXFSet = NULL; }
/*N*/ 		if( pXLSet ){ delete pXLSet; pXLSet = NULL; }
/*N*/ 	}
/*N*/ 
/*N*/ 	return( pBitmap );
/*N*/ }

/************************************************************************/

/*N*/ XubString& XLineEndList::ConvertName( XubString& rStrName )
/*N*/ {
/*N*/ 	BOOL bFound = FALSE;
/*N*/ 
/*N*/ 	for( USHORT i=0; i<(RID_SVXSTR_LEND_DEF_END-RID_SVXSTR_LEND_DEF_START+1) && !bFound; i++ )
/*N*/ 	{
/*N*/ 		XubString aStrDefName =	SVX_RESSTR( RID_SVXSTR_LEND_DEF_START + i );
/*N*/ 		if( rStrName.Search( aStrDefName ) == 0 )
/*N*/ 		{
/*N*/ 			rStrName.Replace( 0, aStrDefName.Len(),	SVX_RESSTR( RID_SVXSTR_LEND_START + i ) );
/*N*/ 			bFound = TRUE;
/*N*/ 		}
/*N*/ 	}
/*N*/ 
/*N*/ 	return rStrName;
/*N*/ }

/************************************************************************/

/*N*/ SvStream& XLineEndList::ImpRead( SvStream& rIn )
/*N*/ {
/*N*/ 	// Lesen
/*N*/ 	rIn.SetStreamCharSet( RTL_TEXTENCODING_IBM_850 );
/*N*/ 
/*N*/ 	delete pBmpList;
/*N*/ 	pBmpList = new List( 16, 16 );
/*N*/ 
/*N*/ 	XLineEndEntry* pEntry = NULL;
/*N*/ 	long	nVersion;
/*N*/ 	long	nCount;
/*N*/ 	XubString	aName;
/*N*/ 	long	nFlags;
/*N*/ 
/*N*/ 	rIn >> nVersion;
/*N*/ 
/*N*/ 	if( nVersion >= 0 ) // 1. Version
/*N*/ 	{
/*N*/ 		nCount = nVersion;
/*N*/ 		for( long nI = 0; nI < nCount; nI++ )
/*N*/ 		{
/*N*/ 			// UNICODE: rIn >> aName;
/*N*/ 			rIn.ReadByteString(aName);
/*N*/ 
/*N*/ 			aName = ConvertName( aName );
/*N*/ 			USHORT nPoints;
/*N*/ 			sal_uInt32 nTemp;
/*N*/ 			Point  aPoint;
/*N*/ 			rIn >> nTemp; nPoints = (USHORT)nTemp;
/*N*/ 			XPolygon* pXPoly = new XPolygon(nPoints);
/*N*/ 			for (USHORT nPoint = 0; nPoint < nPoints; nPoint++)
/*N*/ 			{
/*N*/ 				rIn >> aPoint.X();
/*N*/ 				rIn >> aPoint.Y();
/*N*/ 				rIn >> nFlags;
/*N*/ 				pXPoly->Insert(nPoint, aPoint, (XPolyFlags)nFlags);
/*N*/ 			}
/*N*/ 
/*N*/ 			pEntry = new XLineEndEntry( *pXPoly, aName );
/*N*/ 			Insert( pEntry, nI );
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else if( nVersion == -1L ) // 2. Version
/*N*/ 	{
/*N*/ 		rIn >> nCount;
/*N*/ 		for( long nI = 0; nI < nCount; nI++ )
/*N*/ 		{
/*N*/ 			// UNICODE: rIn >> aName;
/*N*/ 			rIn.ReadByteString(aName);
/*N*/ 			aName = ConvertName( aName );
/*N*/ 
/*N*/ 			XPolygon* pXPoly = new XPolygon;
/*N*/ 			rIn >> *pXPoly;
/*N*/ 
/*N*/ 			pEntry = new XLineEndEntry( *pXPoly, aName );
/*N*/ 			Insert( pEntry, nI );
/*N*/ 		}
/*N*/ 	}
/*N*/ 	else // ab 3.00a
/*N*/ 	{
/*N*/ 		rIn >> nCount;
/*N*/ 		for( long nI = 0; nI < nCount; nI++ )
/*N*/ 		{
/*N*/ 			// Versionsverwaltung
/*N*/ 			XIOCompat aIOC( rIn, STREAM_READ );
/*N*/ 
/*N*/ 			// UNICODE: rIn >> aName;
/*N*/ 			rIn.ReadByteString(aName);
/*N*/ 			aName = ConvertName( aName );
/*N*/ 
/*N*/ 			XPolygon aXPoly;
/*N*/ 			rIn >> aXPoly;
/*N*/ 
/*N*/ 			if (aIOC.GetVersion() > 0)
/*N*/ 			{
/*N*/ 				// lesen neuer Daten ...
/*N*/ 			}
/*N*/ 
/*N*/ 			pEntry = new XLineEndEntry( aXPoly, aName );
/*N*/ 			Insert( pEntry, nI );
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return( rIn );
/*N*/ }



}
