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



#ifndef _SV_COMBOBOX_HXX
#define _SV_COMBOBOX_HXX

#include <vcl/dllapi.h>
#include <vcl/ctrl.hxx>
#include <vcl/combobox.h>
#include <vcl/edit.hxx>

class UserDrawEvent;

class ImplListBoxFloatingWindow;
class ImplListBox;
class ImplBtn;

// ------------
// - ComboBox -
// ------------

class VCL_DLLPUBLIC ComboBox : public Edit
{
private:
    Edit*                       mpSubEdit;
    ImplListBox*                mpImplLB;
    ImplBtn*                    mpBtn;
    ImplListBoxFloatingWindow*  mpFloatWin;
    sal_uInt16                      mnDDHeight;
    xub_Unicode                 mcMultiSep;
    sal_Bool                        mbDDAutoSize        : 1;
    sal_Bool                        mbSyntheticModify   : 1;
    sal_Bool                        mbMatchCase         : 1;
    Link                        maSelectHdl;
    Link                        maDoubleClickHdl;

//#if 0 // _SOLAR__PRIVATE
private:
    SAL_DLLPRIVATE void     ImplInitComboBoxData();
    SAL_DLLPRIVATE void     ImplUpdateFloatSelection();

    DECL_DLLPRIVATE_LINK(   ImplSelectHdl, void* );
    DECL_DLLPRIVATE_LINK(   ImplCancelHdl, void* );
    DECL_DLLPRIVATE_LINK(   ImplDoubleClickHdl, void* );
    DECL_DLLPRIVATE_LINK(   ImplClickBtnHdl, void* );
    DECL_DLLPRIVATE_LINK(   ImplPopupModeEndHdl, void* );
    DECL_DLLPRIVATE_LINK(   ImplSelectionChangedHdl, void* );
    DECL_DLLPRIVATE_LINK(   ImplUserDrawHdl, UserDrawEvent* );
    DECL_DLLPRIVATE_LINK(   ImplAutocompleteHdl, Edit* );

protected:
    using Window::ImplInit;
    SAL_DLLPRIVATE void     ImplInit( Window* pParent, WinBits nStyle );
    SAL_DLLPRIVATE WinBits  ImplInitStyle( WinBits nStyle );
    SAL_DLLPRIVATE void     ImplLoadRes( const ResId& rResId );
    SAL_DLLPRIVATE void     ImplCalcEditHeight();
//#endif

protected:
                    ComboBox( WindowType nType );
    sal_Bool            IsDropDownBox() const { return mpFloatWin ? sal_True : sal_False; }

    virtual void  FillLayoutData() const;
public:
                    ComboBox( Window* pParent, WinBits nStyle = 0 );
                    ComboBox( Window* pParent, const ResId& rResId );
                    ~ComboBox();

    virtual void    Draw( OutputDevice* pDev, const Point& rPos, const Size& rSize, sal_uLong nFlags );
    virtual void    Resize();
    virtual long    PreNotify( NotifyEvent& rNEvt );
    virtual long    Notify( NotifyEvent& rNEvt );
    virtual void    StateChanged( StateChangedType nType );
    virtual void    DataChanged( const DataChangedEvent& rDCEvt );

    virtual void    UserDraw( const UserDrawEvent& rUDEvt );
    virtual void    Select();
    virtual void    DoubleClick();

    virtual void    Modify();

    virtual const Wallpaper& GetDisplayBackground() const;

    virtual void    SetPosSizePixel( long nX, long nY, long nWidth, long nHeight, sal_uInt16 nFlags = WINDOW_POSSIZE_ALL );
    void            SetPosSizePixel( const Point& rNewPos, const Size& rNewSize )
                        { Edit::SetPosSizePixel( rNewPos, rNewSize ); }
    void			SetDropDownSizePixel( const Size& rNewSize )
    { if( IsDropDownBox() ) SetPosSizePixel( 0, 0, rNewSize.Width(), rNewSize.Height(), WINDOW_POSSIZE_SIZE | WINDOW_POSSIZE_DROPDOWN ); }

    Rectangle		GetDropDownPosSizePixel() const;
    Rectangle		GetListPosSizePixel() const;
    // returns empty rectangle in DropDown mode,
    // else it returns the PosSize of the ListBox

    void            SetDropDownLineCount( sal_uInt16 nLines );
    sal_uInt16          GetDropDownLineCount() const;

    void            EnableAutoSize( sal_Bool bAuto );
    sal_Bool            IsAutoSizeEnabled() const               { return mbDDAutoSize; }

    void            EnableDDAutoWidth( sal_Bool b );
    sal_Bool            IsDDAutoWidthEnabled() const;

    void            SetText( const XubString& rStr );
    void            SetText( const XubString& rStr, const Selection& rNewSelection );

    sal_uInt16          InsertEntry( const XubString& rStr, sal_uInt16 nPos = COMBOBOX_APPEND );
    sal_uInt16          InsertEntry( const XubString& rStr, const Image& rImage, sal_uInt16 nPos = COMBOBOX_APPEND );

    void            RemoveEntry( const XubString& rStr );
    void            RemoveEntry( sal_uInt16 nPos );

    void            Clear();

    sal_uInt16          GetEntryPos( const XubString& rStr ) const;
    sal_uInt16          GetEntryPos( const void* pData ) const;
    Image           GetEntryImage( sal_uInt16 nPos ) const;
    XubString       GetEntry( sal_uInt16 nPos ) const;
    sal_uInt16          GetEntryCount() const;

    sal_Bool            IsTravelSelect() const;
    sal_Bool            IsInDropDown() const;
    void			ToggleDropDown();

    long            CalcWindowSizePixel( sal_uInt16 nLines ) const;

    void            SetUserItemSize( const Size& rSz );
    const Size&     GetUserItemSize() const;

    void            EnableUserDraw( sal_Bool bUserDraw );
    sal_Bool            IsUserDrawEnabled() const;

    void            DrawEntry( const UserDrawEvent& rEvt, sal_Bool bDrawImage, sal_Bool bDrawText, sal_Bool bDrawTextAtImagePos = sal_False );
    void            SetBorderStyle( sal_uInt16 nBorderStyle );

    void            SetSeparatorPos( sal_uInt16 n );
    void            SetSeparatorPos();
    sal_uInt16          GetSeparatorPos() const;

    void            EnableAutocomplete( sal_Bool bEnable, sal_Bool bMatchCase = sal_False );
    sal_Bool            IsAutocompleteEnabled() const;

    void            EnableMultiSelection( sal_Bool bMulti );
    sal_Bool            IsMultiSelectionEnabled() const;
    void            SetMultiSelectionSeparator( xub_Unicode cSep ) { mcMultiSep = cSep; }
    xub_Unicode     GetMultiSelectionSeparator() const { return mcMultiSep; }

    void            SetSelectHdl( const Link& rLink )       { maSelectHdl = rLink; }
    const Link&     GetSelectHdl() const                    { return maSelectHdl; }
    void            SetDoubleClickHdl( const Link& rLink )  { maDoubleClickHdl = rLink; }
    const Link&     GetDoubleClickHdl() const               { return maDoubleClickHdl; }

    Size            CalcMinimumSize() const;
    virtual Size    GetOptimalSize(WindowSizeType eType) const;
    Size            CalcAdjustedSize( const Size& rPrefSize ) const;
    using Edit::CalcSize;
    Size            CalcSize( sal_uInt16 nColumns, sal_uInt16 nLines ) const;
    void            GetMaxVisColumnsAndLines( sal_uInt16& rnCols, sal_uInt16& rnLines ) const;

    void            SetMRUEntries( const XubString& rEntries, xub_Unicode cSep = ';' );
    XubString       GetMRUEntries( xub_Unicode cSep = ';' ) const;
    void            SetMaxMRUCount( sal_uInt16 n );
    sal_uInt16          GetMaxMRUCount() const;

    void            SetEntryData( sal_uInt16 nPos, void* pNewData );
    void*           GetEntryData( sal_uInt16 nPos ) const;

	void			SetTopEntry( sal_uInt16 nPos );
	void            ShowProminentEntry( sal_uInt16 nPos );
	sal_uInt16			GetTopEntry() const;

	void            SetProminentEntryType( ProminentEntry eType );
	ProminentEntry  GetProminentEntryType() const;

    sal_uInt16			GetDisplayLineCount() const;

	sal_uInt16			GetSelectEntryCount() const;
	sal_uInt16			GetSelectEntryPos( sal_uInt16 nSelIndex = 0 ) const;
	sal_Bool			IsEntryPosSelected( sal_uInt16 nPos ) const;
	void			SelectEntryPos( sal_uInt16 nPos, sal_Bool bSelect = sal_True );
	void			SetNoSelection();
	Rectangle       GetBoundingRectangle( sal_uInt16 nItem ) const;

    /** checks whether a certain point lies within the bounds of
        a list item and returns the item as well as the character position
        the point is at.

        <p>If the point is inside an item the item pos is put into <code>rPos</code> and
        the item-relative character index is returned. If the point is not inside
        an item -1 is returned and rPos is unchanged.</p>

        @param rPoint
        tells the point for which an item is requested.

        @param rPos
        gets the item at the specified point <code>rPoint</code>

        @returns
        the item-relative character index at point <code>rPos</code> or -1
        if no item is at that point.
     */
    using Control::GetIndexForPoint;
    long GetIndexForPoint( const Point& rPoint, sal_uInt16& rPos ) const;
};

#endif  // _COMBOBOX_HXX
