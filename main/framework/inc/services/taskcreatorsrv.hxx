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



#ifndef __FRAMEWORK_SERVICES_TASKCREATORSRV_HXX_
#define __FRAMEWORK_SERVICES_TASKCREATORSRV_HXX_

//_______________________________________________
// own includes

#include <threadhelp/threadhelpbase.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <general.h>
#include <stdtypes.h>

//_______________________________________________
// interface includes
#include <com/sun/star/uno/XInterface.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/awt/Rectangle.hpp>

//_______________________________________________
// other includes
#include <cppuhelper/weak.hxx>
#include <comphelper/sequenceashashmap.hxx>

//_______________________________________________
// definition

namespace framework
{

//_______________________________________________
/**
 *  TODO document me
 */
class TaskCreatorService : public  css::lang::XTypeProvider
                         , public  css::lang::XServiceInfo
                         , public  css::lang::XSingleServiceFactory
                           // attention! Must be the first base class to guarentee right initialize lock ...
                         , private ThreadHelpBase
                         , public  ::cppu::OWeakObject
{
    //___________________________________________
    // types

    public:
        
        /// [XFrame] if it's set, it will be used as parent frame for the new created frame.
		static const ::rtl::OUString ARGUMENT_PARENTFRAME;
    
        /** [OUString] if it's not a special name (beginning with "_" ... which are not allowed here!)
                       it will be set as the API name of the new created frame.
         */
		static const ::rtl::OUString ARGUMENT_FRAMENAME;
    
        /// [sal_Bool] If its set to sal_True we will make the new created frame visible.
		static const ::rtl::OUString ARGUMENT_MAKEVISIBLE;
    
        /** [sal_Bool] If not "ContainerWindow" property is set it force creation of a
                       top level window as new container window.
         */
		static const ::rtl::OUString ARGUMENT_CREATETOPWINDOW;
    
        /// [Rectangle] Place the new created frame on this place and resize the container window.
		static const ::rtl::OUString ARGUMENT_POSSIZE;
    
        /// [XWindow] an outside created window, used as container window of the new created frame.
		static const ::rtl::OUString ARGUMENT_CONTAINERWINDOW;
        
        /** [sal_Bool] enable/disable special mode, where this frame will be part of
                       the persistent window state feature suitable for any office module window
         */
        static const ::rtl::OUString ARGUMENT_SUPPORTPERSISTENTWINDOWSTATE;

        /** [sal_Bool] enable/disable special mode, where the title bar of our 
                       the new created frame will be updated automaticly.
                       Default = ON !
         */
        static const ::rtl::OUString ARGUMENT_ENABLE_TITLEBARUPDATE;
    //___________________________________________
    // member

    private:

        //---------------------------------------
        /** @short  the global uno service manager.
            @descr  Must be used to create own needed services.
         */
        css::uno::Reference< css::lang::XMultiServiceFactory > m_xSMGR;

    //___________________________________________
    // interface

    public:

                 TaskCreatorService(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR);
        virtual ~TaskCreatorService(                                                                   );

        // XInterface, XTypeProvider, XServiceInfo
        FWK_DECLARE_XINTERFACE
        FWK_DECLARE_XTYPEPROVIDER
        DECLARE_XSERVICEINFO

        // XSingleServiceFactory
        virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstance()
            throw(css::uno::Exception       ,
                  css::uno::RuntimeException);

        virtual css::uno::Reference< css::uno::XInterface > SAL_CALL createInstanceWithArguments(const css::uno::Sequence< css::uno::Any >& lArguments)
            throw(css::uno::Exception       ,
                  css::uno::RuntimeException);
    //___________________________________________
    // helper

    private:

        css::uno::Reference< css::awt::XWindow > implts_createContainerWindow( const css::uno::Reference< css::awt::XWindow >& xParentWindow ,
                                                                               const css::awt::Rectangle&                      aPosSize      ,
                                                                                     sal_Bool                                  bTopWindow    );
    
        void implts_applyDocStyleToWindow(const css::uno::Reference< css::awt::XWindow >& xWindow) const;
    
        css::uno::Reference< css::frame::XFrame > implts_createFrame( const css::uno::Reference< css::frame::XFrame >& xParentFrame     ,
                                                                      const css::uno::Reference< css::awt::XWindow >&  xContainerWindow ,
                                                                      const ::rtl::OUString&                           sName            );
    
        void implts_establishWindowStateListener( const css::uno::Reference< css::frame::XFrame >& xFrame );
        void implts_establishTitleBarUpdate( const css::uno::Reference< css::frame::XFrame >& xFrame );
    
        void implts_establishDocModifyListener( const css::uno::Reference< css::frame::XFrame >& xFrame );
    
        ::rtl::OUString impl_filterNames( const ::rtl::OUString& sName );
};

} // namespace framework

#endif // __FRAMEWORK_SERVICES_TASKCREATORSRV_HXX_
