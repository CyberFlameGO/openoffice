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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "hintids.hxx"
#include <tools/debug.hxx>

#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif

#include <bf_xmloff/nmspmap.hxx>
#include <bf_xmloff/xmlnmspe.hxx>
#include <bf_xmloff/xmlimp.hxx>
#include <bf_xmloff/xmltkmap.hxx>
#ifndef _XMLOFF_XMLBASE64IMPORTCONTEXT_HXX
#include <bf_xmloff/XMLBase64ImportContext.hxx>
#endif

#ifndef _SVX_UNOMID_HXX
#include <bf_svx/unomid.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <bf_svx/brshitem.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <bf_xmloff/xmluconv.hxx>
#endif

#include "xmlbrshi.hxx"
#include "xmlbrshe.hxx"
#include "xmlexp.hxx"
#include "xmlimpit.hxx"
#include "xmlexpit.hxx"
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

enum SvXMLTokenMapAttrs
{
	XML_TOK_BGIMG_HREF,
	XML_TOK_BGIMG_TYPE,
	XML_TOK_BGIMG_ACTUATE,
	XML_TOK_BGIMG_SHOW,
	XML_TOK_BGIMG_POSITION,
	XML_TOK_BGIMG_REPEAT,
	XML_TOK_BGIMG_FILTER,
	XML_TOK_NGIMG_END=XML_TOK_UNKNOWN
};

static __FAR_DATA SvXMLTokenMapEntry aBGImgAttributesAttrTokenMap[] =
{
	{ XML_NAMESPACE_XLINK, XML_HREF, 		XML_TOK_BGIMG_HREF		},
	{ XML_NAMESPACE_XLINK, XML_TYPE, 		XML_TOK_BGIMG_TYPE		},
	{ XML_NAMESPACE_XLINK, XML_ACTUATE,	    XML_TOK_BGIMG_ACTUATE	},
	{ XML_NAMESPACE_XLINK, XML_SHOW, 		XML_TOK_BGIMG_SHOW 		},
	{ XML_NAMESPACE_STYLE, XML_POSITION, 	XML_TOK_BGIMG_POSITION	},
	{ XML_NAMESPACE_STYLE, XML_REPEAT, 	    XML_TOK_BGIMG_REPEAT	},
	{ XML_NAMESPACE_STYLE, XML_FILTER_NAME, XML_TOK_BGIMG_FILTER	},
	XML_TOKEN_MAP_END
};

TYPEINIT1( SwXMLBrushItemImportContext, SvXMLImportContext );

void SwXMLBrushItemImportContext::ProcessAttrs(
		const Reference< xml::sax::XAttributeList >& xAttrList,
	    const SvXMLUnitConverter& rUnitConv )
{
	SvXMLTokenMap aTokenMap( aBGImgAttributesAttrTokenMap );

	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		const OUString& rAttrName = xAttrList->getNameByIndex( i );
		OUString aLocalName;
		sal_uInt16 nPrefix =
			GetImport().GetNamespaceMap().GetKeyByAttrName( rAttrName,
															&aLocalName );
		const OUString& rValue = xAttrList->getValueByIndex( i );

		switch( aTokenMap.Get( nPrefix, aLocalName ) )
		{
		case XML_TOK_BGIMG_HREF:
            SvXMLImportItemMapper::PutXMLValue(
                *pItem, GetImport().ResolveGraphicObjectURL( rValue,sal_False),
                MID_GRAPHIC_LINK, rUnitConv );
			break;
		case XML_TOK_BGIMG_TYPE:
		case XML_TOK_BGIMG_ACTUATE:
		case XML_TOK_BGIMG_SHOW:
			break;
		case XML_TOK_BGIMG_POSITION:
            SvXMLImportItemMapper::PutXMLValue(
                *pItem, rValue, MID_GRAPHIC_POSITION, rUnitConv );
			break;
		case XML_TOK_BGIMG_REPEAT:
            SvXMLImportItemMapper::PutXMLValue(
                *pItem, rValue, MID_GRAPHIC_REPEAT, rUnitConv );
			break;
		case XML_TOK_BGIMG_FILTER:
            SvXMLImportItemMapper::PutXMLValue(
                *pItem, rValue, MID_GRAPHIC_FILTER, rUnitConv );
			break;
		}
	}

}

SvXMLImportContext *SwXMLBrushItemImportContext::CreateChildContext(
		sal_uInt16 nPrefix, const OUString& rLocalName,
		const Reference< xml::sax::XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;
	if( xmloff::token::IsXMLToken( rLocalName,
										xmloff::token::XML_BINARY_DATA ) )
	{
		if( !(pItem->GetGraphicLink() || pItem->GetGraphic() ) && !xBase64Stream.is() )
		{
			xBase64Stream = GetImport().GetStreamForGraphicObjectURLFromBase64();
			if( xBase64Stream.is() )
				pContext = new XMLBase64ImportContext( GetImport(), nPrefix,
													rLocalName, xAttrList,
													xBase64Stream );
		}
	}
	if( !pContext )
	{
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
	}

	return pContext;
}

void SwXMLBrushItemImportContext::EndElement()
{
	if( xBase64Stream.is() )
	{
		OUString sURL( GetImport().ResolveGraphicObjectURLFromBase64( xBase64Stream ) );
		xBase64Stream = 0;
        SvXMLImportItemMapper::PutXMLValue( *pItem, sURL, MID_GRAPHIC_LINK, GetImport().GetMM100UnitConverter() );
	}

	if( !(pItem->GetGraphicLink() || pItem->GetGraphic() ) )
		pItem->SetGraphicPos( GPOS_NONE );
	else if( GPOS_NONE == pItem->GetGraphicPos() )
		pItem->SetGraphicPos( GPOS_TILED );
}

SwXMLBrushItemImportContext::SwXMLBrushItemImportContext(
		SvXMLImport& rImport, sal_uInt16 nPrfx,
		const OUString& rLName,
		const Reference< xml::sax::XAttributeList >& xAttrList,
		const SvXMLUnitConverter& rUnitConv,
		const SvxBrushItem& rItem ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	pItem( new SvxBrushItem( rItem ) )
{
	// delete any grephic that is existing
	pItem->SetGraphicPos( GPOS_NONE );

	ProcessAttrs( xAttrList, rUnitConv );
}

SwXMLBrushItemImportContext::SwXMLBrushItemImportContext(
		SvXMLImport& rImport, sal_uInt16 nPrfx,
		const OUString& rLName,
		const Reference< xml::sax::XAttributeList > & xAttrList,
		const SvXMLUnitConverter& rUnitConv,
		sal_uInt16 nWhich ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	pItem( new SvxBrushItem( nWhich ) )
{
	ProcessAttrs( xAttrList, rUnitConv );
}

SwXMLBrushItemImportContext::~SwXMLBrushItemImportContext()
{
	delete pItem;
}

SwXMLBrushItemExport::SwXMLBrushItemExport( SwXMLExport& rExp ) :
	rExport( rExp )
{
}

SwXMLBrushItemExport::~SwXMLBrushItemExport()
{
}


void SwXMLBrushItemExport::exportXML( const SvxBrushItem& rItem )
{
	GetExport().CheckAttrList();

	OUString sValue, sURL;
	const SvXMLUnitConverter& rUnitConv = GetExport().GetTwipUnitConverter();
	if( SvXMLExportItemMapper::QueryXMLValue(
            rItem, sURL, MID_GRAPHIC_LINK, rUnitConv ) )
	{
		sValue = GetExport().AddEmbeddedGraphicObject( sURL );
		if( sValue.getLength() )
		{
			GetExport().AddAttribute( XML_NAMESPACE_XLINK, XML_HREF, sValue );
			GetExport().AddAttribute( XML_NAMESPACE_XLINK, XML_TYPE, XML_SIMPLE );
	//		AddAttribute( XML_NAMESPACE_XLINK, XML_SHOW, ACP2WS(sXML_embed) );
			GetExport().AddAttribute( XML_NAMESPACE_XLINK, XML_ACTUATE, XML_ONLOAD );
		}

		if( SvXMLExportItemMapper::QueryXMLValue(
                rItem, sValue, MID_GRAPHIC_POSITION, rUnitConv ) )
			GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_POSITION, sValue );

		if( SvXMLExportItemMapper::QueryXMLValue(
                rItem, sValue, MID_GRAPHIC_REPEAT, rUnitConv ) )
			GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_REPEAT, sValue );

		if( SvXMLExportItemMapper::QueryXMLValue(
                rItem, sValue, MID_GRAPHIC_FILTER, rUnitConv ) )
			GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_FILTER_NAME, sValue );
	}

	{
		SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE, XML_BACKGROUND_IMAGE,
								  sal_True, sal_True );
		if( sURL.getLength() )
		{
			// optional office:binary-data
			GetExport().AddEmbeddedGraphicObjectAsBase64( sURL );
		}
	}
}


}
