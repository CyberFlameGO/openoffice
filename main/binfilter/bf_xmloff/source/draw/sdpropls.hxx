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



#ifndef _SDPROPLS_HXX
#define _SDPROPLS_HXX

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_ 
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _XMLOFF_XMLNUME_HXX 
#include "xmlnume.hxx"
#endif

#ifndef _XMLOFF_PROPMAPPINGTYPES_HXX
#include "maptype.hxx"
#endif

#ifndef _XMLOFF_XMLTYPES_HXX
#include "xmltypes.hxx"
#endif

#ifndef _SVTOOLS_XMLEMENT_HXX 
#include "xmlement.hxx"
#endif

#ifndef _XMLOFF_PROPERTYHANDLERFACTORY_HXX
#include <prhdlfac.hxx>
#endif

#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX 
#include "xmlprmap.hxx"
#endif

#ifndef _XMLOFF_XMLTEXTLISTAUTOSTYLEPOOL_HXX 
#include "XMLTextListAutoStylePool.hxx"
#endif

#ifndef _XMLOFF_XMLEXPPR_HXX 
#include "xmlexppr.hxx"
#endif
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// entry list for graphic properties

extern const XMLPropertyMapEntry aXMLSDProperties[];

//////////////////////////////////////////////////////////////////////////////
// entry list for presentation page properties

extern const XMLPropertyMapEntry aXMLSDPresPageProps[];

//////////////////////////////////////////////////////////////////////////////
// types of own properties

#define	XML_SD_TYPE_STROKE							(XML_SD_TYPES_START +  0)
#define	XML_SD_TYPE_PRESPAGE_TYPE					(XML_SD_TYPES_START +  1)
#define	XML_SD_TYPE_PRESPAGE_STYLE					(XML_SD_TYPES_START +  2)
#define	XML_SD_TYPE_PRESPAGE_SPEED					(XML_SD_TYPES_START +  3)
#define	XML_SD_TYPE_PRESPAGE_DURATION				(XML_SD_TYPES_START +  4)
#define	XML_SD_TYPE_PRESPAGE_VISIBILITY				(XML_SD_TYPES_START +  5)
#define XML_SD_TYPE_MARKER							(XML_SD_TYPES_START +  6 )
#define XML_SD_TYPE_OPACITY							(XML_SD_TYPES_START +  7 )
#define XML_SD_TYPE_LINEJOIN						(XML_SD_TYPES_START +  8 )
#define XML_SD_TYPE_FILLSTYLE						(XML_SD_TYPES_START +  9 )
#define XML_SD_TYPE_GRADIENT						(XML_SD_TYPES_START + 10 )
#define XML_SD_TYPE_GRADIENT_STEPCOUNT				(XML_SD_TYPES_START + 11 )
#define XML_SD_TYPE_SHADOW							(XML_SD_TYPES_START + 12 )
#define XML_SD_TYPE_TEXT_CROSSEDOUT					(XML_SD_TYPES_START + 13 )
#define XML_SD_TYPE_NUMBULLET						(XML_SD_TYPES_START + 14 )
#define XML_SD_TYPE_WRITINGMODE						(XML_SD_TYPES_START + 15 )
#define XML_SD_TYPE_BITMAP_MODE						(XML_SD_TYPES_START + 16 )
#define XML_SD_TYPE_BITMAPREPOFFSETX				(XML_SD_TYPES_START + 17 )
#define XML_SD_TYPE_BITMAPREPOFFSETY				(XML_SD_TYPES_START + 18 )
#define XML_SD_TYPE_FILLBITMAPSIZE					(XML_SD_TYPES_START + 19 )
#define XML_SD_TYPE_LOGICAL_SIZE					(XML_SD_TYPES_START + 20 )
#define XML_SD_TYPE_BITMAP_REFPOINT					(XML_SD_TYPES_START + 21 )
#define XML_SD_TYPE_PRESPAGE_BACKSIZE				(XML_SD_TYPES_START + 22 )
#define XML_TYPE_TEXT_ANIMATION_BLINKING			(XML_SD_TYPES_START + 23 )
#define XML_TYPE_TEXT_ANIMATION_STEPS				(XML_SD_TYPES_START + 24 )
#define XML_SD_TYPE_TEXT_ALIGN						(XML_SD_TYPES_START + 25 )
#define XML_SD_TYPE_VERTICAL_ALIGN					(XML_SD_TYPES_START + 26 )
#define XML_SD_TYPE_FITTOSIZE						(XML_SD_TYPES_START + 27 )
#define XML_SD_TYPE_MEASURE_HALIGN					(XML_SD_TYPES_START + 28 )
#define XML_SD_TYPE_MEASURE_VALIGN					(XML_SD_TYPES_START + 29 )
#define XML_SD_TYPE_MEASURE_UNIT					(XML_SD_TYPES_START + 30 )
#define XML_SD_TYPE_MEASURE_PLACING					(XML_SD_TYPES_START + 31 )
#define XML_SD_TYPE_CONTROL_BORDER					(XML_SD_TYPES_START + 32 )

// 3D property types
#define XML_SD_TYPE_BACKFACE_CULLING				(XML_SD_TYPES_START + 40 )
#define XML_SD_TYPE_NORMALS_KIND					(XML_SD_TYPES_START + 41 )
#define XML_SD_TYPE_NORMALS_DIRECTION				(XML_SD_TYPES_START + 42 )
#define XML_SD_TYPE_TEX_GENERATION_MODE_X			(XML_SD_TYPES_START + 43 )
#define XML_SD_TYPE_TEX_GENERATION_MODE_Y			(XML_SD_TYPES_START + 44 )
#define XML_SD_TYPE_TEX_KIND						(XML_SD_TYPES_START + 45 )
#define XML_SD_TYPE_TEX_MODE						(XML_SD_TYPES_START + 46 )

//////////////////////////////////////////////////////////////////////////////
// #FontWork# types
#define	XML_SD_TYPE_FONTWORK_STYLE					(XML_SD_TYPES_START + 47 )
#define	XML_SD_TYPE_FONTWORK_ADJUST					(XML_SD_TYPES_START + 48 )
#define	XML_SD_TYPE_FONTWORK_SHADOW					(XML_SD_TYPES_START + 49 )
#define	XML_SD_TYPE_FONTWORK_FORM					(XML_SD_TYPES_START + 50 )

//////////////////////////////////////////////////////////////////////////////
// Caption types
#define XML_SD_TYPE_CAPTION_ANGLE_TYPE				(XML_SD_TYPES_START + 60 )
#define XML_SD_TYPE_CAPTION_IS_ESC_REL				(XML_SD_TYPES_START + 61 )
#define XML_SD_TYPE_CAPTION_ESC_REL					(XML_SD_TYPES_START + 62 )
#define XML_SD_TYPE_CAPTION_ESC_ABS					(XML_SD_TYPES_START + 63 )
#define XML_SD_TYPE_CAPTION_ESC_DIR					(XML_SD_TYPES_START + 64 )
#define XML_SD_TYPE_CAPTION_TYPE					(XML_SD_TYPES_START + 65 )

//////////////////////////////////////////////////////////////////////////////

#define CTF_NUMBERINGRULES			1000
//#define CTF_NUMBERINGRULES_NAME		1001
#define CTF_WRITINGMODE				1002
#define CTF_REPEAT_OFFSET_X			1003
#define CTF_REPEAT_OFFSET_Y			1004
#define CTF_PAGE_SOUND_URL			1005
#define CTF_PAGE_VISIBLE			1006
#define CTF_PAGE_TRANS_TYPE			1007
#define CTF_PAGE_TRANS_STYLE		1008
#define CTF_PAGE_TRANS_SPEED		1009
#define CTF_PAGE_TRANS_DURATION		1010
#define CTF_PAGE_BACKSIZE			1011
#define CTF_DASHNAME				1012
#define CTF_LINESTARTNAME			1013
#define CTF_LINEENDNAME				1014
#define CTF_FILLGRADIENTNAME		1015
#define CTF_FILLHATCHNAME			1016
#define CTF_FILLBITMAPNAME			1017
#define CTF_FILLTRANSNAME			1018
#define CTF_TEXTANIMATION_BLINKING	1019
#define CTF_TEXTANIMATION_KIND		1020

//////////////////////////////////////////////////////////////////////////////
// #FontWork#
#define	CTF_FONTWORK_STYLE				1021
#define	CTF_FONTWORK_ADJUST				1022
#define	CTF_FONTWORK_DISTANCE			1023
#define	CTF_FONTWORK_START				1024
#define	CTF_FONTWORK_MIRROR				1025
#define	CTF_FONTWORK_OUTLINE			1026
#define	CTF_FONTWORK_SHADOW				1027
#define	CTF_FONTWORK_SHADOWCOLOR		1028
#define	CTF_FONTWORK_SHADOWOFFSETX		1029
#define	CTF_FONTWORK_SHADOWOFFSETY		1030
#define	CTF_FONTWORK_FORM				1031
#define	CTF_FONTWORK_HIDEFORM			1032
#define	CTF_FONTWORK_SHADOWTRANSPARENCE	1033

//////////////////////////////////////////////////////////////////////////////
// OLE
#define CTF_SD_OLE_VIS_AREA_LEFT		1040
#define CTF_SD_OLE_VIS_AREA_TOP			1041
#define CTF_SD_OLE_VIS_AREA_WIDTH		1042
#define CTF_SD_OLE_VIS_AREA_HEIGHT		1043
#define CTF_SD_OLE_ISINTERNAL			1044

#define CTF_SD_MOVE_PROTECT				1045
#define CTF_SD_SIZE_PROTECT				1046

//////////////////////////////////////////////////////////////////////////////
// caption
#define CTF_CAPTION_ISESCREL			1047
#define CTF_CAPTION_ESCREL				1048
#define CTF_CAPTION_ESCABS				1049

//////////////////////////////////////////////////////////////////////////////
// enum maps for attributes

extern SvXMLEnumMapEntry aXML_ConnectionKind_EnumMap[];
extern SvXMLEnumMapEntry aXML_CircleKind_EnumMap[];

//////////////////////////////////////////////////////////////////////////////
// factory for own graphic properties

class XMLSdPropHdlFactory : public XMLPropertyHandlerFactory
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > mxModel;

public:
	XMLSdPropHdlFactory( ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > );
	virtual ~XMLSdPropHdlFactory();
	virtual const XMLPropertyHandler* GetPropertyHandler( sal_Int32 nType ) const;
};

class XMLShapePropertySetMapper : public XMLPropertySetMapper
{
public:
	XMLShapePropertySetMapper(const UniReference< XMLPropertyHandlerFactory >& rFactoryRef);
	~XMLShapePropertySetMapper();
};

class XMLShapeExportPropertyMapper : public SvXMLExportPropertyMapper
{
private:
	XMLTextListAutoStylePool *mpListAutoPool;
	SvXMLExport& mrExport;
	SvxXMLNumRuleExport maNumRuleExp;
	sal_Bool mbIsInAutoStyles;

	const ::rtl::OUString msCDATA;
	const ::rtl::OUString msTrue;
	const ::rtl::OUString msFalse;

protected:
	virtual void ContextFilter(
		::std::vector< XMLPropertyState >& rProperties,
		::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet > rPropSet ) const;
public:
	XMLShapeExportPropertyMapper( const UniReference< XMLPropertySetMapper >& rMapper, XMLTextListAutoStylePool *pListAutoPool, SvXMLExport& rExport );
	virtual ~XMLShapeExportPropertyMapper();

	virtual void		handleElementItem(
                            SvXMLExport& rExport,
							const XMLPropertyState& rProperty,
							sal_uInt16 nFlags,
							const ::std::vector< XMLPropertyState >* pProperties = 0,
							sal_uInt32 nIdx = 0
							) const;

	void SetAutoStyles( sal_Bool bIsInAutoStyles ) { mbIsInAutoStyles = bIsInAutoStyles; }

	virtual void handleSpecialItem(
			SvXMLAttributeList& rAttrList,
			const XMLPropertyState& rProperty,
			const SvXMLUnitConverter& rUnitConverter,
			const SvXMLNamespaceMap& rNamespaceMap,
			const ::std::vector< XMLPropertyState > *pProperties = 0,
			sal_uInt32 nIdx = 0 ) const;
};

class XMLPageExportPropertyMapper : public SvXMLExportPropertyMapper
{
private:
	SvXMLExport& mrExport;

	const ::rtl::OUString msCDATA;
	const ::rtl::OUString msTrue;
	const ::rtl::OUString msFalse;

protected:
	virtual void ContextFilter(
		::std::vector< XMLPropertyState >& rProperties,
		::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet > rPropSet ) const;
public:
	XMLPageExportPropertyMapper( const UniReference< XMLPropertySetMapper >& rMapper, SvXMLExport& rExport );
	virtual ~XMLPageExportPropertyMapper();

	virtual void		handleElementItem(
                            SvXMLExport& rExport,
                            const XMLPropertyState& rProperty,
							sal_uInt16 nFlags,
							const ::std::vector< XMLPropertyState >* pProperties = 0,
							sal_uInt32 nIdx = 0
							) const;
};

}//end of namespace binfilter
#endif	//  _SDPROPLS_HXX
