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



#ifndef _ERRCODE_HXX //autogen
#include <tools/errcode.hxx>
#endif
#include "sbx.hxx"
#include "sbxconv.hxx"
#include "sbxres.hxx"

namespace binfilter {

// AB 29.10.99 Unicode
#ifndef _USE_NO_NAMESPACE
using namespace rtl;
#endif


// Die Konversion eines Items auf String wird ueber die Put-Methoden
// der einzelnen Datentypen abgewickelt, um doppelten Code zu vermeiden.

XubString ImpGetString( const SbxValues* p )
{
	SbxValues aTmp;
	XubString aRes;
	aTmp.eType = SbxSTRING;
	aTmp.pString = &aRes;
	switch( +p->eType )
	{
		case SbxNULL:
			SbxBase::SetError( SbxERR_CONVERSION );
		case SbxEMPTY:
			break;
		case SbxCHAR:
			ImpPutChar( &aTmp, p->nChar ); break;
		case SbxBYTE:
			ImpPutByte( &aTmp, p->nByte ); break;
		case SbxINTEGER:
			ImpPutInteger( &aTmp, p->nInteger ); break;
		case SbxBOOL:
			ImpPutBool( &aTmp, p->nUShort ); break;
		case SbxUSHORT:
			ImpPutUShort( &aTmp, p->nUShort ); break;
		case SbxLONG:
			ImpPutLong( &aTmp, p->nLong ); break;
		case SbxULONG:
			ImpPutULong( &aTmp, p->nULong ); break;
		case SbxSINGLE:
			ImpPutSingle( &aTmp, p->nSingle ); break;
		case SbxDOUBLE:
			ImpPutDouble( &aTmp, p->nDouble ); break;
		case SbxCURRENCY:
			ImpPutCurrency( &aTmp, p->nLong64 ); break;
		case SbxDECIMAL:
		case SbxBYREF | SbxDECIMAL:
			ImpPutDecimal( &aTmp, p->pDecimal ); break;
		case SbxSALINT64:
			ImpPutInt64( &aTmp, p->nInt64 ); break;
		case SbxSALUINT64:
			ImpPutUInt64( &aTmp, p->uInt64 ); break;
		case SbxBYREF | SbxSTRING:
		case SbxSTRING:
		case SbxLPSTR:
			if( p->pString )
				aRes = *p->pString;
			break;
		case SbxOBJECT:
		{
			SbxValue* pVal = PTR_CAST(SbxValue,p->pObj);
			if( pVal )
				aRes = pVal->GetString();
			else
				SbxBase::SetError( SbxERR_NO_OBJECT );
			break;
		}
		case SbxERROR:
			// Hier wird der String "Error n" erzeugt
			aRes = SbxRes( STRING_ERRORMSG );
			aRes += p->nUShort; break;
		case SbxDATE:
			ImpPutDate( &aTmp, p->nDouble ); break;

		case SbxBYREF | SbxCHAR:
			ImpPutChar( &aTmp, *p->pChar ); break;
		case SbxBYREF | SbxBYTE:
			ImpPutByte( &aTmp, *p->pByte ); break;
		case SbxBYREF | SbxINTEGER:
		case SbxBYREF | SbxBOOL:
			ImpPutInteger( &aTmp, *p->pInteger ); break;
		case SbxBYREF | SbxLONG:
			ImpPutLong( &aTmp, *p->pLong ); break;
		case SbxBYREF | SbxULONG:
			ImpPutULong( &aTmp, *p->pULong ); break;
		case SbxBYREF | SbxERROR:
		case SbxBYREF | SbxUSHORT:
			ImpPutUShort( &aTmp, *p->pUShort ); break;
		case SbxBYREF | SbxSINGLE:
			ImpPutSingle( &aTmp, *p->pSingle ); break;
		case SbxBYREF | SbxDATE:
		case SbxBYREF | SbxDOUBLE:
			ImpPutDouble( &aTmp, *p->pDouble ); break;
		case SbxBYREF | SbxCURRENCY:
			ImpPutCurrency( &aTmp, *p->pLong64 ); break;
		case SbxBYREF | SbxSALINT64:
			ImpPutInt64( &aTmp, *p->pnInt64 ); break;
		case SbxBYREF | SbxSALUINT64:
			ImpPutUInt64( &aTmp, *p->puInt64 ); break;
		default:
			SbxBase::SetError( SbxERR_CONVERSION );
	}
	return aRes;
}

// AB 10.4.97, neue Funktion fuer SbxValue::GetCoreString()
XubString ImpGetCoreString( const SbxValues* p )
{
	// Vorerst nur fuer double
	if( ( p->eType & (~SbxBYREF) ) == SbxDOUBLE )
	{
		SbxValues aTmp;
		XubString aRes;
		aTmp.eType = SbxSTRING;
		aTmp.pString = &aRes;
		if( p->eType == SbxDOUBLE )
			ImpPutDouble( &aTmp, p->nDouble, /*bCoreString=*/TRUE );
		else
			ImpPutDouble( &aTmp, *p->pDouble, /*bCoreString=*/TRUE );
		return aRes;
	}
	else
		return ImpGetString( p );
}

void ImpPutString( SbxValues* p, const XubString* n )
{
	SbxValues aTmp;
	aTmp.eType = SbxSTRING;
	XubString* pTmp = NULL;
	// Sicherheitshalber, falls ein NULL-Ptr kommt
	if( !n )
		n = pTmp = new XubString;
	aTmp.pString = (XubString*) n;
	switch( +p->eType )
	{
		case SbxCHAR:
			p->nChar = ImpGetChar( &aTmp ); break;
		case SbxBYTE:
			p->nByte = ImpGetByte( &aTmp ); break;
		case SbxINTEGER:
		case SbxBOOL:
			p->nInteger = ImpGetInteger( &aTmp ); break;
		case SbxLONG:
			p->nLong = ImpGetLong( &aTmp ); break;
		case SbxULONG:
			p->nULong = ImpGetULong( &aTmp ); break;
		case SbxERROR:
		case SbxUSHORT:
			p->nUShort = ImpGetUShort( &aTmp ); break;
		case SbxSINGLE:
			p->nSingle = ImpGetSingle( &aTmp ); break;
		case SbxDATE:
			p->nDouble = ImpGetDate( &aTmp ); break;
		case SbxDOUBLE:
			p->nDouble = ImpGetDouble( &aTmp ); break;
		case SbxULONG64:
			p->nLong64 = ImpGetCurrency( &aTmp ); break;
		case SbxDECIMAL:
		case SbxBYREF | SbxDECIMAL:
			releaseDecimalPtr( p->pDecimal );
			p->pDecimal = ImpGetDecimal( &aTmp ); break;
		case SbxSALINT64:
			p->nInt64 = ImpGetInt64( &aTmp ); break;
		case SbxSALUINT64:
			p->uInt64 = ImpGetUInt64( &aTmp ); break;

		case SbxBYREF | SbxSTRING:
		case SbxSTRING:
		case SbxLPSTR:
			if( n->Len() )
			{
				if( !p->pString )
					p->pString = new XubString;
				*p->pString = *n;
			}
			else
				delete p->pString, p->pString = NULL;
			break;
		case SbxOBJECT:
		{
			SbxValue* pVal = PTR_CAST(SbxValue,p->pObj);
			if( pVal )
				pVal->PutString( *n );
			else
				SbxBase::SetError( SbxERR_NO_OBJECT );
			break;
		}
		case SbxBYREF | SbxCHAR:
			*p->pChar = ImpGetChar( p ); break;
		case SbxBYREF | SbxBYTE:
			*p->pByte = ImpGetByte( p ); break;
		case SbxBYREF | SbxINTEGER:
			*p->pInteger = ImpGetInteger( p ); break;
		case SbxBYREF | SbxBOOL:
			*p->pUShort = sal::static_int_cast< UINT16 >( ImpGetBool( p ) );
            break;
		case SbxBYREF | SbxERROR:
		case SbxBYREF | SbxUSHORT:
			*p->pUShort = ImpGetUShort( p ); break;
		case SbxBYREF | SbxLONG:
			*p->pLong = ImpGetLong( p ); break;
		case SbxBYREF | SbxULONG:
			*p->pULong = ImpGetULong( p ); break;
		case SbxBYREF | SbxSINGLE:
			*p->pSingle = ImpGetSingle( p ); break;
		case SbxBYREF | SbxDATE:
			*p->pDouble = ImpGetDate( p ); break;
		case SbxBYREF | SbxDOUBLE:
			*p->pDouble = ImpGetDouble( p ); break;
		case SbxBYREF | SbxCURRENCY:
			*p->pLong64 = ImpGetCurrency( p ); break;
		default:
			SbxBase::SetError( SbxERR_CONVERSION );
	}
	delete pTmp;
}

}

