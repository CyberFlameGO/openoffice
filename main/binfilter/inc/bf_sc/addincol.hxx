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



#ifndef SC_ADDINCOL_HXX
#define SC_ADDINCOL_HXX

#include <bf_svtools/bf_solar.h>



#ifndef _COM_SUN_STAR_SHEET_XVOLATILERESULT_HPP_
#include <com/sun/star/sheet/XVolatileResult.hpp>
#endif

#ifndef _COM_SUN_STAR_SHEET_XADDIN_HPP_
#include <com/sun/star/sheet/XAddIn.hpp>
#endif

#ifndef _COM_SUN_STAR_SHEET_XRESULTLISTENER_HPP_
#include <com/sun/star/sheet/XResultListener.hpp>
#endif

#ifndef _COM_SUN_STAR_SHEET_RESULTEVENT_HPP_
#include <com/sun/star/sheet/ResultEvent.hpp>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif


#ifndef _RTL_USTRING_H_ 
#include <rtl/ustring.h>
#endif

#include <hash_map>

class String;
namespace binfilter {


class SfxObjectShell;
class ScUnoAddInFuncData;
class ScMatrix;
class ScFuncDesc;


struct ScAddInStringHashCode
{
    size_t operator()( const String& rStr ) const
	{
        return rtl_ustr_hashCode_WithLength( rStr.GetBuffer(), rStr.Len() );
	}
};
typedef ::std::hash_map< String, const ScUnoAddInFuncData*, ScAddInStringHashCode, ::std::equal_to< String > > ScAddInHashMap;


enum ScAddInArgumentType
{
	SC_ADDINARG_NONE,					// -
	SC_ADDINARG_INTEGER,				// long
	SC_ADDINARG_DOUBLE,					// double
	SC_ADDINARG_STRING,					// string
	SC_ADDINARG_INTEGER_ARRAY,			// sequence<sequence<long>>
	SC_ADDINARG_DOUBLE_ARRAY,			// sequence<sequence<double>>
	SC_ADDINARG_STRING_ARRAY,			// sequence<sequence<string>>
	SC_ADDINARG_MIXED_ARRAY,			// sequence<sequence<any>>
	SC_ADDINARG_VALUE_OR_ARRAY,			// any
	SC_ADDINARG_CELLRANGE,				// XCellRange
	SC_ADDINARG_CALLER,					// XPropertySet
	SC_ADDINARG_VARARGS					// sequence<any>
};


class ScUnoAddInCollection
{
private:
	long					nFuncCount;
	ScUnoAddInFuncData**	ppFuncData;
    ScAddInHashMap*         pExactHashMap;      // exact internal name
    ScAddInHashMap*         pNameHashMap;       // internal name upper
    ScAddInHashMap*         pLocalHashMap;      // localized name upper
	BOOL					bInitialized;

	void		Initialize();
	void		ReadFromAddIn( const ::com::sun::star::uno::Reference<
								::com::sun::star::uno::XInterface>& xInterface );

public:
				ScUnoAddInCollection();
				~ScUnoAddInCollection();

                        /// User enetered name. rUpperName MUST already be upper case!
	String				FindFunction( const String& rUpperName, BOOL bLocalFirst );
	const ScUnoAddInFuncData*   GetFuncData( const String& rName );             // exact name



								// both leave rRet... unchanged, if no matching name is found
};


class ScUnoAddInCall
{
private:
	const ScUnoAddInFuncData*	pFuncData;
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>			aArgs;
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>			aVarArg;
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface>	xCaller;
	BOOL						bValidCount;
	// result:
	USHORT						nErrCode;
	BOOL						bHasString;
	double						fValue;
	String						aString;
	ScMatrix*					pMatrix;
	::com::sun::star::uno::Reference< ::com::sun::star::sheet::XVolatileResult> xVarRes;

	void			ExecuteCallWithArgs(
						::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>& rCallArgs);

public:
					// exact name
	ScUnoAddInCall( ScUnoAddInCollection& rColl, const String& rName,
									long nParamCount );
					~ScUnoAddInCall();

	BOOL				NeedsCaller() const;
	void				SetCaller( const ::com::sun::star::uno::Reference<
									::com::sun::star::uno::XInterface>& rInterface );
	void				SetCallerFromObjectShell( SfxObjectShell* pSh );

	BOOL				ValidParamCount();
	ScAddInArgumentType	GetArgType( long nPos );
	void				SetParam( long nPos, const ::com::sun::star::uno::Any& rValue );

	void				ExecuteCall();

	void				SetResult( const ::com::sun::star::uno::Any& rNewRes );

	USHORT				GetErrCode() const		{ return nErrCode; }
	BOOL				HasString() const		{ return bHasString; }
	BOOL				HasMatrix() const		{ return ( pMatrix != NULL ); }
	BOOL				HasVarRes() const		{ return ( xVarRes.is() ); }
	double				GetValue() const		{ return fValue; }
	const String&		GetString() const		{ return aString; }
	const ScMatrix*		GetMatrix() const		{ return pMatrix; }
	::com::sun::star::uno::Reference< ::com::sun::star::sheet::XVolatileResult>
						GetVarRes() const		{ return xVarRes; }
};


} //namespace binfilter
#endif

