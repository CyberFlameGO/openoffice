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


#ifndef SC_XMLDRANI_HXX
#define SC_XMLDRANI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif
#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif
#ifndef _COM_SUN_STAR_SHEET_DATAIMPORTMODE_HPP_
#include <com/sun/star/sheet/DataImportMode.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_SUBTOTALCOLUMN_HPP_
#include <com/sun/star/sheet/SubTotalColumn.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_TABLEFILTERFIELD_HPP_
#include <com/sun/star/sheet/TableFilterField.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_CELLADDRESS_HPP_
#include <com/sun/star/table/CellAddress.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_CELLRANGEADDRESS_HPP_
#include <com/sun/star/table/CellRangeAddress.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_TABLEORIENTATION_HPP_ 
#include <com/sun/star/table/TableOrientation.hpp>
#endif
namespace binfilter {

class ScXMLImport;

class ScXMLDatabaseRangesContext : public SvXMLImportContext
{
	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLDatabaseRangesContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList);

	virtual ~ScXMLDatabaseRangesContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLDatabaseRangeContext : public SvXMLImportContext
{
	::rtl::OUString 	sDatabaseRangeName;
	::rtl::OUString	sRangeAddress;
	::rtl::OUString	sDatabaseName;
	::rtl::OUString	sSourceObject;
	::com::sun::star::uno::Sequence < ::com::sun::star::sheet::SubTotalColumn> aSubTotalColumns;
	::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue> aSortSequence;
	::com::sun::star::uno::Sequence < ::com::sun::star::sheet::TableFilterField> aFilterFields;
	::com::sun::star::table::CellAddress aFilterOutputPosition;
	::com::sun::star::table::CellRangeAddress aFilterConditionSourceRangeAddress;
	::com::sun::star::sheet::DataImportMode nSourceType;
	::com::sun::star::table::TableOrientation eOrientation;
	sal_Int32		nRefresh;
	sal_Int16		nSubTotalsUserListIndex;
	sal_Int16		nSubTotalRuleGroupFieldNumber;
	sal_Bool		bContainsSort : 1;
	sal_Bool		bContainsSubTotal : 1;
	sal_Bool 		bNative : 1;
	sal_Bool		bIsSelection : 1;
	sal_Bool		bKeepFormats : 1;
	sal_Bool		bMoveCells : 1;
	sal_Bool		bStripData : 1;
	sal_Bool		bContainsHeader : 1;
	sal_Bool		bAutoFilter : 1;
	sal_Bool		bSubTotalsBindFormatsToContent : 1;
	sal_Bool		bSubTotalsIsCaseSensitive : 1;
	sal_Bool		bSubTotalsInsertPageBreaks : 1;
	sal_Bool		bSubTotalsSortGroups : 1;
	sal_Bool		bSubTotalsEnabledUserList : 1;
	sal_Bool		bSubTotalsAscending : 1;
	sal_Bool		bFilterCopyOutputData : 1;
	sal_Bool		bFilterIsCaseSensitive : 1;
	sal_Bool		bFilterSkipDuplicates : 1;
	sal_Bool		bFilterUseRegularExpressions : 1;
	sal_Bool		bFilterConditionSourceRange : 1;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLDatabaseRangeContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList);

	virtual ~ScXMLDatabaseRangeContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();

	void SetDatabaseName(const ::rtl::OUString sTempDatabaseName) { sDatabaseName = sTempDatabaseName; }
	void SetSourceObject(const ::rtl::OUString sTempSourceObject) { sSourceObject = sTempSourceObject; }
	void SetSourceType(const ::com::sun::star::sheet::DataImportMode nTempSourceType) { nSourceType = nTempSourceType; }
	void SetNative(const sal_Bool bTempNative) { bNative = bTempNative; }
	void SetSubTotalsBindFormatsToContent(const sal_Bool bTemp ) { bSubTotalsBindFormatsToContent = bTemp; }
	void SetSubTotalsIsCaseSensitive(const sal_Bool bTemp) { bSubTotalsIsCaseSensitive = bTemp; }
	void SetSubTotalsInsertPageBreaks(const sal_Bool bTemp) { bSubTotalsInsertPageBreaks = bTemp; }
	void SetSubTotalsEnabledUserList(const sal_Bool bTemp) { bSubTotalsEnabledUserList = bTemp; }
	void SetSubTotalsUserListIndex(const sal_Int16 nTemp) { nSubTotalsUserListIndex = nTemp; }
	void SetSubTotalsAscending(const sal_Bool bTemp) { bSubTotalsAscending = bTemp; }
	void SetSubTotalsSortGroups(const sal_Bool bTemp) { bSubTotalsSortGroups = bTemp; }
	void SetSubTotalRuleGroupFieldNumber(const sal_Int16 nTemp) { nSubTotalRuleGroupFieldNumber = nTemp; }
	void AddSubTotalColumn(const ::com::sun::star::sheet::SubTotalColumn aSubTotalColumn)
		{ aSubTotalColumns.realloc(aSubTotalColumns.getLength() + 1); aSubTotalColumns[aSubTotalColumns.getLength() - 1] = aSubTotalColumn; }
	void SetSortSequence(const ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue>& aTempSortSequence) { aSortSequence = aTempSortSequence; }
	void SetFilterCopyOutputData(const sal_Bool bTemp) { bFilterCopyOutputData = bTemp; }
	void SetFilterIsCaseSensitive(const sal_Bool bTemp) { bFilterIsCaseSensitive = bTemp; }
	void SetFilterSkipDuplicates(const sal_Bool bTemp) { bFilterSkipDuplicates = bTemp; }
	void SetFilterUseRegularExpressions(const sal_Bool bTemp) { bFilterUseRegularExpressions = bTemp; }
	void SetFilterFields(const ::com::sun::star::uno::Sequence < ::com::sun::star::sheet::TableFilterField>& aTemp) { aFilterFields = aTemp; }
	void SetFilterOutputPosition(const ::com::sun::star::table::CellAddress& aTemp) { aFilterOutputPosition = aTemp; }
	void SetFilterConditionSourceRangeAddress(const ::com::sun::star::table::CellRangeAddress& aTemp) { aFilterConditionSourceRangeAddress = aTemp;
																									bFilterConditionSourceRange = sal_True; }
};

class ScXMLSourceSQLContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSourceSQLContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSourceSQLContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLSourceTableContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSourceTableContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSourceTableContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLSourceQueryContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSourceQueryContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSourceQueryContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLSubTotalRulesContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSubTotalRulesContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSubTotalRulesContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLSortGroupsContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSortGroupsContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSortGroupsContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLSubTotalRuleContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSubTotalRuleContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSubTotalRuleContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLSubTotalFieldContext : public SvXMLImportContext
{
	ScXMLDatabaseRangeContext* pDatabaseRangeContext;
	::rtl::OUString sFieldNumber;
	::rtl::OUString sFunction;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLSubTotalFieldContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
										ScXMLDatabaseRangeContext* pTempDatabaseRangeContext);

	virtual ~ScXMLSubTotalFieldContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

} //namespace binfilter
#endif
