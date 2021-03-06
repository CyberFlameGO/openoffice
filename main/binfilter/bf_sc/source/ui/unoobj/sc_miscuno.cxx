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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <bf_svtools/bf_solar.h>
#include <tools/debug.hxx>

#include "miscuno.hxx"
#include "unoguard.hxx"
namespace binfilter {

using namespace ::com::sun::star;

//------------------------------------------------------------------------

SC_SIMPLE_SERVICE_INFO( ScEmptyEnumeration, "ScEmptyEnumeration", "stardiv.unknown" )
SC_SIMPLE_SERVICE_INFO( ScEmptyEnumerationAccess, "ScEmptyEnumerationAccess", "stardiv.unknown" )
//SC_SIMPLE_SERVICE_INFO( ScIndexEnumeration, "ScIndexEnumeration", "stardiv.unknown" )
SC_SIMPLE_SERVICE_INFO( ScPrintSettingsObj, "ScPrintSettingsObj", "stardiv.unknown" )

SC_SIMPLE_SERVICE_INFO( ScNameToIndexAccess, "ScNameToIndexAccess", "stardiv.unknown" )

//------------------------------------------------------------------------

//	static
uno::Reference<uno::XInterface> ScUnoHelpFunctions::AnyToInterface( const uno::Any& rAny )
{
	if ( rAny.getValueTypeClass() == uno::TypeClass_INTERFACE )
	{
		uno::Reference<uno::XInterface> xInterface;
		rAny >>= xInterface;
		return xInterface;
	}
	return uno::Reference<uno::XInterface>();	//! Exception?
}

//	static
sal_Bool ScUnoHelpFunctions::GetBoolProperty( const uno::Reference<beans::XPropertySet>& xProp,
											const ::rtl::OUString& rName, sal_Bool bDefault )
{
	sal_Bool bRet = bDefault;
	if ( xProp.is() )
	{
		try
		{
			uno::Any aAny = xProp->getPropertyValue( rName );
			//!	type conversion???
			//	operator >>= shouldn't be used for bool (?)
			if ( aAny.getValueTypeClass() == uno::TypeClass_BOOLEAN )
			{
				//!	safe way to get bool value from any???
				bRet = *(sal_Bool*)aAny.getValue();
			}
		}
		catch(uno::Exception&)
		{
			// keep default
		}
	}
	return bRet;
}

//	static
sal_Int32 ScUnoHelpFunctions::GetLongProperty( const uno::Reference<beans::XPropertySet>& xProp,
											const ::rtl::OUString& rName, long nDefault )
{
	sal_Int32 nRet = nDefault;
	if ( xProp.is() )
	{
		try
		{
			uno::Any aAny = xProp->getPropertyValue( rName );
			//!	type conversion???
			aAny >>= nRet;
		}
		catch(uno::Exception&)
		{
			// keep default
		}
	}
	return nRet;
}

//	static
sal_Int32 ScUnoHelpFunctions::GetEnumProperty( const uno::Reference<beans::XPropertySet>& xProp,
											const ::rtl::OUString& rName, long nDefault )
{
	sal_Int32 nRet = nDefault;
	if ( xProp.is() )
	{
		try
		{
			uno::Any aAny = xProp->getPropertyValue( rName );

			if ( aAny.getValueTypeClass() == uno::TypeClass_ENUM )
			{
				//!	get enum value from any???
				nRet = *(sal_Int32*)aAny.getValue();
			}
			else
			{
				//!	type conversion???
				aAny >>= nRet;
			}
		}
		catch(uno::Exception&)
		{
			// keep default
		}
	}
	return nRet;
}

//	static
sal_Bool ScUnoHelpFunctions::GetBoolFromAny( const uno::Any& aAny )
{
	if ( aAny.getValueTypeClass() == uno::TypeClass_BOOLEAN )
		return *(sal_Bool*)aAny.getValue();
	return FALSE;
}

//	static
sal_Int16 ScUnoHelpFunctions::GetInt16FromAny( const uno::Any& aAny )
{
	sal_Int16 nRet;
	if ( aAny >>= nRet )
		return nRet;
	return 0;
}

//	static
sal_Int32 ScUnoHelpFunctions::GetInt32FromAny( const uno::Any& aAny )
{
	sal_Int32 nRet;
	if ( aAny >>= nRet )
		return nRet;
	return 0;
}

//	static
sal_Int32 ScUnoHelpFunctions::GetEnumFromAny( const uno::Any& aAny )
{
	sal_Int32 nRet = 0;
	if ( aAny.getValueTypeClass() == uno::TypeClass_ENUM )
		nRet = *(sal_Int32*)aAny.getValue();
	else
		aAny >>= nRet;
	return nRet;
}

//	static
void ScUnoHelpFunctions::SetBoolInAny( uno::Any& rAny, sal_Bool bValue )
{
	rAny.setValue( &bValue, getBooleanCppuType() );
}

//------------------------------------------------------------------------

ScIndexEnumeration::ScIndexEnumeration(const uno::Reference<container::XIndexAccess>& rInd,
                                       const ::rtl::OUString& rServiceName) :
	xIndex( rInd ),
    sServiceName(rServiceName),
	nPos( 0 )
{
}

ScIndexEnumeration::~ScIndexEnumeration()
{
}

// XEnumeration

sal_Bool SAL_CALL ScIndexEnumeration::hasMoreElements() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return ( nPos < xIndex->getCount() );
}

uno::Any SAL_CALL ScIndexEnumeration::nextElement() throw(container::NoSuchElementException,
										lang::WrappedTargetException, uno::RuntimeException)
{
	ScUnoGuard aGuard;
    uno::Any aReturn;
    try
    {
        aReturn = xIndex->getByIndex(nPos++);
    }
    catch (lang::IndexOutOfBoundsException&)
    {
        throw container::NoSuchElementException();
    }
	return aReturn;
}

::rtl::OUString SAL_CALL ScIndexEnumeration::getImplementationName()
	throw(::com::sun::star::uno::RuntimeException)
{
	return ::rtl::OUString::createFromAscii("ScIndexEnumeration");
}

sal_Bool SAL_CALL ScIndexEnumeration::supportsService( const ::rtl::OUString& ServiceName )
	throw(::com::sun::star::uno::RuntimeException)
{
	return sServiceName == ServiceName;
}

::com::sun::star::uno::Sequence< ::rtl::OUString >
	SAL_CALL ScIndexEnumeration::getSupportedServiceNames(void)
	throw(::com::sun::star::uno::RuntimeException)
{
	::com::sun::star::uno::Sequence< ::rtl::OUString > aRet(1);
	::rtl::OUString* pArray = aRet.getArray();
	pArray[0] = sServiceName;
	return aRet;
}

//------------------------------------------------------------------------

ScEmptyEnumerationAccess::~ScEmptyEnumerationAccess()
{
}

// XEnumerationAccess

uno::Reference<container::XEnumeration> SAL_CALL ScEmptyEnumerationAccess::createEnumeration()
													throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return new ScEmptyEnumeration;
}

uno::Type SAL_CALL ScEmptyEnumerationAccess::getElementType() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return getCppuType((uno::Reference<uno::XInterface>*)0);	// or what?
}

sal_Bool SAL_CALL ScEmptyEnumerationAccess::hasElements() throw(uno::RuntimeException)
{
	return FALSE;
}

//------------------------------------------------------------------------

ScEmptyEnumeration::ScEmptyEnumeration()
{
}

ScEmptyEnumeration::~ScEmptyEnumeration()
{
}

// XEnumeration

sal_Bool SAL_CALL ScEmptyEnumeration::hasMoreElements() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return FALSE;
}

uno::Any SAL_CALL ScEmptyEnumeration::nextElement() throw(container::NoSuchElementException,
										lang::WrappedTargetException, uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return uno::Any();
}

//------------------------------------------------------------------------

ScNameToIndexAccess::ScNameToIndexAccess( const ::com::sun::star::uno::Reference<
											::com::sun::star::container::XNameAccess>& rNameObj ) :
	xNameAccess( rNameObj )
{
	//!	test for XIndexAccess interface at rNameObj, use that instead!

	if ( xNameAccess.is() )
		aNames = xNameAccess->getElementNames();
}

ScNameToIndexAccess::~ScNameToIndexAccess()
{
}

// XIndexAccess

sal_Int32 SAL_CALL ScNameToIndexAccess::getCount(  ) throw(::com::sun::star::uno::RuntimeException)
{
	return aNames.getLength();
}

::com::sun::star::uno::Any SAL_CALL ScNameToIndexAccess::getByIndex( sal_Int32 nIndex )
								throw(::com::sun::star::lang::IndexOutOfBoundsException,
										::com::sun::star::lang::WrappedTargetException,
										::com::sun::star::uno::RuntimeException)
{
	if ( xNameAccess.is() && nIndex >= 0 && nIndex < aNames.getLength() )
		return xNameAccess->getByName( aNames.getConstArray()[nIndex] );

	throw lang::IndexOutOfBoundsException();
	return uno::Any();
}

// XElementAccess

::com::sun::star::uno::Type SAL_CALL ScNameToIndexAccess::getElementType(  )
								throw(::com::sun::star::uno::RuntimeException)
{
	if ( xNameAccess.is() )
		return xNameAccess->getElementType();
	else
		return uno::Type();
}

sal_Bool SAL_CALL ScNameToIndexAccess::hasElements(  ) throw(::com::sun::star::uno::RuntimeException)
{
	return getCount() > 0;
}

//------------------------------------------------------------------------

ScPrintSettingsObj::~ScPrintSettingsObj()
{
}

// XPropertySet

uno::Reference<beans::XPropertySetInfo> SAL_CALL ScPrintSettingsObj::getPropertySetInfo()
														throw(uno::RuntimeException)
{
	return NULL;
}

void SAL_CALL ScPrintSettingsObj::setPropertyValue(
						const ::rtl::OUString& aPropertyName, const uno::Any& aValue )
				throw(beans::UnknownPropertyException, beans::PropertyVetoException,
						lang::IllegalArgumentException, lang::WrappedTargetException,
						uno::RuntimeException)
{
	//!	later...
}

uno::Any SAL_CALL ScPrintSettingsObj::getPropertyValue( const ::rtl::OUString& aPropertyName )
				throw(beans::UnknownPropertyException, lang::WrappedTargetException,
						uno::RuntimeException)
{
	//!	later...
	return uno::Any();
}

SC_IMPL_DUMMY_PROPERTY_LISTENER( ScPrintSettingsObj )


//------------------------------------------------------------------------



}
