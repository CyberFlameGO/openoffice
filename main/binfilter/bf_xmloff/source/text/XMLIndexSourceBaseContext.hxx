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



#ifndef _XMLOFF_XMLINDEXSOURCEBASECONTEXT_HXX_
#define _XMLOFF_XMLINDEXSOURCEBASECONTEXT_HXX_

#ifndef _XMLOFF_XMLICTXT_HXX 
#include "xmlictxt.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif


namespace com { namespace sun { namespace star {
	namespace xml { namespace sax { class XAttributeList; } }
	namespace beans { class XPropertySet; }
} } }
namespace binfilter {

enum IndexSourceParamEnum
{
	XML_TOK_INDEXSOURCE_OUTLINE_LEVEL,
	XML_TOK_INDEXSOURCE_USE_INDEX_MARKS,
	XML_TOK_INDEXSOURCE_INDEX_SCOPE,
	XML_TOK_INDEXSOURCE_RELATIVE_TABS,
	XML_TOK_INDEXSOURCE_USE_OTHER_OBJECTS,
	XML_TOK_INDEXSOURCE_USE_SHEET,
	XML_TOK_INDEXSOURCE_USE_CHART,
	XML_TOK_INDEXSOURCE_USE_DRAW,
	XML_TOK_INDEXSOURCE_USE_IMAGE,
	XML_TOK_INDEXSOURCE_USE_MATH,
	XML_TOK_INDEXSOURCE_MAIN_ENTRY_STYLE,
	XML_TOK_INDEXSOURCE_IGNORE_CASE,
	XML_TOK_INDEXSOURCE_SEPARATORS,
	XML_TOK_INDEXSOURCE_COMBINE_ENTRIES,
	XML_TOK_INDEXSOURCE_COMBINE_WITH_DASH,
	XML_TOK_INDEXSOURCE_KEYS_AS_ENTRIES,
	XML_TOK_INDEXSOURCE_COMBINE_WITH_PP,
	XML_TOK_INDEXSOURCE_CAPITALIZE,
	XML_TOK_INDEXSOURCE_USE_OBJECTS,
	XML_TOK_INDEXSOURCE_USE_GRAPHICS,
	XML_TOK_INDEXSOURCE_USE_TABLES,
	XML_TOK_INDEXSOURCE_USE_FRAMES,
	XML_TOK_INDEXSOURCE_COPY_OUTLINE_LEVELS,
	XML_TOK_INDEXSOURCE_USE_CAPTION,
	XML_TOK_INDEXSOURCE_SEQUENCE_NAME,
	XML_TOK_INDEXSOURCE_SEQUENCE_FORMAT,
	XML_TOK_INDEXSOURCE_COMMA_SEPARATED,
	XML_TOK_INDEXSOURCE_USE_INDEX_SOURCE_STYLES,
    XML_TOK_INDEXSOURCE_SORT_ALGORITHM,
    XML_TOK_INDEXSOURCE_LANGUAGE,
    XML_TOK_INDEXSOURCE_COUNTRY,
    XML_TOK_INDEXSOURCE_USER_INDEX_NAME,
    XML_TOK_INDEXSOURCE_USE_OUTLINE_LEVEL
};


/**
 * Superclass for index source elements
 */
class XMLIndexSourceBaseContext : public SvXMLImportContext
{
	const ::rtl::OUString sCreateFromChapter;
	const ::rtl::OUString sIsRelativeTabstops;

	sal_Bool bUseLevelFormats;

	sal_Bool bChapterIndex;		/// chapter-wise or document index?
	sal_Bool bRelativeTabs;		/// tab stops relative to margin or indent?

protected:

	/// property set of index; must be accessible to subclasses
	::com::sun::star::uno::Reference< 
		::com::sun::star::beans::XPropertySet> & rIndexPropertySet;

public:

	TYPEINFO();

	XMLIndexSourceBaseContext(
		SvXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName,
		::com::sun::star::uno::Reference< 
			::com::sun::star::beans::XPropertySet> & rPropSet,
		sal_Bool bLevelFormats);

	~XMLIndexSourceBaseContext();

protected:

	virtual void StartElement(
		const ::com::sun::star::uno::Reference< 
			::com::sun::star::xml::sax::XAttributeList> & xAttrList);

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
