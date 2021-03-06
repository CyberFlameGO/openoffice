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
#include "precompiled_dtrans.hxx"


//_________________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________________
#include <cppuhelper/servicefactory.hxx>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/datatransfer/XMimeContentType.hpp>
#include <com/sun/star/datatransfer/XMimeContentTypeFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <rtl/ustring.hxx>
#include <sal/types.h>
#include <osl/diagnose.h>

#include <stdio.h>

#include <vector>

//-------------------------------------------------------------
// my defines
//-------------------------------------------------------------

#define TEST_CLIPBOARD
#define RDB_SYSPATH  "d:\\projects\\src621\\dtrans\\wntmsci7\\bin\\applicat.rdb"

//------------------------------------------------------------
//	namesapces
//------------------------------------------------------------

using namespace	::rtl;
using namespace ::std;
using namespace ::cppu;
using namespace ::com::sun::star::datatransfer;
using namespace ::com::sun::star::uno;
using namespace	::com::sun::star::lang;
using namespace ::com::sun::star::container;

//------------------------------------------------------------
//	globales
//------------------------------------------------------------

//################################################################

/*
void CheckMimeContentType( const OUString& aCntType, const OUString& aType, const OUString& aSubtype, sal_Int32 nParams )
{

	Reference< XMimeContentType > xMimeCntType = xMCntTypeFactory->createMimeContentType( aCntType );

	OSL_ASSERT( aType    == xMimeCntType->getMediaType ( ) );
	OSL_ASSERT( aSubtype == xMimeCntType->getMediaSubtype ( ) );
	
	try
	{
		Sequence< OUString > seqParam = xMimeCntType->getParameters( );
		OSL_ASSERT( seqParam.getLength( ) == nParams );

		OUString param;
		OUString pvalue;
		for ( sal_Int32 i = 0; i < seqParam.getLength( ); i++ )
		{
			param  = seqParam[i];
			OSL_ASSERT( xMimeCntType->hasParameter( param ) );

			pvalue = xMimeCntType->getParameterValue( param );
		}

		pvalue = xMimeCntType->getParameterValue( OUString::createFromAscii( "aparam" ) );
	}
	catch( IllegalArgumentException& )
	{
		printf( "FAILED: Invalid Mime Contenttype detected\n" );
	}
	catch( NoSuchElementException& )
	{
		
	}
}
*/

//----------------------------------------------------------------
//	
//----------------------------------------------------------------

void ShutdownServiceMgr( Reference< XMultiServiceFactory >& SrvMgr )
{
	// Cast factory to XComponent
	Reference< XComponent > xComponent( SrvMgr, UNO_QUERY );

	if ( !xComponent.is() )
		OSL_ENSURE(sal_False, "Error shuting down");
	
	// Dispose and clear factory
	xComponent->dispose();
	SrvMgr.clear();
	SrvMgr = Reference< XMultiServiceFactory >();
}

//----------------------------------------------------------------
//	
//----------------------------------------------------------------

sal_Bool readCntTypesFromFileIntoVector( char* fname, vector< string >& vecData )
{
	FILE* fstream;

	fstream = fopen( fname, "r+" );
	if ( !fstream )
		return sal_False;

	// set pointer to file start
	fseek( fstream, 0L, SEEK_SET );

	char line[1024];
	while ( fscanf( fstream, "%[^\n]s", line ) != EOF )
	{		
		vecData.push_back( line );
		fgetc( fstream );
	}

	fclose( fstream );

	return sal_True;
}

//----------------------------------------------------------------
//	
//----------------------------------------------------------------

sal_Bool processCntTypesAndWriteResultIntoFile( char* fname, vector< string >& vecData, Reference< XMimeContentTypeFactory > cnttypeFactory )
{
	FILE* fstream;

	fstream = fopen( fname, "w" );
	if ( !fstream )
		return sal_False;

	// set pointer to file start
	fseek( fstream, 0L, SEEK_SET );

	vector< string >::iterator iter_end = vecData.end( );
	const char* pStr = NULL;

	for ( vector< string >::iterator iter = vecData.begin( ); iter != iter_end; ++iter )
	{	
		try
		{
			fprintf( fstream, "Gelesen: %s\n", iter->c_str( ) );

			Reference< XMimeContentType > xMCntTyp = cnttypeFactory->createMimeContentType( OUString::createFromAscii( iter->c_str( ) ) );

			fwprintf( fstream, OUString::createFromAscii( "Type: %s\n" ),  xMCntTyp->getMediaType( ).getStr( ) );
			fwprintf( fstream, OUString::createFromAscii( "Subtype: %s\n" ), xMCntTyp->getMediaSubtype( ).getStr( ) );

			Sequence< OUString > seqParam = xMCntTyp->getParameters( );
			sal_Int32 nParams = seqParam.getLength( );

			for ( sal_Int32 i = 0; i < nParams; i++ )
			{
				fwprintf( fstream, OUString::createFromAscii("PName: %s\n" ), seqParam[i].getStr( ) );
				fwprintf( fstream, OUString::createFromAscii("PValue: %s\n" ), xMCntTyp->getParameterValue( seqParam[i] ).getStr( ) );
			}			
		}
		catch( IllegalArgumentException& ex )
		{
			fwprintf( fstream, OUString::createFromAscii( "Fehlerhafter Content-Type gelesen!!!\n\n" ) ); 
		}
		catch( NoSuchElementException& )
		{		
			fwprintf( fstream, OUString::createFromAscii( "Parameterwert nicht vorhanden\n" ) );
		}
		catch( ... )
		{
			fwprintf( fstream, OUString::createFromAscii( "Unbekannter Fehler!!!\n\n" ) );
		}

		fwprintf( fstream, OUString::createFromAscii( "\n#############################################\n\n" ) );
	}

	fclose( fstream );
	
	return sal_True;
}

//----------------------------------------------------------------
//	main
//----------------------------------------------------------------

int SAL_CALL main( int nArgc, char* argv[] )
{
	if ( nArgc != 3 )
		printf( "Start with: testcnttype input-file output-file\n" );

	//-------------------------------------------------
	// get the global service-manager
	//-------------------------------------------------
	OUString rdbName = OUString( RTL_CONSTASCII_USTRINGPARAM( RDB_SYSPATH ) );
	Reference< XMultiServiceFactory > g_xFactory( createRegistryServiceFactory( rdbName ) );

	// Print a message if an error occured.
	if ( !g_xFactory.is( ) )
	{
		OSL_ENSURE(sal_False, "Can't create RegistryServiceFactory");
		return(-1);
	}

	vector< string > vecCntTypes;

	// open input-file and read the data
	if ( !readCntTypesFromFileIntoVector( argv[1], vecCntTypes ) )
	{
		printf( "Can't open input file" );
		ShutdownServiceMgr( g_xFactory );
	}

	Reference< XMimeContentTypeFactory > 
		xMCntTypeFactory( g_xFactory->createInstance( OUString::createFromAscii( "com.sun.star.datatransfer.MimeContentTypeFactory" ) ), UNO_QUERY );

	if ( !xMCntTypeFactory.is( ) )
	{
		OSL_ENSURE( sal_False, "Error creating MimeContentTypeFactory Service" );
		return(-1);
	}

	if ( !processCntTypesAndWriteResultIntoFile( argv[2], vecCntTypes, xMCntTypeFactory ) )
	{
		printf( "Can't open output file" );
		ShutdownServiceMgr( g_xFactory );
	}

	//--------------------------------------------------
	// shutdown the service manager
	//--------------------------------------------------

	ShutdownServiceMgr( g_xFactory );

	return 0;	
}
