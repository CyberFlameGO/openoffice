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



#ifndef SC_XMLEXPORTDATABASERANGES_HXX
#define SC_XMLEXPORTDATABASERANGES_HXX

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XSHEETFILTERDESCRIPTOR_HPP_
#include <com/sun/star/sheet/XSheetFilterDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XSUBTOTALDESCRIPTOR_HPP_
#include <com/sun/star/sheet/XSubTotalDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XSPREADSHEETDOCUMENT_HPP_
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#endif
namespace binfilter {

class ScXMLExport;
class ScDocument;
class ScMyEmptyDatabaseRangesContainer;

class ScXMLExportDatabaseRanges
{
	ScXMLExport&	rExport;
	ScDocument*		pDoc;

	void WriteImportDescriptor(const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue> aImportDescriptor);
	::rtl::OUString getOperatorXML(const ::com::sun::star::sheet::FilterOperator aFilterOperator, const sal_Bool bUseRegularExpressions) const;
	void WriteCondition(const ::com::sun::star::sheet::TableFilterField& aFilterField, sal_Bool bIsCaseSensitive, sal_Bool bUseRegularExpressions);
	void WriteFilterDescriptor(const ::com::sun::star::uno::Reference < ::com::sun::star::sheet::XSheetFilterDescriptor>& xSheetFilterDescriptor, const ::rtl::OUString sDatabaseRangeName);
	void WriteSortDescriptor(const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue> aSortProperties);
	void WriteSubTotalDescriptor(const ::com::sun::star::uno::Reference < ::com::sun::star::sheet::XSubTotalDescriptor> xSubTotalDescriptor, const ::rtl::OUString sDatabaseRangeName);
public:
	ScXMLExportDatabaseRanges(ScXMLExport& rExport);
	~ScXMLExportDatabaseRanges();
	ScMyEmptyDatabaseRangesContainer GetEmptyDatabaseRanges();
	void WriteDatabaseRanges(const ::com::sun::star::uno::Reference < ::com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc);
};

} //namespace binfilter
#endif

