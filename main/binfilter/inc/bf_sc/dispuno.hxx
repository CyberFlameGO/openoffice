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



#ifndef SC_DISPUNO_HXX
#define SC_DISPUNO_HXX

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDERINTERCEPTOR_HPP_
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#endif
#ifndef _COM_SUN_STAR_VIEW_XSELECTIONCHANGELISTENER_HPP_
#include <com/sun/star/view/XSelectionChangeListener.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif

#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif


#ifndef SC_SCGLOB_HXX
#include "global.hxx"		// ScImportParam
#endif

namespace com { namespace sun { namespace star { namespace frame {
	class XDispatchProviderInterception;
} } } }
namespace binfilter {

class ScTabViewShell;


typedef ::com::sun::star::uno::Reference<
			::com::sun::star::frame::XStatusListener >* XStatusListenerPtr;
SV_DECL_PTRARR_DEL( XStatusListenerArr_Impl, XStatusListenerPtr, 4, 4 )//STRIP008 ;


class ScDispatchProviderInterceptor : public cppu::WeakImplHelper2<
										::com::sun::star::frame::XDispatchProviderInterceptor,
										::com::sun::star::lang::XEventListener>,
									public SfxListener
{
	ScTabViewShell*		pViewShell;

	// the component which's dispatches we're intercepting
	::com::sun::star::uno::Reference<
		::com::sun::star::frame::XDispatchProviderInterception> m_xIntercepted;

	// chaining
	::com::sun::star::uno::Reference<
		::com::sun::star::frame::XDispatchProvider> m_xSlaveDispatcher;
	::com::sun::star::uno::Reference<
		::com::sun::star::frame::XDispatchProvider> m_xMasterDispatcher;

	// own dispatch
	::com::sun::star::uno::Reference<
		::com::sun::star::frame::XDispatch> m_xMyDispatch;

public:

							ScDispatchProviderInterceptor(ScTabViewShell* pViewSh);
	virtual					~ScDispatchProviderInterceptor();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XDispatchProvider
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > SAL_CALL
							queryDispatch( const ::com::sun::star::util::URL& aURL,
										const ::rtl::OUString& aTargetFrameName,
										sal_Int32 nSearchFlags )
									throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference<
										::com::sun::star::frame::XDispatch > > SAL_CALL
							queryDispatches( const ::com::sun::star::uno::Sequence<
										::com::sun::star::frame::DispatchDescriptor >& aDescripts )
									throw(::com::sun::star::uno::RuntimeException);

							// XDispatchProviderInterceptor
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > SAL_CALL
							getSlaveDispatchProvider() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	setSlaveDispatchProvider( const ::com::sun::star::uno::Reference<
								::com::sun::star::frame::XDispatchProvider >& xNewDispatchProvider )
								throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > SAL_CALL
							getMasterDispatchProvider() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	setMasterDispatchProvider( const ::com::sun::star::uno::Reference<
								::com::sun::star::frame::XDispatchProvider >& xNewSupplier )
								throw(::com::sun::star::uno::RuntimeException);

							// XEventListener
	virtual void SAL_CALL	disposing( const ::com::sun::star::lang::EventObject& Source )
								throw(::com::sun::star::uno::RuntimeException);
};


class ScDispatch : public cppu::WeakImplHelper2<
									::com::sun::star::frame::XDispatch,
									::com::sun::star::view::XSelectionChangeListener >,
								public SfxListener
{
	ScTabViewShell*			pViewShell;
	XStatusListenerArr_Impl	aDataSourceListeners;
	ScImportParam			aLastImport;
	sal_Bool				bListeningToView;

public:

							ScDispatch(ScTabViewShell* pViewSh);
	virtual					~ScDispatch();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XDispatch
	virtual void SAL_CALL	dispatch( const ::com::sun::star::util::URL& aURL,
								const ::com::sun::star::uno::Sequence<
									::com::sun::star::beans::PropertyValue >& aArgs )
								throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	addStatusListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::frame::XStatusListener >& xControl,
								const ::com::sun::star::util::URL& aURL )
								throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	removeStatusListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::frame::XStatusListener >& xControl,
								const ::com::sun::star::util::URL& aURL )
								throw(::com::sun::star::uno::RuntimeException);

                            // XSelectionChangeListener
    virtual void SAL_CALL   selectionChanged( const ::com::sun::star::lang::EventObject& aEvent )
                                throw (::com::sun::star::uno::RuntimeException);

                            // XEventListener
    virtual void SAL_CALL   disposing( const ::com::sun::star::lang::EventObject& Source )
                                throw (::com::sun::star::uno::RuntimeException);
};


} //namespace binfilter
#endif

