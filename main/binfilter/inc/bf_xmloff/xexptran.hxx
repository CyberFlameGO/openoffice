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



#ifndef _XEXPTRANSFORM_HXX
#define _XEXPTRANSFORM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTL_USTRING_HXX_
#include "rtl/ustring.hxx"
#endif

#ifndef _B2D_MATRIX3D_HXX
#include <bf_goodies/matrix3d.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_POINTSEQUENCESEQUENCE_HPP_
#include <com/sun/star/drawing/PointSequenceSequence.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_POINTSEQUENCE_HPP_
#include <com/sun/star/drawing/PointSequence.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_FLAGSEQUENCESEQUENCE_HPP_
#include <com/sun/star/drawing/FlagSequenceSequence.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_FLAGSEQUENCE_HPP_
#include <com/sun/star/drawing/FlagSequence.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_HOMOGENMATRIX_HPP_
#include <com/sun/star/drawing/HomogenMatrix.hpp>
#endif

namespace binfilter {
class Vector3D;
class Matrix4D;
class Matrix3D;

//////////////////////////////////////////////////////////////////////////////

struct ImpSdXMLExpTransObj2DBase;
struct ImpSdXMLExpTransObj3DBase;
class SvXMLUnitConverter;

//////////////////////////////////////////////////////////////////////////////

DECLARE_LIST(ImpSdXMLExpTransObj2DBaseList, ImpSdXMLExpTransObj2DBase*)//STRIP007 ;
DECLARE_LIST(ImpSdXMLExpTransObj3DBaseList, ImpSdXMLExpTransObj3DBase*)//STRIP007 ;

//////////////////////////////////////////////////////////////////////////////

class SdXMLImExTransform2D
{
	ImpSdXMLExpTransObj2DBaseList	maList;
	::rtl::OUString					msString;

	void EmptyList();

public:
	SdXMLImExTransform2D() {}
	~SdXMLImExTransform2D() { EmptyList(); }

	void AddRotate(double fNew);
	void AddTranslate(const Vector2D& rNew);
	void AddSkewX(double fNew);

	sal_Bool NeedsAction() const { return (sal_Bool)(maList.Count() > 0L); }
	void GetFullTransform(Matrix3D& rFullTrans);
	const ::rtl::OUString& GetExportString(const SvXMLUnitConverter& rConv);
	void SetString(const ::rtl::OUString& rNew, const SvXMLUnitConverter& rConv);
};

//////////////////////////////////////////////////////////////////////////////

class SdXMLImExTransform3D
{
	ImpSdXMLExpTransObj3DBaseList	maList;
	::rtl::OUString					msString;

	void EmptyList();

public:
	SdXMLImExTransform3D() {}
	SdXMLImExTransform3D(const ::rtl::OUString& rNew, const SvXMLUnitConverter& rConv);
	~SdXMLImExTransform3D() { EmptyList(); }

	void AddMatrix(const Matrix4D& rNew);

	void AddHomogenMatrix(const ::com::sun::star::drawing::HomogenMatrix& xHomMat);
	sal_Bool NeedsAction() const { return (sal_Bool)(maList.Count() > 0L); }
	void GetFullTransform(Matrix4D& rFullTrans);
	BOOL GetFullHomogenTransform( ::com::sun::star::drawing::HomogenMatrix& xHomMat);
	const ::rtl::OUString& GetExportString(const SvXMLUnitConverter& rConv);
	void SetString(const ::rtl::OUString& rNew, const SvXMLUnitConverter& rConv);
};

//////////////////////////////////////////////////////////////////////////////

class SdXMLImExViewBox
{
	::rtl::OUString				msString;
	sal_Int32					mnX;
	sal_Int32					mnY;
	sal_Int32					mnW;
	sal_Int32					mnH;

public:
	SdXMLImExViewBox(sal_Int32 nX = 0L, sal_Int32 nY = 0L, sal_Int32 nW = 1000L, sal_Int32 nH = 1000L);
	SdXMLImExViewBox(const ::rtl::OUString& rNew, const SvXMLUnitConverter& rConv);

	sal_Int32 GetX() const { return mnX; }
	sal_Int32 GetY() const { return mnY; }
	sal_Int32 GetWidth() const { return mnW; }
	sal_Int32 GetHeight() const { return mnH; }
	const ::rtl::OUString& GetExportString(const SvXMLUnitConverter& rConv);
};

//////////////////////////////////////////////////////////////////////////////

class SdXMLImExPointsElement
{
	::rtl::OUString				msString;
	::com::sun::star::drawing::PointSequenceSequence	maPoly;

public:
	SdXMLImExPointsElement( ::com::sun::star::drawing::PointSequence* pPoints, 
		const SdXMLImExViewBox& rViewBox,
		const ::com::sun::star::awt::Point& rObjectPos,
		const ::com::sun::star::awt::Size& rObjectSize,
		const SvXMLUnitConverter& rConv,
		// #96328#
		const sal_Bool bClosed = sal_True);
	SdXMLImExPointsElement(const ::rtl::OUString& rNew, 
		const SdXMLImExViewBox& rViewBox,
		const ::com::sun::star::awt::Point& rObjectPos,
		const ::com::sun::star::awt::Size& rObjectSize,
		const SvXMLUnitConverter& rConv);

	const ::rtl::OUString& GetExportString() const { return msString; }
	const ::com::sun::star::drawing::PointSequenceSequence& GetPointSequenceSequence() const { return maPoly; }
};

//////////////////////////////////////////////////////////////////////////////

class SdXMLImExSvgDElement
{
	::rtl::OUString					msString;
	const SdXMLImExViewBox&			mrViewBox;
	sal_Bool						mbIsClosed;
	sal_Bool						mbIsCurve;

	sal_Int32						mnLastX;
	sal_Int32						mnLastY;

	::com::sun::star::drawing::PointSequenceSequence		maPoly;
	::com::sun::star::drawing::FlagSequenceSequence		maFlag;

public:
	SdXMLImExSvgDElement(const SdXMLImExViewBox& rViewBox);
	SdXMLImExSvgDElement(const ::rtl::OUString& rNew, 
		const SdXMLImExViewBox& rViewBox,
		const ::com::sun::star::awt::Point& rObjectPos,
		const ::com::sun::star::awt::Size& rObjectSize,
		const SvXMLUnitConverter& rConv);

	void AddPolygon(
		::com::sun::star::drawing::PointSequence* pPoints, 
		::com::sun::star::drawing::FlagSequence* pFlags,
		const ::com::sun::star::awt::Point& rObjectPos,
		const ::com::sun::star::awt::Size& rObjectSize,
		const SvXMLUnitConverter& rConv, 
		sal_Bool bClosed = FALSE, sal_Bool bRelative = TRUE);

	const ::rtl::OUString& GetExportString() const { return msString; }
	sal_Bool IsClosed() const { return mbIsClosed; }
	sal_Bool IsCurve() const { return mbIsCurve; }
	const ::com::sun::star::drawing::PointSequenceSequence& GetPointSequenceSequence() const { return maPoly; }
	const ::com::sun::star::drawing::FlagSequenceSequence& GetFlagSequenceSequence() const { return maFlag; }
};


}//end of namespace binfilter
#endif	//  _XEXPTRANSFORM_HXX
