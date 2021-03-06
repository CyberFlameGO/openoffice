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
#include "precompiled_svtools.hxx"

#ifndef INCLUDED_RTL_MATH_HXX
#include <rtl/math.hxx>
#endif

#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <vector>
using namespace std;

#include <vcl/svapp.hxx>

#include "svl/solar.hrc"
#include <svtools/filedlg.hxx>
#include "bmpcore.hxx"
#include "bmp.hrc"

// ----------
// - BmpApp -
// ----------

class BmpApp : public BmpCreator
{
private:

	String	        aOutputFileName;
	sal_uInt8	        cExitCode;
                    
	sal_Bool	        GetCommandOption( const ::std::vector< String >& rArgs, const String& rSwitch, String& rSwitchParam );
	sal_Bool	        GetCommandOptions( const ::std::vector< String >& rArgs, const String& rSwitch, ::std::vector< String >& rSwitchParams );

    void	        SetExitCode( sal_uInt8 cExit )
			        {
			        	if( ( EXIT_NOERROR == cExitCode ) || ( cExit != EXIT_NOERROR ) )
			        		cExitCode = cExit;
			        }
    void            ShowUsage();

    virtual void    Message( const String& rText, sal_uInt8 cExitCode );

public:

                    BmpApp();
                    ~BmpApp();

    int             Start( const ::std::vector< String >& rArgs );
};

// -----------------------------------------------------------------------------

BmpApp::BmpApp()
{
}

// -----------------------------------------------------------------------------

BmpApp::~BmpApp()
{
}

// -----------------------------------------------------------------------

sal_Bool BmpApp::GetCommandOption( const ::std::vector< String >& rArgs, const String& rSwitch, String& rParam )
{
	sal_Bool bRet = sal_False;

	for( int i = 0, nCount = rArgs.size(); ( i < nCount ) && !bRet; i++ )
	{
		String	aTestStr( '-' );

		for( int n = 0; ( n < 2 ) && !bRet; n++ )
		{
			aTestStr += rSwitch;

			if( aTestStr.CompareIgnoreCaseToAscii( rArgs[ i ] ) == COMPARE_EQUAL )
			{
				bRet = sal_True;

				if( i < ( nCount - 1 ) )
					rParam = rArgs[ i + 1 ];
				else
					rParam = String();
			}

			if( 0 == n )
				aTestStr = '/';
		}
	}

	return bRet;
}

// -----------------------------------------------------------------------

sal_Bool BmpApp::GetCommandOptions( const ::std::vector< String >& rArgs, const String& rSwitch, ::std::vector< String >& rParams )
{
	sal_Bool bRet = sal_False;

	for( int i = 0, nCount = rArgs.size(); ( i < nCount ); i++ )
	{
		String	aTestStr( '-' );

		for( int n = 0; ( n < 2 ) && !bRet; n++ )
		{
			aTestStr += rSwitch;

			if( aTestStr.CompareIgnoreCaseToAscii( rArgs[ i ] ) == COMPARE_EQUAL )
			{
				if( i < ( nCount - 1 ) )
					rParams.push_back( rArgs[ i + 1 ] );
				else
					rParams.push_back( String() );
			
			    break;
			}

			if( 0 == n )
				aTestStr = '/';
		}
	}

	return( rParams.size() > 0 );
}

// -----------------------------------------------------------------------

void BmpApp::Message( const String& rText, sal_uInt8 cExit )
{
	if( EXIT_NOERROR != cExit )
		SetExitCode( cExit );

	ByteString aText( rText, RTL_TEXTENCODING_UTF8 );
	aText.Append( "\r\n" );
	fprintf( stderr, aText.GetBuffer() );
}

// -----------------------------------------------------------------------------

void BmpApp::ShowUsage()
{   
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "Usage:" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "    bmp srs_inputfile output_dir lang_dir lang_num -i input_dir [-i input_dir ][-f err_file]" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "Options:" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "   -i ...        name of directory to be searched for input files [multiple occurence is possible]" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "   -f            name of file, output should be written to" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "Examples:" ) ), EXIT_NOERROR );
    Message( String( RTL_CONSTASCII_USTRINGPARAM( "    bmp /home/test.srs /home/out enus 01 -i /home/res -f /home/out/bmp.err" ) ), EXIT_NOERROR );
}

// -----------------------------------------------------------------------------

int BmpApp::Start( const ::std::vector< String >& rArgs )
{
	String aOutName;

	cExitCode = EXIT_NOERROR;

	if( rArgs.size() >= 6 )
	{
		LangInfo                aLangInfo;
    	sal_uInt16	                nCurCmd = 0;
        const String            aSrsName( rArgs[ nCurCmd++ ] );
        ::std::vector< String > aInDirVector;
	    ByteString	            aLangDir;
        
        aOutName = rArgs[ nCurCmd++ ];
		
		aLangDir = ByteString( rArgs[ nCurCmd++ ], RTL_TEXTENCODING_ASCII_US );
        aLangInfo.mnLangNum = static_cast< sal_uInt16 >( rArgs[ nCurCmd++ ].ToInt32() );

        memcpy( aLangInfo.maLangDir, aLangDir.GetBuffer(), aLangDir.Len() + 1 );

		GetCommandOption( rArgs, 'f', aOutputFileName );
		GetCommandOptions( rArgs, 'i', aInDirVector );

        Create( aSrsName, aInDirVector, aOutName, aLangInfo );
	}
    else
    {
        ShowUsage();
        cExitCode = EXIT_COMMONERROR;
    }
    
    if( ( EXIT_NOERROR == cExitCode ) && aOutputFileName.Len() && aOutName.Len() )
	{
		SvFileStream	aOStm( aOutputFileName, STREAM_WRITE | STREAM_TRUNC );
		ByteString		aStr( "Successfully generated ImageList(s) in: " );

		aOStm.WriteLine( aStr.Append( ByteString( aOutName, RTL_TEXTENCODING_UTF8 ) ) );
		aOStm.Close();
	}

    return cExitCode;
}

// --------
// - Main -
// --------

int main( int nArgCount, char* ppArgs[] )
{
#ifdef UNX
    static char aDisplayVar[ 1024 ];

    strcpy( aDisplayVar, "DISPLAY=" );
    putenv( aDisplayVar );
#endif
    
    ::std::vector< String > aArgs;
    BmpApp                  aBmpApp;

    InitVCL( com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >() );

    for( int i = 1; i < nArgCount; i++ )
        aArgs.push_back( String( ppArgs[ i ], RTL_TEXTENCODING_ASCII_US ) );
    
    return aBmpApp.Start( aArgs );
}
