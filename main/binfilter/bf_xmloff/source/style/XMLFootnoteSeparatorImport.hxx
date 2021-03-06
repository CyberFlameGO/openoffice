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



#ifndef _XMLOFF_XMLFOOTNOTESEPARATORIMPORT_HXX
#define _XMLOFF_XMLFOOTNOTESEPARATORIMPORT_HXX



#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif

#ifndef _UNIVERSALL_REFERENCE_HXX
#include "uniref.hxx"
#endif

#include <vector>
namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
	namespace uno { template<class X> class Reference; }
	namespace xml { namespace sax { class XAttributeList; } }
} } }
namespace binfilter {


class SvXMLImport;
struct XMLPropertyState;
class XMLPropertySetMapper;



/**
 * Import the footnote-separator element in page styles.
 */
class XMLFootnoteSeparatorImport : public SvXMLImportContext
{
	::std::vector<XMLPropertyState> & rProperties;
	UniReference<XMLPropertySetMapper> rMapper;
	sal_Int32 nPropIndex;

public:
	
	TYPEINFO();

	XMLFootnoteSeparatorImport(
		SvXMLImport& rImport, 
		sal_uInt16 nPrefix, 
		const ::rtl::OUString& rLocalName, 
		::std::vector<XMLPropertyState> & rProperties,
		const UniReference<XMLPropertySetMapper> & rMapperRef,
		sal_Int32 nIndex);

	~XMLFootnoteSeparatorImport();

	virtual void StartElement( 
		const ::com::sun::star::uno::Reference<
				::com::sun::star::xml::sax::XAttributeList> & xAttrList );
};

}//end of namespace binfilter
#endif
