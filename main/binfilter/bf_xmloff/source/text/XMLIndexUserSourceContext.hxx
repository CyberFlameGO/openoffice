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



#ifndef _XMLOFF_XMLINDEXUSERSOURCECONTEXT_HXX_
#define _XMLOFF_XMLINDEXUSERSOURCECONTEXT_HXX_

#ifndef _XMLOFF_XMLINDEXSOURCEBASECONTEXT_HXX_
#include "XMLIndexSourceBaseContext.hxx"
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


/**
 * Import user defined index source element
 */
class XMLIndexUserSourceContext : public XMLIndexSourceBaseContext
{
	const ::rtl::OUString sCreateFromEmbeddedObjects;
	const ::rtl::OUString sCreateFromGraphicObjects; 
	const ::rtl::OUString sCreateFromMarks;
	const ::rtl::OUString sCreateFromTables;
	const ::rtl::OUString sCreateFromTextFrames;
	const ::rtl::OUString sUseLevelFromSource;
	const ::rtl::OUString sCreateFromLevelParagraphStyles;
    const ::rtl::OUString sUserIndexName;

	sal_Bool bUseObjects;
	sal_Bool bUseGraphic; 
	sal_Bool bUseMarks;
	sal_Bool bUseTables;
	sal_Bool bUseFrames;
	sal_Bool bUseLevelFromSource;
	sal_Bool bUseLevelParagraphStyles;
    ::rtl::OUString sIndexName;

public:

	TYPEINFO();

	XMLIndexUserSourceContext(
		SvXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName,
		::com::sun::star::uno::Reference< 
			::com::sun::star::beans::XPropertySet> & rPropSet);

	~XMLIndexUserSourceContext();

protected:

	virtual void ProcessAttribute(
		enum IndexSourceParamEnum eParam, 
		const ::rtl::OUString& rValue);

	virtual void EndElement();

	virtual SvXMLImportContext* CreateChildContext( 
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList );
};

}//end of namespace binfilter
#endif
