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



#ifndef _XMLREDLINEIMPORTHELPER_HXX
#define _XMLREDLINEIMPORTHELPER_HXX

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif

#ifndef _REDLINE_HXX
#include "redline.hxx"
#endif

#ifndef _REDLENUM_HXX
#include "redlenum.hxx"
#endif

#include <map>
namespace com { namespace sun { namespace star {
	namespace text { class XTextCursor; }
	namespace text { class XTextRange; }
	namespace frame { class XModel; }
} } }
namespace binfilter {

class RedlineInfo;
class SwRedlineData;
class SwDoc;
//STRIP008 namespace com { namespace sun { namespace star {
//STRIP008 	namespace text { class XTextCursor; }
//STRIP008 	namespace text { class XTextRange; }
//STRIP008 	namespace frame { class XModel; }
//STRIP008 } } }


typedef ::std::map< ::rtl::OUString, RedlineInfo* > RedlineMapType;

class XMLRedlineImportHelper
{
	const ::rtl::OUString sEmpty;
	const ::rtl::OUString sInsertion;
	const ::rtl::OUString sDeletion;
	const ::rtl::OUString sFormatChange;
	const ::rtl::OUString sShowChanges;
	const ::rtl::OUString sRecordChanges;
	const ::rtl::OUString sRedlineProtectionKey;

	RedlineMapType aRedlineMap;

	/// if sal_True, no redlines should be inserted into document
	/// (This typically happen when a document is loaded in 'insert'-mode.)
	sal_Bool bIgnoreRedlines;

	/// save information for saving and reconstruction of the redline mode
	::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet> xModelPropertySet;
	::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet> xImportInfoPropertySet;
	sal_Bool bShowChanges;
	sal_Bool bRecordChanges;
	::com::sun::star::uno::Sequence<sal_Int8> aProtectionKey;

public:

	XMLRedlineImportHelper(
		sal_Bool bIgnoreRedlines,		/// ignore redlines mode

		// property sets of model + import info for saving + restoring the 
		// redline mode
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rModel,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::beans::XPropertySet> & rImportInfoSet ); 
	virtual ~XMLRedlineImportHelper();

	/// create a redline object
	/// (The redline will be inserted into the document after both start
	///  and end cursor has been set.)
	void Add( 
		const ::rtl::OUString& rType,		/// redline type (insert, del,... )
		const ::rtl::OUString& rId,			/// use to identify this redline
		const ::rtl::OUString& rAuthor,		/// name of the author
		const ::rtl::OUString& rComment,	/// redline comment
		const ::com::sun::star::util::DateTime& rDateTime, /// date+time
        sal_Bool bMergeLastParagraph);      /// merge last paragraph?

	/// create a text section for the redline, and return an
	/// XText/XTextCursor that may be used to write into it.
	::com::sun::star::uno::Reference< 
		::com::sun::star::text::XTextCursor> CreateRedlineTextSection(
			::com::sun::star::uno::Reference< 	/// needed to get the document
					::com::sun::star::text::XTextCursor> xOldCursor,
			const ::rtl::OUString& rId);	/// ID used to RedlineAdd() call

	/// Set start or end position for a redline in the text body.
	/// Accepts XTextRange objects.
	void SetCursor(
		const ::rtl::OUString& rId,		/// ID used in RedlineAdd() call
		sal_Bool bStart,				/// start or end Range
		::com::sun::star::uno::Reference< 	/// the actual XTextRange
			::com::sun::star::text::XTextRange> & rRange,
		/// text range is (from an XML view) outside of a paragraph
		/// (i.e. before a table)
		sal_Bool bIsOusideOfParagraph);	

	/**
	 * Adjust the start (end) position for a redline that begins in a
	 * start node. It takes the cursor positions _inside_ the redlined 
	 * element (e.g. section or table).
	 *
	 * We will do sanity checking of the given text range: It will
	 * only be considered valid if it points to the next text node
	 * after the position given in a previous SetCursor */
	void AdjustStartNodeCursor(
	 	const ::rtl::OUString& rId,		/// ID used in RedlineAdd() call
		sal_Bool bStart,
		/// XTextRange _inside_ a table/section
		::com::sun::star::uno::Reference<
			::com::sun::star::text::XTextRange> & rRange);

	/// set redline mode: show changes
	void SetShowChanges( sal_Bool bShowChanges );

	/// set redline mode: record changes
	void SetRecordChanges( sal_Bool bRecordChanges );

	/// set redline protection key
	void SetProtectionKey( 
		const ::com::sun::star::uno::Sequence<sal_Int8> & rKey );

private:

	inline sal_Bool IsReady(RedlineInfo* pRedline);

	void InsertIntoDocument(RedlineInfo* pRedline);

	SwRedlineData* ConvertRedline(
		RedlineInfo* pRedline,	/// RedlineInfo to be converted
		SwDoc* pDoc);			/// document needed for Author-ID conversion

	/** save the redline mode (if rPropertySet is non-null) */
	void SaveRedlineMode(
		const ::com::sun::star::uno::Reference<
		::com::sun::star::beans::XPropertySet> & rPropertySet);

	/** don't restore the saved redline mode */
 	void DontRestoreRedlineMode();

};

} //namespace binfilter
#endif

