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



#ifndef SC_CURSUNO_HXX
#define SC_CURSUNO_HXX

#ifndef SC_CELLSUNO_HXX
#include "cellsuno.hxx"
#endif

#ifndef _COM_SUN_STAR_TABLE_XCELLCURSOR_HPP_
#include <com/sun/star/table/XCellCursor.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XUSEDAREACURSOR_HPP_
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>
#endif
namespace binfilter {

class ScCellCursorObj : public ScCellRangeObj,
						public ::com::sun::star::sheet::XSheetCellCursor,
						public ::com::sun::star::sheet::XUsedAreaCursor,
						public ::com::sun::star::table::XCellCursor
{
public:
							ScCellCursorObj(ScDocShell* pDocSh, const ScRange& rR);
	virtual					~ScCellCursorObj();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType )
									throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

							// XSheetCellCursor
	virtual void SAL_CALL	collapseToCurrentRegion() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	collapseToCurrentArray() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	collapseToMergedArea() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	expandToEntireColumns() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	expandToEntireRows() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	collapseToSize( sal_Int32 nColumns, sal_Int32 nRows )
								throw(::com::sun::star::uno::RuntimeException);

							// XUsedAreaCursor
	virtual void SAL_CALL	gotoStartOfUsedArea( sal_Bool bExpand )
								throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	gotoEndOfUsedArea( sal_Bool bExpand )
								throw(::com::sun::star::uno::RuntimeException);

							// XCellCursor
	virtual void SAL_CALL	gotoStart() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	gotoEnd() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	gotoNext() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	gotoPrevious() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	gotoOffset( sal_Int32 nColumnOffset, sal_Int32 nRowOffset )
								throw(::com::sun::star::uno::RuntimeException);

							// XSheetCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > SAL_CALL
							getSpreadsheet() throw(::com::sun::star::uno::RuntimeException);

							// XCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL
							getCellByPosition( sal_Int32 nColumn, sal_Int32 nRow )
								throw(::com::sun::star::lang::IndexOutOfBoundsException,
									::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getCellRangeByPosition( sal_Int32 nLeft, sal_Int32 nTop,
								sal_Int32 nRight, sal_Int32 nBottom )
									throw(::com::sun::star::lang::IndexOutOfBoundsException,
										::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getCellRangeByName( const ::rtl::OUString& aRange )
								throw(::com::sun::star::uno::RuntimeException);

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName()
								throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
								throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
								throw(::com::sun::star::uno::RuntimeException);

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes()
								throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
								throw(::com::sun::star::uno::RuntimeException);
};


} //namespace binfilter
#endif

