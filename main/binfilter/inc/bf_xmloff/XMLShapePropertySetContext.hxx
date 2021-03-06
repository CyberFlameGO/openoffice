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


#ifndef _XMLOFF_XMLSHAPEPROPERTYSETCONTEXT_HXX
#define _XMLOFF_XMLSHAPEPROPERTYSETCONTEXT_HXX


#ifndef _XMLOFF_XMLPROPERTYSETCONTEXT_HXX 
#include "xmlprcon.hxx"
#endif
namespace binfilter {

class XMLShapePropertySetContext : public SvXMLPropertySetContext
{
	SvXMLImportContextRef mxBulletStyle;
	sal_Int32 mnBulletIndex;

public:

	TYPEINFO();

	XMLShapePropertySetContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
				const ::rtl::OUString& rLName,
				 const ::com::sun::star::uno::Reference<
				 		::com::sun::star::xml::sax::XAttributeList >& xAttrList,
				 ::std::vector< XMLPropertyState > &rProps,
				 const UniReference < SvXMLImportPropertyMapper > &rMap );

	virtual ~XMLShapePropertySetContext();

	virtual void EndElement();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
								   const ::rtl::OUString& rLocalName,
								   const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList,
								   ::std::vector< XMLPropertyState > &rProperties,
								   const XMLPropertyState& rProp);
};

}//end of namespace binfilter
#endif
