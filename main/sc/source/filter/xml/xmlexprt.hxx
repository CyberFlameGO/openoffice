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


#ifndef SC_XMLEXPRT_HXX
#define SC_XMLEXPRT_HXX

#include <xmloff/xmlexp.hxx>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/table/XCellRange.hpp>

namespace com { namespace sun { namespace star {
	namespace beans { class XPropertySet; }
} } }

#include <hash_map>

class ScOutlineArray;
class SvXMLExportPropertyMapper;
class ScMyShapesContainer;
class ScMyMergedRangesContainer;
class ScMyValidationsContainer;
class ScMyNotEmptyCellsIterator;
class ScChangeTrackingExportHelper;
class ScColumnStyles;
class ScRowStyles;
class ScFormatRangeStyles;
class ScRowFormatRanges;
class ScMyOpenCloseColumnRowGroup;
class ScMyAreaLinksContainer;
class ScMyDetectiveOpContainer;
struct ScMyCell;
class ScDocument;
class ScMySharedData;
class ScMyDefaultStyles;
class XMLNumberFormatAttributesExportHelper;
class ScChartListener;
class SfxItemPool;
class ScAddress;
class ScBaseCell;

typedef std::vector< com::sun::star::uno::Reference < com::sun::star::drawing::XShapes > > ScMyXShapesVec;

class ScXMLExport : public SvXMLExport
{
	ScDocument*					pDoc;
	com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheet> xCurrentTable;
    com::sun::star::uno::Reference <com::sun::star::table::XCellRange> xCurrentTableCellRange;

    com::sun::star::uno::Reference<com::sun::star::io::XInputStream> xSourceStream;
    sal_Int32                   nSourceStreamPos;

	UniReference < XMLPropertyHandlerFactory >	xScPropHdlFactory;
	UniReference < XMLPropertySetMapper >		xCellStylesPropertySetMapper;
	UniReference < XMLPropertySetMapper >		xColumnStylesPropertySetMapper;
	UniReference < XMLPropertySetMapper >		xRowStylesPropertySetMapper;
	UniReference < XMLPropertySetMapper >		xTableStylesPropertySetMapper;
	UniReference < SvXMLExportPropertyMapper >	xCellStylesExportPropertySetMapper;
	UniReference < SvXMLExportPropertyMapper >	xColumnStylesExportPropertySetMapper;
	UniReference < SvXMLExportPropertyMapper >	xRowStylesExportPropertySetMapper;
	UniReference < SvXMLExportPropertyMapper >	xTableStylesExportPropertySetMapper;
	XMLNumberFormatAttributesExportHelper* pNumberFormatAttributesExportHelper;
    typedef ::std::hash_map<sal_Int32, sal_Int32>  NumberFormatIndexMap;
    NumberFormatIndexMap                aNumFmtIndexMap;
	ScMySharedData*						pSharedData;
	ScColumnStyles*					pColumnStyles;
	ScRowStyles*					pRowStyles;
	ScFormatRangeStyles*				pCellStyles;
	ScRowFormatRanges*					pRowFormatRanges;
	std::vector<rtl::OUString>			aTableStyles;
	com::sun::star::table::CellRangeAddress	aRowHeaderRange;
	ScMyOpenCloseColumnRowGroup*		pGroupColumns;
	ScMyOpenCloseColumnRowGroup*		pGroupRows;
	ScMyDefaultStyles*					pDefaults;
	ScChartListener*					pChartListener;
    const ScMyCell*                     pCurrentCell;

	ScMyMergedRangesContainer*	pMergedRangesContainer;
	ScMyValidationsContainer*	pValidationsContainer;
	ScMyNotEmptyCellsIterator*	pCellsItr;
	ScChangeTrackingExportHelper*	pChangeTrackingExportHelper;
	const rtl::OUString			sLayerID;
	const rtl::OUString			sCaptionShape;
    rtl::OUString               sExternalRefTabStyleName;
    rtl::OUString               sAttrName;
    rtl::OUString               sAttrStyleName;
    rtl::OUString               sAttrColumnsRepeated;
    rtl::OUString               sAttrFormula;
    rtl::OUString               sAttrValueType;
    rtl::OUString               sAttrStringValue;
    rtl::OUString               sElemCell;
    rtl::OUString               sElemCoveredCell;
    rtl::OUString               sElemCol;
    rtl::OUString               sElemRow;
    rtl::OUString               sElemTab;
    rtl::OUString               sElemP;
    sal_Int32					nOpenRow;
    sal_Int32                   nProgressCount;
	sal_uInt16					nCurrentTable;
	sal_Bool					bHasRowHeader;
	sal_Bool					bRowHeaderOpen;
	sal_Bool					mbShowProgress;

    sal_Int32       GetNumberFormatStyleIndex(sal_Int32 nNumFmt) const;
	sal_Bool		HasDrawPages(com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xDoc);
	void			CollectSharedData(sal_Int32& nTableCount, sal_Int32& nShapesCount, const sal_Int32 nCellCount);
	void			CollectShapesAutoStyles(const sal_Int32 nTableCount);
	void			WriteTablesView(const com::sun::star::uno::Any& aTableView);
	void			WriteView(const com::sun::star::uno::Any& aView);
	virtual void _ExportFontDecls();
	virtual void _ExportStyles( sal_Bool bUsed );
	virtual void _ExportAutoStyles();
	virtual void _ExportMasterStyles();
	virtual void SetBodyAttributes();
	virtual void _ExportContent();
	virtual void _ExportMeta();

	void CollectInternalShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );

	com::sun::star::table::CellRangeAddress GetEndAddress(const com::sun::star::uno::Reference<com::sun::star::sheet::XSpreadsheet>& xTable,
														const sal_Int32 nTable);
//	ScMyEmptyDatabaseRangesContainer GetEmptyDatabaseRanges();
	void GetAreaLinks( com::sun::star::uno::Reference< com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc, ScMyAreaLinksContainer& rAreaLinks );
	void GetDetectiveOpList( ScMyDetectiveOpContainer& rDetOp );
	void WriteSingleColumn(const sal_Int32 nRepeatColumns, const sal_Int32 nStyleIndex,
		const sal_Int32 nIndex, const sal_Bool bIsAutoStyle, const sal_Bool bIsVisible);
	void WriteColumn(const sal_Int32 nColumn, const sal_Int32 nRepeatColumns,
		const sal_Int32 nStyleIndex, const sal_Bool bIsVisible);
	void OpenHeaderColumn();
	void CloseHeaderColumn();
	void ExportColumns(const sal_Int32 nTable, const com::sun::star::table::CellRangeAddress& aColumnHeaderRange, const sal_Bool bHasColumnHeader);
    void ExportExternalRefCacheStyles();
	void ExportFormatRanges(const sal_Int32 nStartCol, const sal_Int32 nStartRow,
		const sal_Int32 nEndCol, const sal_Int32 nEndRow, const sal_Int32 nSheet);
	void WriteRowContent();
	void WriteRowStartTag(sal_Int32 nRow, const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nEmptyRows);
	void OpenHeaderRows();
	void CloseHeaderRows();
	void OpenNewRow(const sal_Int32 nIndex, const sal_Int8 nFlag, const sal_Int32 nStartRow, const sal_Int32 nEmptyRows);
	void OpenAndCloseRow(const sal_Int32 nIndex, const sal_Int8 nFlag,
		const sal_Int32 nStartRow, const sal_Int32 nEmptyRows);
	void OpenRow(const sal_Int32 nTable, const sal_Int32 nStartRow, const sal_Int32 nRepeatRow);
	void CloseRow(const sal_Int32 nRow);
	void GetColumnRowHeader(sal_Bool& bHasColumnHeader, com::sun::star::table::CellRangeAddress& aColumnHeaderRange,
        sal_Bool& bHasRowHeader, com::sun::star::table::CellRangeAddress& aRowHeaderRange,
        rtl::OUString& rPrintRanges) const;
	void FillFieldGroup(ScOutlineArray* pFields, ScMyOpenCloseColumnRowGroup* pGroups);
	void FillColumnRowGroups();

//UNUSED2008-05  sal_Bool GetMerge (const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheet>& xTable,
//UNUSED2008-05                     sal_Int32 nCol, sal_Int32 nRow,
//UNUSED2008-05                     com::sun::star::table::CellRangeAddress& aCellAddress);

	sal_Bool GetMerged (const com::sun::star::table::CellRangeAddress* pCellRange,
		const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheet>& xTable);

	sal_Bool GetCellText (ScMyCell& rMyCell, const ScAddress& aPos) const;

    void WriteCell(ScMyCell& aCell, sal_Int32 nEqualCellCount);
	void WriteAreaLink(const ScMyCell& rMyCell);
	void WriteAnnotation(ScMyCell& rMyCell);
	void WriteDetective(const ScMyCell& rMyCell);
	void ExportShape(const com::sun::star::uno::Reference < com::sun::star::drawing::XShape >& xShape, com::sun::star::awt::Point* pPoint);
	void WriteShapes(const ScMyCell& rMyCell);
	void WriteTableShapes();
    void SetRepeatAttribute(sal_Int32 nEqualCellCount, bool bIncProgress);

	sal_Bool IsCellTypeEqual (const ScMyCell& aCell1, const ScMyCell& aCell2) const;
 	sal_Bool IsEditCell(const com::sun::star::table::CellAddress& aAddress, ScMyCell* pMyCell = NULL) const;
//UNUSED2008-05  sal_Bool IsEditCell(const com::sun::star::uno::Reference <com::sun::star::table::XCell>& xCell) const;
	sal_Bool IsEditCell(ScMyCell& rCell) const;
    sal_Bool IsMultiLineFormulaCell(ScMyCell& rCell) const;
//UNUSED2008-05  sal_Bool IsAnnotationEqual(const com::sun::star::uno::Reference<com::sun::star::table::XCell>& xCell1,
//UNUSED2008-05                             const com::sun::star::uno::Reference<com::sun::star::table::XCell>& xCell2);
	sal_Bool IsCellEqual (ScMyCell& aCell1, ScMyCell& aCell2);

	void WriteCalculationSettings(const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc);
	void WriteTableSource();
	void WriteScenario();	// core implementation
	void WriteTheLabelRanges(const com::sun::star::uno::Reference< com::sun::star::sheet::XSpreadsheetDocument >& xSpreadDoc);
	void WriteLabelRanges( const com::sun::star::uno::Reference< com::sun::star::container::XIndexAccess >& xRangesIAccess, sal_Bool bColumn );
	void WriteNamedExpressions(const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheetDocument>& xSpreadDoc);
    void WriteExternalRefCaches();
	void WriteConsolidation();	// core implementation

	void CollectUserDefinedNamespaces(const SfxItemPool* pPool, sal_uInt16 nAttrib);

    void AddStyleFromCells(
        const com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet >& xProperties,
        const com::sun::star::uno::Reference< com::sun::star::sheet::XSpreadsheet >& xTable,
        sal_Int32 nTable, const rtl::OUString* pOldName );
    void AddStyleFromColumn(
        const com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet >& xColumnProperties,
        const rtl::OUString* pOldName, sal_Int32& rIndex, sal_Bool& rIsVisible );
    void AddStyleFromRow(
        const com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet >& xRowProperties,
        const rtl::OUString* pOldName, sal_Int32& rIndex );

    void IncrementProgressBar(sal_Bool bEditCell, sal_Int32 nInc = 1);

    void CopySourceStream( sal_Int32 nStartOffset, sal_Int32 nEndOffset, sal_Int32& rNewStart, sal_Int32& rNewEnd );

protected:
	virtual SvXMLAutoStylePoolP* CreateAutoStylePool();
	virtual XMLPageExport* CreatePageExport();
	virtual XMLShapeExport* CreateShapeExport();
	virtual XMLFontAutoStylePool* CreateFontAutoStylePool();
public:
	// #110680#
	ScXMLExport(
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
		const sal_uInt16 nExportFlag);

	virtual ~ScXMLExport();

	static sal_Int16 GetFieldUnit();
	inline ScDocument*			GetDocument()			{ return pDoc; }
	inline const ScDocument*	GetDocument() const		{ return pDoc; }
//UNUSED2008-05  sal_Bool IsMatrix (const com::sun::star::uno::Reference <com::sun::star::table::XCellRange>& xCellRange,
//UNUSED2008-05                     const com::sun::star::uno::Reference <com::sun::star::sheet::XSpreadsheet>& xTable,
//UNUSED2008-05                     const sal_Int32 nCol, const sal_Int32 nRow,
//UNUSED2008-05                     com::sun::star::table::CellRangeAddress& aCellAddress, sal_Bool& bIsFirst) const;
	sal_Bool IsMatrix (const ScAddress& aCell,
		com::sun::star::table::CellRangeAddress& aCellAddress, sal_Bool& bIsFirst) const;

	UniReference < XMLPropertySetMapper > GetCellStylesPropertySetMapper() { return xCellStylesPropertySetMapper; }
	UniReference < XMLPropertySetMapper > GetTableStylesPropertySetMapper() { return xTableStylesPropertySetMapper; }

    void SetSourceStream( const com::sun::star::uno::Reference<com::sun::star::io::XInputStream>& xNewStream );

	void GetChangeTrackViewSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rProps);
	virtual void GetViewSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rProps);
	virtual void GetConfigurationSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rProps);

    virtual void exportAnnotationMeta( const com::sun::star::uno::Reference < com::sun::star::drawing::XShape >& xShape);

	void CreateSharedData(const sal_Int32 nTableCount);
	void SetSharedData(ScMySharedData* pTemp) { pSharedData = pTemp; }
	ScMySharedData* GetSharedData() { return pSharedData; }
	XMLNumberFormatAttributesExportHelper* GetNumberFormatAttributesExportHelper();

	// Export the document.
	virtual sal_uInt32 exportDoc( enum ::xmloff::token::XMLTokenEnum eClass = ::xmloff::token::XML_TOKEN_INVALID );

	// XExporter
	virtual void SAL_CALL setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

	// XFilter
    virtual sal_Bool SAL_CALL filter( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL cancel() throw(::com::sun::star::uno::RuntimeException);

	// XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

	// XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    virtual void DisposingModel();
};

#endif

