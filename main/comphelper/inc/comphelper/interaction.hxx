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



#ifndef _COMPHELPER_INTERACTION_HXX_
#define _COMPHELPER_INTERACTION_HXX_

#include <comphelper/uno3.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/task/XInteractionApprove.hpp>
#include <com/sun/star/task/XInteractionDisapprove.hpp>
#include <com/sun/star/task/XInteractionAbort.hpp>
#include <com/sun/star/task/XInteractionRetry.hpp>
#include <com/sun/star/task/XInteractionPassword.hpp>
#include <com/sun/star/task/XInteractionRequest.hpp>
#include "comphelper/comphelperdllapi.h"

//.........................................................................
namespace comphelper
{
//.........................................................................

	//=========================================================================
	//= OInteractionSelect
	//=========================================================================
	/** base class for concrete XInteractionContinuation implementations.<p/>
		Instances of the classes maintain a flag indicating if the handler was called.
	*/
	class OInteractionSelect
	{
		sal_Bool	m_bSelected : 1;	/// indicates if the select event occured

	protected:
		OInteractionSelect() : m_bSelected(sal_False) { }

	public:
		/// determines whether or not this handler was selected
		sal_Bool	wasSelected() const { return m_bSelected; }
		/// resets the state to "not selected", so you may reuse the handler
		void		reset() { m_bSelected = sal_False; }

	protected:
		void	implSelected() { m_bSelected = sal_True; }
	};

	//=========================================================================
	//= OInteraction
	//=========================================================================
	/** template for instantiating concret interaction handlers<p/>
		the template argument must eb an interface derived from XInteractionContinuation
	*/
	template <class INTERACTION>
	class OInteraction
			:public ::cppu::WeakImplHelper1< INTERACTION >
			,public OInteractionSelect
	{
	public:
		OInteraction() { }

	// XInteractionContinuation
	    virtual void SAL_CALL select(  ) throw(::com::sun::star::uno::RuntimeException);
	};

	//.........................................................................
	template <class INTERACTION>
	void SAL_CALL OInteraction< INTERACTION >::select(  ) throw(::com::sun::star::uno::RuntimeException)
	{
		implSelected();
	}

	//=========================================================================
	//= OInteractionApprove
	//=========================================================================
	typedef OInteraction< ::com::sun::star::task::XInteractionApprove >	OInteractionApprove;

	//=========================================================================
	//= OInteractionDispprove
	//=========================================================================
	typedef OInteraction< ::com::sun::star::task::XInteractionDisapprove >	OInteractionDisapprove;

	//=========================================================================
	//= OInteractionAbort
	//=========================================================================
	typedef OInteraction< ::com::sun::star::task::XInteractionAbort >	OInteractionAbort;

	//=========================================================================
	//= OInteractionRetry
	//=========================================================================
	typedef OInteraction< ::com::sun::star::task::XInteractionRetry >	OInteractionRetry;

    //=========================================================================
	//= OInteractionPassword
	//=========================================================================
    class COMPHELPER_DLLPUBLIC OInteractionPassword : public OInteraction< ::com::sun::star::task::XInteractionPassword >
	{
	public:
		OInteractionPassword()
        {
        }

        OInteractionPassword( const ::rtl::OUString& _rInitialPassword )
            :m_sPassword( _rInitialPassword )
        {
        }

        // XInteractionPassword
        virtual void SAL_CALL setPassword( const ::rtl::OUString& _Password ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getPassword(  ) throw (::com::sun::star::uno::RuntimeException);

    private:
        ::rtl::OUString m_sPassword;
	};

	//=========================================================================
	//= OInteractionRequest
	//=========================================================================
	typedef ::cppu::WeakImplHelper1	<	::com::sun::star::task::XInteractionRequest
									>	OInteractionRequest_Base;
	/** implements an interaction request (<type scope="com.sun.star.task">XInteractionRequest</type>)<p/>
		at run time, you can freely add any interaction continuation objects
	*/
	class COMPHELPER_DLLPUBLIC OInteractionRequest : public OInteractionRequest_Base
	{
		::com::sun::star::uno::Any
					m_aRequest;			/// the request we represent
		::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > >
					m_aContinuations;	/// all registered continuations

	public:
		OInteractionRequest(const ::com::sun::star::uno::Any& _rRequestDescription);

		/// add a new continuation
		void addContinuation(const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation >& _rxContinuation);
		/// clear all continuations
		void clearContinuations();

	// XInteractionRequest
		virtual ::com::sun::star::uno::Any SAL_CALL getRequest(  ) throw(::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionContinuation > > SAL_CALL getContinuations(  ) throw(::com::sun::star::uno::RuntimeException);
	};
//.........................................................................
}	// namespace comphelper
//.........................................................................

#endif // _COMPHELPER_INTERACTION_HXX_


