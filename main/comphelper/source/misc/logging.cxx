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
#include "precompiled_comphelper.hxx"

#include <comphelper/logging.hxx>
#include <comphelper/componentcontext.hxx>

/** === begin UNO includes === **/
#include <com/sun/star/logging/LoggerPool.hpp>
#include <com/sun/star/logging/LogLevel.hpp>
#include <com/sun/star/resource/XResourceBundle.hpp>
#include <com/sun/star/resource/XResourceBundleLoader.hpp>
/** === end UNO includes === **/

#include <rtl/ustrbuf.hxx>

//........................................................................
namespace comphelper
{
//........................................................................

	/** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::XComponentContext;
    using ::com::sun::star::logging::XLoggerPool;
    using ::com::sun::star::logging::LoggerPool;
    using ::com::sun::star::logging::XLogger;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::logging::XLogHandler;
    using ::com::sun::star::resource::XResourceBundle;
    using ::com::sun::star::resource::XResourceBundleLoader;
	/** === end UNO using === **/
    namespace LogLevel = ::com::sun::star::logging::LogLevel;

	//====================================================================
	//= EventLogger_Impl - declaration
	//====================================================================
    class EventLogger_Impl
    {
    private:
        ::comphelper::ComponentContext  m_aContext;
        ::rtl::OUString                 m_sLoggerName;
        Reference< XLogger >            m_xLogger;

    public:
        EventLogger_Impl( const Reference< XComponentContext >& _rxContext, const ::rtl::OUString& _rLoggerName )
            :m_aContext( _rxContext )
            ,m_sLoggerName( _rLoggerName )
        {
            impl_createLogger_nothrow();
        }

        inline bool isValid() const { return m_xLogger.is(); }
        inline const ::rtl::OUString&  getName() const { return m_sLoggerName; }
        inline const Reference< XLogger >& getLogger() const { return m_xLogger; }
        inline const ::comphelper::ComponentContext& getContext() const { return m_aContext; }

    private:
        void    impl_createLogger_nothrow();
    };

	//====================================================================
	//= EventLogger_Impl - implementation
	//====================================================================
	//--------------------------------------------------------------------
    void EventLogger_Impl::impl_createLogger_nothrow()
    {
        try
        {
            Reference< XLoggerPool > xPool( LoggerPool::get( m_aContext.getUNOContext() ), UNO_QUERY_THROW );
            if ( m_sLoggerName.getLength() )
                m_xLogger = xPool->getNamedLogger( m_sLoggerName );
            else
                m_xLogger = xPool->getDefaultLogger();
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger_Impl::impl_createLogger_nothrow: caught an exception!" );
        }
    }

	//====================================================================
	//= EventLogger
	//====================================================================
	//--------------------------------------------------------------------
    EventLogger::EventLogger( const Reference< XComponentContext >& _rxContext, const ::rtl::OUString& _rLoggerName )
        :m_pImpl( new EventLogger_Impl( _rxContext, _rLoggerName ) )
    {
    }

	//--------------------------------------------------------------------
    EventLogger::EventLogger( const Reference< XComponentContext >& _rxContext, const sal_Char* _pAsciiLoggerName )
        :m_pImpl( new EventLogger_Impl( _rxContext, ::rtl::OUString::createFromAscii( _pAsciiLoggerName ) ) )
    {
    }

	//--------------------------------------------------------------------
    EventLogger::~EventLogger()
    {
    }

	//--------------------------------------------------------------------
    const ::rtl::OUString& EventLogger::getName() const
    {
        return m_pImpl->getName();
    }

	//--------------------------------------------------------------------
    sal_Int32 EventLogger::getLogLevel() const
    {
        try
        {
            if ( m_pImpl->isValid() )
                return m_pImpl->getLogger()->getLevel();
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger::getLogLevel: caught an exception!" );
        }

        return LogLevel::OFF;
    }

	//--------------------------------------------------------------------
    void EventLogger::setLogLevel( const sal_Int32 _nLogLevel ) const
    {
        try
        {
            if ( m_pImpl->isValid() )
                m_pImpl->getLogger()->setLevel( _nLogLevel );
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger::setLogLevel: caught an exception!" );
        }
    }

	//--------------------------------------------------------------------
    bool EventLogger::isLoggable( const sal_Int32 _nLogLevel ) const
    {
        if ( !m_pImpl->isValid() )
            return false;

        try
        {
            return m_pImpl->getLogger()->isLoggable( _nLogLevel );
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger::isLoggable: caught an exception!" );
        }

        return false;
    }

	//--------------------------------------------------------------------
    bool EventLogger::addLogHandler( const Reference< XLogHandler >& _rxLogHandler )
    {
        try
        {
            if ( m_pImpl->isValid() )
            {
                m_pImpl->getLogger()->addLogHandler( _rxLogHandler );
                return true;
            }
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger::addLogHandler: caught an exception!" );
        }
        return false;
    }

	//--------------------------------------------------------------------
    bool EventLogger::removeLogHandler( const Reference< XLogHandler >& _rxLogHandler )
    {
        try
        {
            if ( m_pImpl->isValid() )
            {
                m_pImpl->getLogger()->removeLogHandler( _rxLogHandler );
                return true;
            }
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger::removeLogHandler: caught an exception!" );
        }
        return false;
    }

    //--------------------------------------------------------------------
    namespace
    {
        void    lcl_replaceParameter( ::rtl::OUString& _inout_Message, const ::rtl::OUString& _rPlaceHolder, const ::rtl::OUString& _rReplacement )
        {
            sal_Int32 nPlaceholderPosition = _inout_Message.indexOf( _rPlaceHolder );
            OSL_ENSURE( nPlaceholderPosition >= 0, "lcl_replaceParameter: placeholder not found!" );
            if ( nPlaceholderPosition < 0 )
                return;

            _inout_Message = _inout_Message.replaceAt( nPlaceholderPosition, _rPlaceHolder.getLength(), _rReplacement );
        }
    }

	//--------------------------------------------------------------------
    bool EventLogger::impl_log( const sal_Int32 _nLogLevel,
        const sal_Char* _pSourceClass, const sal_Char* _pSourceMethod, const ::rtl::OUString& _rMessage,
        const OptionalString& _rArgument1, const OptionalString& _rArgument2,
        const OptionalString& _rArgument3, const OptionalString& _rArgument4,
        const OptionalString& _rArgument5, const OptionalString& _rArgument6 ) const
    {
        // (if ::rtl::OUString had an indexOfAscii, we could save those ugly statics ...)
        static ::rtl::OUString sPH1( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "$1$" ) ) );
        static ::rtl::OUString sPH2( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "$2$" ) ) );
        static ::rtl::OUString sPH3( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "$3$" ) ) );
        static ::rtl::OUString sPH4( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "$4$" ) ) );
        static ::rtl::OUString sPH5( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "$5$" ) ) );
        static ::rtl::OUString sPH6( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "$6$" ) ) );

        ::rtl::OUString sMessage( _rMessage );
        if ( !!_rArgument1 )
            lcl_replaceParameter( sMessage, sPH1, *_rArgument1 );

        if ( !!_rArgument2 )
            lcl_replaceParameter( sMessage, sPH2, *_rArgument2 );

        if ( !!_rArgument3 )
            lcl_replaceParameter( sMessage, sPH3, *_rArgument3 );

        if ( !!_rArgument4 )
            lcl_replaceParameter( sMessage, sPH4, *_rArgument4 );

        if ( !!_rArgument5 )
            lcl_replaceParameter( sMessage, sPH5, *_rArgument5 );

        if ( !!_rArgument6 )
            lcl_replaceParameter( sMessage, sPH6, *_rArgument6 );

        try
        {
            Reference< XLogger > xLogger( m_pImpl->getLogger() );
            OSL_PRECOND( xLogger.is(), "EventLogger::impl_log: should never be called without a logger!" );
            if ( _pSourceClass && _pSourceMethod )
            {
                xLogger->logp(
                    _nLogLevel,
                    ::rtl::OUString::createFromAscii( _pSourceClass ),
                    ::rtl::OUString::createFromAscii( _pSourceMethod ),
                    sMessage
                );
            }
            else
            {
                xLogger->log( _nLogLevel, sMessage );
            }
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "EventLogger::impl_log: caught an exception!" );
        }

        return false;
    }

	//====================================================================
	//= ResourceBasedEventLogger_Data
	//====================================================================
    struct ResourceBasedEventLogger_Data
    {
        /// the base name of the resource bundle
        ::rtl::OUString                 sBundleBaseName;
        /// did we already attempt to load the bundle?
        bool                            bBundleLoaded;
        /// the lazily loaded bundle
        Reference< XResourceBundle >    xBundle;

        ResourceBasedEventLogger_Data()
            :sBundleBaseName()
            ,bBundleLoaded( false )
            ,xBundle()
        {
        }
    };

    //--------------------------------------------------------------------
    bool    lcl_loadBundle_nothrow( const ComponentContext& _rContext, ResourceBasedEventLogger_Data& _rLoggerData )
    {
        if ( _rLoggerData.bBundleLoaded )
            return _rLoggerData.xBundle.is();

        // no matter what happens below, don't attempt creation ever again
        _rLoggerData.bBundleLoaded = true;

        try
        {
            Reference< XResourceBundleLoader > xLoader( _rContext.getSingleton( "com.sun.star.resource.OfficeResourceLoader" ), UNO_QUERY_THROW );
            _rLoggerData.xBundle = Reference< XResourceBundle >( xLoader->loadBundle_Default( _rLoggerData.sBundleBaseName ), UNO_QUERY_THROW );
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "lcl_loadBundle_nothrow: caught an exception!" );
        }

        return _rLoggerData.xBundle.is();
    }

    //--------------------------------------------------------------------
    ::rtl::OUString lcl_loadString_nothrow( const Reference< XResourceBundle >& _rxBundle, const sal_Int32 _nMessageResID )
    {
        OSL_PRECOND( _rxBundle.is(), "lcl_loadString_nothrow: this will crash!" );
        ::rtl::OUString sMessage;
        try
        {
            ::rtl::OUStringBuffer aBuffer;
            aBuffer.appendAscii( "string:" );
            aBuffer.append( _nMessageResID );
            OSL_VERIFY( _rxBundle->getDirectElement( aBuffer.makeStringAndClear() ) >>= sMessage );
        }
        catch( const Exception& e )
        {
            (void)e;
            OSL_ENSURE( false, "lcl_loadString_nothrow: caught an exception!" );
        }
        return sMessage;
    }

	//====================================================================
	//= ResourceBasedEventLogger
	//====================================================================
	//--------------------------------------------------------------------
    ResourceBasedEventLogger::ResourceBasedEventLogger( const Reference< XComponentContext >& _rxContext, const ::rtl::OUString& _rResourceBundleBaseName,
        const ::rtl::OUString& _rLoggerName )
        :EventLogger( _rxContext, _rLoggerName )
        ,m_pData( new ResourceBasedEventLogger_Data )
    {
        m_pData->sBundleBaseName = _rResourceBundleBaseName;
    }

	//--------------------------------------------------------------------
    ResourceBasedEventLogger::ResourceBasedEventLogger( const Reference< XComponentContext >& _rxContext, const sal_Char* _pResourceBundleBaseName,
        const sal_Char* _pAsciiLoggerName )
        :EventLogger( _rxContext, _pAsciiLoggerName )
        ,m_pData( new ResourceBasedEventLogger_Data )
    {
        m_pData->sBundleBaseName = ::rtl::OUString::createFromAscii( _pResourceBundleBaseName );
    }

	//--------------------------------------------------------------------
    ::rtl::OUString ResourceBasedEventLogger::impl_loadStringMessage_nothrow( const sal_Int32 _nMessageResID ) const
    {
        ::rtl::OUString sMessage;
        if ( lcl_loadBundle_nothrow( m_pImpl->getContext(), *m_pData ) )
            sMessage = lcl_loadString_nothrow( m_pData->xBundle, _nMessageResID );
        if ( sMessage.getLength() == 0 )
        {
            ::rtl::OUStringBuffer aBuffer;
            aBuffer.appendAscii( "<invalid event resource: '" );
            aBuffer.append( m_pData->sBundleBaseName );
            aBuffer.appendAscii( ":" );
            aBuffer.append( _nMessageResID );
            aBuffer.appendAscii( "'>" );
            sMessage = aBuffer.makeStringAndClear();
        }
        return sMessage;
    }

//........................................................................
} // namespace comphelper
//........................................................................
