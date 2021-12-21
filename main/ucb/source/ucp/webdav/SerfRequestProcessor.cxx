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
#include "precompiled_webdav.hxx"

#include "SerfRequestProcessor.hxx"
#include "SerfRequestProcessorImpl.hxx"
#include "SerfRequestProcessorImplFac.hxx"
#include "SerfCallbacks.hxx"
#include "SerfSession.hxx"

#include <apr_strings.h>

//to examine returned http code
#include "DAVException.hxx"

namespace http_dav_ucp
{

SerfRequestProcessor::SerfRequestProcessor( SerfSession& rSerfSession,
                                            const rtl::OUString & inPath,
                                            const bool bUseChunkedEncoding )
    : mrSerfSession( rSerfSession )
    , mPathStr( 0 )
    , mbUseChunkedEncoding( bUseChunkedEncoding )
    , mDestPathStr( 0 )
    , mContentType( 0 )
    , mReferer( 0 )
    , mpProcImpl( 0 )
    , mbProcessingDone( false )
    , mpDAVException()
    , mnHTTPStatusCode( SC_NONE )
    , mHTTPStatusCodeText()
    , mRedirectLocation()
    , mnSuccessfulCredentialAttempts( 0 )
    , mbInputOfCredentialsAborted( false )
    , mbSetupSerfRequestCalled( false )
    , mbAcceptSerfResponseCalled( false )
    , mbHandleSerfResponseCalled( false )
{
    mPathStr = apr_pstrdup( mrSerfSession.getAprPool(), 
                            rtl::OUStringToOString( inPath, RTL_TEXTENCODING_UTF8 ).getStr() );
}

SerfRequestProcessor::~SerfRequestProcessor()
{
    delete mpProcImpl;
    delete mpDAVException;
}

void SerfRequestProcessor::prepareProcessor()
{
    delete mpDAVException;
    mpDAVException = 0;
    mnHTTPStatusCode = SC_NONE;
    mHTTPStatusCodeText = rtl::OUString();
    mRedirectLocation = rtl::OUString();

    mnSuccessfulCredentialAttempts = 0;
    mbInputOfCredentialsAborted = false;
    mbSetupSerfRequestCalled = false;
    mbAcceptSerfResponseCalled = false;
    mbHandleSerfResponseCalled = false;
}

// PROPFIND - allprop & named
bool SerfRequestProcessor::processPropFind( const Depth inDepth,
                                            const std::vector< ::rtl::OUString > & inPropNames,
                                            std::vector< DAVResource > & ioResources,
                                            apr_status_t& outSerfStatus )
{
    mpProcImpl = createPropFindReqProcImpl( mPathStr,
                                            mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                            inDepth,
                                            inPropNames,
                                            ioResources );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// PROPFIND - property names
bool SerfRequestProcessor::processPropFind( const Depth inDepth,
                                            std::vector< DAVResourceInfo > & ioResInfo,
                                            apr_status_t& outSerfStatus )
{
    mpProcImpl = createPropFindReqProcImpl( mPathStr,
                                            mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                            inDepth,
                                            ioResInfo );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// PROPPATCH
bool SerfRequestProcessor::processPropPatch( const std::vector< ProppatchValue > & inProperties,
                                             const com::sun::star::ucb::Lock  inLock,
                                             apr_status_t& outSerfStatus )
{
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createPropPatchReqProcImpl( mPathStr,
                                             mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                             inProperties,
                                             inLockToken );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// GET
bool SerfRequestProcessor::processGet( const com::sun::star::uno::Reference< SerfInputStream >& xioInStrm, 
                                       apr_status_t& outSerfStatus )
{
    mpProcImpl = createGetReqProcImpl( mPathStr,
                                       mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                       xioInStrm );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// GET inclusive header fields
bool SerfRequestProcessor::processGet( const com::sun::star::uno::Reference< SerfInputStream >& xioInStrm, 
                                       const std::vector< ::rtl::OUString > & inHeaderNames,
                                       DAVResource & ioResource,
                                       apr_status_t& outSerfStatus )
{
    mpProcImpl = createGetReqProcImpl( mPathStr,
                                       mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                       xioInStrm,
                                       inHeaderNames,
                                       ioResource );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// GET
bool SerfRequestProcessor::processGet( const com::sun::star::uno::Reference< com::sun::star::io::XOutputStream >& xioOutStrm, 
                                       apr_status_t& outSerfStatus )
{
    mpProcImpl = createGetReqProcImpl( mPathStr,
                                       mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                       xioOutStrm );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// GET inclusive header fields
bool SerfRequestProcessor::processGet( const com::sun::star::uno::Reference< com::sun::star::io::XOutputStream >& xioOutStrm, 
                                       const std::vector< ::rtl::OUString > & inHeaderNames,
                                       DAVResource & ioResource,
                                       apr_status_t& outSerfStatus )
{
    mpProcImpl = createGetReqProcImpl( mPathStr,
                                       mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                       xioOutStrm,
                                       inHeaderNames,
                                       ioResource );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// HEAD
bool SerfRequestProcessor::processHead( const std::vector< ::rtl::OUString > & inHeaderNames,
                                        DAVResource & ioResource,
                                        apr_status_t& outSerfStatus )
{
    mpProcImpl = createHeadReqProcImpl( mPathStr,
                                        mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                        inHeaderNames,
                                        ioResource );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// PUT
bool SerfRequestProcessor::processPut( const char* inData,
                                       apr_size_t inDataLen,
                                       const com::sun::star::ucb::Lock inLock,
                                       apr_status_t& outSerfStatus )
{
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createPutReqProcImpl( mPathStr,
                                       mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                       inData,
                                       inLockToken,
                                       inDataLen );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// POST
bool SerfRequestProcessor::processPost( const char* inData,
                                        apr_size_t inDataLen,
                                        const rtl::OUString & inContentType,
                                        const rtl::OUString & inReferer,
                                        const com::sun::star::ucb::Lock inLock,
                                        const com::sun::star::uno::Reference< SerfInputStream >& xioInStrm,
                                        apr_status_t& outSerfStatus )
{
    mContentType = apr_pstrdup( mrSerfSession.getAprPool(), 
                                rtl::OUStringToOString( inContentType, RTL_TEXTENCODING_UTF8 ).getStr() );
    mReferer = apr_pstrdup( mrSerfSession.getAprPool(), 
                                rtl::OUStringToOString( inReferer, RTL_TEXTENCODING_UTF8 ).getStr() );
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createPostReqProcImpl( mPathStr,
                                        mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                        inData,
                                        inDataLen,
                                        inLockToken,
                                        mContentType,
                                        mReferer,
                                        xioInStrm );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// POST
bool SerfRequestProcessor::processPost( const char* inData,
                                        apr_size_t inDataLen,
                                        const rtl::OUString & inContentType,
                                        const rtl::OUString & inReferer,
                                        const com::sun::star::ucb::Lock inLock,
                                        const com::sun::star::uno::Reference< com::sun::star::io::XOutputStream >& xioOutStrm,
                                        apr_status_t& outSerfStatus )
{
    mContentType = apr_pstrdup( mrSerfSession.getAprPool(), 
                                rtl::OUStringToOString( inContentType, RTL_TEXTENCODING_UTF8 ).getStr() );
    mReferer = apr_pstrdup( mrSerfSession.getAprPool(), 
                            rtl::OUStringToOString( inReferer, RTL_TEXTENCODING_UTF8 ).getStr() );
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createPostReqProcImpl( mPathStr,
                                        mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                        inData,
                                        inDataLen,
                                        inLockToken,
                                        mContentType,
                                        mReferer,
                                        xioOutStrm );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// DELETE
bool SerfRequestProcessor::processDelete( const com::sun::star::ucb::Lock inLock,
                                          apr_status_t& outSerfStatus )
{
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createDeleteReqProcImpl( mPathStr,
                                          mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                          inLockToken );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// MKCOL
bool SerfRequestProcessor::processMkCol( const com::sun::star::ucb::Lock inLock,
                                         apr_status_t& outSerfStatus )
{
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createMkColReqProcImpl( mPathStr,
                                         mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                         inLockToken );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// COPY
bool SerfRequestProcessor::processCopy( const rtl::OUString & inDestinationPath,
                                        const bool inOverwrite,
                                        const com::sun::star::ucb::Lock inLock,
                                        apr_status_t& outSerfStatus )
{
    mDestPathStr = apr_pstrdup( mrSerfSession.getAprPool(), 
                                rtl::OUStringToOString( inDestinationPath, RTL_TEXTENCODING_UTF8 ).getStr() );
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createCopyReqProcImpl( mPathStr,
                                        mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                        mDestPathStr,
                                        inOverwrite,
                                        inLockToken );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

// MOVE
bool SerfRequestProcessor::processMove( const rtl::OUString & inDestinationPath,
                                        const bool inOverwrite,
                                        const com::sun::star::ucb::Lock  inLock,
                                        apr_status_t& outSerfStatus )
{
    mDestPathStr = apr_pstrdup( mrSerfSession.getAprPool(), 
                                rtl::OUStringToOString( inDestinationPath, RTL_TEXTENCODING_UTF8 ).getStr() );
    char * inLockToken = static_cast<char*>(0);
    if(inLock.LockTokens.getLength() > 0)
    {
        inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                               rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );
    }
    mpProcImpl = createMoveReqProcImpl( mPathStr,
                                        mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                        mDestPathStr,
                                        inOverwrite,
                                        inLockToken );
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

//LOCK creating a new lock
bool SerfRequestProcessor::processLock( const rtl::OUString & inDestinationPath,
                                        const com::sun::star::ucb::Lock& inLock,
                                        DAVPropertyValue & outLock,
                                        apr_status_t& outSerfStatus )
{
    mDestPathStr = apr_pstrdup( mrSerfSession.getAprPool(),
                                rtl::OUStringToOString( inDestinationPath, RTL_TEXTENCODING_UTF8 ).getStr() );
    char * Timeout;
    if(inLock.Timeout == -1)
        Timeout = apr_psprintf( mrSerfSession.getAprPool(), "Infinite" );
    else
        Timeout = apr_psprintf( mrSerfSession.getAprPool(), "Second-%ld", inLock.Timeout );

    mpProcImpl = createLockReqProcImpl( mPathStr,
                                        mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                        inLock,
                                        Timeout,
                                        outLock);
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

//LOCK refresh an existing lock
bool SerfRequestProcessor::processLockRefresh( const rtl::OUString & inDestinationPath,
                                        const com::sun::star::ucb::Lock& inLock,
                                        DAVPropertyValue & outLock,
                                        apr_status_t& outSerfStatus )
{
    mDestPathStr = apr_pstrdup( mrSerfSession.getAprPool(),
                                rtl::OUStringToOString( inDestinationPath, RTL_TEXTENCODING_UTF8 ).getStr() );
    char * Timeout;
    if(inLock.Timeout == -1)
        Timeout = apr_psprintf( mrSerfSession.getAprPool(), "Infinite" );
    else
        Timeout = apr_psprintf( mrSerfSession.getAprPool(), "Second-%ld", inLock.Timeout );

    char * inLockToken = apr_psprintf( mrSerfSession.getAprPool(), "(<%s>)",
                                 rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );

    mpProcImpl = createLockRefreshProcImpl( mPathStr,
                                            mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                            inLock,
                                            inLockToken,
                                            Timeout,
                                            outLock);
    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

//ULOCK unlock an existing lock
bool SerfRequestProcessor::processUnlock( const rtl::OUString & inDestinationPath,
                                        const com::sun::star::ucb::Lock& inLock,
                                        apr_status_t& outSerfStatus )
{
    mDestPathStr = apr_pstrdup( mrSerfSession.getAprPool(),
                                rtl::OUStringToOString( inDestinationPath, RTL_TEXTENCODING_UTF8 ).getStr() );

    char * aToken = apr_psprintf( mrSerfSession.getAprPool(), "<%s>",
                                 rtl::OUStringToOString(inLock.LockTokens[0], RTL_TEXTENCODING_UTF8 ).getStr() );

    mpProcImpl = createUnlockProcImpl( mPathStr,
                                       mrSerfSession.getRequestEnvironment().m_aRequestHeaders,
                                       inLock,
                                       aToken );

    outSerfStatus = runProcessor();

    return outSerfStatus == APR_SUCCESS;
}

apr_status_t SerfRequestProcessor::runProcessor()
{
    prepareProcessor();

    // activate chunked encoding, if requested
    if ( mbUseChunkedEncoding )
    {
        mpProcImpl->activateChunkedEncoding();
    }

    // create serf request
    OSL_ASSERT(mrSerfSession.getSerfConnection() != NULL);
    serf_connection_request_create( mrSerfSession.getSerfConnection(),
                                    Serf_SetupRequest,
                                    this );

    // perform serf request
    mbProcessingDone = false;
    apr_status_t status = APR_SUCCESS;
    serf_context_t* pSerfContext = mrSerfSession.getSerfContext();
    apr_pool_t* pAprPool = mrSerfSession.getAprPool();
    while ( true )
    {
        status = serf_context_run( pSerfContext,
                                   SERF_DURATION_FOREVER,
                                   pAprPool );
        if ( APR_STATUS_IS_TIMEUP( status ) )
        {
            continue;
        }
        if ( status != APR_SUCCESS )
        {
#if OSL_DEBUG_LEVEL > 0
            char buff[512];
            OSL_TRACE("SerfRequestProcessor::runProcessor, status != APR_SUCCESS: %d (%s)",status, apr_strerror(status, buff, 512));
#endif
            break;
        }
        if ( mbProcessingDone )
        {
            break;
        }
    }

    postprocessProcessor( status );

    return status;
}

void SerfRequestProcessor::postprocessProcessor( const apr_status_t inStatus )
{
    if ( inStatus == APR_SUCCESS )
    {
        return;
    }

    OSL_TRACE("SerfRequestProcessor::postprocessProcessor:%d",__LINE__);
    switch ( inStatus )
    {
    case APR_EGENERAL:
    case SERF_ERROR_AUTHN_FAILED:
        // general error; <mnHTTPStatusCode> provides more information
        {
            switch ( mnHTTPStatusCode )
            {
            case SC_NONE:
                if ( !mbSetupSerfRequestCalled )
                {
                    mpDAVException = new DAVException( DAVException::DAV_HTTP_LOOKUP,
                                                       SerfUri::makeConnectionEndPointString( mrSerfSession.getHostName(),
                                                                                              mrSerfSession.getPort() ) );
                }
                else if ( mbInputOfCredentialsAborted )
                {
                    mpDAVException = new DAVException( DAVException::DAV_HTTP_NOAUTH,
                                                       SerfUri::makeConnectionEndPointString( mrSerfSession.getHostName(),
                                                                                              mrSerfSession.getPort() ) );
                }
                else
                {
                    mpDAVException = new DAVException( DAVException::DAV_HTTP_ERROR,
                                                       mHTTPStatusCodeText,
                                                       mnHTTPStatusCode );
                }
                break;
            case SC_MOVED_PERMANENTLY:
            case SC_MOVED_TEMPORARILY:
            case SC_SEE_OTHER:
            case SC_TEMPORARY_REDIRECT:
                mpDAVException = new DAVException( DAVException::DAV_HTTP_REDIRECT,
                                                   mRedirectLocation );
                break;
            case SC_LOCKED:
                mpDAVException = new DAVException( DAVException::DAV_LOCKED,
                                                   mHTTPStatusCodeText,
                                                   mnHTTPStatusCode );
                break;
            default:
                mpDAVException = new DAVException( DAVException::DAV_HTTP_ERROR,
                                                   mHTTPStatusCodeText,
                                                   mnHTTPStatusCode );
                break;
            }
        }
        break;

    default:
        {
            // APR can help us describe this error
            char *buff = new char[512];
            apr_strerror(inStatus, buff, 512);
            rtl::OUString message(buff, strlen(buff), RTL_TEXTENCODING_DONTKNOW);
            delete[] buff;
            mpDAVException = new DAVException( DAVException::DAV_HTTP_ERROR, message );
        }
        break;
    }
}

apr_status_t SerfRequestProcessor::provideSerfCredentials( char ** outUsername,
                                                           char ** outPassword,
                                                           serf_request_t * inRequest,
                                                           int inCode,
                                                           const char *inAuthProtocol,
                                                           const char *inRealm,
                                                           apr_pool_t *inAprPool )
{
    // as each successful provided credentials are tried twice - see below - the
    // number of real attempts is half of the value of <mnSuccessfulCredentialAttempts>
    if ( (mnSuccessfulCredentialAttempts / 2) >= 5 ||
         mbInputOfCredentialsAborted )
    {
        mbInputOfCredentialsAborted = true;
        return SERF_ERROR_AUTHN_FAILED;
    }

    // because serf keeps credentials only for a connection in case of digest authentication
    // we give each successful provided credentials a second try in order to workaround the
    // situation that the connection for which the credentials have been provided has been closed
    // before the provided credentials could be applied for the request.
    apr_status_t status = mrSerfSession.provideSerfCredentials( (mnSuccessfulCredentialAttempts % 2) == 1,
                                                                outUsername,
                                                                outPassword,
                                                                inRequest,
                                                                inCode,
                                                                inAuthProtocol,
                                                                inRealm,
                                                                inAprPool );
    if ( status != APR_SUCCESS )
    {
        mbInputOfCredentialsAborted = true;
    }
    else
    {
        ++mnSuccessfulCredentialAttempts;
    }

    return status;
}

apr_status_t SerfRequestProcessor::setupSerfRequest( serf_request_t * inSerfRequest,
                                   serf_bucket_t ** outSerfRequestBucket,
                                   serf_response_acceptor_t * outSerfResponseAcceptor,
                                   void ** outSerfResponseAcceptorBaton,
                                   serf_response_handler_t * outSerfResponseHandler,
                                   void ** outSerfResponseHandlerBaton,
                                   apr_pool_t * /*inAprPool*/ )
{
    mbSetupSerfRequestCalled = true;
    *outSerfRequestBucket = mpProcImpl->createSerfRequestBucket( inSerfRequest );

    // apply callbacks for accepting response and handling response
    *outSerfResponseAcceptor = Serf_AcceptResponse;
    *outSerfResponseAcceptorBaton = this;
    *outSerfResponseHandler = Serf_HandleResponse;
    *outSerfResponseHandlerBaton = this;

    return APR_SUCCESS;
}

serf_bucket_t* SerfRequestProcessor::acceptSerfResponse( serf_request_t * inSerfRequest,
                                                         serf_bucket_t * inSerfStreamBucket,
                                                         apr_pool_t * inAprPool )
{
    mbAcceptSerfResponseCalled = true;
    return mrSerfSession.acceptSerfResponse( inSerfRequest,
                                             inSerfStreamBucket,
                                             inAprPool );
}

apr_status_t SerfRequestProcessor::handleSerfResponse( serf_request_t * inSerfRequest,
                                                       serf_bucket_t * inSerfResponseBucket,
                                                       apr_pool_t * inAprPool )
{
    mbHandleSerfResponseCalled = true;

    // some general response handling and error handling
    {
        if ( !inSerfResponseBucket )
        {
            /* A NULL response can come back if the request failed completely */
            mbProcessingDone = true;
            return APR_EGENERAL;
        }

        serf_status_line sl;
        apr_status_t status = serf_bucket_response_status( inSerfResponseBucket, &sl );
        if ( status )
        {
            mbProcessingDone = false; // allow another try in order to get a response
            return status;
        }
        serf_bucket_t *headers = serf_bucket_response_get_headers( inSerfResponseBucket );

        // check header according:
        // http://tools.ietf.org/html/rfc7231#section-7.4.2
        // need to do this so we can adjust the protocol accordingly
        // serf_bucket_headers_get is case independent
        const char* server = serf_bucket_headers_get( headers, "server" );
        if( server )
        {
            //update the server type on session
            mrSerfSession.setServerHeaderField( ::rtl::OUString::createFromAscii( server ) );
        }
        //the following extension is MS IIS specific,
        //see https://msdn.microsoft.com/en-us/library/cc250064.aspx
        //site last checked on 2015-03-02
        //TODO i126305 need to be added when serf is updated to a version supporting Windows authentication
        //const char* msDavExtErr = serf_bucket_headers_get( headers, "X-MSDAVEXT_ERROR" );

        // TODO - check, if response status code handling is correct
        mnHTTPStatusCode = ( sl.version != 0 && sl.code >= 0 )
                           ? static_cast< sal_uInt16 >( sl.code )
                           : SC_NONE;
        if ( sl.reason )
        {
            mHTTPStatusCodeText = ::rtl::OUString::createFromAscii( sl.reason );
        }
        if ( ( sl.version == 0 || sl.code < 0 ) ||
             mnHTTPStatusCode >= 300 )
        {
            if ( mnHTTPStatusCode == 301 ||
                 mnHTTPStatusCode == 302 ||
                 mnHTTPStatusCode == 303 ||
                 mnHTTPStatusCode == 307 )
            {
                // new location for certain redirections
                const char* location = serf_bucket_headers_get( headers, "Location" );
                if ( location )
                {
                    mRedirectLocation = rtl::OUString::createFromAscii( location );
                }
                mbProcessingDone = true;
                return APR_EGENERAL;
            }
            else if ( mrSerfSession.isHeadRequestInProgress() &&
                      ( mnHTTPStatusCode == 401 || mnHTTPStatusCode == 407 ) )
            {
                // keep going as authentication is not required on HEAD request.
                // the response already contains header fields.
            }
            else
            {
                mbProcessingDone = true;
                return APR_EGENERAL;
            }
        }
    }

    // request specific processing of the response bucket
    apr_status_t status = APR_SUCCESS;
    mbProcessingDone = mpProcImpl->processSerfResponseBucket( inSerfRequest,
                                                              inSerfResponseBucket,
                                                              inAprPool,
                                                              status );

    return status;
}

} // namespace http_dav_ucp

