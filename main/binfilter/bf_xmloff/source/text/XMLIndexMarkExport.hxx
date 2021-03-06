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



#ifndef _XMLOFF_XMLINDEXMARKEXPORT_HXX_
#define _XMLOFF_XMLINDEXMARKEXPORT_HXX_

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_ 
#include <com/sun/star/uno/Sequence.h>
#endif

namespace com { namespace sun { namespace star { 
	namespace beans { class XPropertySet; }
 } } }
namespace rtl {
	class OUString;
	class OUStringBuffer;
}
namespace binfilter {
class SvXMLExport;
class XMLTextParagraphExport;


/** 
 * This class handles the export of index marks for table of content,
 * alphabetical and user index. 
 *
 * Marks for bibliography indices are internally modelled as text
 * fields and thus handled in txtparae.cxx
 */
class XMLIndexMarkExport
{
	::rtl::OUString sLevel;
	::rtl::OUString sUserIndexName;
	::rtl::OUString sPrimaryKey;
	::rtl::OUString sSecondaryKey;
	::rtl::OUString sDocumentIndexMark;
	::rtl::OUString sIsStart;
	::rtl::OUString sIsCollapsed;
	::rtl::OUString sAlternativeText;
    ::rtl::OUString sTextReading;
    ::rtl::OUString sPrimaryKeyReading;
    ::rtl::OUString sSecondaryKeyReading;
    ::rtl::OUString sMainEntry;


	SvXMLExport& rExport;
	XMLTextParagraphExport& rParaExport;

public:
	XMLIndexMarkExport(SvXMLExport& rExp,
					   XMLTextParagraphExport& rParaExp);

	~XMLIndexMarkExport();

	/**
	 * export by the property set of its *text* *portion*.
	 *
	 * The text portion supplies us with the properties of the index
	 * mark itself, as well as the information whether we are at the
	 * start or end of an index mark, or whether the index mark is
	 * collapsed.
	 */
	void ExportIndexMark(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rPropSet,
		sal_Bool bAutoStyles);

protected:

	/// export attributes of table-of-content index marks
	void ExportTOCMarkAttributes(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rPropSet);

	/// export attributes of user index marks
	void ExportUserIndexMarkAttributes(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rPropSet);

	/// export attributes of alphabetical  index marks
	void ExportAlphabeticalIndexMarkAttributes(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rPropSet);

	/// create a numerical ID for this index mark 
	/// (represented by its properties)
	void GetID(
		::rtl::OUStringBuffer& sBuffer,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rPropSet);

};

}//end of namespace binfilter
#endif
