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




#ifndef _XMLOFF_PROPERTYHANDLER_CHARHEIGHTTYPES_HXX
#include <chrhghdl.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _XMLOFF_XMLEHELP_HXX
#include "xmlehelp.hxx"
#endif

#ifndef _RTL_USTRBUF_HXX_ 
#include <rtl/ustrbuf.hxx>
#endif


namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;

// this is a copy of defines in svx/inc/escpitem.hxx
#define DFLT_ESC_PROP	 58
#define DFLT_ESC_AUTO_SUPER	101
#define DFLT_ESC_AUTO_SUB  -101

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementPropHdl
//

XMLCharHeightHdl::~XMLCharHeightHdl()
{
	// nothing to do
}

sal_Bool XMLCharHeightHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
    double fSize;

	if( rStrImpValue.indexOf( sal_Unicode('%') ) == -1 )
	{
		MapUnit eSrcUnit = SvXMLExportHelper::GetUnitFromString( rStrImpValue, MAP_POINT );
        if( SvXMLUnitConverter::convertDouble( fSize, rStrImpValue, eSrcUnit, MAP_POINT ))
		{
			rValue <<= (float)fSize;
			return sal_True; 
		}
	}

	return sal_False;
}

sal_Bool XMLCharHeightHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;

	float fSize = 0;
	if( rValue >>= fSize )
	{
        SvXMLUnitConverter::convertDouble( aOut, (double)fSize, TRUE, MAP_POINT, MAP_POINT );
        aOut.append( sal_Unicode('p'));
        aOut.append( sal_Unicode('t'));
	}
	
	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength() != 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementHeightPropHdl
//

XMLCharHeightPropHdl::~XMLCharHeightPropHdl()
{
	// nothing to do
}

sal_Bool XMLCharHeightPropHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Int32 nPrc = 100;

	if( rStrImpValue.indexOf( sal_Unicode('%') ) != -1 )
	{
		if( rUnitConverter.convertPercent( nPrc, rStrImpValue ) )
		{
			rValue <<= (sal_Int16)nPrc;
			return sal_True; 
		}
	}

	return sal_False;
}

sal_Bool XMLCharHeightPropHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut( rStrExpValue );

	sal_Int16 nValue;
	if( rValue >>= nValue )
	{
		rUnitConverter.convertPercent( aOut, nValue );
	}

	rStrExpValue = aOut.makeStringAndClear();
	return rStrExpValue.getLength() != 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// class XMLEscapementPropHdl
//

XMLCharHeightDiffHdl::~XMLCharHeightDiffHdl()
{
	// nothing to do
}

sal_Bool XMLCharHeightDiffHdl::importXML( const OUString& rStrImpValue, uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Int32 nRel = 0;

	if( SvXMLUnitConverter::convertMeasure( nRel, rStrImpValue, MAP_POINT ) )
	{
		rValue <<= (float)nRel;
		return sal_True; 
	}

	return sal_False;
}

sal_Bool XMLCharHeightDiffHdl::exportXML( OUString& rStrExpValue, const uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	OUStringBuffer aOut;

	float nRel = 0;
	if( (rValue >>= nRel) && (nRel != 0) )
	{
		SvXMLUnitConverter::convertMeasure( aOut, nRel, MAP_POINT, MAP_POINT );
		rStrExpValue = aOut.makeStringAndClear();
	}
	
	return rStrExpValue.getLength() != 0;
}

}//end of namespace binfilter
