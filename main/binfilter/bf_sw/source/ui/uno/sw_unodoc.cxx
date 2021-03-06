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



// System - Includes -----------------------------------------------------

#include <tools/string.hxx>
#include <bf_sfx2/docfac.hxx>

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#include "swmodule.hxx"

#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
namespace binfilter {

using namespace ::com::sun::star;

//============================================================
// com.sun.star.comp.Writer.TextDocument

uno::Sequence< ::rtl::OUString > SAL_CALL SwTextDocument_getSupportedServiceNames() throw()
{
    // useable for component registration only!
    uno::Sequence< ::rtl::OUString > aRet ( 1 );
	::rtl::OUString* pArray = aRet.getArray();
	pArray[0] = ::rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.text.TextDocument" ) );

	return aRet;
}

::rtl::OUString SAL_CALL SwTextDocument_getImplementationName() throw()
{
	return ::rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.comp.Writer.TextDocument" ) );
}

uno::Reference< uno::XInterface > SAL_CALL SwTextDocument_createInstance( const uno::Reference< lang::XMultiServiceFactory > &rSMgr )
	throw( uno::Exception )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

	// to create the service the SW_MOD should be already initialized
	DBG_ASSERT( SW_MOD(), "No StarWriter module!" );

	if ( SW_MOD() )
	{
		::rtl::OUString aFactoryURL( RTL_CONSTASCII_USTRINGPARAM ( "private:factory/swriter" ) );
		const SfxObjectFactory* pFactory = SfxObjectFactory::GetFactory( aFactoryURL );
		if ( pFactory )
		{
			SfxObjectShell* pShell = pFactory->CreateObject();
			if( pShell )
				return uno::Reference< uno::XInterface >( pShell->GetModel() );
		}
	}

	return uno::Reference< uno::XInterface >();
}

//============================================================
// com.sun.star.comp.Writer.WebDocument

uno::Sequence< ::rtl::OUString > SAL_CALL SwWebDocument_getSupportedServiceNames() throw()
{
    // useable for component registration only!
    uno::Sequence< ::rtl::OUString > aRet ( 1 );
	::rtl::OUString* pArray = aRet.getArray();
    pArray[0] = ::rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.text.WebDocument" ) );

	return aRet;
}

::rtl::OUString SAL_CALL SwWebDocument_getImplementationName() throw()
{
	return ::rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.comp.Writer.WebDocument" ) );
}

uno::Reference< uno::XInterface > SAL_CALL SwWebDocument_createInstance( const uno::Reference< lang::XMultiServiceFactory > &rSMgr )
	throw( uno::Exception )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

	// to create the service the SW_MOD should be already initialized
	DBG_ASSERT( SW_MOD(), "No StarWriter module!" );

	if ( SW_MOD() )
	{
		::rtl::OUString aFactoryURL( RTL_CONSTASCII_USTRINGPARAM ( "private:factory/swriter/web" ) );
		const SfxObjectFactory* pFactory = SfxObjectFactory::GetFactory( aFactoryURL );
		if ( pFactory )
		{
			SfxObjectShell* pShell = pFactory->CreateObject();
			if( pShell )
				return uno::Reference< uno::XInterface >( pShell->GetModel() );
		}
	}

	return uno::Reference< uno::XInterface >();
}

//============================================================
// com.sun.star.comp.Writer.GlobalDocument

uno::Sequence< ::rtl::OUString > SAL_CALL SwGlobalDocument_getSupportedServiceNames() throw()
{
    // useable for component registration only!
    uno::Sequence< ::rtl::OUString > aRet ( 1 );
	::rtl::OUString* pArray = aRet.getArray();
    pArray[0] = ::rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.text.GlobalDocument" ) );

	return aRet;
}

::rtl::OUString SAL_CALL SwGlobalDocument_getImplementationName() throw()
{
	return ::rtl::OUString ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.comp.Writer.GlobalDocument" ) );
}

uno::Reference< uno::XInterface > SAL_CALL SwGlobalDocument_createInstance( const uno::Reference< lang::XMultiServiceFactory > &rSMgr )
	throw( uno::Exception )
{
    ::vos::OGuard aGuard( Application::GetSolarMutex() );

	// to create the service the SW_MOD should be already initialized
	DBG_ASSERT( SW_MOD(), "No StarWriter module!" );

	if ( SW_MOD() )
	{
		::rtl::OUString aFactoryURL( RTL_CONSTASCII_USTRINGPARAM ( "private:factory/swriter/GlobalDocument" ) );
		const SfxObjectFactory* pFactory = SfxObjectFactory::GetFactory( aFactoryURL );
		if ( pFactory )
		{
			SfxObjectShell* pShell = pFactory->CreateObject();
			if( pShell )
				return uno::Reference< uno::XInterface >( pShell->GetModel() );
		}
	}

	return uno::Reference< uno::XInterface >();
}

}
