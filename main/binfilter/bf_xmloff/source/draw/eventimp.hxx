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



#ifndef _XMLOFF_EVENTIMP_HXX
#define _XMLOFF_EVENTIMP_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif

#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_ 
#include <com/sun/star/drawing/XShape.hpp>
#endif
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// office:events inside a shape

class SdXMLEventsContext : public SvXMLImportContext
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxShape;

public:
	TYPEINFO();

	SdXMLEventsContext( SvXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& rxShape );
	virtual ~SdXMLEventsContext();

	virtual SvXMLImportContext * CreateChildContext( USHORT nPrefix, const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList );
};

}//end of namespace binfilter
#endif	//  _XMLOFF_EVENTIMP_HXX

