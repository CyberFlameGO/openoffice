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



#ifndef _SVDOGRAF_HXX
#define _SVDOGRAF_HXX

#include <com/sun/star/io/XInputStream.hpp>
#include <vcl/graph.hxx>
#include <svx/svdorect.hxx>
#include <svtools/grfmgr.hxx>
#include "svx/svxdllapi.h"

//////////////////////////////////////////////////////////////////////////////
// predeclarations

namespace sdr
{
	namespace properties
	{
		class GraphicProperties;
	} // end of namespace properties
	namespace contact
	{
		class ViewObjectContactOfGraphic;
	} // end of namespace contact
} // end of namespace sdr

// -----------
// - Defines -
// -----------

#define SDRGRAFOBJ_TRANSFORMATTR_NONE		0x00000000UL
#define SDRGRAFOBJ_TRANSFORMATTR_COLOR		0x00000001UL
#define SDRGRAFOBJ_TRANSFORMATTR_MIRROR		0x00000002UL
#define SDRGRAFOBJ_TRANSFORMATTR_ROTATE		0x00000004UL
#define SDRGRAFOBJ_TRANSFORMATTR_ALL		0xffffffffUL

// ---------------------
// - SdrGrafObjGeoData -
// ---------------------

// #109872#
class SdrGrafObjGeoData : public SdrTextObjGeoData
{
public:
	sal_Bool					bMirrored;

	SdrGrafObjGeoData()
	:	bMirrored(sal_False) 
	{
	}
};

// --------------
// - SdrGrafObj -
// --------------

class GraphicObject;
class SdrGraphicLink;

class SVX_DLLPUBLIC SdrGrafObj : public SdrRectObj
{
private:
	// to allow sdr::properties::GraphicProperties access to SetXPolyDirty()
	friend class sdr::properties::GraphicProperties;

	// to allow sdr::contact::ViewObjectContactOfGraphic access to ImpUpdateGraphicLink()
	friend class sdr::contact::ViewObjectContactOfGraphic;
	friend class SdrGraphicLink;

protected:
	virtual sdr::contact::ViewContact* CreateObjectSpecificViewContact();
	virtual sdr::properties::BaseProperties* CreateObjectSpecificProperties();

	void					ImpSetAttrToGrafInfo(); // Werte vom Pool kopieren
	void					ImpSetGrafInfoToAttr(); // Werte in den Pool kopieren
	GraphicAttr				aGrafInfo;

	Rectangle				aCropRect;			// Wenn aCropRect nicht Empty ist, dann enthaelt es den sichtbaren
												// Ausschnitt der Grafik in logischen Eingeiten der Grafik! Also Bitmap->=Pixel
	String					aFileName;			// Wenn es sich um einen Link handelt, steht hier der Dateiname drin.
	String					aFilterName;
	GraphicObject*			pGraphic;			// Zur Beschleunigung von Bitmapausgaben, besonders von gedrehten.
    GraphicObject*			mpReplacementGraphic;
	SdrGraphicLink*			pGraphicLink;		// Und hier noch ein Pointer fuer gelinkte Grafiken
	bool    				bMirrored;			// True bedeutet, die Grafik ist horizontal, d.h. ueber die Y-Achse gespiegelt auszugeben.

	// #111096#
	// Flag for allowing text animation. Default is sal_true.
	unsigned					mbGrafAnimationAllowed : 1;

	// #i25616#
	unsigned					mbInsidePaint : 1;
	unsigned					mbIsPreview   : 1;

protected:

	void					ImpLinkAnmeldung();
	void					ImpLinkAbmeldung();
	sal_Bool				ImpUpdateGraphicLink( sal_Bool bAsynchron = sal_True ) const;
	void					ImpSetLinkedGraphic( const Graphic& rGraphic );
							DECL_LINK( ImpSwapHdl, GraphicObject* );

public:

							TYPEINFO();

							SdrGrafObj();
							SdrGrafObj(const Graphic& rGrf);
							SdrGrafObj(const Graphic& rGrf, const Rectangle& rRect);
	virtual					~SdrGrafObj();

    void					SetGraphicObject( const GraphicObject& rGrfObj );
	const GraphicObject&	GetGraphicObject( bool bForceSwapIn = false) const;
    const GraphicObject*    GetReplacementGraphicObject() const;

	void					NbcSetGraphic(const Graphic& rGrf);
	void					SetGraphic(const Graphic& rGrf);
	const Graphic&			GetGraphic() const;

	Graphic					GetTransformedGraphic( sal_uIntPtr nTransformFlags = SDRGRAFOBJ_TRANSFORMATTR_ALL ) const;

	GraphicType				GetGraphicType() const;

	// #111096#
	// Keep ATM for SD.
	sal_Bool IsAnimated() const;
	sal_Bool IsEPS() const;
	sal_Bool IsSwappedOut() const;

	const MapMode&			GetGrafPrefMapMode() const;
	const Size&				GetGrafPrefSize() const;

	void					SetGrafStreamURL( const String& rGraphicStreamURL );
	String					GetGrafStreamURL() const;

	void					ForceSwapIn() const;
	void					ForceSwapOut() const;

	void					SetGraphicLink(const String& rFileName, const String& rFilterName);
	void					ReleaseGraphicLink();
	sal_Bool IsLinkedGraphic() const { return (sal_Bool)aFileName.Len(); }

	void					SetFileName(const String& rFileName);
	const String&			GetFileName() const { return aFileName; }
	void					SetFilterName(const String& rFilterName);
	const String&			GetFilterName() const { return aFilterName; }

	void					StartAnimation(OutputDevice* pOutDev, const Point& rPoint, const Size& rSize, long nExtraData=0L);
	void					StopAnimation(OutputDevice* pOutDev=NULL, long nExtraData=0L);

	virtual void			TakeObjInfo(SdrObjTransformInfoRec& rInfo) const;
	virtual sal_uInt16			GetObjIdentifier() const;

	virtual void			TakeObjNameSingul(String& rName) const;
	virtual void			TakeObjNamePlural(String& rName) const;
	
	// #i25616#
	virtual basegfx::B2DPolyPolygon TakeXorPoly() const;

	virtual void			operator=(const SdrObject& rObj);

    virtual sal_uInt32 GetHdlCount() const;
	virtual SdrHdl*			GetHdl(sal_uInt32 nHdlNum) const;

	virtual void			NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
	virtual void			NbcRotate(const Point& rRef, long nWink, double sn, double cs);
	virtual void			NbcMirror(const Point& rRef1, const Point& rRef2);
	virtual void			NbcShear (const Point& rRef, long nWink, double tn, FASTBOOL bVShear);
	virtual void			NbcSetSnapRect(const Rectangle& rRect);
	virtual void			NbcSetLogicRect(const Rectangle& rRect);
	virtual SdrObjGeoData*	NewGeoData() const;
	virtual void			SaveGeoData(SdrObjGeoData& rGeo) const;
	virtual void			RestGeoData(const SdrObjGeoData& rGeo);

	FASTBOOL				HasGDIMetaFile() const;
	const GDIMetaFile*		GetGDIMetaFile() const;

	virtual void			SetPage(SdrPage* pNewPage);
	virtual void			SetModel(SdrModel* pNewModel);

    bool isEmbeddedSvg() const;
    GDIMetaFile getMetafileFromEmbeddedSvg() const;

    virtual SdrObject*		DoConvertToPolyObj(sal_Bool bBezier, bool bAddText) const;

	virtual void			AdjustToMaxRect( const Rectangle& rMaxRect, bool bShrinkOnly = false );

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

	bool					IsMirrored() { return bMirrored; }
	void					SetMirrored( bool _bMirrored ) { bMirrored = _bMirrored; }

	// #111096#
	// Access to GrafAnimationAllowed flag
	sal_Bool IsGrafAnimationAllowed() const;
	void SetGrafAnimationAllowed(sal_Bool bNew);

	// #i25616#
	sal_Bool IsObjectTransparent() const;
	
	::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > getInputStream();

    // #i103116# FullDrag support
	virtual SdrObject* getFullDragClone() const;
};

#endif //_SVDOGRAF_HXX
