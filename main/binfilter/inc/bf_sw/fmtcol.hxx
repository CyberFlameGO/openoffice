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


#ifndef _FMTCOL_HXX
#define _FMTCOL_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _FORMAT_HXX
#include <format.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>		// fuer MAXLEVEL
#endif
#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif
namespace binfilter {

class SwDoc;		// fuer friend

class SwFmtColl : public SwFmt
{
protected:
	SwFmtColl( SwAttrPool& rPool, const sal_Char* pFmtName,
				const USHORT* pWhichRanges, SwFmtColl* pDerFrom,
				USHORT nFmtWhich )
	  	: SwFmt( rPool, pFmtName, pWhichRanges, pDerFrom, nFmtWhich )
	{ SetAuto( FALSE ); }

	SwFmtColl( SwAttrPool& rPool, const String &rFmtName,
				const USHORT* pWhichRanges, SwFmtColl* pDerFrom,
				USHORT nFmtWhich )
	  	: SwFmt( rPool, rFmtName, pWhichRanges, pDerFrom, nFmtWhich )
	{ SetAuto( FALSE ); }


private:
	// erstmal wird nicht kopiert und nicht zugewiesen
	SwFmtColl(const SwFmtColl & );
	const SwFmtColl &operator=(const SwFmtColl &);
};


class SwTxtFmtColl: public SwFmtColl
{
	friend class SwDoc;
protected:
	BYTE nOutlineLevel;
	SwTxtFmtColl *pNextTxtFmtColl;

	SwTxtFmtColl( SwAttrPool& rPool, const sal_Char* pFmtCollName,
					SwTxtFmtColl* pDerFrom = 0,
					USHORT nFmtWh = RES_TXTFMTCOLL )
		: SwFmtColl( rPool, pFmtCollName, aTxtFmtCollSetRange,
						pDerFrom, nFmtWh ),
		nOutlineLevel( NO_NUMBERING )
	{ pNextTxtFmtColl = this; }

	SwTxtFmtColl( SwAttrPool& rPool, const String &rFmtCollName,
					SwTxtFmtColl* pDerFrom = 0,
					USHORT nFmtWh = RES_TXTFMTCOLL )
		: SwFmtColl( rPool, rFmtCollName, aTxtFmtCollSetRange,
						pDerFrom, nFmtWh ),
		nOutlineLevel( NO_NUMBERING )
	{ pNextTxtFmtColl = this; }

public:

	// zum "abfischen" von UL-/LR-/FontHeight Aenderungen
	virtual void Modify( SfxPoolItem*, SfxPoolItem* );

	TYPEINFO();		//Bereits in Basisklasse Client drin.

	inline void SetOutlineLevel( BYTE );
	inline BYTE GetOutlineLevel() const { return nOutlineLevel; }

	inline void SetNextTxtFmtColl(SwTxtFmtColl& rNext);
	SwTxtFmtColl& GetNextTxtFmtColl() const { return *pNextTxtFmtColl; }

	BOOL IsAtDocNodeSet() const;

/*----------------- JP 09.08.94 17:36 -------------------
 wird die Funktionalitaet von Zeichenvorlagen an Absatzvorlagen
 ueberhaupt benoetigt ??

 Wenn, ja dann muessen im TextNode und hier in der TxtCollection ein 2.
 Attset fuer die Char-Attribute angelegt werden; damit die Vererbung
 und der Zugriff auf die gesetzen Attribute richtig funktioniert!!

	virtual BOOL SetDerivedFrom( SwFmtColl* pDerFrom = 0 );

	inline SwCharFmt* GetCharFmt() const;
	inline BOOL IsCharFmtSet() const;
	void SetCharFmt(SwCharFmt *);
	void ResetCharFmt();
inline BOOL SwTxtFmtColl::IsCharFmtSet() const
{
	return aCharDepend.GetRegisteredIn() ? TRUE : FALSE;
}
inline SwCharFmt* SwTxtFmtColl::GetCharFmt() const
{
	return (SwCharFmt*)aCharDepend.GetRegisteredIn();
}
--------------------------------------------------*/
};

typedef SwTxtFmtColl* SwTxtFmtCollPtr;
SV_DECL_PTRARR(SwTxtFmtColls,SwTxtFmtCollPtr,2,4)


class SwGrfFmtColl: public SwFmtColl
{
	friend class SwDoc;
protected:
	SwGrfFmtColl( SwAttrPool& rPool, const sal_Char* pFmtCollName,
					SwGrfFmtColl* pDerFrom = 0 )
		: SwFmtColl( rPool, pFmtCollName, aGrfFmtCollSetRange,
					pDerFrom, RES_GRFFMTCOLL )
	{}

	SwGrfFmtColl( SwAttrPool& rPool, const String &rFmtCollName,
					SwGrfFmtColl* pDerFrom = 0 )
		: SwFmtColl( rPool, rFmtCollName, aGrfFmtCollSetRange,
					pDerFrom, RES_GRFFMTCOLL )
	{}

public:
	TYPEINFO();		//Bereits in Basisklasse Client drin.
};

typedef SwGrfFmtColl* SwGrfFmtCollPtr;
SV_DECL_PTRARR(SwGrfFmtColls,SwGrfFmtCollPtr,2,4)



//FEATURE::CONDCOLL
// --------- Bedingte Vorlagen -------------------------------

enum Master_CollConditions
{
	PARA_IN_LIST		= 0x0001,
	PARA_IN_OUTLINE		= 0x0002,
	PARA_IN_FRAME		= 0x0004,
	PARA_IN_TABLEHEAD	= 0x0008,
	PARA_IN_TABLEBODY	= 0x0010,
	PARA_IN_SECTION		= 0x0020,
	PARA_IN_FOOTENOTE 	= 0x0040,
	PARA_IN_FOOTER 		= 0x0080,
	PARA_IN_HEADER 		= 0x0100,
	PARA_IN_ENDNOTE 	= 0x0200,
	// ...
	USRFLD_EXPRESSION	= (int)0x8000
};


class SwCollCondition : public SwClient
{
	ULONG nCondition;
	union
	{
		ULONG nSubCondition;
		String* pFldExpression;
	} aSubCondition;

public:
	TYPEINFO();		//Bereits in Basisklasse Client drin.


	SwCollCondition( SwTxtFmtColl* pColl, ULONG nMasterCond,
					ULONG nSubCond = 0 );

	virtual ~SwCollCondition();

	int operator==( const SwCollCondition& rCmp ) const;
	int operator!=( const SwCollCondition& rCmp ) const
							{ return ! (*this == rCmp); }

	ULONG GetCondition() const 		{ return nCondition; }
	ULONG GetSubCondition() const	{ return aSubCondition.nSubCondition; }
	const String* GetFldExpression() const
									{ return aSubCondition.pFldExpression; }

	void SetCondition( ULONG nCond, ULONG nSubCond );
	SwTxtFmtColl* GetTxtFmtColl() const 	{ return (SwTxtFmtColl*)GetRegisteredIn(); }
};


typedef SwCollCondition* SwCollConditionPtr;
SV_DECL_PTRARR_DEL( SwFmtCollConditions, SwCollConditionPtr, 0, 5 )//STRIP008 ;

class SwConditionTxtFmtColl : public SwTxtFmtColl
{
	friend class SwDoc;
protected:
	SwFmtCollConditions aCondColls;

	SwConditionTxtFmtColl( SwAttrPool& rPool, const sal_Char* pFmtCollName,
							SwTxtFmtColl* pDerFrom = 0 )
		: SwTxtFmtColl( rPool, pFmtCollName, pDerFrom, RES_CONDTXTFMTCOLL )
	{}
	SwConditionTxtFmtColl( SwAttrPool& rPool, const String &rFmtCollName,
							SwTxtFmtColl* pDerFrom = 0 )
		: SwTxtFmtColl( rPool, rFmtCollName, pDerFrom, RES_CONDTXTFMTCOLL )
	{}

public:
	TYPEINFO();		//Bereits in Basisklasse Client drin.

	virtual ~SwConditionTxtFmtColl();

	// zum "abfischen" von Aenderungen
//	virtual void Modify( SfxPoolItem*, SfxPoolItem* );

	const SwCollCondition* HasCondition( const SwCollCondition& rCond ) const;
	const SwFmtCollConditions& GetCondColls() const		{ return aCondColls; }
	void InsertCondition( const SwCollCondition& rCond );

};

//FEATURE::CONDCOLL

// ------------- Inline Implementierungen --------------------

inline void SwTxtFmtColl::SetNextTxtFmtColl( SwTxtFmtColl& rNext )
{
	pNextTxtFmtColl = &rNext;
}

inline void SwTxtFmtColl::SetOutlineLevel( BYTE nLevel )
{
	ASSERT( nLevel < MAXLEVEL || nLevel == NO_NUMBERING ,
							"SwTxtFmtColl: Level too low" );
	nOutlineLevel = nLevel;
}


} //namespace binfilter
#endif

