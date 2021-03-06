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
#include "precompiled_connectivity.hxx"
#ifndef _CONNECTIVITY_CONNECTIONWRAPPER_HXX_
#include "connectivity/ConnectionWrapper.hxx"
#endif
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <comphelper/uno3.hxx>
#include <comphelper/sequence.hxx>
#include <cppuhelper/typeprovider.hxx>
#include <com/sun/star/reflection/XProxyFactory.hpp>
#include <rtl/digest.h>
#include <algorithm>

#include <algorithm>

using namespace connectivity;
//------------------------------------------------------------------------------
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::sdbc;
using namespace ::com::sun::star::reflection;
// --------------------------------------------------------------------------------
OConnectionWrapper::OConnectionWrapper()
{

}
// -----------------------------------------------------------------------------
void OConnectionWrapper::setDelegation(Reference< XAggregation >& _rxProxyConnection,oslInterlockedCount& _rRefCount)
{
	OSL_ENSURE(_rxProxyConnection.is(),"OConnectionWrapper: Connection must be valid!");
	osl_incrementInterlockedCount( &_rRefCount );
	if (_rxProxyConnection.is())
	{
		// transfer the (one and only) real ref to the aggregate to our member
		m_xProxyConnection = _rxProxyConnection;
		_rxProxyConnection = NULL;
		::comphelper::query_aggregation(m_xProxyConnection,m_xConnection);
		m_xTypeProvider.set(m_xConnection,UNO_QUERY);
		m_xUnoTunnel.set(m_xConnection,UNO_QUERY);
		m_xServiceInfo.set(m_xConnection,UNO_QUERY);

		// set ourself as delegator
		Reference<XInterface> xIf = static_cast< XUnoTunnel* >( this );
		m_xProxyConnection->setDelegator( xIf );

	}
	osl_decrementInterlockedCount( &_rRefCount );
}
// -----------------------------------------------------------------------------
void OConnectionWrapper::setDelegation(const Reference< XConnection >& _xConnection
									   ,const Reference< XMultiServiceFactory>& _xORB
									   ,oslInterlockedCount& _rRefCount)
{
	OSL_ENSURE(_xConnection.is(),"OConnectionWrapper: Connection must be valid!");
	osl_incrementInterlockedCount( &_rRefCount );

	m_xConnection = _xConnection;
	m_xTypeProvider.set(m_xConnection,UNO_QUERY);
	m_xUnoTunnel.set(m_xConnection,UNO_QUERY);
	m_xServiceInfo.set(m_xConnection,UNO_QUERY);

	Reference< XProxyFactory >	xProxyFactory(_xORB->createInstance(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.reflection.ProxyFactory"))),UNO_QUERY);
	Reference< XAggregation > xConProxy = xProxyFactory->createProxy(_xConnection);
	if (xConProxy.is())
	{
		// transfer the (one and only) real ref to the aggregate to our member
		m_xProxyConnection = xConProxy;

		// set ourself as delegator
		Reference<XInterface> xIf = static_cast< XUnoTunnel* >( this );
		m_xProxyConnection->setDelegator( xIf );

	}
	osl_decrementInterlockedCount( &_rRefCount );
}
// -----------------------------------------------------------------------------
void OConnectionWrapper::disposing()
{
m_xConnection.clear();
}
//-----------------------------------------------------------------------------
OConnectionWrapper::~OConnectionWrapper()
{
	if (m_xProxyConnection.is())
		m_xProxyConnection->setDelegator(NULL);
}

// XServiceInfo
// --------------------------------------------------------------------------------
::rtl::OUString SAL_CALL OConnectionWrapper::getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException)
{
	return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sdbc.drivers.OConnectionWrapper" ) );
}

// --------------------------------------------------------------------------------
::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL OConnectionWrapper::getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException)
{
	// first collect the services which are supported by our aggregate
	Sequence< ::rtl::OUString > aSupported;
	if ( m_xServiceInfo.is() )
		aSupported = m_xServiceInfo->getSupportedServiceNames();

	// append our own service, if necessary
	::rtl::OUString sConnectionService( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sdbc.Connection" ) );
	if ( 0 == ::comphelper::findValue( aSupported, sConnectionService, sal_True ).getLength() )
	{
		sal_Int32 nLen = aSupported.getLength();
		aSupported.realloc( nLen + 1 );
		aSupported[ nLen ] = sConnectionService;
	}

	// outta here
	return aSupported;
}

// --------------------------------------------------------------------------------
sal_Bool SAL_CALL OConnectionWrapper::supportsService( const ::rtl::OUString& _rServiceName ) throw(::com::sun::star::uno::RuntimeException)
{
	return ::comphelper::findValue( getSupportedServiceNames(), _rServiceName, sal_True ).getLength() != 0;
}

// --------------------------------------------------------------------------------
Any SAL_CALL OConnectionWrapper::queryInterface( const Type& _rType ) throw (RuntimeException)
{
	Any aReturn = OConnection_BASE::queryInterface(_rType);
	return aReturn.hasValue() ? aReturn : (m_xProxyConnection.is() ? m_xProxyConnection->queryAggregation(_rType) : aReturn);
}
// --------------------------------------------------------------------------------
Sequence< Type > SAL_CALL OConnectionWrapper::getTypes(  ) throw (::com::sun::star::uno::RuntimeException)
{
	return ::comphelper::concatSequences(
		OConnection_BASE::getTypes(),
		m_xTypeProvider->getTypes()
	);
}
// -----------------------------------------------------------------------------
// com::sun::star::lang::XUnoTunnel
sal_Int64 SAL_CALL OConnectionWrapper::getSomething( const Sequence< sal_Int8 >& rId ) throw(RuntimeException)
{
	if (rId.getLength() == 16 && 0 == rtl_compareMemory(getUnoTunnelImplementationId().getConstArray(),  rId.getConstArray(), 16 ) )
		return reinterpret_cast< sal_Int64 >( this );

	if(m_xUnoTunnel.is())
		return m_xUnoTunnel->getSomething(rId);
	return 0;
}

// -----------------------------------------------------------------------------
Sequence< sal_Int8 > OConnectionWrapper::getUnoTunnelImplementationId()
{
	static ::cppu::OImplementationId * pId = 0;
	if (! pId)
	{
		::osl::MutexGuard aGuard( ::osl::Mutex::getGlobalMutex() );
		if (! pId)
		{
			static ::cppu::OImplementationId aId;
			pId = &aId;
		}
	}
	return pId->getImplementationId();
}
// -----------------------------------------------------------------------------
namespace
{
	class TPropertyValueLessFunctor : public ::std::binary_function< ::com::sun::star::beans::PropertyValue,::com::sun::star::beans::PropertyValue,bool>
	{
	public:
		TPropertyValueLessFunctor()
		{}
		bool operator() (const ::com::sun::star::beans::PropertyValue& lhs, const ::com::sun::star::beans::PropertyValue& rhs) const
		{
			return !!(lhs.Name.equalsIgnoreAsciiCase( rhs.Name ));
		}
	};

}

// -----------------------------------------------------------------------------
// creates a unique id out of the url and sequence of properties
void OConnectionWrapper::createUniqueId( const ::rtl::OUString& _rURL
					,Sequence< PropertyValue >& _rInfo
					,sal_uInt8* _pBuffer
					,const ::rtl::OUString& _rUserName
					,const ::rtl::OUString& _rPassword)
{
	// first we create the digest we want to have
	rtlDigest aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );
	rtlDigestError aError = rtl_digest_update(aDigest,_rURL.getStr(),_rURL.getLength()*sizeof(sal_Unicode));
	if ( _rUserName.getLength() )
		aError = rtl_digest_update(aDigest,_rUserName.getStr(),_rUserName.getLength()*sizeof(sal_Unicode));
	if ( _rPassword.getLength() )
		aError = rtl_digest_update(aDigest,_rPassword.getStr(),_rPassword.getLength()*sizeof(sal_Unicode));
	// now we need to sort the properties
	PropertyValue* pBegin = _rInfo.getArray();
	PropertyValue* pEnd   = pBegin + _rInfo.getLength();
	::std::sort(pBegin,pEnd,TPropertyValueLessFunctor());

	pBegin = _rInfo.getArray();
	pEnd   = pBegin + _rInfo.getLength();
	for (; pBegin != pEnd; ++pBegin)
	{
		// we only include strings an integer values
		::rtl::OUString sValue;
		if ( pBegin->Value >>= sValue )
			;
		else
		{
			sal_Int32 nValue = 0;
			if ( pBegin->Value >>= nValue )
				sValue = ::rtl::OUString::valueOf(nValue);
			else
			{
				Sequence< ::rtl::OUString> aSeq;
				if ( pBegin->Value >>= aSeq )
				{
					const ::rtl::OUString* pSBegin = aSeq.getConstArray();
					const ::rtl::OUString* pSEnd   = pSBegin + aSeq.getLength();
					for(;pSBegin != pSEnd;++pSBegin)
						aError = rtl_digest_update(aDigest,pSBegin->getStr(),pSBegin->getLength()*sizeof(sal_Unicode));
				}
			}
		}
		if ( sValue.getLength() > 0 )
		{
			// we don't have to convert this into UTF8 because we don't store on a file system
			aError = rtl_digest_update(aDigest,sValue.getStr(),sValue.getLength()*sizeof(sal_Unicode));
		}
	}

	aError = rtl_digest_get(aDigest,_pBuffer,RTL_DIGEST_LENGTH_SHA1);
	// we have to destroy the digest
	rtl_digest_destroy(aDigest);
}
// -----------------------------------------------------------------------------


