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


#ifndef _SVT_FILEVIEW_HXX
#define	_SVT_FILEVIEW_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENT_HPP_
#include <com/sun/star/ucb/XContent.hpp>
#endif

#ifndef _SV_CTRL_HXX
#include <vcl/ctrl.hxx>
#endif
#ifndef _SV_IMAGE_HXX
#include <vcl/image.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_BUTTON_HXX
#include <vcl/button.hxx>
#endif
#ifndef _SV_DIALOG_HXX
#include <vcl/dialog.hxx>
#endif

namespace binfilter
{

// class SvtFileView -----------------------------------------------------

#define FILEVIEW_ONLYFOLDER			0x0001
#define FILEVIEW_MULTISELECTION		0x0002

#define FILEVIEW_SHOW_TITLE			0x0010
#define FILEVIEW_SHOW_SIZE			0x0020
#define FILEVIEW_SHOW_DATE			0x0040
#define FILEVIEW_SHOW_ALL			0x0070

class ViewTabListBox_Impl;
class SvtFileView_Impl;
class SvLBoxEntry;
class HeaderBar;
class IUrlFilter;

/// the result of an action in the FileView
enum FileViewResult
{
    eSuccess,
    eFailure,
    eTimeout,
    eStillRunning
};

/// describes parameters for doing an action on the FileView asynchronously
struct FileViewAsyncAction
{
    sal_uInt32  nMinTimeout;    /// minimum time to wait for a result, in milliseconds
    sal_uInt32  nMaxTimeout;    /// maximum time to wait for a result, in milliseconds, until eTimeout is returned
    Link        aFinishHandler; /// the handler to be called when the action is finished. Called in every case, no matter of the result

    FileViewAsyncAction()
    {
        nMinTimeout = nMaxTimeout = 0;
    }
};

class  SvtFileView : public Control
{
private:
    SvtFileView_Impl*       mpImp;

	void					OpenFolder( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aContents );

	DECL_LINK( 				HeaderSelect_Impl, HeaderBar * );
	DECL_LINK( 				HeaderEndDrag_Impl, HeaderBar * );

protected:
	virtual void GetFocus();

public:
	SvtFileView( Window* pParent, const ResId& rResId, sal_Bool bOnlyFolder, sal_Bool bMultiSelection );
	SvtFileView( Window* pParent, const ResId& rResId, sal_Int8 nFlags );
	~SvtFileView();

    const String&           GetViewURL() const;
	String					GetURL( SvLBoxEntry* pEntry ) const;
	String					GetCurrentURL() const;

    sal_Bool				GetParentURL( String& _rParentURL ) const;
	sal_Bool				CreateNewFolder( const String& rNewFolder );

	void					SetHelpId( sal_uInt32 nHelpId );
	sal_uInt32				GetHelpId( ) const;
	void					SetSizePixel( const Size& rNewSize );
    using Window::SetPosSizePixel;
	virtual void			SetPosSizePixel( const Point& rNewPos, const Size& rNewSize );

    /** initialize the view with the content of a folder given by URL, and aply an immediate filter

        @param rFolderURL
            the URL of the folder whose content is to be read
        @param rFilter
            the initial filter to be applied
        @param pAsyncDescriptor
            If not <NULL/>, this struct describes the parameters for doing the
            action asynchronously.
    */
	FileViewResult          Initialize(
                                const String& rFolderURL,
                                const String& rFilter,
                                const FileViewAsyncAction* pAsyncDescriptor
                            );

    /** initialze the view with a sequence of contents, which have already been obtained elsewhere

        This method will never return <member>eStillRunning</member>, since it will fill the
        view synchronously
    */
    sal_Bool                Initialize( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aContents );

    /** initializes the view with the content of a folder given by an UCB content
    */
    sal_Bool				Initialize( const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XContent>& _xContent, const String& rFilter );

    /** reads the current content of the current folder again, and applies the given filter to it

        Note 1: The folder is really read a second time. This implies that any new elements (which were
        not present when you called Initialize the last time) are now displayed.

        Note 2: This method must not be called when you previously initialized the view from a sequence
        of strings, or a UNO content object.

        @param rFilter
            the filter to be applied
        @param pAsyncDescriptor
            If not <NULL/>, this struct describes the parameters for doing the
            action asynchronously.
    */
	FileViewResult          ExecuteFilter(
                                const String& rFilter,
                                const FileViewAsyncAction* pAsyncDescriptor
                            );

    /** cancels a running async action (if any)

        @seealso Initialize
        @seealso ExecuteFilter
        @seealso FileViewAsyncAction
    */
    void                    CancelRunningAsyncAction();

    /** initializes the view with the parent folder of the current folder

        @param rNewURL
            the URL of the folder which we just navigated to
        @param pAsyncDescriptor
            If not <NULL/>, this struct describes the parameters for doing the
            action asynchronously.
    */
	FileViewResult          PreviousLevel(
                                const FileViewAsyncAction* pAsyncDescriptor
                            );

    void					SetNoSelection();
	void					ResetCursor();

	void					SetSelectHdl( const Link& rHdl );
	void					SetDoubleClickHdl( const Link& rHdl );
    void                    SetOpenDoneHdl( const Link& rHdl );

	ULONG           		GetSelectionCount() const;
	SvLBoxEntry*			FirstSelected() const;
	SvLBoxEntry*			NextSelected( SvLBoxEntry* pEntry ) const;
	void					EnableAutoResize();
	void					SetFocus();

    void                    EnableContextMenu( sal_Bool bEnable );
    void                    EnableDelete( sal_Bool bEnable );
	void					EnableNameReplacing( sal_Bool bEnable = sal_True );
								// translate folder names or display doc-title instead of file name
								// EnableContextMenu( TRUE )/EnableDelete(TRUE) disable name replacing!

							// save and load column size and sort order
	String                  GetConfigString() const;
	void					SetConfigString( const String& rCfgStr );

    void                    SetUrlFilter( const IUrlFilter* _pFilter );
    const IUrlFilter*       GetUrlFilter( ) const;

    void                    EndInplaceEditing( bool _bCancel );

protected:
    virtual void            StateChanged( StateChangedType nStateChange );
};

// struct SvtContentEntry ------------------------------------------------

struct SvtContentEntry
{
	sal_Bool	mbIsFolder;
	UniString	maURL;

	SvtContentEntry( const UniString& rURL, sal_Bool bIsFolder ) :
		mbIsFolder( bIsFolder ), maURL( rURL ) {}
};

// -----------------------------------------------------------------------
// QueryDeleteDlg_Impl
// -----------------------------------------------------------------------

enum QueryDeleteResult_Impl
{
	QUERYDELETE_YES = 0,
	QUERYDELETE_NO,
	QUERYDELETE_ALL,
	QUERYDELETE_CANCEL
};

class  QueryDeleteDlg_Impl : public ModalDialog
{
	FixedText               _aEntryLabel;
	FixedText               _aEntry;
	FixedText               _aQueryMsg;

	PushButton              _aYesButton;
	PushButton              _aAllButton;
	PushButton              _aNoButton;
	CancelButton            _aCancelButton;

    QueryDeleteResult_Impl  _eResult;

private:

	DECL_DLLPRIVATE_STATIC_LINK( QueryDeleteDlg_Impl, ClickLink, PushButton* );

public:

							QueryDeleteDlg_Impl( Window* pParent,
												 const String& rName );

    void                    EnableAllButton() { _aAllButton.Enable( sal_True ); }
    QueryDeleteResult_Impl  GetResult() const { return _eResult; }
};

}

#endif // _SVT_FILEVIEW_HXX

