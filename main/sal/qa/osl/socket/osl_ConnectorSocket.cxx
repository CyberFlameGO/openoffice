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
#include "precompiled_sal.hxx"

/**  test coder preface:
	1. the BSD socket function will meet "unresolved external symbol error" on Windows platform
	if you are not including ws2_32.lib in makefile.mk,  the including format will be like this:

	.IF "$(GUI)" == "WNT"
	SHL1STDLIBS +=	$(SOLARLIBDIR)$/cppunit.lib
	SHL1STDLIBS +=  ws2_32.lib
	.ENDIF

	likewise on Solaris platform.
	.IF "$(GUI)" == "UNX"
	SHL1STDLIBS+=$(SOLARLIBDIR)$/libcppunit$(DLLPOSTFIX).a
	SHL1STDLIBS += -lsocket -ldl -lnsl
	.ENDIF

	2. since the Socket implementation of osl is only IPv4 oriented, our test are mainly focus on IPv4
	category.

	3. some fragment of Socket source implementation are lack of comment so it is hard for testers
	guess what the exact functionality or usage of a member.  Hope the Socket section's comment
	will be added.

	4. following functions are declared but not implemented:
	inline sal_Bool SAL_CALL operator== (const SocketAddr & Addr) const;
 */

//------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------

#include <testshl/simpleheader.hxx>

#include "osl_Socket_Const.h"
#include "sockethelper.hxx"

using namespace osl;
using namespace rtl;

#define IP_PORT_MYPORT2 8883
#define IP_PORT_FTP     21
#define IP_PORT_MYPORT3 8884

//------------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------------

class CloseSocketThread : public Thread
{
	::osl::Socket &m_sSocket;
protected:
	void SAL_CALL run( )
	{
		thread_sleep( 1 );
		m_sSocket.close( );
	}
public:
	CloseSocketThread(::osl::Socket & sSocket )
		: m_sSocket( sSocket )
	{
	}

	~CloseSocketThread( )
	{
		if ( isRunning( ) )
		{
			t_print("# error: CloseSocketThread not terminated.\n" );
		}
	}
};

namespace osl_ConnectorSocket
{

	/** testing the method:
		ConnectorSocket(oslAddrFamily Family = osl_Socket_FamilyInet,
						oslProtocol	Protocol = osl_Socket_ProtocolIp,
						oslSocketType	Type = osl_Socket_TypeStream);
	*/

	class ctors : public CppUnit::TestFixture
	{
	public:
		void ctors_001()
		{
			/// Socket constructor.
			::osl::ConnectorSocket csSocket( osl_Socket_FamilyInet, osl_Socket_ProtocolIp, osl_Socket_TypeStream );

			CPPUNIT_ASSERT_MESSAGE( "test for ctors_001 constructor function: check if the connector socket was created successfully.",
									osl_Socket_TypeStream ==  csSocket.getType( ) );
		}

		CPPUNIT_TEST_SUITE( ctors );
		CPPUNIT_TEST( ctors_001 );
		CPPUNIT_TEST_SUITE_END();

	}; // class ctors

	/** testing the method:
		oslSocketResult SAL_CALL connect(const SocketAddr& TargetHost, const TimeValue* pTimeout = 0);
	*/

	class connect : public CppUnit::TestFixture
	{
	public:
		TimeValue *pTimeout;
		::osl::AcceptorSocket asAcceptorSocket;
		::osl::ConnectorSocket csConnectorSocket;


		// initialization
		void setUp( )
		{
			pTimeout  = ( TimeValue* )malloc( sizeof( TimeValue ) );
			pTimeout->Seconds = 3;
			pTimeout->Nanosec = 0;
		//	sHandle = osl_createSocket( osl_Socket_FamilyInet, osl_Socket_TypeStream, osl_Socket_ProtocolIp );
		}

		void tearDown( )
		{
			free( pTimeout );
		//	sHandle = NULL;
			asAcceptorSocket.close( );
			csConnectorSocket.close( );
		}


		void connect_001()
		{
			::osl::SocketAddr saLocalSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT2 );
			::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT2 );
			::osl::SocketAddr saPeerSocketAddr( rtl::OUString::createFromAscii("129.158.217.202"), IP_PORT_FTP );
			::osl::StreamSocket ssConnection;

			/// launch server socket
			asAcceptorSocket.setOption( osl_Socket_OptionReuseAddr, 1 ); //sal_True);
			sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
			CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket bind address failed.", sal_True == bOK1 );
			sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
			CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket listen failed.",  sal_True == bOK2 );

			//asAcceptorSocket.enableNonBlockingMode( sal_True );
			//oslSocketResult eResultAccept = asAcceptorSocket.acceptConnection(ssConnection); /// waiting for incoming connection...
			//CPPUNIT_ASSERT_MESSAGE( "accept failed.",  osl_Socket_Ok == eResultAccept );
			/// launch client socket
			oslSocketResult eResult = csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...
			CPPUNIT_ASSERT_MESSAGE( "connect failed.",  osl_Socket_Ok == eResult );

			/// get peer information
			csConnectorSocket.getPeerAddr( saPeerSocketAddr );/// connected.

			CPPUNIT_ASSERT_MESSAGE( "test for connect function: try to create a connection with remote host. and check the setup address.",
									( sal_True == compareSocketAddr( saPeerSocketAddr, saLocalSocketAddr ) ) &&
									( osl_Socket_Ok == eResult ));
		}
		//non-blocking mode connect?
		void connect_002()
		{
			::osl::SocketAddr saLocalSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
			::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
			::osl::SocketAddr saPeerSocketAddr( rtl::OUString::createFromAscii("129.158.217.202"), IP_PORT_FTP );

			asAcceptorSocket.setOption( osl_Socket_OptionReuseAddr, 1 ); //sal_True);
			asAcceptorSocket.enableNonBlockingMode( sal_True );
			sal_Bool bOK1 = asAcceptorSocket.bind( saLocalSocketAddr );
			CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket bind address failed.", sal_True == bOK1 );
			sal_Bool bOK2 = asAcceptorSocket.listen( 1 );
			CPPUNIT_ASSERT_MESSAGE( "AcceptorSocket listen failed.",  sal_True == bOK2 );

			csConnectorSocket.enableNonBlockingMode( sal_True );

			oslSocketResult eResult = csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...
			CPPUNIT_ASSERT_MESSAGE( "connect failed.",  osl_Socket_InProgress == eResult ||  osl_Socket_Ok == eResult );

			/// get peer information
			csConnectorSocket.getPeerAddr( saPeerSocketAddr );

			CPPUNIT_ASSERT_MESSAGE( "test for connect function: try to create a connection with remote host. and check the setup address.",
				sal_True == compareSocketAddr( saPeerSocketAddr, saLocalSocketAddr  )  ) ;
		}
		// really an error or just delayed
		// how to design senarios that will return osl_Socket_Interrupted, osl_Socket_TimedOut
		void connect_003()
		{
			::osl::SocketAddr saTargetSocketAddr1( rtl::OUString::createFromAscii("127.0.0.1"), IP_PORT_MYPORT3 );
			::osl::SocketAddr saTargetSocketAddr2( rtl::OUString::createFromAscii("123.345.67.89"), IP_PORT_MYPORT3 );

			csConnectorSocket.enableNonBlockingMode( sal_False );

			oslSocketResult eResult1 = csConnectorSocket.connect( saTargetSocketAddr1, pTimeout );
			oslSocketResult eResult2 = csConnectorSocket.connect( saTargetSocketAddr2, pTimeout );
			CloseSocketThread myCloseThread( csConnectorSocket );
			oslSocketResult eResult3 = csConnectorSocket.connect( saTargetSocketAddr2, pTimeout );
			myCloseThread.join();
			CPPUNIT_ASSERT_MESSAGE( "connect should failed.",  osl_Socket_Error == eResult1 &&
				osl_Socket_Error == eResult2 &&  osl_Socket_Error == eResult3 );

		}

		// really an error in non-blocking mode
		void connect_004()
		{
			::osl::SocketAddr saTargetSocketAddr( rtl::OUString::createFromAscii("123.345.67.89"), IP_PORT_MYPORT3 );

			csConnectorSocket.enableNonBlockingMode( sal_True );

			oslSocketResult eResult = csConnectorSocket.connect( saTargetSocketAddr, pTimeout );   /// connecting to server...
			CPPUNIT_ASSERT_MESSAGE( "connect should failed.",  osl_Socket_Error == eResult );
		}
		/** here need a case: immediate connection, say in non-blocking mode connect return osl_Socket_Ok
		*/

		CPPUNIT_TEST_SUITE( connect );
		CPPUNIT_TEST( connect_001 );
		CPPUNIT_TEST( connect_002 );
		CPPUNIT_TEST( connect_003 );
		CPPUNIT_TEST( connect_004 );
		CPPUNIT_TEST_SUITE_END();

	}; // class connect


// -----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_ConnectorSocket::ctors, "osl_ConnectorSocket");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(osl_ConnectorSocket::connect, "osl_ConnectorSocket");

} // namespace osl_ConnectorSocket

// -----------------------------------------------------------------------------

// this macro creates an empty function, which will called by the RegisterAllFunctions()
// to let the user the possibility to also register some functions by hand.
NOADDITIONAL;
