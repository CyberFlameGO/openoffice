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

/* genericfilter: mostly generic code for registering the filter
 */

/* "This product is not manufactured, approved, or supported by 
 * Corel Corporation or Corel Corporation Limited."
 * Portions of this code Copyright 2000 by Sun Microsystems, Inc.
 * Rest is Copyright (C) 2002 William Lachance (wlach@interlog.com)
 */
#include <stdio.h>

#include <osl/mutex.hxx>
#include <osl/thread.h>
#include <cppuhelper/factory.hxx>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#include "WordPerfectImportFilter.hxx"

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;

extern "C"
{
//==================================================================================================
void SAL_CALL component_getImplementationEnvironment(
	const sal_Char ** ppEnvTypeName, uno_Environment ** /* ppEnv */ )
{
	*ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
//==================================================================================================
void * SAL_CALL component_getFactory(
	const sal_Char * pImplName, void * pServiceManager, void * /* pRegistryKey */ )
{
	void * pRet = 0;

    OUString implName = OUString::createFromAscii( pImplName );
	if ( pServiceManager && implName.equals(WordPerfectImportFilter_getImplementationName()) )
	{
		Reference< XSingleServiceFactory > xFactory( createSingleFactory(
			reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
			OUString::createFromAscii( pImplName ),
			WordPerfectImportFilter_createInstance, WordPerfectImportFilter_getSupportedServiceNames() ) );
		
		if (xFactory.is())
		{
			xFactory->acquire();
			pRet = xFactory.get();
		}
	}
	
	return pRet;
}
}
