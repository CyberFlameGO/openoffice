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



#ifndef SC_RANGENAM_HXX
#define SC_RANGENAM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_SCGLOB_HXX
#include "global.hxx" // -> enum UpdateRefMode
#endif
#ifndef SC_COLLECT_HXX
#include "collect.hxx"
#endif
namespace rtl {
	class OUStringBuffer;
}
namespace binfilter {

//------------------------------------------------------------------------

class ScDocument;
class ScMultipleReadHeader;
class ScMultipleWriteHeader;

//STRIP008 namespace rtl {
//STRIP008 	class OUStringBuffer;
//STRIP008 }


//------------------------------------------------------------------------

typedef USHORT RangeType;

#define RT_NAME				((RangeType)0x0000)
#define RT_DATABASE			((RangeType)0x0001)
#define RT_CRITERIA			((RangeType)0x0002)
#define RT_PRINTAREA		((RangeType)0x0004)
#define RT_COLHEADER		((RangeType)0x0008)
#define RT_ROWHEADER		((RangeType)0x0010)
#define RT_ABSAREA			((RangeType)0x0020)
#define RT_REFAREA			((RangeType)0x0040)
#define RT_ABSPOS			((RangeType)0x0080)
#define RT_SHARED			((RangeType)0x0100)
#define RT_SHAREDMOD		((RangeType)0x0200)

//------------------------------------------------------------------------

class ScTokenArray;
class ScIndexMap;

class ScRangeData : public DataObject
{
#if defined( ICC ) && defined( OS2 )
	friend static int _Optlink	 ICCQsortNameCompare( const void* a, const void* b);
#endif
private:
	String			aName;
	ScTokenArray*	pCode;
	ScAddress   	aPos;
	RangeType		eType;
	ScDocument* 	pDoc;
	USHORT			nIndex;
	USHORT			nExportIndex;
	BOOL			bModified;			// wird bei UpdateReference gesetzt/geloescht

	friend class ScRangeName;
	ScRangeData( USHORT nIndex );
public:
					ScRangeData( ScDocument* pDoc,
								 const String& rName,
								 const String& rSymbol,
                                 const ScAddress& rAdr = ScAddress(),
								 RangeType nType = RT_NAME,
								 BOOL bEnglish = FALSE );
									// rTarget ist ABSPOS Sprungmarke
					ScRangeData(const ScRangeData& rScRangeData);
					ScRangeData( SvStream& rStream,
								 ScMultipleReadHeader& rHdr,
								 USHORT nVer );

	virtual			~ScRangeData();


	virtual	DataObject* Clone() const;

	BOOL			Store( SvStream& rStream, ScMultipleWriteHeader& rHdr ) const;


	void			GetName( String& rName ) const	{ rName = aName; }
	const String&	GetName( void ) const			{ return aName; }
	ScAddress 		GetPos() const					{ return aPos; }
	// Der Index muss eindeutig sein. Ist er 0, wird ein neuer Index vergeben
	void			SetIndex( USHORT nInd )			{ nIndex = nExportIndex = nInd; }
	USHORT	GetIndex() const					{ return nIndex; }
	void			SetExportIndex( USHORT nInd )	{ nExportIndex = nInd; }
	USHORT	GetExportIndex() const 				{ return nExportIndex; }
	ScTokenArray*	GetCode()						{ return pCode; }
	USHORT			GetErrCode();
	BOOL			HasReferences() const;
	void			SetDocument( ScDocument* pDocument){ pDoc = pDocument; }
	ScDocument*		GetDocument() const				{ return pDoc; }
	void			SetType( RangeType nType )		{ eType = nType; }
	void			AddType( RangeType nType )		{ eType = eType|nType; }
	RangeType		GetType() const					{ return eType; }
	BOOL			HasType( RangeType nType ) const;
	void 			GetSymbol(String& rSymbol) const;
	void			GetEnglishSymbol(String& rSymbol, BOOL bCompileXML = FALSE) const;
	void 			UpdateSymbol( String& rSymbol, const ScAddress&,
									BOOL bEnglish = FALSE, BOOL bCompileXML = FALSE );
	void 			UpdateSymbol( ::rtl::OUStringBuffer& rBuffer, const ScAddress&,
									BOOL bEnglish = FALSE, BOOL bCompileXML = FALSE );
/*N*/ 	void 			UpdateReference( UpdateRefMode eUpdateRefMode,
/*N*/ 							 const ScRange& r,
/*N*/ 							 short nDx, short nDy, short nDz );
	BOOL			IsModified() const				{ return bModified; }



	BOOL			IsReference( ScRange& rRef ) const;



	void			ValidateTabRefs();

	void			ReplaceRangeNamesInUse( const ScIndexMap& rMap );

	BOOL			IsBeyond( USHORT nMaxRow ) const;

	static void		MakeValidName( String& rName );
#ifdef WNT
	static int __cdecl	QsortNameCompare( const void*, const void* );
#else
	static int		QsortNameCompare( const void*, const void* );
#endif
};

inline BOOL ScRangeData::HasType( RangeType nType ) const
{
	return ( ( eType & nType ) == nType );
}

#if defined( ICC ) && defined( OS2 )
	static int _Optlink	 ICCQsortNameCompare( const void* a, const void* b)
							{ ScRangeData::QsortNameCompare(a,b); }
#endif

//------------------------------------------------------------------------

class ScRangeName : public SortedCollection
{
private:
	ScDocument* pDoc;
	USHORT nSharedMaxIndex;
public:
	ScRangeName(USHORT nLim = 4, USHORT nDel = 4, BOOL bDup = FALSE,
				ScDocument* pDocument = NULL) :
		SortedCollection	( nLim, nDel, bDup ),
		pDoc				( pDocument ),
		nSharedMaxIndex		( 1 ) {}			// darf nicht 0 sein!!

	ScRangeName(const ScRangeName& rScRangeName, ScDocument* pDocument);

	virtual	DataObject* 	Clone(ScDocument* pDoc) const
							 { return new ScRangeName(*this, pDoc); }
	ScRangeData*			operator[]( const USHORT nIndex) const
							 { return (ScRangeData*)At(nIndex); }
	virtual	short			Compare(DataObject* pKey1, DataObject* pKey2) const;

	BOOL					Load( SvStream& rStream, USHORT nVer );
	BOOL					Store( SvStream& rStream ) const;
	BOOL					SearchName( const String& rName, USHORT& rPos ) const;
/*N*/ 	void					UpdateReference(UpdateRefMode eUpdateRefMode,
/*N*/ 								const ScRange& rRange,
/*N*/ 								short nDx, short nDy, short nDz );
	void 					UpdateTabRef(USHORT nTable, USHORT nFlag, USHORT nNewTable = 0);
/*N*/ 	void					UpdateTranspose( const ScRange& rSource, const ScAddress& rDest );
/*N*/ 	void					UpdateGrow( const ScRange& rArea, USHORT nGrowX, USHORT nGrowY );
	virtual	BOOL			Insert(DataObject* pDataObject);
	ScRangeData* 			FindIndex(USHORT nIndex);
	USHORT 					GetSharedMaxIndex()				{ return nSharedMaxIndex; }
	void 					SetSharedMaxIndex(USHORT nInd)	{ nSharedMaxIndex = nInd; }
	USHORT 					GetEntryIndex();
};

} //namespace binfilter
#endif

