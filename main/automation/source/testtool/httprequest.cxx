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
#include "precompiled_automation.hxx"

#include <stdio.h>
#include "httprequest.hxx"
#include <vos/socket.hxx>
#include <tools/debug.hxx>


void HttpRequest::Init()
{
    nResultId = 0;
    aHeader.Erase();
    aContentType.Erase();
    delete pStream;
    pStream = NULL;
}

HttpRequest::HttpRequest()
: nStatus( HTTP_INIT )
, nResultId( 0 )
, pStream( NULL )
{}

HttpRequest::~HttpRequest()
{
    delete pStream;
    pStream = NULL;
}

void HttpRequest::SetRequest( ByteString aHost, ByteString aPath, sal_uInt16 nPort )
{
    nStatus = HTTP_REQUEST_SET;
    Init();
    aRequestHost = aHost;
    aRequestPath = aPath;
    nRequestPort = nPort;
}

void HttpRequest::SetProxy( ByteString aHost, sal_uInt16 nPort )
{
    nStatus = HTTP_REQUEST_SET;
    Init();
    aProxyHost = aHost;
    nProxyPort = nPort;
}

sal_Bool HttpRequest::Execute()
{
    nStatus = HTTP_REQUEST_PENDING;
    Init();

    // Open channel to standard redir host
	vos::OInetSocketAddr aConnectAddr;

    if ( aProxyHost.Len() )
    {
	    aConnectAddr.setAddr( rtl::OUString( UniString( aProxyHost, RTL_TEXTENCODING_UTF8 ) ) );
	    aConnectAddr.setPort( nProxyPort );
    }
    else
    {
	    aConnectAddr.setAddr( rtl::OUString( UniString( aRequestHost, RTL_TEXTENCODING_UTF8 ) ) );
	    aConnectAddr.setPort( nRequestPort );
    }

	TimeValue aTV;
	aTV.Seconds = 10;		// Warte 10 Sekunden
	aTV.Nanosec = 0;

    pOutSocket = new vos::OConnectorSocket();
	if ( pOutSocket->connect( aConnectAddr, &aTV ) == vos::ISocketTypes::TResult_Ok )
	{
//		pOutSocket->setTcpNoDelay( 1 );
	}
	else
    {
		delete pOutSocket;
        nStatus = HTTP_REQUEST_ERROR;
        return sal_False;
    }


    SendString( pOutSocket, "GET " );
    if ( aProxyHost.Len() )
    {
        //GET http://staroffice-doc.germany.sun.com/cgi-bin/htdig/binarycopy.sh?CopyIt=++CopyIt++ HTTP/1.0
        SendString( pOutSocket, "http://" );
        SendString( pOutSocket, aRequestHost );
        SendString( pOutSocket, ":" );
        SendString( pOutSocket, ByteString::CreateFromInt32( nRequestPort ) );
        SendString( pOutSocket, aRequestPath );
        SendString( pOutSocket, " HTTP/1.0\n" );

        SendString( pOutSocket, "Proxy-Connection: Keep-Alive\n" );
    }
    else
    {
        //GET /cgi-bin/htdig/binarycopy.sh?CopyIt=++CopyIt++ HTTP/1.0
        SendString( pOutSocket, aRequestPath );
        SendString( pOutSocket, " HTTP/1.0\n" );

        SendString( pOutSocket, "Connection: Keep-Alive\n" );
    }
        
    SendString( pOutSocket, "User-Agent: Mozilla/4.7 [de] (Linux; I)" );
    SendString( pOutSocket, "Host: " );
    SendString( pOutSocket, aRequestHost );
    // Terminate with empty line
    SendString( pOutSocket, "\n\n" );



#define BUFFRE_SIZE 0x10000    // 64K Buffer
    char* pBuffer = new char[ BUFFRE_SIZE ];

    sal_Bool bWasError = ( nStatus != HTTP_REQUEST_PENDING );

    sal_uLong nDataRead;
    pStream = new SvMemoryStream( 0x10000, 0x10000 );
    while ( !bWasError )
    {
	    bWasError |= ( BUFFRE_SIZE != ( nDataRead = pOutSocket->read( pBuffer, BUFFRE_SIZE ) ) );

        pStream->Write( pBuffer, nDataRead );
    }

    delete [] pBuffer;
	pOutSocket->shutdown();
	pOutSocket->close();

    pStream->Seek( 0 );
    
    ByteString aLine;
    sal_Bool bInsideHeader = sal_True;
    while ( bInsideHeader )
    {
        pStream->ReadLine( aLine );
        if ( !aLine.Len() )
            bInsideHeader = sal_False;
        else
        {
            if ( IsItem( "HTTP/", aLine ) )
                nResultId = (sal_uInt16)aLine.GetToken( 1, ' ' ).ToInt32();
            if ( IsItem( "Content-Type:", aLine ) )
            {
                aContentType = aLine.Copy( 13 );
                aContentType.EraseLeadingAndTrailingChars();
            }
            aHeader += aLine;
            aHeader += "\n";
        }
    }

    if ( nStatus == HTTP_REQUEST_PENDING )
    {
        nStatus = HTTP_REQUEST_DONE;
        return sal_True;
    }
    else
    {
        nStatus = HTTP_REQUEST_ERROR;
        return sal_False;
    }
}
/*
HTTP/1.1 200 OK
Date: Tue, 22 Jan 2002 14:16:20 GMT
Server: Apache/1.3.14 (Unix)  (SuSE/Linux) mod_throttle/3.0 mod_layout/1.0 mod_f
astcgi/2.2.2 mod_jk
Set-Cookie2: JSESSIONID=ffkpgb7tm1;Version=1;Discard;Path="/bugtracker"
Set-Cookie: JSESSIONID=ffkpgb7tm1;Path=/bugtracker
Servlet-Engine: Tomcat Web Server/3.2.1 (JSP 1.1; Servlet 2.2; Java 1.3.0; Linux
 2.4.0-4GB i386; java.vendor=Sun Microsystems Inc.)
Connection: close
Content-Type: text/xml; charset=ISO-8859-1
  */
void HttpRequest::SendString( vos::OStreamSocket* pSocket , ByteString aText )
{
    if ( nStatus == HTTP_REQUEST_PENDING )
        pSocket->write( aText.GetBuffer(), aText.Len() );
}

sal_Bool HttpRequest::IsItem( ByteString aItem, ByteString aLine )
{
    return aItem.Match( aLine ) == STRING_MATCH;
}


void HttpRequest::Abort()
{
    if ( pOutSocket )
    {
        nStatus = HTTP_REQUEST_ERROR;
      	pOutSocket->shutdown();
    	pOutSocket->close();
    }
}




SvMemoryStream* HttpRequest::GetBody()
{
    return pStream;
}

sal_uInt16 HttpRequest::GetStatus()
{
    return nStatus;
}

