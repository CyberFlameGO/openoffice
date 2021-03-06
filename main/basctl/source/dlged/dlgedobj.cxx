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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_basctl.hxx"

#include <vector>
#include <algorithm>
#include <dlgeddef.hxx>
#include "dlgedobj.hxx"
#include "dlged.hxx"
#include "dlgedmod.hxx"
#include "dlgedpage.hxx"
#include "dlgedview.hxx"
#include "dlgedlist.hxx"
#include <iderid.hxx>
#include <localizationmgr.hxx> 

#ifndef _BASCTL_DLGRESID_HRC
#include <dlgresid.hrc>
#endif
#include <tools/resmgr.hxx>
#include <tools/shl.hxx>
#include <unotools/sharedunocomponent.hxx>
#include <com/sun/star/awt/XTabControllerModel.hpp>
#include <com/sun/star/awt/XUnoControlContainer.hpp>
#include <com/sun/star/awt/XVclContainerPeer.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/script/XScriptEventsSupplier.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/types.hxx>
#include <vcl/svapp.hxx>

#include <algorithm>
#include <functional>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::script;
using ::rtl::OUString;


TYPEINIT1(DlgEdObj, SdrUnoObj);
DBG_NAME(DlgEdObj);

//----------------------------------------------------------------------------

DlgEdObj::DlgEdObj()
		  :SdrUnoObj(String(), sal_False)
		  ,bIsListening(sal_False)
          ,pDlgEdForm( NULL )
{
	DBG_CTOR(DlgEdObj, NULL);
}

//----------------------------------------------------------------------------

DlgEdObj::DlgEdObj(const ::rtl::OUString& rModelName, 
				   const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rxSFac)
		  :SdrUnoObj(rModelName, rxSFac, sal_False)
		  ,bIsListening(sal_False)
          ,pDlgEdForm( NULL )
{
	DBG_CTOR(DlgEdObj, NULL);
}

//----------------------------------------------------------------------------

DlgEdObj::~DlgEdObj()
{
	DBG_DTOR(DlgEdObj, NULL);

	if ( isListening() )
		EndListening();
}

//----------------------------------------------------------------------------

void DlgEdObj::SetPage(SdrPage* _pNewPage)
{
	// now set the page
	SdrUnoObj::SetPage(_pNewPage);
}

//----------------------------------------------------------------------------

namespace
{
    /** returns the DlgEdForm which the given DlgEdObj belongs to
        (which might in fact be the object itself)

        Failure to obtain the form will be reported with an assertion in the non-product
        version.
     */
    bool lcl_getDlgEdForm( DlgEdObj* _pObject, DlgEdForm*& _out_pDlgEdForm )
    {
        _out_pDlgEdForm = dynamic_cast< DlgEdForm* >( _pObject );
        if ( !_out_pDlgEdForm )
            _out_pDlgEdForm = _pObject->GetDlgEdForm();
        DBG_ASSERT( _out_pDlgEdForm, "lcl_getDlgEdForm: no form!" );
        return ( _out_pDlgEdForm != NULL );
    }
}

//----------------------------------------------------------------------------

uno::Reference< awt::XControl > DlgEdObj::GetControl() const
{
    const DlgEdForm* pForm = GetDlgEdForm();
    const DlgEditor* pEditor = pForm ? pForm->GetDlgEditor() : NULL;
    SdrView* pView = pEditor ? pEditor->GetView() : NULL;
    Window* pWindow = pEditor ? pEditor->GetWindow() : NULL;
    OSL_ENSURE( ( pView && pWindow ) || !pForm, "DlgEdObj::GetControl: no view or no window!" );

    uno::Reference< awt::XControl > xControl;
    if ( pView && pWindow )
        xControl = GetUnoControl( *pView, *pWindow );

    return xControl;
}

//----------------------------------------------------------------------------

bool DlgEdObj::TransformSdrToControlCoordinates( 
    sal_Int32 nXIn, sal_Int32 nYIn, sal_Int32 nWidthIn, sal_Int32 nHeightIn, 
    sal_Int32& nXOut, sal_Int32& nYOut, sal_Int32& nWidthOut, sal_Int32& nHeightOut )
{
    // input position and size
    Size aPos( nXIn, nYIn );
    Size aSize( nWidthIn, nHeightIn );

    // form position
    DlgEdForm* pForm = NULL;
    if ( !lcl_getDlgEdForm( this, pForm ) )
        return false;
    Rectangle aFormRect = pForm->GetSnapRect();
    Size aFormPos( aFormRect.Left(), aFormRect.Top() );

    // convert 100th_mm to pixel
    OutputDevice* pDevice = Application::GetDefaultDevice();
    DBG_ASSERT( pDevice, "DlgEdObj::TransformSdrToControlCoordinates: missing default device!" );
    if ( !pDevice )
        return false;
    aPos = pDevice->LogicToPixel( aPos, MapMode( MAP_100TH_MM ) );
    aSize = pDevice->LogicToPixel( aSize, MapMode( MAP_100TH_MM ) );
    aFormPos = pDevice->LogicToPixel( aFormPos, MapMode( MAP_100TH_MM ) );

    // subtract form position
    aPos.Width() -= aFormPos.Width();
    aPos.Height() -= aFormPos.Height();

    // take window borders into account
    Reference< beans::XPropertySet > xPSetForm( pForm->GetUnoControlModel(), UNO_QUERY );
    DBG_ASSERT( xPSetForm.is(), "DlgEdObj::TransformFormToSdrCoordinates: no form property set!" );
    if ( !xPSetForm.is() )
        return false;
	bool bDecoration = true;
    xPSetForm->getPropertyValue( DLGED_PROP_DECORATION ) >>= bDecoration;
	if( bDecoration )
	{
		awt::DeviceInfo aDeviceInfo = pForm->getDeviceInfo();
		aPos.Width() -= aDeviceInfo.LeftInset;
		aPos.Height() -= aDeviceInfo.TopInset;
	}
    
    // convert pixel to logic units
    aPos = pDevice->PixelToLogic( aPos, MapMode( MAP_APPFONT ) );
    aSize = pDevice->PixelToLogic( aSize, MapMode( MAP_APPFONT ) );

    // set out parameters
    nXOut = aPos.Width();
    nYOut = aPos.Height();
    nWidthOut = aSize.Width();
    nHeightOut = aSize.Height();

    return true;
}

//----------------------------------------------------------------------------

bool DlgEdObj::TransformSdrToFormCoordinates( 
    sal_Int32 nXIn, sal_Int32 nYIn, sal_Int32 nWidthIn, sal_Int32 nHeightIn, 
    sal_Int32& nXOut, sal_Int32& nYOut, sal_Int32& nWidthOut, sal_Int32& nHeightOut )
{
    // input position and size
    Size aPos( nXIn, nYIn );
    Size aSize( nWidthIn, nHeightIn );

    // convert 100th_mm to pixel
    OutputDevice* pDevice = Application::GetDefaultDevice();
    DBG_ASSERT( pDevice, "DlgEdObj::TransformSdrToFormCoordinates: missing default device!" );
    if ( !pDevice )
        return false;
    aPos = pDevice->LogicToPixel( aPos, MapMode( MAP_100TH_MM ) );
    aSize = pDevice->LogicToPixel( aSize, MapMode( MAP_100TH_MM ) );

    // take window borders into account
    DlgEdForm* pForm = NULL;
    if ( !lcl_getDlgEdForm( this, pForm ) )
        return false;

    // take window borders into account
    Reference< beans::XPropertySet > xPSetForm( pForm->GetUnoControlModel(), UNO_QUERY );
    DBG_ASSERT( xPSetForm.is(), "DlgEdObj::TransformFormToSdrCoordinates: no form property set!" );
    if ( !xPSetForm.is() )
        return false;
	bool bDecoration = true;
    xPSetForm->getPropertyValue( DLGED_PROP_DECORATION ) >>= bDecoration;
	if( bDecoration )
	{
		awt::DeviceInfo aDeviceInfo = pForm->getDeviceInfo();
		aSize.Width() -= aDeviceInfo.LeftInset + aDeviceInfo.RightInset;
		aSize.Height() -= aDeviceInfo.TopInset + aDeviceInfo.BottomInset;
	}

    // convert pixel to logic units
    aPos = pDevice->PixelToLogic( aPos, MapMode( MAP_APPFONT ) );
    aSize = pDevice->PixelToLogic( aSize, MapMode( MAP_APPFONT ) );

    // set out parameters
    nXOut = aPos.Width();
    nYOut = aPos.Height();
    nWidthOut = aSize.Width();
    nHeightOut = aSize.Height();

    return true;
}

//----------------------------------------------------------------------------

bool DlgEdObj::TransformControlToSdrCoordinates( 
    sal_Int32 nXIn, sal_Int32 nYIn, sal_Int32 nWidthIn, sal_Int32 nHeightIn, 
    sal_Int32& nXOut, sal_Int32& nYOut, sal_Int32& nWidthOut, sal_Int32& nHeightOut )
{
    // input position and size
    Size aPos( nXIn, nYIn );
    Size aSize( nWidthIn, nHeightIn );

    // form position
    DlgEdForm* pForm = NULL;
    if ( !lcl_getDlgEdForm( this, pForm ) )
        return false;

    Reference< beans::XPropertySet > xPSetForm( pForm->GetUnoControlModel(), UNO_QUERY );
    DBG_ASSERT( xPSetForm.is(), "DlgEdObj::TransformControlToSdrCoordinates: no form property set!" );
    if ( !xPSetForm.is() )
        return false;
    sal_Int32 nFormX = 0, nFormY = 0, nFormWidth, nFormHeight;
    xPSetForm->getPropertyValue( DLGED_PROP_POSITIONX ) >>= nFormX;
    xPSetForm->getPropertyValue( DLGED_PROP_POSITIONY ) >>= nFormY;
    xPSetForm->getPropertyValue( DLGED_PROP_WIDTH ) >>= nFormWidth;
    xPSetForm->getPropertyValue( DLGED_PROP_HEIGHT ) >>= nFormHeight;
    Size aFormPos( nFormX, nFormY );

    // convert logic units to pixel
    OutputDevice* pDevice = Application::GetDefaultDevice();
    DBG_ASSERT( pDevice, "DlgEdObj::TransformControlToSdrCoordinates: missing default device!" );
    if ( !pDevice )
        return false;
    aPos = pDevice->LogicToPixel( aPos, MapMode( MAP_APPFONT ) );
    aSize = pDevice->LogicToPixel( aSize, MapMode( MAP_APPFONT ) );
    aFormPos = pDevice->LogicToPixel( aFormPos, MapMode( MAP_APPFONT ) );

    // add form position
    aPos.Width() += aFormPos.Width();
    aPos.Height() += aFormPos.Height();

    // take window borders into account
	bool bDecoration = true;
    xPSetForm->getPropertyValue( DLGED_PROP_DECORATION ) >>= bDecoration;
	if( bDecoration )
	{
		awt::DeviceInfo aDeviceInfo = pForm->getDeviceInfo();
		aPos.Width() += aDeviceInfo.LeftInset;
		aPos.Height() += aDeviceInfo.TopInset;
	}

    // convert pixel to 100th_mm
    aPos = pDevice->PixelToLogic( aPos, MapMode( MAP_100TH_MM ) );
    aSize = pDevice->PixelToLogic( aSize, MapMode( MAP_100TH_MM ) );

    // set out parameters
    nXOut = aPos.Width();
    nYOut = aPos.Height();
    nWidthOut = aSize.Width();
    nHeightOut = aSize.Height();

    return true;
}

//----------------------------------------------------------------------------

bool DlgEdObj::TransformFormToSdrCoordinates( 
    sal_Int32 nXIn, sal_Int32 nYIn, sal_Int32 nWidthIn, sal_Int32 nHeightIn, 
    sal_Int32& nXOut, sal_Int32& nYOut, sal_Int32& nWidthOut, sal_Int32& nHeightOut )
{
    // input position and size
    Size aPos( nXIn, nYIn );
    Size aSize( nWidthIn, nHeightIn );

    // convert logic units to pixel
    OutputDevice* pDevice = Application::GetDefaultDevice();
    DBG_ASSERT( pDevice, "DlgEdObj::TransformFormToSdrCoordinates: missing default device!" );
    if ( !pDevice )
        return false;
    aPos = pDevice->LogicToPixel( aPos, MapMode( MAP_APPFONT ) );
    aSize = pDevice->LogicToPixel( aSize, MapMode( MAP_APPFONT ) );

    // take window borders into account
    DlgEdForm* pForm = NULL;
    if ( !lcl_getDlgEdForm( this, pForm ) )
        return false;

    // take window borders into account
    Reference< beans::XPropertySet > xPSetForm( pForm->GetUnoControlModel(), UNO_QUERY );
    DBG_ASSERT( xPSetForm.is(), "DlgEdObj::TransformFormToSdrCoordinates: no form property set!" );
    if ( !xPSetForm.is() )
        return false;
	bool bDecoration = true;
    xPSetForm->getPropertyValue( DLGED_PROP_DECORATION ) >>= bDecoration;
	if( bDecoration )
	{
		awt::DeviceInfo aDeviceInfo = pForm->getDeviceInfo();
		aSize.Width() += aDeviceInfo.LeftInset + aDeviceInfo.RightInset;
		aSize.Height() += aDeviceInfo.TopInset + aDeviceInfo.BottomInset;
	}

    // convert pixel to 100th_mm
    aPos = pDevice->PixelToLogic( aPos, MapMode( MAP_100TH_MM ) );
    aSize = pDevice->PixelToLogic( aSize, MapMode( MAP_100TH_MM ) );

    // set out parameters
    nXOut = aPos.Width();
    nYOut = aPos.Height();
    nWidthOut = aSize.Width();
    nHeightOut = aSize.Height();

    return true;
}

//----------------------------------------------------------------------------

void DlgEdObj::SetRectFromProps()
{
    // get control position and size from properties
    Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
    if ( xPSet.is() )
    {		
        sal_Int32 nXIn = 0, nYIn = 0, nWidthIn = 0, nHeightIn = 0;
        xPSet->getPropertyValue( DLGED_PROP_POSITIONX ) >>= nXIn;
        xPSet->getPropertyValue( DLGED_PROP_POSITIONY ) >>= nYIn;
        xPSet->getPropertyValue( DLGED_PROP_WIDTH ) >>= nWidthIn;
        xPSet->getPropertyValue( DLGED_PROP_HEIGHT ) >>= nHeightIn;

        // transform coordinates
        sal_Int32 nXOut, nYOut, nWidthOut, nHeightOut;
        if ( TransformControlToSdrCoordinates( nXIn, nYIn, nWidthIn, nHeightIn, nXOut, nYOut, nWidthOut, nHeightOut ) )
        {
            // set rectangle position and size
            Point aPoint( nXOut, nYOut );
            Size aSize( nWidthOut, nHeightOut );
            SetSnapRect( Rectangle( aPoint, aSize ) );
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdObj::SetPropsFromRect()
{
    // get control position and size from rectangle
    Rectangle aRect_ = GetSnapRect();
    sal_Int32 nXIn = aRect_.Left();
    sal_Int32 nYIn = aRect_.Top();
    sal_Int32 nWidthIn = aRect_.GetWidth();
    sal_Int32 nHeightIn = aRect_.GetHeight();

    // transform coordinates
    sal_Int32 nXOut, nYOut, nWidthOut, nHeightOut;
    if ( TransformSdrToControlCoordinates( nXIn, nYIn, nWidthIn, nHeightIn, nXOut, nYOut, nWidthOut, nHeightOut ) )
    {
        // set properties
        Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
        if ( xPSet.is() )
        {
            Any aValue;
            aValue <<= nXOut;
            xPSet->setPropertyValue( DLGED_PROP_POSITIONX, aValue );
            aValue <<= nYOut;
            xPSet->setPropertyValue( DLGED_PROP_POSITIONY, aValue );
            aValue <<= nWidthOut;
            xPSet->setPropertyValue( DLGED_PROP_WIDTH, aValue );
            aValue <<= nHeightOut;
            xPSet->setPropertyValue( DLGED_PROP_HEIGHT, aValue );
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdObj::PositionAndSizeChange( const beans::PropertyChangeEvent& evt )
{
    DlgEdPage* pPage_ = 0;
    if ( pDlgEdForm )
    {
        DlgEditor* pEditor = pDlgEdForm->GetDlgEditor();
        if ( pEditor )
            pPage_ = pEditor->GetPage();
    }
    DBG_ASSERT( pPage_, "DlgEdObj::PositionAndSizeChange: no page!" );
    if ( pPage_ )
    {
        sal_Int32 nPageXIn = 0;
        sal_Int32 nPageYIn = 0;
        Size aPageSize = pPage_->GetSize();
        sal_Int32 nPageWidthIn = aPageSize.Width();
        sal_Int32 nPageHeightIn = aPageSize.Height();
        sal_Int32 nPageX, nPageY, nPageWidth, nPageHeight;
        if ( TransformSdrToControlCoordinates( nPageXIn, nPageYIn, nPageWidthIn, nPageHeightIn, nPageX, nPageY, nPageWidth, nPageHeight ) )
        {
            Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
            if ( xPSet.is() )
            {		
                sal_Int32 nX = 0, nY = 0, nWidth = 0, nHeight = 0;
                xPSet->getPropertyValue( DLGED_PROP_POSITIONX ) >>= nX;
                xPSet->getPropertyValue( DLGED_PROP_POSITIONY ) >>= nY;
                xPSet->getPropertyValue( DLGED_PROP_WIDTH ) >>= nWidth;
                xPSet->getPropertyValue( DLGED_PROP_HEIGHT ) >>= nHeight;

                sal_Int32 nValue = 0;
                evt.NewValue >>= nValue;
                sal_Int32 nNewValue = nValue;

                if ( evt.PropertyName == DLGED_PROP_POSITIONX )
                {
                    if ( nNewValue + nWidth > nPageX + nPageWidth )
                        nNewValue = nPageX + nPageWidth - nWidth;
                    if ( nNewValue < nPageX )
                        nNewValue = nPageX;
                }
                else if ( evt.PropertyName == DLGED_PROP_POSITIONY )
                {
                    if ( nNewValue + nHeight > nPageY + nPageHeight )
                        nNewValue = nPageY + nPageHeight - nHeight;
                    if ( nNewValue < nPageY )
                        nNewValue = nPageY;
                }
                else if ( evt.PropertyName == DLGED_PROP_WIDTH )
                {
                    if ( nX + nNewValue > nPageX + nPageWidth )
                        nNewValue = nPageX + nPageWidth - nX;
                    if ( nNewValue < 1 )
                        nNewValue = 1;
                }
                else if ( evt.PropertyName == DLGED_PROP_HEIGHT )
                {
                    if ( nY + nNewValue > nPageY + nPageHeight )
                        nNewValue = nPageY + nPageHeight - nY;
                    if ( nNewValue < 1 )
                        nNewValue = 1;
                }

                if ( nNewValue != nValue )
                {
                    Any aNewValue;
                    aNewValue <<= nNewValue;
                    EndListening( sal_False );
                    xPSet->setPropertyValue( evt.PropertyName, aNewValue );
                    StartListening();
                }
            }
        }
    }
    
    SetRectFromProps();
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::NameChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException)
{
	// get old name
	::rtl::OUString aOldName;
	evt.OldValue >>= aOldName;

	// get new name
	::rtl::OUString aNewName;
	evt.NewValue >>= aNewName;

	if ( !aNewName.equals(aOldName) )
	{
		Reference< container::XNameAccess > xNameAcc((GetDlgEdForm()->GetUnoControlModel()), UNO_QUERY);
		if ( xNameAcc.is() && xNameAcc->hasByName(aOldName) )
		{	
			if ( !xNameAcc->hasByName(aNewName) && aNewName.getLength() != 0 )
			{
				// remove the control by the old name and insert the control by the new name in the container 
				Reference< container::XNameContainer > xCont(xNameAcc, UNO_QUERY );
				if ( xCont.is() )
				{
					Reference< awt::XControlModel > xCtrl(GetUnoControlModel(), UNO_QUERY);
					Any aAny;
					aAny <<= xCtrl;
					xCont->removeByName( aOldName );
					xCont->insertByName( aNewName , aAny );

					DlgEditor* pEditor;
					if ( ISA(DlgEdForm) )
						pEditor = ((DlgEdForm*)this)->GetDlgEditor();
					else
						pEditor = GetDlgEdForm()->GetDlgEditor();
					LocalizationMgr::renameControlResourceIDsForEditorObject( pEditor, aAny, aNewName );
				}		
			}
			else
			{
				// set old name property
				EndListening(sal_False);
				Reference< beans::XPropertySet >  xPSet(GetUnoControlModel(), UNO_QUERY);
				Any aName;
				aName <<= aOldName;
				xPSet->setPropertyValue( DLGED_PROP_NAME, aName );
				StartListening();
			}
		}
	}
}

//----------------------------------------------------------------------------

sal_Int32 DlgEdObj::GetStep() const
{
	// get step property
	sal_Int32 nStep = 0;
	uno::Reference< beans::XPropertySet >  xPSet( GetUnoControlModel(), uno::UNO_QUERY );
	if (xPSet.is())
	{
		xPSet->getPropertyValue( DLGED_PROP_STEP ) >>= nStep;
	}
	return nStep;
}

//----------------------------------------------------------------------------

void DlgEdObj::UpdateStep()
{
	sal_Int32 nCurStep = GetDlgEdForm()->GetStep();
	sal_Int32 nStep = GetStep();

	SdrLayerAdmin& rLayerAdmin = GetModel()->GetLayerAdmin();
	SdrLayerID nHiddenLayerId   = rLayerAdmin.GetLayerID( String( RTL_CONSTASCII_USTRINGPARAM( "HiddenLayer" ) ), sal_False );
	SdrLayerID nControlLayerId   = rLayerAdmin.GetLayerID( rLayerAdmin.GetControlLayerName(), sal_False );

	if( nCurStep )
	{
		if ( nStep && (nStep != nCurStep) )
		{
			SetLayer( nHiddenLayerId );
		}
		else
		{
			SetLayer( nControlLayerId );
		}
	}
	else
	{
		SetLayer( nControlLayerId );
	}
}

//----------------------------------------------------------------------------

void DlgEdObj::TabIndexChange( const beans::PropertyChangeEvent& evt ) throw (RuntimeException)
{
    DlgEdForm* pForm = GetDlgEdForm();
    if ( pForm )
    {
	    // stop listening with all children
	    ::std::vector<DlgEdObj*> aChildList = pForm->GetChilds();
	    ::std::vector<DlgEdObj*>::iterator aIter;
	    for ( aIter = aChildList.begin() ; aIter != aChildList.end() ; ++aIter )
	    {
		    (*aIter)->EndListening( sal_False );
	    }

	    Reference< container::XNameAccess > xNameAcc( pForm->GetUnoControlModel() , UNO_QUERY );
	    if ( xNameAcc.is() )
	    {
		    // get sequence of control names
		    Sequence< ::rtl::OUString > aNames = xNameAcc->getElementNames();
		    const ::rtl::OUString* pNames = aNames.getConstArray();
		    sal_Int32 nCtrls = aNames.getLength();
		    sal_Int16 i;

		    // create a map of tab indices and control names, sorted by tab index
		    IndexToNameMap aIndexToNameMap;
		    for ( i = 0; i < nCtrls; ++i )
		    {
			    // get control name
			    ::rtl::OUString aName( pNames[i] );

			    // get tab index
			    sal_Int16 nTabIndex = -1;
			    Any aCtrl = xNameAcc->getByName( aName );
			    Reference< beans::XPropertySet > xPSet;
   			    aCtrl >>= xPSet;
			    if ( xPSet.is() && xPSet == Reference< beans::XPropertySet >( evt.Source, UNO_QUERY ) )
				    evt.OldValue >>= nTabIndex;
			    else if ( xPSet.is() )
				    xPSet->getPropertyValue( DLGED_PROP_TABINDEX ) >>= nTabIndex;

			    // insert into map
			    aIndexToNameMap.insert( IndexToNameMap::value_type( nTabIndex, aName ) );
		    }

		    // create a helper list of control names, sorted by tab index
		    ::std::vector< ::rtl::OUString > aNameList( aIndexToNameMap.size() );
		    ::std::transform(
				    aIndexToNameMap.begin(), aIndexToNameMap.end(),
				    aNameList.begin(),
				    ::std::select2nd< IndexToNameMap::value_type >( )
			    );

		    // check tab index
		    sal_Int16 nOldTabIndex = 0;
		    evt.OldValue >>= nOldTabIndex;
		    sal_Int16 nNewTabIndex = 0;
		    evt.NewValue >>= nNewTabIndex;
		    if ( nNewTabIndex < 0 )
			    nNewTabIndex = 0;
		    else if ( nNewTabIndex > nCtrls - 1 )
			    nNewTabIndex = sal::static_int_cast<sal_Int16>( nCtrls - 1 );

		    // reorder helper list
		    ::rtl::OUString aCtrlName = aNameList[nOldTabIndex];
		    aNameList.erase( aNameList.begin() + nOldTabIndex );
		    aNameList.insert( aNameList.begin() + nNewTabIndex , aCtrlName );

		    // set new tab indices
		    for ( i = 0; i < nCtrls; ++i )
		    {
			    Any aCtrl = xNameAcc->getByName( aNameList[i] );
			    Reference< beans::XPropertySet > xPSet;
   			    aCtrl >>= xPSet;
			    if ( xPSet.is() )
			    {
				    Any aTabIndex;
				    aTabIndex <<= (sal_Int16) i;
				    xPSet->setPropertyValue( DLGED_PROP_TABINDEX, aTabIndex );
			    }
		    }

		    // reorder objects in drawing page
		    GetModel()->GetPage(0)->SetObjectOrdNum( nOldTabIndex + 1, nNewTabIndex + 1 );

            // #110559#
            pForm->UpdateTabOrderAndGroups();
	    }

	    // start listening with all children
	    for ( aIter = aChildList.begin() ; aIter != aChildList.end() ; ++aIter )
	    {
		    (*aIter)->StartListening();
	    }
    }
}

//----------------------------------------------------------------------------

sal_Bool DlgEdObj::supportsService( const sal_Char* _pServiceName ) const
{
	sal_Bool bSupports = sal_False;

	Reference< lang::XServiceInfo > xServiceInfo( GetUnoControlModel() , UNO_QUERY );
		// TODO: cache xServiceInfo as member?
	if ( xServiceInfo.is() )
		bSupports = xServiceInfo->supportsService( ::rtl::OUString::createFromAscii( _pServiceName ) );

	return bSupports;
}

//----------------------------------------------------------------------------

::rtl::OUString DlgEdObj::GetDefaultName() const
{	
	sal_uInt16 nResId = 0;
	::rtl::OUString aDefaultName;	
	if ( supportsService( "com.sun.star.awt.UnoControlDialogModel" ) )
	{
		nResId = RID_STR_CLASS_DIALOG;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlButtonModel" ) )
	{
		nResId = RID_STR_CLASS_BUTTON;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlRadioButtonModel" ) )
	{
		nResId = RID_STR_CLASS_RADIOBUTTON;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlCheckBoxModel" ) )
	{
		nResId = RID_STR_CLASS_CHECKBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlListBoxModel" ) )
	{
		nResId = RID_STR_CLASS_LISTBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlComboBoxModel" ) )
	{
		nResId = RID_STR_CLASS_COMBOBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlGroupBoxModel" ) )
	{
		nResId = RID_STR_CLASS_GROUPBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlEditModel" ) )
	{
		nResId = RID_STR_CLASS_EDIT;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlFixedTextModel" ) )
	{
		nResId = RID_STR_CLASS_FIXEDTEXT;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlImageControlModel" ) )
	{
		nResId = RID_STR_CLASS_IMAGECONTROL;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlProgressBarModel" ) )
	{
		nResId = RID_STR_CLASS_PROGRESSBAR;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlScrollBarModel" ) )
	{
		nResId = RID_STR_CLASS_SCROLLBAR;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlFixedLineModel" ) )
	{
		nResId = RID_STR_CLASS_FIXEDLINE;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlDateFieldModel" ) )
	{
		nResId = RID_STR_CLASS_DATEFIELD;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlTimeFieldModel" ) )
	{
		nResId = RID_STR_CLASS_TIMEFIELD;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlNumericFieldModel" ) )
	{
		nResId = RID_STR_CLASS_NUMERICFIELD;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlCurrencyFieldModel" ) )
	{
		nResId = RID_STR_CLASS_CURRENCYFIELD;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlFormattedFieldModel" ) )
	{
		nResId = RID_STR_CLASS_FORMATTEDFIELD;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlPatternFieldModel" ) )
	{
		nResId = RID_STR_CLASS_PATTERNFIELD;
	}
    else if ( supportsService( "com.sun.star.awt.UnoControlFileControlModel" ) )
	{
		nResId = RID_STR_CLASS_FILECONTROL;
	}
    else if ( supportsService( "com.sun.star.awt.tree.TreeControlModel" ) )
	{
		nResId = RID_STR_CLASS_TREECONTROL;
	}

    else
    {
		nResId = RID_STR_CLASS_CONTROL;        
    }

	if (nResId)
	{
		aDefaultName = ::rtl::OUString( String(IDEResId(nResId)) );			
	}

	return aDefaultName;
}

//----------------------------------------------------------------------------

::rtl::OUString DlgEdObj::GetUniqueName() const
{
	::rtl::OUString aUniqueName;
	uno::Reference< container::XNameAccess > xNameAcc((GetDlgEdForm()->GetUnoControlModel()), uno::UNO_QUERY);

	if ( xNameAcc.is() )
	{
		sal_Int32 n = 0;
		::rtl::OUString aDefaultName = GetDefaultName();

		do
		{
			aUniqueName = aDefaultName + ::rtl::OUString::valueOf(++n);
		}	while (xNameAcc->hasByName(aUniqueName));
	}

	return aUniqueName;
}

//----------------------------------------------------------------------------

sal_uInt32 DlgEdObj::GetObjInventor()   const
{
	return DlgInventor;
}

//----------------------------------------------------------------------------

sal_uInt16 DlgEdObj::GetObjIdentifier() const
{
	if ( supportsService( "com.sun.star.awt.UnoControlDialogModel" ))
	{
		return OBJ_DLG_DIALOG;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlButtonModel" ))
	{
		return OBJ_DLG_PUSHBUTTON;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlRadioButtonModel" ))
	{
		return OBJ_DLG_RADIOBUTTON;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlCheckBoxModel" ))
	{
		return OBJ_DLG_CHECKBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlListBoxModel" ))
	{
		return OBJ_DLG_LISTBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlComboBoxModel" ))
	{
		return OBJ_DLG_COMBOBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlGroupBoxModel" ))
	{
		return OBJ_DLG_GROUPBOX;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlEditModel" ))
	{
		return OBJ_DLG_EDIT;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlFixedTextModel" ))
	{
		return OBJ_DLG_FIXEDTEXT;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlImageControlModel" ))
	{
		return OBJ_DLG_IMAGECONTROL;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlProgressBarModel" ))
	{
		return OBJ_DLG_PROGRESSBAR;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlScrollBarModel" ))
	{
		return OBJ_DLG_HSCROLLBAR;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlFixedLineModel" ))
	{
		return OBJ_DLG_HFIXEDLINE;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlDateFieldModel" ))
	{
		return OBJ_DLG_DATEFIELD;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlTimeFieldModel" ))
	{
		return OBJ_DLG_TIMEFIELD;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlNumericFieldModel" ))
	{
		return OBJ_DLG_NUMERICFIELD;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlCurrencyFieldModel" ))
	{
		return OBJ_DLG_CURRENCYFIELD;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlFormattedFieldModel" ))
	{
		return OBJ_DLG_FORMATTEDFIELD;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlPatternFieldModel" ))
	{
		return OBJ_DLG_PATTERNFIELD;
	}
	else if ( supportsService( "com.sun.star.awt.UnoControlFileControlModel" ))
	{
		return OBJ_DLG_FILECONTROL;
	}
	else if ( supportsService( "com.sun.star.awt.tree.TreeControlModel" ))
	{
		return OBJ_DLG_TREECONTROL;
	}
    else
	{
		return OBJ_DLG_CONTROL;
	}
}

//----------------------------------------------------------------------------

void DlgEdObj::clonedFrom(const DlgEdObj* _pSource)
{
	// set parent form
	pDlgEdForm = _pSource->pDlgEdForm;

	// add child to parent form
	pDlgEdForm->AddChild( this );

	Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
	if ( xPSet.is() )
	{
		// set new name
		::rtl::OUString aOUniqueName( GetUniqueName() );
		Any aUniqueName;
		aUniqueName <<= aOUniqueName;
		xPSet->setPropertyValue( DLGED_PROP_NAME, aUniqueName );

		Reference< container::XNameContainer > xCont( GetDlgEdForm()->GetUnoControlModel() , UNO_QUERY );
		if ( xCont.is() )
		{
			// set tabindex
   			Sequence< OUString > aNames = xCont->getElementNames();
			Any aTabIndex;
			aTabIndex <<= (sal_Int16) aNames.getLength();
			xPSet->setPropertyValue( DLGED_PROP_TABINDEX, aTabIndex );

			// insert control model in dialog model
			Reference< awt::XControlModel > xCtrl( xPSet , UNO_QUERY );
			Any aCtrl;
			aCtrl <<= xCtrl;
			xCont->insertByName( aOUniqueName , aCtrl );

            // #110559#
            pDlgEdForm->UpdateTabOrderAndGroups();
		}
	}

	// start listening
	StartListening();
}

//----------------------------------------------------------------------------

SdrObject* DlgEdObj::Clone() const
{
	SdrObject* pReturn = SdrUnoObj::Clone();

	DlgEdObj* pDlgEdObj = PTR_CAST(DlgEdObj, pReturn);
	DBG_ASSERT( pDlgEdObj != NULL, "DlgEdObj::Clone: invalid clone!" );
	if ( pDlgEdObj )
		pDlgEdObj->clonedFrom( this );

	return pReturn;
}

//----------------------------------------------------------------------------

SdrObject* DlgEdObj::getFullDragClone() const
{
	// no need to really add the clone for dragging, it's a temporary
    // object
    SdrObject* pObj = new SdrUnoObj(String());
    *pObj = *((const SdrUnoObj*)this);

    return pObj;
}

//----------------------------------------------------------------------------

void DlgEdObj::operator= (const SdrObject& rObj)
{
	SdrUnoObj::operator= (rObj);
}

//----------------------------------------------------------------------------

void DlgEdObj::NbcMove( const Size& rSize )
{
	SdrUnoObj::NbcMove( rSize );
	
	// stop listening
	EndListening(sal_False);
	
	// set geometry properties	
	SetPropsFromRect();

	// start listening
	StartListening();

	// dialog model changed
	GetDlgEdForm()->GetDlgEditor()->SetDialogModelChanged(sal_True);
}

//----------------------------------------------------------------------------

void DlgEdObj::NbcResize(const Point& rRef, const Fraction& xFract, const Fraction& yFract)
{
	SdrUnoObj::NbcResize( rRef, xFract, yFract );

	// stop listening
	EndListening(sal_False);
	
	// set geometry properties	
	SetPropsFromRect();

	// start listening
	StartListening();

	// dialog model changed
	GetDlgEdForm()->GetDlgEditor()->SetDialogModelChanged(sal_True);
}

//----------------------------------------------------------------------------

FASTBOOL DlgEdObj::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
	FASTBOOL bResult = SdrUnoObj::EndCreate(rStat, eCmd);

	SetDefaults();
	StartListening();

	return bResult;
}

//----------------------------------------------------------------------------

void DlgEdObj::SetDefaults()
{
	// set parent form
	pDlgEdForm = ((DlgEdPage*)GetPage())->GetDlgEdForm();

    if ( pDlgEdForm )
    {
	    // add child to parent form
	    pDlgEdForm->AddChild( this );

	    Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
	    if ( xPSet.is() )
	    {
		    // get unique name
		    ::rtl::OUString aOUniqueName( GetUniqueName() );

		    // set name property
		    Any aUniqueName;
		    aUniqueName <<= aOUniqueName;
		    xPSet->setPropertyValue( DLGED_PROP_NAME, aUniqueName );

		    // set labels	
		    if ( supportsService( "com.sun.star.awt.UnoControlButtonModel" ) ||
			    supportsService( "com.sun.star.awt.UnoControlRadioButtonModel" ) ||
			    supportsService( "com.sun.star.awt.UnoControlCheckBoxModel" ) ||
			    supportsService( "com.sun.star.awt.UnoControlGroupBoxModel" ) ||
			    supportsService( "com.sun.star.awt.UnoControlFixedTextModel" ) )
		    {
			    xPSet->setPropertyValue( DLGED_PROP_LABEL, aUniqueName );
		    }

		    // set number formats supplier for formatted field
		    if ( supportsService( "com.sun.star.awt.UnoControlFormattedFieldModel" ) )
		    {
			    Reference< util::XNumberFormatsSupplier > xSupplier = GetDlgEdForm()->GetDlgEditor()->GetNumberFormatsSupplier();
			    if ( xSupplier.is() )
			    {
				    Any aSupplier;
				    aSupplier <<= xSupplier;
				    xPSet->setPropertyValue( DLGED_PROP_FORMATSSUPPLIER, aSupplier );
			    }
		    }

		    // set geometry properties
		    SetPropsFromRect();

		    Reference< container::XNameContainer > xCont( GetDlgEdForm()->GetUnoControlModel() , UNO_QUERY );
		    if ( xCont.is() )
		    {
			    // set tabindex
   			    Sequence< OUString > aNames = xCont->getElementNames();
			    uno::Any aTabIndex;
			    aTabIndex <<= (sal_Int16) aNames.getLength();
			    xPSet->setPropertyValue( DLGED_PROP_TABINDEX, aTabIndex );

			    // set step
			    Reference< beans::XPropertySet > xPSetForm( xCont, UNO_QUERY );
			    if ( xPSetForm.is() )
			    {
				    Any aStep = xPSetForm->getPropertyValue( DLGED_PROP_STEP );
				    xPSet->setPropertyValue( DLGED_PROP_STEP, aStep );
			    }

			    // insert control model in dialog model
			    Reference< awt::XControlModel > xCtrl( xPSet , UNO_QUERY );
			    Any aAny;
			    aAny <<= xCtrl;
			    xCont->insertByName( aOUniqueName , aAny );

				DlgEditor* pEditor;
				if ( ISA(DlgEdForm) )
					pEditor = ((DlgEdForm*)this)->GetDlgEditor();
				else
					pEditor = GetDlgEdForm()->GetDlgEditor();
				LocalizationMgr::setControlResourceIDsForNewEditorObject( pEditor, aAny, aOUniqueName );

                // #110559#
                pDlgEdForm->UpdateTabOrderAndGroups();
		    }
	    }

	    // dialog model changed 
	    pDlgEdForm->GetDlgEditor()->SetDialogModelChanged( sal_True );
    }
}

//----------------------------------------------------------------------------

IMPL_LINK(DlgEdObj, OnCreate, void*, EMPTYTAG)
{
	(void)EMPTYTAG;
	/*
	if (pTempView)
		pTempView->ObjectCreated(this);
	*/
	return 0;
}

//----------------------------------------------------------------------------

void DlgEdObj::StartListening()
{
	DBG_ASSERT(!isListening(), "DlgEdObj::StartListening: already listening!");

	if (!isListening())
	{
		bIsListening = sal_True;
		
		// XPropertyChangeListener
		Reference< XPropertySet > xControlModel( GetUnoControlModel() , UNO_QUERY );
		if (!m_xPropertyChangeListener.is() && xControlModel.is())
		{
			// create listener
			m_xPropertyChangeListener = static_cast< ::com::sun::star::beans::XPropertyChangeListener*>( new DlgEdPropListenerImpl( (DlgEdObj*)this ) );	

			// register listener to properties
			xControlModel->addPropertyChangeListener( ::rtl::OUString() , m_xPropertyChangeListener );
		}

		// XContainerListener
		Reference< XScriptEventsSupplier > xEventsSupplier( GetUnoControlModel() , UNO_QUERY );
		if( !m_xContainerListener.is() && xEventsSupplier.is() )
		{
			// create listener
			m_xContainerListener = static_cast< ::com::sun::star::container::XContainerListener*>( new DlgEdEvtContListenerImpl( (DlgEdObj*)this ) );	

			// register listener to script event container			
			Reference< XNameContainer > xEventCont = xEventsSupplier->getEvents();
			DBG_ASSERT(xEventCont.is(), "DlgEdObj::StartListening: control model has no script event container!");
			Reference< XContainer > xCont( xEventCont , UNO_QUERY );
			if (xCont.is())
				xCont->addContainerListener( m_xContainerListener );
		}
	}
}

//----------------------------------------------------------------------------

void DlgEdObj::EndListening(sal_Bool bRemoveListener)
{
	DBG_ASSERT(isListening(), "DlgEdObj::EndListening: not listening currently!");

	if (isListening())
	{
		bIsListening = sal_False;

		if (bRemoveListener)
		{
			// XPropertyChangeListener
			Reference< XPropertySet > xControlModel(GetUnoControlModel(), UNO_QUERY);
			if ( m_xPropertyChangeListener.is() && xControlModel.is() )
			{
				// remove listener
				xControlModel->removePropertyChangeListener( ::rtl::OUString() , m_xPropertyChangeListener );
			}
			m_xPropertyChangeListener.clear();

			// XContainerListener
			Reference< XScriptEventsSupplier > xEventsSupplier( GetUnoControlModel() , UNO_QUERY );
			if( m_xContainerListener.is() && xEventsSupplier.is() )
			{
				// remove listener
				Reference< XNameContainer > xEventCont = xEventsSupplier->getEvents();
				DBG_ASSERT(xEventCont.is(), "DlgEdObj::EndListening: control model has no script event container!");
				Reference< XContainer > xCont( xEventCont , UNO_QUERY );
				if (xCont.is())
					xCont->removeContainerListener( m_xContainerListener );
			}
			m_xContainerListener.clear();
		}
	}
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::_propertyChange( const  ::com::sun::star::beans::PropertyChangeEvent& evt ) throw( ::com::sun::star::uno::RuntimeException)
{
	if (isListening())
	{
		DlgEdForm* pRealDlgEdForm = dynamic_cast< DlgEdForm* >(this);
		if( pRealDlgEdForm == 0 )
			pRealDlgEdForm = GetDlgEdForm();

		DlgEditor* pDlgEditor = pRealDlgEdForm ? pRealDlgEdForm->GetDlgEditor() : 0;

		if( !pDlgEditor || pDlgEditor->isInPaint() )
			return;

		// dialog model changed
		pDlgEditor->SetDialogModelChanged(sal_True);
		
        // update position and size
        if ( evt.PropertyName == DLGED_PROP_POSITIONX || evt.PropertyName == DLGED_PROP_POSITIONY ||
             evt.PropertyName == DLGED_PROP_WIDTH || evt.PropertyName == DLGED_PROP_HEIGHT ||
			 evt.PropertyName == DLGED_PROP_DECORATION )
        {
            PositionAndSizeChange( evt );

			if ( evt.PropertyName == DLGED_PROP_DECORATION )
			{
				if ( ISA(DlgEdForm) )
					((DlgEdForm*)this)->GetDlgEditor()->ResetDialog();
				else
					GetDlgEdForm()->GetDlgEditor()->ResetDialog();
			}
		}
		// change name of control in dialog model
		else if ( evt.PropertyName == DLGED_PROP_NAME )
		{
			if ( !ISA(DlgEdForm) )
			{
				NameChange(evt);
			}
		}
		// update step
		else if ( evt.PropertyName == DLGED_PROP_STEP )
		{
			UpdateStep();
		}
		// change tabindex
		else if ( evt.PropertyName == DLGED_PROP_TABINDEX )
		{
			if ( !ISA(DlgEdForm) )
			{
				TabIndexChange(evt);
			}
		}
	}
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::_elementInserted(const ::com::sun::star::container::ContainerEvent& ) throw(::com::sun::star::uno::RuntimeException)
{
	if (isListening())
	{
		// dialog model changed
		if ( ISA(DlgEdForm) )
		{
			((DlgEdForm*)this)->GetDlgEditor()->SetDialogModelChanged(sal_True);
		}
		else
		{
			GetDlgEdForm()->GetDlgEditor()->SetDialogModelChanged(sal_True);
		}
	}
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::_elementReplaced(const ::com::sun::star::container::ContainerEvent& ) throw(::com::sun::star::uno::RuntimeException)
{
	if (isListening())
	{
		// dialog model changed
		if ( ISA(DlgEdForm) )
		{
			((DlgEdForm*)this)->GetDlgEditor()->SetDialogModelChanged(sal_True);
		}
		else
		{
			GetDlgEdForm()->GetDlgEditor()->SetDialogModelChanged(sal_True);
		}
	}
}

//----------------------------------------------------------------------------

void SAL_CALL DlgEdObj::_elementRemoved(const ::com::sun::star::container::ContainerEvent& ) throw(::com::sun::star::uno::RuntimeException)
{
	if (isListening())
	{
		// dialog model changed
		if ( ISA(DlgEdForm) )
		{
			((DlgEdForm*)this)->GetDlgEditor()->SetDialogModelChanged(sal_True);
		}
		else
		{
			GetDlgEdForm()->GetDlgEditor()->SetDialogModelChanged(sal_True);
		}
	}
}

//----------------------------------------------------------------------------

void DlgEdObj::SetLayer(SdrLayerID nLayer)
{
	SdrLayerID nOldLayer = GetLayer();

	if ( nLayer != nOldLayer )
	{
		SdrUnoObj::SetLayer( nLayer );

		DlgEdHint aHint( DLGED_HINT_LAYERCHANGED, this );
		GetDlgEdForm()->GetDlgEditor()->Broadcast( aHint );
	}
}

//----------------------------------------------------------------------------

TYPEINIT1(DlgEdForm, DlgEdObj);
DBG_NAME(DlgEdForm);

//----------------------------------------------------------------------------

DlgEdForm::DlgEdForm()
		  :DlgEdObj()
{
	DBG_CTOR(DlgEdForm, NULL);
}

//----------------------------------------------------------------------------

DlgEdForm::~DlgEdForm()
{
	DBG_DTOR(DlgEdForm, NULL);
}

//----------------------------------------------------------------------------

void DlgEdForm::SetDlgEditor( DlgEditor* pEditor )
{
    pDlgEditor = pEditor;
    ImplInvalidateDeviceInfo();
}

//----------------------------------------------------------------------------

void DlgEdForm::ImplInvalidateDeviceInfo()
{
    mpDeviceInfo.reset();
}

//----------------------------------------------------------------------------

void DlgEdForm::SetRectFromProps()
{
    // get form position and size from properties
    Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
    if ( xPSet.is() )
    {		
        sal_Int32 nXIn = 0, nYIn = 0, nWidthIn = 0, nHeightIn = 0;
        xPSet->getPropertyValue( DLGED_PROP_POSITIONX ) >>= nXIn;
        xPSet->getPropertyValue( DLGED_PROP_POSITIONY ) >>= nYIn;
        xPSet->getPropertyValue( DLGED_PROP_WIDTH ) >>= nWidthIn;
        xPSet->getPropertyValue( DLGED_PROP_HEIGHT ) >>= nHeightIn;

        // transform coordinates
        sal_Int32 nXOut, nYOut, nWidthOut, nHeightOut;
        if ( TransformFormToSdrCoordinates( nXIn, nYIn, nWidthIn, nHeightIn, nXOut, nYOut, nWidthOut, nHeightOut ) )
        {
            // set rectangle position and size
            Point aPoint( nXOut, nYOut );
            Size aSize( nWidthOut, nHeightOut );
            SetSnapRect( Rectangle( aPoint, aSize ) );
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdForm::SetPropsFromRect()
{
    // get form position and size from rectangle
    Rectangle aRect_ = GetSnapRect();
    sal_Int32 nXIn = aRect_.Left();
    sal_Int32 nYIn = aRect_.Top();
    sal_Int32 nWidthIn = aRect_.GetWidth();
    sal_Int32 nHeightIn = aRect_.GetHeight();

    // transform coordinates
    sal_Int32 nXOut, nYOut, nWidthOut, nHeightOut;
    if ( TransformSdrToFormCoordinates( nXIn, nYIn, nWidthIn, nHeightIn, nXOut, nYOut, nWidthOut, nHeightOut ) )
    {
        // set properties
        Reference< beans::XPropertySet > xPSet( GetUnoControlModel(), UNO_QUERY );
        if ( xPSet.is() )
        {
            Any aValue;
            aValue <<= nXOut;
            xPSet->setPropertyValue( DLGED_PROP_POSITIONX, aValue );
            aValue <<= nYOut;
            xPSet->setPropertyValue( DLGED_PROP_POSITIONY, aValue );
            aValue <<= nWidthOut;
            xPSet->setPropertyValue( DLGED_PROP_WIDTH, aValue );
            aValue <<= nHeightOut;
            xPSet->setPropertyValue( DLGED_PROP_HEIGHT, aValue );
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdForm::AddChild( DlgEdObj* pDlgEdObj )
{
	pChilds.push_back( pDlgEdObj );
}

//----------------------------------------------------------------------------

void DlgEdForm::RemoveChild( DlgEdObj* pDlgEdObj )
{
	pChilds.erase( ::std::find( pChilds.begin() , pChilds.end() , pDlgEdObj ) );
}

//----------------------------------------------------------------------------

void DlgEdForm::PositionAndSizeChange( const beans::PropertyChangeEvent& evt )
{
    DlgEditor* pEditor = GetDlgEditor();
    DBG_ASSERT( pEditor, "DlgEdForm::PositionAndSizeChange: no dialog editor!" );
    if ( pEditor )    
    {    
        DlgEdPage* pPage_ = pEditor->GetPage();
        DBG_ASSERT( pPage_, "DlgEdForm::PositionAndSizeChange: no page!" );
        if ( pPage_ )
        {
            sal_Int32 nPageXIn = 0;
            sal_Int32 nPageYIn = 0;
            Size aPageSize = pPage_->GetSize();
            sal_Int32 nPageWidthIn = aPageSize.Width();
            sal_Int32 nPageHeightIn = aPageSize.Height();
            sal_Int32 nPageX, nPageY, nPageWidth, nPageHeight;
            if ( TransformSdrToFormCoordinates( nPageXIn, nPageYIn, nPageWidthIn, nPageHeightIn, nPageX, nPageY, nPageWidth, nPageHeight ) )
            {
                Reference< beans::XPropertySet > xPSetForm( GetUnoControlModel(), UNO_QUERY );
                if ( xPSetForm.is() )
                {
                    sal_Int32 nValue = 0;
                    evt.NewValue >>= nValue;
                    sal_Int32 nNewValue = nValue;

                    if ( evt.PropertyName == DLGED_PROP_POSITIONX )
                    {
                        if ( nNewValue < nPageX )
                            nNewValue = nPageX;
                    }
                    else if ( evt.PropertyName == DLGED_PROP_POSITIONY )
                    {
                        if ( nNewValue < nPageY )
                            nNewValue = nPageY;
                    }
                    else if ( evt.PropertyName == DLGED_PROP_WIDTH )
                    {
                        if ( nNewValue < 1 )
                            nNewValue = 1;
                    }
                    else if ( evt.PropertyName == DLGED_PROP_HEIGHT )
                    {
                        if ( nNewValue < 1 )
                            nNewValue = 1;
                    }

                    if ( nNewValue != nValue )
                    {
                        Any aNewValue;
                        aNewValue <<= nNewValue;
                        EndListening( sal_False );
                        xPSetForm->setPropertyValue( evt.PropertyName, aNewValue );
                        StartListening();
                    }
                }
            }

            bool bAdjustedPageSize = pEditor->AdjustPageSize();
            SetRectFromProps();
            ::std::vector< DlgEdObj* >::iterator aIter;
            ::std::vector< DlgEdObj* > aChildList = ((DlgEdForm*)this)->GetChilds();

            if ( bAdjustedPageSize )
            {
                pEditor->InitScrollBars();
                aPageSize = pPage_->GetSize();
                nPageWidthIn = aPageSize.Width();
                nPageHeightIn = aPageSize.Height();
                if ( TransformSdrToControlCoordinates( nPageXIn, nPageYIn, nPageWidthIn, nPageHeightIn, nPageX, nPageY, nPageWidth, nPageHeight ) )
                {
                    for ( aIter = aChildList.begin(); aIter != aChildList.end(); aIter++ )
                    {
                        Reference< beans::XPropertySet > xPSet( (*aIter)->GetUnoControlModel(), UNO_QUERY );
                        if ( xPSet.is() )
                        {		
                            sal_Int32 nX = 0, nY = 0, nWidth = 0, nHeight = 0;
                            xPSet->getPropertyValue( DLGED_PROP_POSITIONX ) >>= nX;
                            xPSet->getPropertyValue( DLGED_PROP_POSITIONY ) >>= nY;
                            xPSet->getPropertyValue( DLGED_PROP_WIDTH ) >>= nWidth;
                            xPSet->getPropertyValue( DLGED_PROP_HEIGHT ) >>= nHeight;

                            sal_Int32 nNewX = nX;
                            if ( nX + nWidth > nPageX + nPageWidth )
                            {
                                nNewX = nPageX + nPageWidth - nWidth;
                                if ( nNewX < nPageX )
                                    nNewX = nPageX;
                            }
                            if ( nNewX != nX )
                            {
                                Any aValue;
                                aValue <<= nNewX;
                                EndListening( sal_False );
                                xPSet->setPropertyValue( DLGED_PROP_POSITIONX, aValue );
                                StartListening();
                            }

                            sal_Int32 nNewY = nY;
                            if ( nY + nHeight > nPageY + nPageHeight )
                            {
                                nNewY = nPageY + nPageHeight - nHeight;
                                if ( nNewY < nPageY )
                                    nNewY = nPageY;
                            }
                            if ( nNewY != nY )
                            {
                                Any aValue;
                                aValue <<= nNewY;
                                EndListening( sal_False );
                                xPSet->setPropertyValue( DLGED_PROP_POSITIONY, aValue );
                                StartListening();
                            }
                        }
                    }
                }
            }

            for ( aIter = aChildList.begin(); aIter != aChildList.end(); aIter++ )
            {
                (*aIter)->SetRectFromProps();
            }
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdForm::UpdateStep()
{
	sal_uLong nObjCount;
	SdrPage* pSdrPage = GetPage();

	if ( pSdrPage && ( ( nObjCount = pSdrPage->GetObjCount() ) > 0 ) )
	{
		for ( sal_uLong i = 0 ; i < nObjCount ; i++ )
		{
			SdrObject* pObj = pSdrPage->GetObj(i);
			DlgEdObj* pDlgEdObj = PTR_CAST(DlgEdObj, pObj);
			if ( pDlgEdObj && !pDlgEdObj->ISA(DlgEdForm) )
				pDlgEdObj->UpdateStep();
		}
	}
}

//----------------------------------------------------------------------------

void DlgEdForm::UpdateTabIndices()
{	
	// stop listening with all children
	::std::vector<DlgEdObj*>::iterator aIter;
	for ( aIter = pChilds.begin() ; aIter != pChilds.end() ; ++aIter )
	{
		(*aIter)->EndListening( sal_False );
	}

	Reference< ::com::sun::star::container::XNameAccess > xNameAcc( GetUnoControlModel() , UNO_QUERY );
	if ( xNameAcc.is() )
	{
		// get sequence of control names
		Sequence< ::rtl::OUString > aNames = xNameAcc->getElementNames();
		const ::rtl::OUString* pNames = aNames.getConstArray();
		sal_Int32 nCtrls = aNames.getLength();

		// create a map of tab indices and control names, sorted by tab index
		IndexToNameMap aIndexToNameMap;
		for ( sal_Int16 i = 0; i < nCtrls; ++i )
		{
			// get name
			::rtl::OUString aName( pNames[i] );

			// get tab index
			sal_Int16 nTabIndex = -1;
			Any aCtrl = xNameAcc->getByName( aName );
			Reference< ::com::sun::star::beans::XPropertySet > xPSet;
   			aCtrl >>= xPSet;
			if ( xPSet.is() )
				xPSet->getPropertyValue( DLGED_PROP_TABINDEX ) >>= nTabIndex;

			// insert into map
			aIndexToNameMap.insert( IndexToNameMap::value_type( nTabIndex, aName ) );
		}

		// set new tab indices
		sal_Int16 nNewTabIndex = 0;
		for ( IndexToNameMap::iterator aIt = aIndexToNameMap.begin(); aIt != aIndexToNameMap.end(); ++aIt )
		{
			Any aCtrl = xNameAcc->getByName( aIt->second );
			Reference< beans::XPropertySet > xPSet;
   			aCtrl >>= xPSet;
			if ( xPSet.is() )
			{
				Any aTabIndex;
				aTabIndex <<= (sal_Int16) nNewTabIndex++;
				xPSet->setPropertyValue( DLGED_PROP_TABINDEX, aTabIndex );
			}
		}

        // #110559#
        UpdateTabOrderAndGroups();
	}

	// start listening with all children
	for ( aIter = pChilds.begin() ; aIter != pChilds.end() ; ++aIter )
	{
		(*aIter)->StartListening();
	}
}

//----------------------------------------------------------------------------

void DlgEdForm::UpdateTabOrder()
{
    // #110559#
    // When the tabindex of a control model changes, the dialog control is
    // notified about those changes. Due to #109067# (bad performance of
    // dialog editor) the dialog control doesn't activate the tab order 
    // in design mode. When the dialog editor has reordered all
    // tabindices, this method allows to activate the taborder afterwards.

    Reference< awt::XUnoControlContainer > xCont( GetControl(), UNO_QUERY );
    if ( xCont.is() )
    {
        Sequence< Reference< awt::XTabController > > aSeqTabCtrls = xCont->getTabControllers();
        const Reference< awt::XTabController >* pTabCtrls = aSeqTabCtrls.getConstArray();
        sal_Int32 nCount = aSeqTabCtrls.getLength();
        for ( sal_Int32 i = 0; i < nCount; ++i )
            pTabCtrls[i]->activateTabOrder();
    }
}

//----------------------------------------------------------------------------

void DlgEdForm::UpdateGroups()
{
    // #110559#
    // The grouping of radio buttons in a dialog is done by vcl.
    // In the dialog editor we have two views (=controls) for one
    // radio button model. One control is owned by the dialog control, 
    // but not visible in design mode. The other control is owned by
    // the drawing layer object. Whereas the grouping of the first
    // control is done by vcl, the grouping of the control in the 
    // drawing layer has to be done here.

    Reference< awt::XTabControllerModel > xTabModel( GetUnoControlModel() , UNO_QUERY );
    if ( xTabModel.is() )
    {
        // create a global list of controls that belong to the dialog
        ::std::vector<DlgEdObj*> aChildList = GetChilds();
        sal_uInt32 nSize = aChildList.size();
        Sequence< Reference< awt::XControl > > aSeqControls( nSize );
        for ( sal_uInt32 i = 0; i < nSize; ++i )
            aSeqControls.getArray()[i] = Reference< awt::XControl >( aChildList[i]->GetControl(), UNO_QUERY );

        sal_Int32 nGroupCount = xTabModel->getGroupCount();
        for ( sal_Int32 nGroup = 0; nGroup < nGroupCount; ++nGroup )
        {
            // get a list of control models that belong to this group
            ::rtl::OUString aName;
            Sequence< Reference< awt::XControlModel > > aSeqModels;
            xTabModel->getGroup( nGroup, aSeqModels, aName );
            const Reference< awt::XControlModel >* pModels = aSeqModels.getConstArray();
            sal_Int32 nModelCount = aSeqModels.getLength();

            // create a list of peers that belong to this group
            Sequence< Reference< awt::XWindow > > aSeqPeers( nModelCount );
            for ( sal_Int32 nModel = 0; nModel < nModelCount; ++nModel )
            {
                // for each control model find the corresponding control in the global list
                const Reference< awt::XControl >* pControls = aSeqControls.getConstArray();
                sal_Int32 nControlCount = aSeqControls.getLength();
                for ( sal_Int32 nControl = 0; nControl < nControlCount; ++nControl )
                {
                    const Reference< awt::XControl > xCtrl( pControls[nControl] );
                    if ( xCtrl.is() )
                    {
                        Reference< awt::XControlModel > xCtrlModel( xCtrl->getModel() );                            
                        if ( (awt::XControlModel*)xCtrlModel.get() == (awt::XControlModel*)pModels[nModel].get() )
                        {
                            // get the control peer and insert into the list of peers
                            aSeqPeers.getArray()[ nModel ] = Reference< awt::XWindow >( xCtrl->getPeer(), UNO_QUERY );
                            break;
                        }
                    }                       
                }
            }

            // set the group at the dialog peer
            Reference< awt::XControl > xDlg( GetControl(), UNO_QUERY );
            if ( xDlg.is() )
            {                    
                Reference< awt::XVclContainerPeer > xDlgPeer( xDlg->getPeer(), UNO_QUERY );
                if ( xDlgPeer.is() )
                    xDlgPeer->setGroup( aSeqPeers );
            }
        }
    }
}

//----------------------------------------------------------------------------

void DlgEdForm::UpdateTabOrderAndGroups()
{
    UpdateTabOrder();
    UpdateGroups();
}

//----------------------------------------------------------------------------

void DlgEdForm::NbcMove( const Size& rSize )
{
	SdrUnoObj::NbcMove( rSize );
	
	// set geometry properties of form
	EndListening(sal_False);
	SetPropsFromRect();
	StartListening();

	// set geometry properties of all childs
	::std::vector<DlgEdObj*>::iterator aIter;
	for ( aIter = pChilds.begin() ; aIter != pChilds.end() ; aIter++ )
	{
		(*aIter)->EndListening(sal_False);
		(*aIter)->SetPropsFromRect();
		(*aIter)->StartListening();
	}

	// dialog model changed
	GetDlgEditor()->SetDialogModelChanged(sal_True);
}

//----------------------------------------------------------------------------

void DlgEdForm::NbcResize(const Point& rRef, const Fraction& xFract, const Fraction& yFract)
{
	SdrUnoObj::NbcResize( rRef, xFract, yFract );

	// set geometry properties of form
	EndListening(sal_False);
	SetPropsFromRect();
	StartListening();

	// set geometry properties of all childs
	::std::vector<DlgEdObj*>::iterator aIter;
	for ( aIter = pChilds.begin() ; aIter != pChilds.end() ; aIter++ )
	{
		(*aIter)->EndListening(sal_False);
		(*aIter)->SetPropsFromRect();
		(*aIter)->StartListening();
	}

	// dialog model changed
	GetDlgEditor()->SetDialogModelChanged(sal_True);
}

//----------------------------------------------------------------------------

FASTBOOL DlgEdForm::EndCreate(SdrDragStat& rStat, SdrCreateCmd eCmd)
{
	FASTBOOL bResult = SdrUnoObj::EndCreate(rStat, eCmd);

	// stop listening
	EndListening(sal_False);

	// set geometry properties
	SetPropsFromRect();

	// dialog model changed 
	GetDlgEditor()->SetDialogModelChanged(sal_True);

	// start listening
	StartListening();

	return bResult;
}

//----------------------------------------------------------------------------

awt::DeviceInfo DlgEdForm::getDeviceInfo() const
{
    awt::DeviceInfo aDeviceInfo;

    DlgEditor* pEditor = GetDlgEditor();
    DBG_ASSERT( pEditor, "DlgEdForm::getDeviceInfo: no editor associated with the form object!" );
    if ( !pEditor )
        return aDeviceInfo;

    Window* pWindow = pEditor->GetWindow();
    DBG_ASSERT( pWindow, "DlgEdForm::getDeviceInfo: no window associated with the editor!" );
    if ( !pWindow )
        return aDeviceInfo;

    // obtain an XControl
    ::utl::SharedUNOComponent< awt::XControl > xDialogControl; // ensures auto-disposal, if needed
    xDialogControl.reset( GetControl(), ::utl::SharedUNOComponent< awt::XControl >::NoTakeOwnership );
    if ( !xDialogControl.is() )
    {
        // don't create a temporary control all the time, this method here is called
        // way too often. Instead, use a cached DeviceInfo.
        // 2007-02-05 / i74065 / frank.schoenheit@sun.com
        if ( !!mpDeviceInfo )
            return *mpDeviceInfo;

        Reference< awt::XControlContainer > xEditorControlContainer( pEditor->GetWindowControlContainer() );
        xDialogControl.reset(
            GetTemporaryControlForWindow( *pWindow, xEditorControlContainer ),
            ::utl::SharedUNOComponent< awt::XControl >::TakeOwnership );
    }

    Reference< awt::XDevice > xDialogDevice;
    if ( xDialogControl.is() )
        xDialogDevice.set( xDialogControl->getPeer(), UNO_QUERY );
    DBG_ASSERT( xDialogDevice.is(), "DlgEdForm::getDeviceInfo: no device!" );
    if ( xDialogDevice.is() )
        aDeviceInfo = xDialogDevice->getInfo();

    mpDeviceInfo.reset( aDeviceInfo );

    return aDeviceInfo;
}

//----------------------------------------------------------------------------


