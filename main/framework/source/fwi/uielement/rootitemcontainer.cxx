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
#include "precompiled_framework.hxx"
//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <uielement/rootitemcontainer.hxx>

#ifndef __FRAMEWORK_UIELEMENT_ITEMCONTAINER_HHX_
#include <uielement/itemcontainer.hxx>
#endif

#ifndef __FRAMEWORK_UIELEMENT_CONSTITEMCONTAINER_HHX_
#include <uielement/constitemcontainer.hxx>
#endif
#include <threadhelp/resetableguard.hxx>
#include <general.h>
#include <properties.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/beans/PropertyAttribute.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________

using namespace cppu;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;

const char WRONG_TYPE_EXCEPTION[] = "Type must be com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >";

const int PROPHANDLE_UINAME     = 1;
const int PROPCOUNT             = 1;
const rtl::OUString PROPNAME_UINAME( RTL_CONSTASCII_USTRINGPARAM( "UIName" ));

namespace framework
{

//*****************************************************************************************************************
//	XInterface, XTypeProvider
//*****************************************************************************************************************
DEFINE_XINTERFACE_10     (  RootItemContainer                                                   ,
                            OWeakObject                                                         ,
                            DIRECT_INTERFACE( ::com::sun::star::lang::XTypeProvider             ),
                            DIRECT_INTERFACE( ::com::sun::star::container::XIndexContainer      ),
                            DIRECT_INTERFACE( ::com::sun::star::lang::XUnoTunnel                ),
                            DIRECT_INTERFACE( ::com::sun::star::lang::XSingleComponentFactory   ),
                            DIRECT_INTERFACE( ::com::sun::star::beans::XMultiPropertySet		),
                            DIRECT_INTERFACE( ::com::sun::star::beans::XFastPropertySet		    ),
                            DIRECT_INTERFACE( ::com::sun::star::beans::XPropertySet				),
                            DERIVED_INTERFACE( ::com::sun::star::container::XIndexReplace, com::sun::star::container::XIndexContainer ),
                            DERIVED_INTERFACE( ::com::sun::star::container::XIndexAccess, com::sun::star::container::XIndexReplace    ),
                            DERIVED_INTERFACE( ::com::sun::star::container::XElementAccess, ::com::sun::star::container::XIndexAccess )
						)

DEFINE_XTYPEPROVIDER_10 (   RootItemContainer                               ,
                            ::com::sun::star::lang::XTypeProvider           ,
                            ::com::sun::star::container::XIndexContainer    ,
                            ::com::sun::star::container::XIndexReplace      ,
                            ::com::sun::star::container::XIndexAccess       ,
                            ::com::sun::star::container::XElementAccess     ,
                            ::com::sun::star::beans::XMultiPropertySet      ,
                            ::com::sun::star::beans::XFastPropertySet       ,
                            ::com::sun::star::beans::XPropertySet           ,
                            ::com::sun::star::lang::XUnoTunnel              ,
                            ::com::sun::star::lang::XSingleComponentFactory
						)

RootItemContainer::RootItemContainer()
    :   ThreadHelpBase              ( )
    ,   ::cppu::OBroadcastHelperVar< ::cppu::OMultiTypeInterfaceContainerHelper, ::cppu::OMultiTypeInterfaceContainerHelper::keyType >( m_aLock.getShareableOslMutex() )
    ,   ::cppu::OPropertySetHelper  ( *(static_cast< ::cppu::OBroadcastHelper* >(this)) )
    ,   ::cppu::OWeakObject()
{
}

RootItemContainer::RootItemContainer( const ConstItemContainer& rConstItemContainer )
    :   ThreadHelpBase              ( )
    ,   ::cppu::OBroadcastHelperVar< ::cppu::OMultiTypeInterfaceContainerHelper, ::cppu::OMultiTypeInterfaceContainerHelper::keyType >( m_aLock.getShareableOslMutex() )
    ,   ::cppu::OPropertySetHelper  ( *(static_cast< ::cppu::OBroadcastHelper* >(this)) )
    ,   ::cppu::OWeakObject()
{
    m_aUIName = rConstItemContainer.m_aUIName;
    copyItemContainer( rConstItemContainer.m_aItemVector );
}

RootItemContainer::RootItemContainer( const Reference< XIndexAccess >& rSourceContainer )
    :   ThreadHelpBase              ( )
    ,   ::cppu::OBroadcastHelperVar< ::cppu::OMultiTypeInterfaceContainerHelper, ::cppu::OMultiTypeInterfaceContainerHelper::keyType >( m_aLock.getShareableOslMutex() )
    ,   ::cppu::OPropertySetHelper  ( *(static_cast< ::cppu::OBroadcastHelper* >(this)) )
    ,   ::cppu::OWeakObject()
{
    // We also have to copy the UIName property
    try
    {
        Reference< XPropertySet > xPropSet( rSourceContainer, UNO_QUERY );
        if ( xPropSet.is() )
        {
            xPropSet->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "UIName" ))) >>= m_aUIName;
        }
    }
    catch ( Exception& )
    {
    }

    if ( rSourceContainer.is() )
    {
        sal_Int32 nCount = rSourceContainer->getCount();
        try
        {
            for ( sal_Int32 i = 0; i < nCount; i++ )
            {
                Sequence< PropertyValue > aPropSeq;
                if ( rSourceContainer->getByIndex( i ) >>= aPropSeq )
                {
                    sal_Int32 nContainerIndex = -1;
                    Reference< XIndexAccess > xIndexAccess;
                    for ( sal_Int32 j = 0; j < aPropSeq.getLength(); j++ )
                    {
                        if ( aPropSeq[j].Name.equalsAscii( "ItemDescriptorContainer" ))
                        {
                            aPropSeq[j].Value >>= xIndexAccess;
                            nContainerIndex = j;
                            break;
                        }
                    }

                    if ( xIndexAccess.is() && nContainerIndex >= 0 )
                        aPropSeq[nContainerIndex].Value <<= deepCopyContainer( xIndexAccess );

                    m_aItemVector.push_back( aPropSeq );
                }
            }
        }
        catch ( IndexOutOfBoundsException& )
        {
        }
    }
}

RootItemContainer::~RootItemContainer()
{
}

// private
void RootItemContainer::copyItemContainer( const std::vector< Sequence< PropertyValue > >& rSourceVector )
{
    const sal_uInt32 nCount = rSourceVector.size();
    m_aItemVector.reserve(nCount);
    for ( sal_uInt32 i = 0; i < nCount; i++ )
    {
        sal_Int32 nContainerIndex = -1;
        Sequence< PropertyValue > aPropSeq( rSourceVector[i] );
        Reference< XIndexAccess > xIndexAccess;
        for ( sal_Int32 j = 0; j < aPropSeq.getLength(); j++ )
        {
            if ( aPropSeq[j].Name.equalsAscii( "ItemDescriptorContainer" ))
            {
                aPropSeq[j].Value >>= xIndexAccess;
                nContainerIndex = j;
                break;
            }
        }

        if ( xIndexAccess.is() && nContainerIndex >= 0 )
            aPropSeq[nContainerIndex].Value <<= deepCopyContainer( xIndexAccess );

        m_aItemVector.push_back( aPropSeq );
    }
}

Reference< XIndexAccess > RootItemContainer::deepCopyContainer( const Reference< XIndexAccess >& rSubContainer )
{
    Reference< XIndexAccess > xReturn;
    if ( rSubContainer.is() )
    {
        ConstItemContainer* pSource = ConstItemContainer::GetImplementation( rSubContainer );
        ItemContainer* pSubContainer( 0 );
        if ( pSource )
            pSubContainer = new ItemContainer( *pSource, m_aShareMutex );
        else
            pSubContainer = new ItemContainer( rSubContainer, m_aShareMutex );
        xReturn = Reference< XIndexAccess >( static_cast< OWeakObject* >( pSubContainer ), UNO_QUERY );
    }

    return xReturn;
}

// XUnoTunnel
sal_Int64 RootItemContainer::getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rIdentifier ) throw(::com::sun::star::uno::RuntimeException)
{
    if( ( rIdentifier.getLength() == 16 ) && ( 0 == rtl_compareMemory( RootItemContainer::GetUnoTunnelId().getConstArray(), rIdentifier.getConstArray(), 16 ) ) )
        return sal::static_int_cast< sal_Int64 >( reinterpret_cast< sal_IntPtr >( this ));
    return 0;
}

const Sequence< sal_Int8 >& RootItemContainer::GetUnoTunnelId() throw()
{
	static ::com::sun::star::uno::Sequence< sal_Int8 > * pSeq = NULL;
	if( !pSeq )
	{
		::osl::Guard< ::osl::Mutex > aGuard( ::osl::Mutex::getGlobalMutex() );
		if( !pSeq )
		{
			static ::com::sun::star::uno::Sequence< sal_Int8 > aSeq( 16 );
			rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
			pSeq = &aSeq;
		}
	}
	return *pSeq;
}

RootItemContainer* RootItemContainer::GetImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rxIFace ) throw()
{
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XUnoTunnel > xUT( rxIFace, ::com::sun::star::uno::UNO_QUERY );
	return xUT.is() ? reinterpret_cast< RootItemContainer* >(sal::static_int_cast< sal_IntPtr >(
                          xUT->getSomething( RootItemContainer::GetUnoTunnelId() ))) : NULL;
}

// XElementAccess
sal_Bool SAL_CALL RootItemContainer::hasElements()
throw ( RuntimeException )
{
    ShareGuard aLock( m_aShareMutex );
    return ( !m_aItemVector.empty() );
}

// XIndexAccess
sal_Int32 SAL_CALL RootItemContainer::getCount()
throw ( RuntimeException )
{
    ShareGuard aLock( m_aShareMutex );
    return m_aItemVector.size();
}

Any SAL_CALL RootItemContainer::getByIndex( sal_Int32 Index )
throw ( IndexOutOfBoundsException, WrappedTargetException, RuntimeException )
{
    ShareGuard aLock( m_aShareMutex );
    if ( sal_Int32( m_aItemVector.size()) > Index )
        return makeAny( m_aItemVector[Index] );
    else
		throw IndexOutOfBoundsException( ::rtl::OUString(), (OWeakObject *)this );
}

// XIndexContainer
void SAL_CALL RootItemContainer::insertByIndex( sal_Int32 Index, const Any& aItem )
throw ( IllegalArgumentException, IndexOutOfBoundsException, WrappedTargetException, RuntimeException )
{
    Sequence< PropertyValue > aSeq;
    if ( aItem >>= aSeq )
    {
        ShareGuard aLock( m_aShareMutex );
        if ( sal_Int32( m_aItemVector.size()) == Index )
            m_aItemVector.push_back( aSeq );
        else if ( sal_Int32( m_aItemVector.size()) >Index )
	    {
		    std::vector< Sequence< PropertyValue > >::iterator aIter = m_aItemVector.begin();
            aIter += Index;
		    m_aItemVector.insert( aIter, aSeq );
        }
        else
		    throw IndexOutOfBoundsException( ::rtl::OUString(), (OWeakObject *)this );
    }
    else
        throw IllegalArgumentException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( WRONG_TYPE_EXCEPTION )),
				                        (OWeakObject *)this, 2 );
}

void SAL_CALL RootItemContainer::removeByIndex( sal_Int32 Index )
throw ( IndexOutOfBoundsException, WrappedTargetException, RuntimeException )
{
    ShareGuard aLock( m_aShareMutex );
	if ( (sal_Int32)m_aItemVector.size() > Index )
	{
		std::vector< Sequence< PropertyValue > >::iterator aIter = m_aItemVector.begin();
		aIter += Index;
		m_aItemVector.erase( aIter );
	}
	else
		throw IndexOutOfBoundsException( ::rtl::OUString(), (OWeakObject *)this );
}

void SAL_CALL RootItemContainer::replaceByIndex( sal_Int32 Index, const Any& aItem )
throw ( IllegalArgumentException, IndexOutOfBoundsException, WrappedTargetException, RuntimeException )
{
    Sequence< PropertyValue > aSeq;
    if ( aItem >>= aSeq )
    {
        ShareGuard aLock( m_aShareMutex );
        if ( sal_Int32( m_aItemVector.size()) > Index )
            m_aItemVector[Index] = aSeq;
        else
		    throw IndexOutOfBoundsException( ::rtl::OUString(), (OWeakObject *)this );
    }
    else
        throw IllegalArgumentException( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( WRONG_TYPE_EXCEPTION )),
				                        (OWeakObject *)this, 2 );
}

Reference< XInterface > SAL_CALL RootItemContainer::createInstanceWithContext( const Reference< XComponentContext >& )
throw ( Exception, RuntimeException)
{
    return (OWeakObject *)(new ItemContainer( m_aShareMutex ));
}

Reference< XInterface > SAL_CALL RootItemContainer::createInstanceWithArgumentsAndContext( const Sequence< Any >&, const Reference< XComponentContext >& )
throw (Exception, RuntimeException)
{
    return (OWeakObject *)(new ItemContainer( m_aShareMutex ));
}

// XPropertySet helper
sal_Bool SAL_CALL RootItemContainer::convertFastPropertyValue( Any&       aConvertedValue ,
                                                               Any&       aOldValue       ,
                                                               sal_Int32  nHandle         ,
                                                               const Any& aValue             )
throw( com::sun::star::lang::IllegalArgumentException )
{
	//	Initialize state with sal_False !!!
	//	(Handle can be invalid)
	sal_Bool bReturn = sal_False;

    switch( nHandle )
	{
        case PROPHANDLE_UINAME:
            bReturn = PropHelper::willPropertyBeChanged(
                        com::sun::star::uno::makeAny(m_aUIName),
                        aValue,
                        aOldValue,
                        aConvertedValue);
            break;
	}

	// Return state of operation.
	return bReturn ;
}

void SAL_CALL RootItemContainer::setFastPropertyValue_NoBroadcast( sal_Int32               nHandle ,
                                                                   const com::sun::star::uno::Any&    aValue  )
throw( com::sun::star::uno::Exception )
{
    switch( nHandle )
	{
        case PROPHANDLE_UINAME:
            aValue >>= m_aUIName;
            break;
	}
}

void SAL_CALL RootItemContainer::getFastPropertyValue( com::sun::star::uno::Any& aValue  ,
                                                       sal_Int32                 nHandle                ) const
{
    switch( nHandle )
	{
        case PROPHANDLE_UINAME:
            aValue <<= m_aUIName;
            break;
	}
}

::cppu::IPropertyArrayHelper& SAL_CALL RootItemContainer::getInfoHelper()
{
	// Optimize this method !
	// We initialize a static variable only one time. And we don't must use a mutex at every call!
	// For the first call; pInfoHelper is NULL - for the second call pInfoHelper is different from NULL!
    static ::cppu::OPropertyArrayHelper* pInfoHelper = NULL;

    if( pInfoHelper == NULL )
	{
		// Ready for multithreading
        osl::MutexGuard aGuard( osl::Mutex::getGlobalMutex() ) ;

		// Control this pointer again, another instance can be faster then these!
        if( pInfoHelper == NULL )
		{
			// Define static member to give structure of properties to baseclass "OPropertySetHelper".
			// "impl_getStaticPropertyDescriptor" is a non exported and static funtion, who will define a static propertytable.
			// "sal_True" say: Table is sorted by name.
            static ::cppu::OPropertyArrayHelper aInfoHelper( impl_getStaticPropertyDescriptor(), sal_True );
			pInfoHelper = &aInfoHelper;
		}
	}

    return(*pInfoHelper);
}

com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo > SAL_CALL RootItemContainer::getPropertySetInfo()
throw (::com::sun::star::uno::RuntimeException)
{
	// Optimize this method !
	// We initialize a static variable only one time. And we don't must use a mutex at every call!
	// For the first call; pInfo is NULL - for the second call pInfo is different from NULL!
    static com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo >* pInfo = NULL;

    if( pInfo == NULL )
	{
		// Ready for multithreading
		osl::MutexGuard aGuard( osl::Mutex::getGlobalMutex() ) ;
		// Control this pointer again, another instance can be faster then these!
        if( pInfo == NULL )
		{
			// Create structure of propertysetinfo for baseclass "OPropertySetHelper".
			// (Use method "getInfoHelper()".)
            static com::sun::star::uno::Reference< com::sun::star::beans::XPropertySetInfo > xInfo( createPropertySetInfo( getInfoHelper() ) );
			pInfo = &xInfo;
		}
	}

	return (*pInfo);
}

const com::sun::star::uno::Sequence< com::sun::star::beans::Property > RootItemContainer::impl_getStaticPropertyDescriptor()
{
	// Create a new static property array to initialize sequence!
	// Table of all predefined properties of this class. Its used from OPropertySetHelper-class!
	// Don't forget to change the defines (see begin of this file), if you add, change or delete a property in this list!!!
	// It's necessary for methods of OPropertySetHelper.
	// ATTENTION:
    //      YOU MUST SORT FOLLOW TABLE BY NAME ALPHABETICAL !!!

    static const com::sun::star::beans::Property pProperties[] =
	{
        com::sun::star::beans::Property( PROPNAME_UINAME, PROPHANDLE_UINAME ,
                                         ::getCppuType((const rtl::OUString*)NULL),
                                         com::sun::star::beans::PropertyAttribute::TRANSIENT )
	};
	// Use it to initialize sequence!
    static const com::sun::star::uno::Sequence< com::sun::star::beans::Property > lPropertyDescriptor( pProperties, PROPCOUNT );
	// Return static "PropertyDescriptor"
    return lPropertyDescriptor;
}

} // namespace framework

