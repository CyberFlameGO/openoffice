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
#include "precompiled_chart2.hxx"

#include "FeatureCommandDispatchBase.hxx"

using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;

namespace chart
{

FeatureCommandDispatchBase::FeatureCommandDispatchBase( const Reference< uno::XComponentContext >& rxContext )
    :CommandDispatch( rxContext )
    ,m_nFeatureId( 0 )
{
}

FeatureCommandDispatchBase::~FeatureCommandDispatchBase()
{
}

void FeatureCommandDispatchBase::initialize()
{
    CommandDispatch::initialize();
    fillSupportedFeatures();
}

bool FeatureCommandDispatchBase::isFeatureSupported( const ::rtl::OUString& rCommandURL )
{
    SupportedFeatures::const_iterator aIter = m_aSupportedFeatures.find( rCommandURL );
    if ( aIter != m_aSupportedFeatures.end() )
    {
        return true;
    }
    return false;
}

void FeatureCommandDispatchBase::fireStatusEvent( const ::rtl::OUString& rURL,
    const Reference< frame::XStatusListener >& xSingleListener /* = 0 */ )
{
    if ( rURL.getLength() == 0 )
    {
        SupportedFeatures::const_iterator aEnd( m_aSupportedFeatures.end() );
        for ( SupportedFeatures::const_iterator aIter( m_aSupportedFeatures.begin() ); aIter != aEnd; ++aIter )
        {
            FeatureState aFeatureState( getState( aIter->first ) );
            fireStatusEventForURL( aIter->first, aFeatureState.aState, aFeatureState.bEnabled, xSingleListener );
        }
    }
    else
    {
        FeatureState aFeatureState( getState( rURL ) );
        fireStatusEventForURL( rURL, aFeatureState.aState, aFeatureState.bEnabled, xSingleListener );
    }
}

// XDispatch
void FeatureCommandDispatchBase::dispatch( const util::URL& URL,
    const Sequence< beans::PropertyValue >& Arguments )
    throw (uno::RuntimeException)
{
    ::rtl::OUString aCommand( URL.Complete );
    if ( getState( aCommand ).bEnabled )
    {
        execute( aCommand, Arguments );
    }
}

void FeatureCommandDispatchBase::implDescribeSupportedFeature( const sal_Char* pAsciiCommandURL,
    sal_uInt16 nId, sal_Int16 nGroup )
{
    ControllerFeature aFeature;
    aFeature.Command = ::rtl::OUString::createFromAscii( pAsciiCommandURL );
    aFeature.nFeatureId = nId;
    aFeature.GroupId = nGroup;

    m_aSupportedFeatures[ aFeature.Command ] = aFeature;
}

void FeatureCommandDispatchBase::fillSupportedFeatures()
{
    describeSupportedFeatures();
}

} //  namespace chart
