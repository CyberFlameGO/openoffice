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


#ifndef _XMLOFF_PAGEHEADERFOOTERCONTEXT_HXX
#define _XMLOFF_PAGEHEADERFOOTERCONTEXT_HXX

#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif
namespace binfilter {

class PageHeaderFooterContext : public SvXMLImportContext
{
	::std::vector< XMLPropertyState > &		rProperties;
	sal_Int32								nStartIndex;
	sal_Int32								nEndIndex;
	sal_Bool								bHeader;
	const UniReference < SvXMLImportPropertyMapper > rMap;

public:

	PageHeaderFooterContext( SvXMLImport& rImport, USHORT nPrfx,
					   const ::rtl::OUString& rLName,
					   const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
						::std::vector< XMLPropertyState > & rProperties,
						const UniReference < SvXMLImportPropertyMapper > &rMap,
						sal_Int32 nStartIndex, sal_Int32 nEndIndex,
						const sal_Bool bHeader);

	virtual ~PageHeaderFooterContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

}//end of namespace binfilter
#endif


