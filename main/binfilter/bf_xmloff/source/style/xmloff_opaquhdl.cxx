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



#ifndef _XMLOFF_PROPERTYHANDLER_OPAQUETYPES_HXX
#include "opaquhdl.hxx"
#endif



// --


#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;


///////////////////////////////////////////////////////////////////////////////
//
// class XMLOpaquePropHdl
//

XMLOpaquePropHdl::~XMLOpaquePropHdl()
{
	// nothing to do
}

sal_Bool XMLOpaquePropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_True;

	sal_Bool bValue = IsXMLToken( rStrImpValue, XML_OPAQUE_FOREGROUND );
	rValue <<= sal_Bool(bValue);
	bRet = sal_True;

	return bRet; 
}

sal_Bool XMLOpaquePropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Bool bValue;

	if (rValue >>= bValue)
	{
		if( bValue )
			rStrExpValue = GetXMLToken( XML_OPAQUE_FOREGROUND );
		else
			rStrExpValue = GetXMLToken( XML_OPAQUE_BACKGROUND );

		bRet = sal_True;
	}
	
	return bRet;
}
}//end of namespace binfilter
