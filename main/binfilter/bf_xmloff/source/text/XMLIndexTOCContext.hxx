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



#ifndef _XMLOFF_XMLINDEXTOCCONTEXT_HXX_
#define _XMLOFF_XMLINDEXTOCCONTEXT_HXX_

#ifndef _XMLOFF_XMLICTXT_HXX 
#include "xmlictxt.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif


namespace com { namespace sun { namespace star {
	namespace xml { namespace sax { class XAttributeList; } }
	namespace beans { class XPropertySet; }
} } }
namespace rtl {	class OUString; }
namespace binfilter {


enum IndexTypeEnum 
{
	TEXT_INDEX_TOC,
	TEXT_INDEX_ALPHABETICAL,
	TEXT_INDEX_TABLE,
	TEXT_INDEX_OBJECT,
	TEXT_INDEX_BIBLIOGRAPHY,
	TEXT_INDEX_USER,
	TEXT_INDEX_ILLUSTRATION,
	
	TEXT_INDEX_UNKNOWN
};


/**
 * Import all indices.
 *
 * Originally, this class would import only the TOC (table of
 * content), but now it's role has been expanded to handle all
 * indices, and hence is named inappropriately. Depending on the
 * element name it decides which index source element context to create. 
 */
class XMLIndexTOCContext : public SvXMLImportContext
{
	const ::rtl::OUString sTitle;
    const ::rtl::OUString sIsProtected;
    const ::rtl::OUString sName;
	
	/** XPropertySet of the index */
	::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet> xTOCPropertySet;

	enum IndexTypeEnum eIndexType;

	/** source element name (for CreateChildContext) */
	const sal_Char* pSourceElementName;

	sal_Bool bValid;

    SvXMLImportContextRef xBodyContextRef;

public:

	TYPEINFO();

	XMLIndexTOCContext(
		SvXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName );

	~XMLIndexTOCContext();

protected:

	virtual void StartElement(
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList);

	virtual void EndElement();

	virtual SvXMLImportContext *CreateChildContext( 
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList );
};

}//end of namespace binfilter
#endif
