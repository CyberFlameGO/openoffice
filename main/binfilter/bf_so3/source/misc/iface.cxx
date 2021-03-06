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



#include <string.h>
#include <stdio.h>


#include <bf_so3/iface.hxx>
#include <sot/agg.hxx>
#include <bf_svtools/ownlist.hxx>

#include "bf_so3/soerr.hxx"

#ifndef _DEBUG_HXX //autogen
#include <tools/debug.hxx>
#endif

namespace binfilter {

/************** class SvObject ******************************************/
SV_IMPL_FACTORY(SvObjectFactory)
	{
	}
};
TYPEINIT1(SvObjectFactory,SvFactory);


SO2_IMPL_CLASS1_DLL(SvObject,SvObjectFactory,SotObject,
				   SvGlobalName( 0x7F7E0E60L, 0xC32D, 0x101B,
							0x80, 0x4C, 0x04, 0x02, 0x1C, 0x00, 0x70, 0x02 ) )

/*************************************************************************
|*  SvObject::GetMemberInterface()
|*
|*	Beschreibung:
*************************************************************************/
::IUnknown * SvObject::GetMemberInterface( const SvGlobalName & )
{
	return NULL;
}

/*************************************************************************
|*  SvObject::TestMemberObjRef()
|*
|*	Beschreibung:
*************************************************************************/
void SvObject::TestMemberObjRef( BOOL /*bFree*/ )
{
}

/*************************************************************************
|*  SvObject::TestMemberObjRef()
|*
|*	Beschreibung:
*************************************************************************/

#ifdef TEST_INVARIANT
void SvObject::TestMemberInvariant( BOOL /*bPrint*/ )
{
#ifdef DBG_UTIL
	if( !Owner() && pClient )
	{
		ByteString aTest( "\t\tpClient == " );
		aTest += ByteString::CreateFromInt32( (ULONG)pClient );
		DBG_TRACE( aTest.GetBuffer() );
	}
	if( Owner() && pService )
	{
		ByteString aTest( "\t\tpService == " );
		aTest += ByteString::CreateFromInt32( (ULONG)pService );
		DBG_TRACE( aTest.GetBuffer() );
	}
#endif
}
#endif

/*************************************************************************
|*    SvObject::SvObject()
|*
|*    Beschreibung
*************************************************************************/
SvObject::SvObject()
	: pObj        ( NULL ) // union mit pClient, pService
	, nExtCount   ( 0 )
{
}

/*************************************************************************
|*
|*    SvObject::~SvObject()
|*
|*    Beschreibung
|*    Ersterstellung    MM 05.06.94
|*    Letzte Aenderung  MM 05.06.94
|*
*************************************************************************/
SvObject::~SvObject()
{
}

/*************************************************************************
|*
|*    SvObject::ReleaseRef()
|*
|*    Beschreibung
|*    Ersterstellung    MM 05.06.94
|*    Letzte Aenderung  MM 05.06.94
|*
*************************************************************************/
UINT32 SvObject::ReleaseRef()
{
#ifdef DBG_UTIL
	if( GetRefCount() == nExtCount )
	{
		ByteString aStr( "Internal RefCount underflow: Count == " );
		aStr += ByteString::CreateFromInt32( GetRefCount() );
		aStr += ", ExtCount == ";
		aStr += ByteString::CreateFromInt32( nExtCount );
		DBG_ERROR( aStr.GetBuffer() );
	}
#endif
	return SotObject::ReleaseRef();
}

//========================================================================
void SvObject::MakeUnknown()
/* [Beschreibung]

	Klassen, die immer extern sind, aber nicht sofort <SvObject::Init>
	rufen, m"ussen diese Methode "uberladen.
	Darin muss das externe Objekt erzeugt und mit Init eingesetzt werden.
    Im Konstruktor muss das Objekt als extern gekennzeichnet werden.

	[Beispiel]

	void MyClass::MyClass()
	{
		SetExtern();
	}

	void MyClass::MakeUnknown()
	{
		CreateBindCtx( 0, &pObjI );
		Init( pObjI );
	}
*/
{
}



#if defined (_INC_WINDOWS) || defined (_WINDOWS_)
RECT GetSysRect( const Rectangle & rRect )
{
	RECT aRect;
	aRect.top    = (int)rRect.Top();
	aRect.left   = (int)rRect.Left();
	if( rRect.Right() == RECT_EMPTY )
		aRect.right  = aRect.left;
	else
		aRect.right  = (int)(rRect.Right() +1);
	if( rRect.Bottom() == RECT_EMPTY )
		aRect.bottom = aRect.top;
	else
		aRect.bottom = (int)(rRect.Bottom() +1);
	return aRect;
}
Rectangle GetSvRect( const RECT & rRect )
{
	Rectangle aRect;
	aRect.Top()     = rRect.top;
	aRect.Left()    = rRect.left;
	if( rRect.right != rRect.left )
		aRect.Right()   = rRect.right -1;
	if( rRect.bottom != rRect.top )
		aRect.Bottom()  = rRect.bottom -1;
	return aRect;
}
#endif

}
