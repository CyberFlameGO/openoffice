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



#ifndef RECT_HXX
#define RECT_HXX

#include <bf_svtools/bf_solar.h>

#include <new>


#ifndef _GEN_HXX //autogen
#include <tools/gen.hxx>
#endif

#ifndef _SV_OUTDEV_HXX //autogen
#include <vcl/outdev.hxx>
#endif

#ifndef _SV_METRIC_HXX //autogen
#include <vcl/metric.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif

#include "format.hxx"
namespace binfilter {


BOOL SmGetGlyphBoundRect(const OutputDevice &rDev,
						 const XubString &rText, Rectangle &rRect);

BOOL SmIsMathAlpha(const XubString &rText);


inline long SmFromTo(long nFrom, long nTo, double fRelDist)
{
	return nFrom + (long) (fRelDist * (nTo - nFrom));
}


////////////////////////////////////////
// SmRect
// ... (to be done)
// This Implementation assumes that the x-axis points to the right and the
// y-axis to the bottom.
// Note: however, italic spaces can be negative!
//

// possible flags for the 'Draw' function below (just for debugging)
#define SM_RECT_CORE	0x0001
#define SM_RECT_ITALIC	0x0002
#define SM_RECT_LINES	0x0004
#define SM_RECT_MID		0x0008

// possible positions and alignments for the 'AlignTo' function
enum RectPos
	// (RP_LEFT : align the current object to the left of the argument, ...)
{	RP_LEFT, RP_RIGHT,
	RP_TOP, RP_BOTTOM,
	RP_ATTRIBUT
};
enum RectHorAlign
{	RHA_LEFT, RHA_CENTER, RHA_RIGHT
};
enum RectVerAlign
{	RVA_TOP, RVA_MID, RVA_BOTTOM, RVA_BASELINE, RVA_CENTERY,
	RVA_ATTRIBUT_HI, RVA_ATTRIBUT_MID, RVA_ATTRIBUT_LO
};

// different methods of copying baselines and mid's in 'ExtendBy' function
enum RectCopyMBL
{	RCP_THIS,	// keep baseline of current object even if it has none
	RCP_ARG,	// as above but for the argument
	RCP_NONE,	// result will have no baseline
	RCP_XOR		// if current object has a baseline keep it else copy
				//	 the arguments baseline (even if it has none)
};


class SmRect
{
	Point	aTopLeft;
	Size	aSize;
	long	nBaseline,
			nAlignT,
			nAlignM,
			nAlignB,
			nGlyphTop,
			nGlyphBottom,
			nItalicLeftSpace,
			nItalicRightSpace,
			nLoAttrFence,
			nHiAttrFence;
    USHORT  nBorderWidth;
	BOOL	bHasBaseline,
			bHasAlignInfo;

protected:
			void BuildRect (const OutputDevice &rDev, const SmFormat *pFormat,
                            const XubString &rText, USHORT nBorderWidth);
			void Init(const OutputDevice &rDev, const SmFormat *pFormat,
                      const XubString &rText, USHORT nBorderWidth);

			void ClearBaseline()	{ bHasBaseline = FALSE; };
	inline	void CopyMBL(const SmRect& rRect);
			void CopyAlignInfo(const SmRect& rRect);

			SmRect & Union(const SmRect &rRect);

public:
			SmRect();
			SmRect(const OutputDevice &rDev, const SmFormat *pFormat,
                   const XubString &rText, long nBorderWidth);
			SmRect(long nWidth, long nHeight);
			SmRect(const SmRect &rRect);


            USHORT  GetBorderWidth() const  { return nBorderWidth; }

            void SetItalicSpaces(long nLeftSpace, long nRightSpace);

			void SetWidth(ULONG nWidth)		{ aSize.Width()  = nWidth; }
			void SetHeight(ULONG nHeight)	{ aSize.Height() = nHeight; }

			void SetLeft(long nLeft);
			void SetRight(long nRight);
			void SetBottom(long nBottom);
			void SetTop(long nTop);

			const Point	& GetTopLeft() const { return aTopLeft; }

			long GetTop()	  const	{ return GetTopLeft().Y(); }
			long GetLeft()	  const	{ return GetTopLeft().X(); }
			long GetBottom()  const	{ return GetTop() + GetHeight() - 1; }
			long GetRight()   const	{ return GetLeft() + GetWidth() - 1; }
			long GetCenterX() const	{ return (GetLeft() + GetRight()) / 2L; }
			long GetCenterY() const	{ return (GetTop() + GetBottom()) / 2L; }
			long GetWidth()   const	{ return GetSize().Width(); }
			long GetHeight()  const	{ return GetSize().Height(); }

			long GetItalicLeftSpace()  const { return nItalicLeftSpace; }
			long GetItalicRightSpace() const { return nItalicRightSpace; }

			void SetHiAttrFence(long nVal)	{ nHiAttrFence = nVal; }
			void SetLoAttrFence(long nVal)	{ nLoAttrFence = nVal; }
			long GetHiAttrFence() const 	{ return nHiAttrFence; }
			long GetLoAttrFence() const 	{ return nLoAttrFence; }

			long GetItalicLeft() const		{ return GetLeft() - GetItalicLeftSpace(); }
			long GetItalicCenterX() const	{ return (GetItalicLeft() + GetItalicRight()) / 2; }
			long GetItalicRight() const		{ return GetRight() + GetItalicRightSpace(); }
			long GetItalicWidth() const		{ return GetWidth() + GetItalicLeftSpace() + GetItalicRightSpace(); }

			BOOL HasBaseline() const		{ return bHasBaseline; }
	inline	long GetBaseline() const;
			long GetBaselineOffset() const	{ return GetBaseline() - GetTop(); }

			void SetAlignTop(long nVal) { nAlignT = nVal; }

			long GetAlignT() const	{ return nAlignT; }
			long GetAlignM() const	{ return nAlignM; }
			long GetAlignB() const	{ return nAlignB; }

			void SetAlignT(long nVal) { nAlignT = nVal; }

			const Point	 GetCenter() const
			{	return Point(GetCenterX(), GetCenterY()); }

			const Size & GetSize() const	{ return aSize; }

			const Size	GetItalicSize() const
			{	return Size(GetItalicWidth(), GetHeight()); }

			void Move  (const Point &rPosition);
			void MoveTo(const Point &rPosition) { Move(rPosition - GetTopLeft()); }

			BOOL IsEmpty() const
			{
				return GetWidth() == 0	||	GetHeight() == 0;
			}

			BOOL HasAlignInfo() const { return bHasAlignInfo; }

			const Point AlignTo(const SmRect &rRect, RectPos ePos,
								RectHorAlign eHor, RectVerAlign eVer) const;

			SmRect & ExtendBy(const SmRect &rRect, RectCopyMBL eCopyMode);
			SmRect & ExtendBy(const SmRect &rRect, RectCopyMBL eCopyMode,
							  long nNewAlignM);
			SmRect & ExtendBy(const SmRect &rRect, RectCopyMBL eCopyMode,
					  BOOL bKeepVerAlignParams);
//            SmRect & ExtendBy(const Point &rPoint);

//            long    OrientedDist(const Point &rPoint) const;
//            BOOL    IsInsideRect(const Point &rPoint) const;
//            BOOL    IsInsideItalicRect(const Point &rPoint) const;

	inline	SmRect & operator = (const SmRect &rRect);

	inline	Rectangle	AsRectangle() const;
            SmRect      AsGlyphRect() const;

};


inline void SmRect::SetItalicSpaces(long nLeftSpace, long nRightSpace)
	// set extra spacing to the left and right for (italic)
	// letters/text
{
	nItalicLeftSpace  = nLeftSpace;
	nItalicRightSpace = nRightSpace;
}


inline void SmRect::CopyMBL(const SmRect &rRect)
	// copy AlignM baseline and value of 'rRect'
{
	nBaseline	 = rRect.nBaseline;
	bHasBaseline = rRect.bHasBaseline;
	nAlignM		 = rRect.nAlignM;
}


inline long SmRect::GetBaseline() const
{
	DBG_ASSERT(HasBaseline(), "Sm: Baseline nicht vorhanden");
	return nBaseline;
}


inline SmRect & SmRect::operator = (const SmRect &rRect)
{
	new (this) SmRect(rRect);	// placement new
	return *this;
}


inline Rectangle SmRect::AsRectangle() const
{
	return Rectangle(Point(GetItalicLeft(), GetTop()), GetItalicSize());
}



} //namespace binfilter
#endif
