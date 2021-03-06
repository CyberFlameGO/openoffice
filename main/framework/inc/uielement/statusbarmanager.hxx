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



#ifndef __FRAMEWORK_UIELEMENT_STATUSBARMANAGER_HXX_
#define __FRAMEWORK_UIELEMENT_STATUSBARMANAGER_HXX_

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________

#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XStatusbarController.hpp>
#include <com/sun/star/frame/XUIControllerRegistration.hpp>
#include <com/sun/star/ui/XUIConfiguration.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XNameAccess.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <rtl/ustring.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <vcl/status.hxx>

namespace framework
{

class FrameworkStatusBar;
class StatusBarManager : public ::com::sun::star::frame::XFrameActionListener         ,
                         public ::com::sun::star::lang::XComponent                    ,
                         public ::com::sun::star::lang::XTypeProvider                 ,
                         public ::com::sun::star::ui::XUIConfigurationListener,
					     public ThreadHelpBase		                                  ,
					     public ::cppu::OWeakObject
{
    friend class FrameworkStatusBar;

    public:
        StatusBarManager( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServicveManager, 
                          const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame, 
                          const rtl::OUString& rResourceName,
                          StatusBar* pStatusBar );
        virtual ~StatusBarManager();

        //  XInterface, XTypeProvider, XServiceInfo
        FWK_DECLARE_XINTERFACE
        FWK_DECLARE_XTYPEPROVIDER
		
        StatusBar* GetStatusBar() const;
        
        // XFrameActionListener
		virtual void SAL_CALL frameAction( const com::sun::star::frame::FrameActionEvent& Action ) throw ( ::com::sun::star::uno::RuntimeException );
        
        // XEventListener
		virtual void SAL_CALL disposing( const com::sun::star::lang::EventObject& Source ) throw ( ::com::sun::star::uno::RuntimeException );
        
        // XUIConfigurationListener
        virtual void SAL_CALL elementInserted( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL elementRemoved( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL elementReplaced( const ::com::sun::star::ui::ConfigurationEvent& Event ) throw (::com::sun::star::uno::RuntimeException); 
        
        // XComponent
        void SAL_CALL dispose() throw ( ::com::sun::star::uno::RuntimeException );
        void SAL_CALL addEventListener( const com::sun::star::uno::Reference< XEventListener >& xListener ) throw( com::sun::star::uno::RuntimeException );
        void SAL_CALL removeEventListener( const com::sun::star::uno::Reference< XEventListener >& xListener ) throw( com::sun::star::uno::RuntimeException );

        void FillStatusBar( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >& rStatusBarData );

    protected:
        void StateChanged( StateChangedType nType );
	    void DataChanged( const DataChangedEvent& rDCEvt );
	    void UserDraw( const UserDrawEvent& rUDEvt );
        void Command( const CommandEvent& rEvt );
        void MouseMove( const MouseEvent& rMEvt );
        void MouseButtonDown( const MouseEvent& rMEvt );
        void MouseButtonUp( const MouseEvent& rMEvt );
        DECL_LINK( Click, StatusBar* );
        DECL_LINK( DoubleClick, StatusBar* );

        void RemoveControllers();
        rtl::OUString RetrieveLabelFromCommand( const rtl::OUString& aCmdURL );
        void CreateControllers();
        void UpdateControllers();
        void AddFrameActionListener();
        void MouseButton( const MouseEvent& rMEvt ,sal_Bool ( SAL_CALL ::com::sun::star::frame::XStatusbarController::*_pMethod )(const ::com::sun::star::awt::MouseEvent&));

    protected:
        typedef std::vector< ::com::sun::star::uno::Reference< com::sun::star::frame::XStatusListener > > StatusBarControllerVector;

        sal_Bool                                                                                        m_bDisposed : 1,
                                                                                                        m_bFrameActionRegistered : 1,
                                                                                                        m_bUpdateControllers : 1;
        sal_Bool                                                                                        m_bModuleIdentified;
        StatusBar*                                                                                      m_pStatusBar;
        rtl::OUString                                                                                   m_aModuleIdentifier;
        rtl::OUString                                                                                   m_aResourceName;
        com::sun::star::uno::Reference< com::sun::star::frame::XFrame >                                 m_xFrame;
        com::sun::star::uno::Reference< com::sun::star::container::XNameAccess >                        m_xUICommandLabels;
        StatusBarControllerVector                                                                       m_aControllerVector;
        ::cppu::OMultiTypeInterfaceContainerHelper                                                      m_aListenerContainer;   /// container for ALL Listener
        ::com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >                  m_xServiceManager;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XUIControllerRegistration >  m_xStatusbarControllerRegistration;
};
    
}

#endif // __FRAMEWORK_UIELEMENT_STATUSBARMANAGER_HXX_
