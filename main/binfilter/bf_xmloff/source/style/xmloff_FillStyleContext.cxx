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




#ifndef _XMLOFF_FILLSTYLECONTEXTS_HXX_
#include "FillStyleContext.hxx"
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLOFF_GRADIENTSTYLE_HXX
#include "GradientStyle.hxx"
#endif
#ifndef _XMLOFF_HATCHSTYLE_HXX
#include "HatchStyle.hxx"
#endif
#ifndef _XMLOFF_IMAGESTYLE_HXX
#include "ImageStyle.hxx"
#endif
#ifndef _XMLOFF_TRANSGRADIENTSTYLE_HXX
#include "TransGradientStyle.hxx"
#endif
#ifndef _XMLOFF_MARKERSTYLE_HXX
#include "MarkerStyle.hxx"
#endif
#ifndef _XMLOFF_DASHSTYLE_HXX
#include "DashStyle.hxx"
#endif



#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_XMLBASE64IMPORTCONTEXT_HXX 
#include "XMLBase64ImportContext.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::rtl;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLGradientStyleContext, SvXMLStyleContext );

XMLGradientStyleContext::XMLGradientStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx, 
										      const OUString& rLName, 
											  const uno::Reference< xml::sax::XAttributeList >& xAttrList)
:	SvXMLStyleContext(rImport, nPrfx, rLName, xAttrList)
{
	// set Family
//	SetFamily( XML_STYLE_FAMILY_FILLSTYLE_GRADIENT_ID );

	// start import
	XMLGradientStyleImport aGradientStyle( GetImport() );
	aGradientStyle.importXML( xAttrList, maAny, maStrName );
}

XMLGradientStyleContext::~XMLGradientStyleContext()
{
}

void XMLGradientStyleContext::EndElement()
{
	uno::Reference< container::XNameContainer > xGradient( GetImport().GetGradientHelper() );

	try
	{
		if(xGradient.is())
		{
			if( xGradient->hasByName( maStrName ) )
			{
				xGradient->replaceByName( maStrName, maAny );
			}
			else
			{
				xGradient->insertByName( maStrName, maAny );
			}
		}
	}
	catch( container::ElementExistException& )
	{}
}

BOOL XMLGradientStyleContext::IsTransient() const
{
	return sal_True;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLHatchStyleContext, SvXMLStyleContext );

XMLHatchStyleContext::XMLHatchStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx, 
										      const OUString& rLName, 
											  const uno::Reference< xml::sax::XAttributeList >& xAttrList)
:	SvXMLStyleContext(rImport, nPrfx, rLName, xAttrList)
{
	// start import
	XMLHatchStyleImport aHatchStyle( GetImport() );
	aHatchStyle.importXML( xAttrList, maAny, maStrName );
}

XMLHatchStyleContext::~XMLHatchStyleContext()
{
}

void XMLHatchStyleContext::EndElement()
{
	uno::Reference< container::XNameContainer > xHatch( GetImport().GetHatchHelper() );

	try
	{
		if(xHatch.is())
		{
			if( xHatch->hasByName( maStrName ) )
			{
				xHatch->replaceByName( maStrName, maAny );
			}
			else
			{
				xHatch->insertByName( maStrName, maAny );
			}
		}
	}
	catch( container::ElementExistException& )
	{}
}

BOOL XMLHatchStyleContext::IsTransient() const
{
	return sal_True;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLBitmapStyleContext, SvXMLStyleContext );

XMLBitmapStyleContext::XMLBitmapStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx, 
										      const OUString& rLName, 
											  const uno::Reference< xml::sax::XAttributeList >& xAttrList)
:	SvXMLStyleContext(rImport, nPrfx, rLName, xAttrList)
{
	// start import
	XMLImageStyle aBitmapStyle;
	aBitmapStyle.importXML( xAttrList, maAny, maStrName, rImport );
}

XMLBitmapStyleContext::~XMLBitmapStyleContext()
{
}

SvXMLImportContext* XMLBitmapStyleContext::CreateChildContext( sal_uInt16 nPrefix, const ::rtl::OUString& rLocalName, const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = NULL;
	if( (XML_NAMESPACE_OFFICE == nPrefix) && xmloff::token::IsXMLToken( rLocalName, xmloff::token::XML_BINARY_DATA ) )
	{
		OUString sURL;
		maAny >>= sURL;
		if( !sURL.getLength() && !mxBase64Stream.is() )
		{
			mxBase64Stream = GetImport().GetStreamForGraphicObjectURLFromBase64();
			if( mxBase64Stream.is() )
				pContext = new XMLBase64ImportContext( GetImport(), nPrefix,
													rLocalName, xAttrList,
													mxBase64Stream );
		}
	}
	if( !pContext )
	{
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
	}

	return pContext;
}

void XMLBitmapStyleContext::EndElement()
{
	OUString sURL;
	maAny >>= sURL;

	if( !sURL.getLength() && mxBase64Stream.is() )
	{
		sURL = GetImport().ResolveGraphicObjectURLFromBase64( mxBase64Stream );
		mxBase64Stream = 0;
		maAny <<= sURL;
	}

	uno::Reference< container::XNameContainer > xBitmap( GetImport().GetBitmapHelper() );

	try
	{
		if(xBitmap.is())
		{
			if( xBitmap->hasByName( maStrName ) )
			{
				xBitmap->replaceByName( maStrName, maAny );
			}
			else
			{
				xBitmap->insertByName( maStrName, maAny );
			}
		}
	}
	catch( container::ElementExistException& )
	{}
}

BOOL XMLBitmapStyleContext::IsTransient() const
{
	return sal_True;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLTransGradientStyleContext, SvXMLStyleContext );

XMLTransGradientStyleContext::XMLTransGradientStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx, 
										      const OUString& rLName, 
											  const uno::Reference< xml::sax::XAttributeList >& xAttrList)
:	SvXMLStyleContext(rImport, nPrfx, rLName, xAttrList)
{
	// start import
	XMLTransGradientStyleImport aTransGradientStyle( GetImport() );
	aTransGradientStyle.importXML( xAttrList, maAny, maStrName );
}

XMLTransGradientStyleContext::~XMLTransGradientStyleContext()
{
}

void XMLTransGradientStyleContext::EndElement()
{
	uno::Reference< container::XNameContainer > xTransGradient( GetImport().GetTransGradientHelper() );

	try
	{
		if(xTransGradient.is())
		{
			if( xTransGradient->hasByName( maStrName ) )
			{
				xTransGradient->replaceByName( maStrName, maAny );
			}
			else
			{
				xTransGradient->insertByName( maStrName, maAny );
			}
		}
	}
	catch( container::ElementExistException& )
	{}
}

BOOL XMLTransGradientStyleContext::IsTransient() const
{
	return sal_True;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLMarkerStyleContext, SvXMLStyleContext );

XMLMarkerStyleContext::XMLMarkerStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx, 
										      const OUString& rLName, 
											  const uno::Reference< xml::sax::XAttributeList >& xAttrList)
:	SvXMLStyleContext(rImport, nPrfx, rLName, xAttrList)
{
	// start import
	XMLMarkerStyleImport aMarkerStyle( GetImport() );
	aMarkerStyle.importXML( xAttrList, maAny, maStrName );
}

XMLMarkerStyleContext::~XMLMarkerStyleContext()
{
}

void XMLMarkerStyleContext::EndElement()
{
	uno::Reference< container::XNameContainer > xMarker( GetImport().GetMarkerHelper() );

	try
	{
		if(xMarker.is())
		{
			if( xMarker->hasByName( maStrName ) )
			{
				xMarker->replaceByName( maStrName, maAny );
			}
			else
			{
				xMarker->insertByName( maStrName, maAny );
			}
		}
	}
	catch( container::ElementExistException& )
	{}
}

BOOL XMLMarkerStyleContext::IsTransient() const
{
	return sal_True;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLDashStyleContext, SvXMLStyleContext );

XMLDashStyleContext::XMLDashStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx, 
										  const OUString& rLName, 
										  const uno::Reference< xml::sax::XAttributeList >& xAttrList)
:	SvXMLStyleContext(rImport, nPrfx, rLName, xAttrList)
{
	// start import
	XMLDashStyleImport aDashStyle( GetImport() );
	aDashStyle.importXML( xAttrList, maAny, maStrName );
}

XMLDashStyleContext::~XMLDashStyleContext()
{
}

void XMLDashStyleContext::EndElement()
{
	uno::Reference< container::XNameContainer > xDashes( GetImport().GetDashHelper() );

	try
	{
		if(xDashes.is())
		{
			if( xDashes->hasByName( maStrName ) )
			{
				xDashes->replaceByName( maStrName, maAny );
			}
			else
			{
				xDashes->insertByName( maStrName, maAny );
			}
		}
	}
	catch( container::ElementExistException& )
	{}
}

BOOL XMLDashStyleContext::IsTransient() const
{
	return sal_True;
}
}//end of namespace binfilter
