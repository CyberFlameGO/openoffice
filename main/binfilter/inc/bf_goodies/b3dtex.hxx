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



#ifndef _B3D_B3DTEX_HXX
#define _B3D_B3DTEX_HXX

#include <bf_svtools/bf_solar.h>


//#ifndef _SV_OPENGL_HXX
//#include <vcl/opengl.hxx>
//#endif

//#ifndef _SV_BITMAPEX_HXX
//#include <vcl/bitmapex.hxx>
//#endif

#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

#ifndef _SV_SALBTYPE_HXX
#include <vcl/salbtype.hxx>
#endif

//#ifndef _TOOLS_TIME_HXX
//#include <tools/time.hxx>
//#endif

// Vorausdeklarationen
//class BitmapReadAccess;
//class BitmapColor;

namespace binfilter {

/*************************************************************************
|*
|* define for lifetime of a texture in texture cache.
|* Parameter of Time(...) call, so hrs, min, sec, 100thsec. 
|* Timer for cache uses ten secs delays
|*
\************************************************************************/

#define	B3D_TEXTURE_LIFETIME				0, 1, 0

/*************************************************************************
|*
|* Long-Zeiger fuer OpenGL Texturdatenuebergabe
|*
\************************************************************************/

#ifdef WIN
typedef UINT8 huge*	GL_UINT8;
#else
typedef UINT8*		GL_UINT8;
#endif

/*************************************************************************
|*
|* Art der Pixeldaten der Textur
|*
\************************************************************************/

enum Base3DTextureKind
{
	Base3DTextureLuminance = 1,
	Base3DTextureIntensity,
	Base3DTextureColor
};

/*************************************************************************
|*
|* Modus der Textur
|*
\************************************************************************/

enum Base3DTextureMode
{
	Base3DTextureReplace = 1,
	Base3DTextureModulate,
	Base3DTextureBlend
};

/*************************************************************************
|*
|* Filtermodus der Textur
|*
\************************************************************************/

enum Base3DTextureFilter
{
	Base3DTextureNearest = 1,
	Base3DTextureLinear
};

/*************************************************************************
|*
|* Wrapping-Modus
|*
\************************************************************************/

enum Base3DTextureWrap
{
	Base3DTextureClamp = 1,
	Base3DTextureRepeat,
	Base3DTextureSingle
};

/*************************************************************************
|*
|* Defines fuer Maskenbildung um Entscheidung innerhalb von ModifyColor
|* zu beschleunigen
|*
\************************************************************************/

#define	B3D_TXT_KIND_LUM		0x00
#define	B3D_TXT_KIND_INT		0x01
#define	B3D_TXT_KIND_COL		0x02

#define	B3D_TXT_MODE_REP		0x04
#define	B3D_TXT_MODE_MOD		0x08
#define	B3D_TXT_MODE_BND		0x0C

#define	B3D_TXT_FLTR_NEA		0x10

/*************************************************************************
|*
|* Klassen fuer TexturAttribute beim Anfordern von Texturen
|*
\************************************************************************/

#define	TEXTURE_ATTRIBUTE_TYPE_COLOR			0x0000
#define	TEXTURE_ATTRIBUTE_TYPE_BITMAP			0x0001
#define	TEXTURE_ATTRIBUTE_TYPE_GRADIENT			0x0002
#define	TEXTURE_ATTRIBUTE_TYPE_HATCH			0x0003

//class TextureAttributes
//{
//private:
//	void*			mpFloatTrans;
//	BOOL			mbGhosted;
//
//public:
//	TextureAttributes(BOOL bGhosted, void* pFT);
//
//	virtual BOOL operator==(const TextureAttributes&) const;
//	virtual UINT16 GetTextureAttributeType() const =0;
//
//	BOOL GetGhostedAttribute() { return mbGhosted; }
//	void* GetFloatTransAttribute() { return mpFloatTrans; }
//};

//class TextureAttributesColor : public TextureAttributes
//{
//private:
//	Color			maColorAttribute;
//
//public:
//	TextureAttributesColor(BOOL bGhosted, void* pFT, Color aColor);
//
//	virtual BOOL operator==(const TextureAttributes&) const;
//	virtual UINT16 GetTextureAttributeType() const;
//
//	Color GetColorAttribute() { return maColorAttribute; }
//};

//class TextureAttributesBitmap : public TextureAttributes
//{
//private:
//	Bitmap			maBitmapAttribute;
//
//public:
//	TextureAttributesBitmap(BOOL bGhosted, void* pFT, Bitmap aBmp);
//
//	virtual BOOL operator==(const TextureAttributes&) const;
//	virtual UINT16 GetTextureAttributeType() const;
//
//	Bitmap GetBitmapAttribute() { return maBitmapAttribute; }
//};

//class TextureAttributesGradient : public TextureAttributes
//{
//private:
//	void*			mpFill;
//	void*			mpStepCount;
//
//public:
//	TextureAttributesGradient(BOOL bGhosted, void* pFT, void* pF, void *pSC);
//
//	virtual BOOL operator==(const TextureAttributes&) const;
//	virtual UINT16 GetTextureAttributeType() const;
//
//	void* GetFillAttribute() { return mpFill; }
//	void* GetStepCountAttribute() { return mpStepCount; }
//};

//class TextureAttributesHatch : public TextureAttributes
//{
//private:
//	void*			mpFill;
//
//public:
//	TextureAttributesHatch(BOOL bGhosted, void* pFT, void* pF);
//
//	virtual BOOL operator==(const TextureAttributes&) const;
//	virtual UINT16 GetTextureAttributeType() const;
//
//	void* GetHatchFillAttribute() { return mpFill; }
//};

/*************************************************************************
|*
|* Klasse fuer Texturen in Base3D
|*
\************************************************************************/

//class B3dTexture
//{
//protected:
//	// Die Bitmap(s) der Textur
//	Bitmap					aBitmap;
//	AlphaMask				aAlphaMask;
//	BitmapReadAccess*		pReadAccess;
//	BitmapReadAccess*		pAlphaReadAccess;
//
//	// Attribute bei der Generierung
//	TextureAttributes*		pAttributes;
//
//	// Gibt die Haeufigkeit der Benutzung wieder
//	Time					maTimeStamp;
//
//	// Farbe fuer Base3DTextureBlend - Modus
//	BitmapColor				aColBlend;
//
//	// Farbe, wenn keine Textur an einer Stelle liegt
//	BitmapColor				aColTexture;
//
//	// Art der Textur
//	Base3DTextureKind		eKind;
//
//	// Modus der Textur
//	Base3DTextureMode		eMode;
//
//	// Filter
//	Base3DTextureFilter		eFilter;
//
//	// Wrapping-Modes fuer beide Freiheitsgrade
//	Base3DTextureWrap		eWrapS;
//	Base3DTextureWrap		eWrapT;
//
//	// Entscheidungsvariable
//	UINT8					nSwitchVal;
//
//	// Vorbestimmbare interne booleans
//	unsigned				bTextureKindChanged	: 1;
//
//	// Konstruktor / Destruktor
//	B3dTexture(TextureAttributes& rAtt,
//		BitmapEx& rBmpEx,
//		Base3DTextureKind=Base3DTextureColor,
//		Base3DTextureMode=Base3DTextureReplace,
//		Base3DTextureFilter=Base3DTextureNearest,
//		Base3DTextureWrap eS=Base3DTextureSingle,
//		Base3DTextureWrap eT=Base3DTextureSingle);
//	virtual ~B3dTexture();
//
//	// Interne Zugriffsfunktion auf die BitMapFarben
//	inline const BitmapColor GetBitmapColor(long nX, long nY);
//	inline const sal_uInt8 GetBitmapTransparency(long nX, long nY);
//
//	// Generate switch val for optimized own texture mapping
//	void SetSwitchVal();
//
//	// time stamp and texture cache methods
//	void Touch() { maTimeStamp = Time() + Time(B3D_TEXTURE_LIFETIME); }
//	const Time& GetTimeStamp() const { return maTimeStamp; }
//
//public:
//	// Zugriff auf die Attribute der Textur
//	TextureAttributes& GetAttributes();
//
//	// Zugriff auf Bitmap
//	Bitmap& GetBitmap() { return aBitmap; }
//	AlphaMask& GetAlphaMask() { return aAlphaMask; }
//	BitmapEx GetBitmapEx() { return BitmapEx(aBitmap, aAlphaMask); }
//	const Size GetBitmapSize() { return aBitmap.GetSizePixel(); }
//
//	// Texturfunktion
//	void ModifyColor(Color& rCol, double fS, double fT);
//
//	// Art der Pixeldaten lesen/bestimmen
//	void SetTextureKind(Base3DTextureKind eNew);
//	Base3DTextureKind GetTextureKind() { return eKind; }
//
//	// Texturmodus lesen/bestimmen
//	void SetTextureMode(Base3DTextureMode eNew);
//	Base3DTextureMode GetTextureMode() { return eMode; }
//
//	// Filtermodus lesen/bestimmen
//	void SetTextureFilter(Base3DTextureFilter eNew);
//	Base3DTextureFilter GetTextureFilter() { return eFilter; }
//
//	// Wrapping fuer beide Freiheitsgrade lesen/bestimmen
//	void SetTextureWrapS(Base3DTextureWrap eNew);
//	Base3DTextureWrap GetTextureWrapS() { return eWrapS; }
//	void SetTextureWrapT(Base3DTextureWrap eNew);
//	Base3DTextureWrap GetTextureWrapT() { return eWrapT; }
//
//	// Blend-Color lesen/bestimmen
//	void SetBlendColor(Color rNew);
//	Color GetBlendColor();
//
//	// Textur-Ersatz-Color lesen/bestimmen
//	void SetTextureColor(Color rNew);
//	Color GetTextureColor();
//
//protected:
//	// Zugriff auf Konstruktor/Destruktor nur fuer die verwaltenden Klassen
//	friend class Base3D;
//	friend class Base3DOpenGL;
//	friend class B3dGlobalData;
//	friend class B3dTextureStore;
//};

/*************************************************************************
|*
|* erweiterte Klasse fuer Texturen in Base3DOpenGL
|*
\************************************************************************/

//class B3dTextureOpenGL : public B3dTexture
//{
//private:
//	// Name dieser Textur in OpenGL
//	GLuint					nTextureName;
//
//	// Konstruktor / Destruktor
//	B3dTextureOpenGL(TextureAttributes& rAtt,
//		BitmapEx& rBmpEx,
//		OpenGL& rOGL,
//		Base3DTextureKind=Base3DTextureColor,
//		Base3DTextureMode=Base3DTextureReplace,
//		Base3DTextureFilter=Base3DTextureNearest,
//		Base3DTextureWrap eS=Base3DTextureClamp,
//		Base3DTextureWrap eT=Base3DTextureClamp);
//	virtual ~B3dTextureOpenGL();
//
//	// In OpenGL die Textur zerstoeren
//	void DestroyOpenGLTexture(OpenGL&);
//
//public:
//	// Setze diese Textur in OpenGL als aktuelle Textur
//	void MakeCurrentTexture(OpenGL&);
//
//	// Erzeuge diese Textur als OpenGL-Textur
//	void CreateOpenGLTexture(OpenGL&);
//
//protected:
//	// Zugriff auf Konstruktor/Destruktor nur fuer die verwaltenden Klassen
//	friend class Base3D;
//	friend class Base3DOpenGL;
//	friend class B3dTextureStore;
//};
}//end of namespace binfilter

#endif          // _B3D_B3DTEX_HXX
