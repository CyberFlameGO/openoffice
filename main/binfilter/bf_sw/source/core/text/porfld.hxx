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


#ifndef _PORFLD_HXX
#define _PORFLD_HXX

#include "swtypes.hxx"
#include "porexp.hxx"
#ifndef _FMTORNT_HXX //autogen
#include <fmtornt.hxx>
#endif
namespace binfilter {

class SwFont;
class SvxBrushItem;
class SwFmtVertOrient;
class SwFrm;

/*************************************************************************
 *                      class SwFldPortion
 *************************************************************************/

class SwFldPortion : public SwExpandPortion
{
	friend class SwTxtFormatter;
protected:
	XubString  aExpand;            // das expandierte Feld
	SwFont  *pFnt;              // Fuer mehrzeilige Felder
	xub_StrLen nNextOffset;		// Offset des Follows im Originalstring
	KSHORT  nViewWidth;         // Screenbreite fuer leere Felder
	sal_Bool bFollow : 1;           // 2. oder weiterer Teil eines Feldes
	sal_Bool bLeft : 1;				// wird von SwNumberPortion benutzt
	sal_Bool bHide : 1;             // wird von SwNumberPortion benutzt
	sal_Bool bCenter : 1; 			// wird von SwNumberPortion benutzt
	sal_Bool bHasFollow : 1; 		// geht in der naechsten Zeile weiter
	sal_Bool bAnimated : 1; 		// wird von SwGrfNumPortion benutzt
	sal_Bool bNoPaint : 1; 			// wird von SwGrfNumPortion benutzt
	sal_Bool bReplace : 1; 			// wird von SwGrfNumPortion benutzt

	inline void SetFont( SwFont *pNew ) { pFnt = pNew; }
	inline const SwFont *GetFont() const { return pFnt; }
	BYTE ScriptChange( const SwTxtSizeInfo &rInf, xub_StrLen& rFull );
public:
    SwFldPortion( const SwFldPortion& rFld );
	SwFldPortion( const XubString &rExpand, SwFont *pFnt = 0 );
	~SwFldPortion();

	void TakeNextOffset( const SwFldPortion* pFld );
	void CheckScript( const SwTxtSizeInfo &rInf );
	inline sal_Bool HasFont() const { return 0 != pFnt; }

	inline const XubString &GetExp() const { return aExpand; }
	virtual sal_Bool GetExpTxt( const SwTxtSizeInfo &rInf, XubString &rTxt ) const;
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );

	// leere Felder sind auch erlaubt
	virtual SwLinePortion *Compress();


	inline sal_Bool IsFollow() const { return bFollow; }
	inline void SetFollow( sal_Bool bNew ) { bFollow = bNew; }

	inline sal_Bool IsLeft() const { return bLeft; }
	inline void SetLeft( sal_Bool bNew ) { bLeft = bNew; }

	inline sal_Bool IsHide() const { return bHide; }
	inline void SetHide( sal_Bool bNew ) { bHide = bNew; }

	inline sal_Bool IsCenter() const { return bCenter; }
	inline void SetCenter( sal_Bool bNew ) { bCenter = bNew; }

	inline sal_Bool HasFollow() const { return bHasFollow; }
	inline void SetHasFollow( sal_Bool bNew ) { bHasFollow = bNew; }

	inline xub_StrLen GetNextOffset() const { return nNextOffset; }
	inline void SetNextOffset( xub_StrLen nNew ) { nNextOffset = nNew; }

	// Felder-Cloner fuer SplitGlue
	virtual SwFldPortion *Clone( const XubString &rExpand ) const;

	// Extra-GetTxtSize wegen pFnt
	virtual SwPosSize GetTxtSize( const SwTxtSizeInfo &rInfo ) const;

    // Accessibility: pass information about this portion to the PortionHandler

	OUTPUT_OPERATOR
};

/*************************************************************************
 *                      class SwHiddenPortion
 *************************************************************************/
// Unterscheidung nur fuer's Painten/verstecken.

class SwHiddenPortion : public SwFldPortion
{
public:
	inline SwHiddenPortion( const XubString &rExpand, SwFont *pFnt = 0 )
		 : SwFldPortion( rExpand, pFnt )
		{ SetLen(1); SetWhichPor( POR_HIDDEN ); }
	virtual sal_Bool GetExpTxt( const SwTxtSizeInfo &rInf, XubString &rTxt ) const;

	// Felder-Cloner fuer SplitGlue
	OUTPUT_OPERATOR
};

/*************************************************************************
 *                      class SwNumberPortion
 *************************************************************************/

class SwNumberPortion : public SwFldPortion
{
protected:
	KSHORT  nFixWidth;      // vgl. Glues
	KSHORT  nMinDist;		// minimaler Abstand zum Text
public:
	SwNumberPortion( const XubString &rExpand, SwFont *pFnt,
		const sal_Bool bLeft, const sal_Bool bCenter, const KSHORT nMinDst );
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );

	// Felder-Cloner fuer SplitGlue

	OUTPUT_OPERATOR
};

/*************************************************************************
 *                      class SwBulletPortion
 *************************************************************************/

class SwBulletPortion : public SwNumberPortion
{
public:
	SwBulletPortion( const xub_Unicode cCh, SwFont *pFnt, const sal_Bool bLeft,
					 const sal_Bool bCenter, const KSHORT nMinDst );
	OUTPUT_OPERATOR
};

/*************************************************************************
 *                      class SwBmpBulletPortion
 *************************************************************************/

class SwGrfNumPortion : public SwNumberPortion
{
public:
SwGrfNumPortion( SwFrm *pFrm, const SvxBrushItem* pGrfBrush,//STRIP001 	SwGrfNumPortion( SwFrm *pFrm, const SvxBrushItem* pGrfBrush,
const SwFmtVertOrient* pGrfOrient, const Size& rGrfSize,//STRIP001 		const SwFmtVertOrient* pGrfOrient, const Size& rGrfSize,
const sal_Bool bLeft, const sal_Bool bCenter, const KSHORT nMinDst ):SwNumberPortion( aEmptyStr, NULL, bLeft, bCenter, nMinDst ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 		const sal_Bool bLeft, const sal_Bool bCenter, const KSHORT nMinDst );
void SetBase( long nLnAscent, long nLnDescent,//STRIP001 	void SetBase( long nLnAscent, long nLnDescent,
long nFlyAscent, long nFlyDescent ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 		long nFlyAscent, long nFlyDescent );
	OUTPUT_OPERATOR
};

CLASSIO( SwHiddenPortion )
CLASSIO( SwNumberPortion )
CLASSIO( SwBulletPortion )
CLASSIO( SwGrfNumPortion )


} //namespace binfilter
#endif
