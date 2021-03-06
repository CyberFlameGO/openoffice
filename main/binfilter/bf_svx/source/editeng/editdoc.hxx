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



#ifndef _EDITDOC_HXX
#define _EDITDOC_HXX

#include <editattr.hxx>
#include <svxfont.hxx>

#ifndef _SFXITEMSET_HXX //autogen
#include <bf_svtools/itemset.hxx>
#endif

#ifndef _SFXSTYLE_HXX //autogen
#include <bf_svtools/style.hxx>
#endif

#ifndef _SFXITEMPOOL_HXX //autogen
#include <bf_svtools/itempool.hxx>
#endif

#ifndef _TOOLS_TABLE_HXX //autogen
#include <tools/table.hxx>
#endif
namespace binfilter {

class ImpEditEngine;
class SvxTabStop;

DBG_NAMEEX( EE_TextPortion )//STRIP008 

#define CHARPOSGROW		16
#define DEFTAB 			720

void CreateFont( SvxFont& rFont, const SfxItemSet& rSet, BOOL bSearchInParent = TRUE, short nScriptType = 0 );
USHORT GetScriptItemId( USHORT nItemId, short nScriptType );
BOOL IsScriptItemValid( USHORT nItemId, short nScriptType );

EditCharAttrib* MakeCharAttrib( SfxItemPool& rPool, const SfxPoolItem& rAttr, USHORT nS, USHORT nE );

class ContentNode;
class EditDoc;

struct EPaM
{
	USHORT nPara;
	USHORT nIndex;

	EPaM()								{ nPara = 0; nIndex = 0; }
	EPaM( USHORT nP, USHORT nI )		{ nPara = nP; nIndex = nI; }
	EPaM( const EPaM& r)				{ nPara = r.nPara; nIndex = r.nIndex; }
	EPaM& operator = ( const EPaM& r )	{ nPara = r.nPara; nIndex = r.nIndex; return *this; }
	inline BOOL operator == ( const EPaM& r ) const;
	inline BOOL operator < ( const EPaM& r ) const;
};

inline BOOL EPaM::operator < ( const EPaM& r ) const
{
	return ( ( nPara < r.nPara ) ||
			 ( ( nPara == r.nPara ) && nIndex < r.nIndex ) ) ? TRUE : FALSE;
}

inline BOOL EPaM::operator == ( const EPaM& r ) const
{
	return ( ( nPara == r.nPara ) && ( nIndex == r.nIndex ) ) ? TRUE : FALSE;
}

struct ScriptTypePosInfo
{
	short	nScriptType;
	USHORT	nStartPos;
	USHORT	nEndPos;

	ScriptTypePosInfo( short _Type, USHORT _Start, USHORT _End )
	{
		nScriptType = _Type;
		nStartPos = _Start;
		nEndPos = _End;
	}
};

SV_DECL_VARARR( ScriptTypePosInfos, ScriptTypePosInfo, 0, 4 )//STRIP008 ;

struct WritingDirectionInfo
{
	BYTE	nType;
	USHORT	nStartPos;
	USHORT	nEndPos;

	WritingDirectionInfo( BYTE _Type, USHORT _Start, USHORT _End )
	{
		nType = _Type;
		nStartPos = _Start;
		nEndPos = _End;
	}
};

SV_DECL_VARARR( WritingDirectionInfos, WritingDirectionInfo, 0, 4 )//STRIP008 ;

typedef EditCharAttrib* EditCharAttribPtr;
SV_DECL_PTRARR( CharAttribArray, EditCharAttribPtr, 0, 4 )//STRIP008 ;



//	----------------------------------------------------------------------
//	class SvxFontTable
//	----------------------------------------------------------------------

//	----------------------------------------------------------------------
//	class SvxColorList
//	----------------------------------------------------------------------

//	----------------------------------------------------------------------
//	class ItemList
//	----------------------------------------------------------------------

// -------------------------------------------------------------------------
// class ContentAttribs
// -------------------------------------------------------------------------
class ContentAttribs
{
private:
	SfxStyleSheet*	pStyle;
	SfxItemSet		aAttribSet;

public:
					ContentAttribs( SfxItemPool& rItemPool );
					ContentAttribs( const ContentAttribs& );
					~ContentAttribs();	// erst bei umfangreicheren Tabs

	SvxTabStop		FindTabStop( long nCurPos, USHORT nDefTab );
	SfxItemSet&		GetItems()							{ return aAttribSet; }
	SfxStyleSheet*	GetStyleSheet() const				{ return pStyle; }
	void			SetStyleSheet( SfxStyleSheet* pS );

	const SfxPoolItem&	GetItem( USHORT nWhich );
	BOOL				HasItem( USHORT nWhich );
};

// -------------------------------------------------------------------------
// class CharAttribList
// -------------------------------------------------------------------------
class CharAttribList
{
private:
	CharAttribArray	aAttribs;
	SvxFont			aDefFont;				// schneller, als jedesmal vom Pool!
	BOOL			bHasEmptyAttribs;

					CharAttribList( const CharAttribList& ) {;}

public:
					CharAttribList();
					~CharAttribList();

	void			RemoveItemsFromPool( SfxItemPool* pItemPool );

	EditCharAttrib*	FindAttrib( USHORT nWhich, USHORT nPos );
	EditCharAttrib*	FindNextAttrib( USHORT nWhich, USHORT nFromPos ) const;
	EditCharAttrib*	FindEmptyAttrib( USHORT nWhich, USHORT nPos );
	EditCharAttrib*	FindFeature( USHORT nPos ) const;


	void			ResortAttribs();
    void            OptimizeRanges( SfxItemPool& rItemPool );

	USHORT			Count()					{ return aAttribs.Count(); }
	void			Clear()					{ aAttribs.Remove( 0, aAttribs.Count()); }
	void			InsertAttrib( EditCharAttrib* pAttrib );

	SvxFont&		GetDefFont() 			{ return aDefFont; }

	BOOL			HasEmptyAttribs() const	{ return bHasEmptyAttribs; }
	BOOL&			HasEmptyAttribs() 		{ return bHasEmptyAttribs; }
	BOOL			HasBoundingAttrib( USHORT nBound );

	CharAttribArray&		GetAttribs() 		{ return aAttribs; }
	const CharAttribArray&	GetAttribs() const	{ return aAttribs; }

	// Debug:
};

// -------------------------------------------------------------------------
// class ContentNode
// -------------------------------------------------------------------------
class ContentNode : public XubString
{
private:
	ContentAttribs	aContentAttribs;
	CharAttribList	aCharAttribList;

public:
					ContentNode( SfxItemPool& rItemPool );
					ContentNode( const XubString& rStr, const ContentAttribs& rContentAttribs );
					~ContentNode();

	ContentAttribs&	GetContentAttribs() 	{ return aContentAttribs; }
	CharAttribList&	GetCharAttribs()		{ return aCharAttribList; }

	void			ExpandAttribs( USHORT nIndex, USHORT nNewChars, SfxItemPool& rItemPool );
	void			CollapsAttribs( USHORT nIndex, USHORT nDelChars, SfxItemPool& rItemPool );
	void			AppendAttribs( ContentNode* pNextNode );
	void			CopyAndCutAttribs( ContentNode* pPrevNode, SfxItemPool& rPool, BOOL bKeepEndingAttribs );

	void			SetStyleSheet( SfxStyleSheet* pS, BOOL bRecalcFont = TRUE );
	SfxStyleSheet*	GetStyleSheet()	{ return aContentAttribs.GetStyleSheet(); }

	void			CreateDefFont();

	BOOL			IsFeature( USHORT nPos ) const { return ( GetChar( nPos ) == CH_FEATURE ); }
};

typedef ContentNode* ContentNodePtr;
SV_DECL_PTRARR( ContentList, ContentNodePtr, 0, 4 )//STRIP008 ;

// -------------------------------------------------------------------------
// class EditPaM
// -------------------------------------------------------------------------
class EditPaM
{
private:
	ContentNode*	pNode;
	USHORT			nIndex;

public:
					EditPaM() 							{ pNode = NULL; nIndex = 0; }
					EditPaM( ContentNode* p, USHORT n )	{ pNode = p; nIndex = n; }

	ContentNode*	GetNode() const					{ return pNode; }
	void			SetNode( ContentNode* p) 		{ pNode = p; }

	USHORT			GetIndex() const				{ return nIndex; }
	USHORT&			GetIndex() 						{ return nIndex; }
	void			SetIndex( USHORT n )			{ nIndex = n; }

	BOOL			IsParaStart() const				{ return nIndex == 0; }
	BOOL			IsParaEnd() const 				{ return nIndex == pNode->Len(); }

	EditPaM&	operator = ( const EditPaM& rPaM );
	friend BOOL operator == ( const EditPaM& r1,  const EditPaM& r2  );
	friend BOOL operator != ( const EditPaM& r1,  const EditPaM& r2  );
};

#define PORTIONKIND_TEXT		0
#define PORTIONKIND_TAB         1
#define PORTIONKIND_LINEBREAK	2
#define PORTIONKIND_FIELD		3
#define PORTIONKIND_HYPHENATOR	4
// #define PORTIONKIND_EXTRASPACE	5

#define DELMODE_SIMPLE			0
#define DELMODE_RESTOFWORD		1
#define DELMODE_RESTOFCONTENT	2

#define CHAR_NORMAL            0x00
#define CHAR_KANA              0x01
#define CHAR_PUNCTUATIONLEFT   0x02
#define CHAR_PUNCTUATIONRIGHT  0x04

// -------------------------------------------------------------------------
// struct ExtraPortionInfos
// -------------------------------------------------------------------------
struct ExtraPortionInfo
{
    long    nOrgWidth;
    long    nWidthFullCompression;

    long    nPortionOffsetX;

    USHORT  nMaxCompression100thPercent;

    BYTE    nAsianCompressionTypes;
    BOOL    bFirstCharIsRightPunktuation;
    BOOL    bCompressed;

    long*    pOrgDXArray;

            ~ExtraPortionInfo();

};


// -------------------------------------------------------------------------
// class TextPortion
// -------------------------------------------------------------------------
class TextPortion
{
private:
    ExtraPortionInfo*   pExtraInfos;
	USHORT		        nLen;
	Size		        aOutSz;
	BYTE		        nKind;
    BYTE                nRightToLeft;
	sal_Unicode	        nExtraValue;


				TextPortion()				{ DBG_CTOR( EE_TextPortion, 0 );
                                              pExtraInfos = NULL; nLen = 0; nKind = PORTIONKIND_TEXT; nExtraValue = 0; nRightToLeft = FALSE;}

public:
				TextPortion( USHORT nL ) : aOutSz( -1, -1 )
											{	DBG_CTOR( EE_TextPortion, 0 );
												pExtraInfos = NULL; nLen = nL; nKind = PORTIONKIND_TEXT; nExtraValue = 0; nRightToLeft = FALSE;}
				TextPortion( const TextPortion& r )	: aOutSz( r.aOutSz )
											{ DBG_CTOR( EE_TextPortion, 0 );
												pExtraInfos = NULL; nLen = r.nLen; nKind = r.nKind; nExtraValue = r.nExtraValue; nRightToLeft = r.nRightToLeft; }

                ~TextPortion()				{ 	DBG_DTOR( EE_TextPortion, 0 ); delete pExtraInfos; }

    USHORT		GetLen() const				{ return nLen; }
	USHORT&		GetLen() 					{ return nLen; }
	void		SetLen( USHORT nL )			{ nLen = nL; }

	Size&		GetSize()					{ return aOutSz; }
	Size		GetSize() const				{ return aOutSz; }

	BYTE&		GetKind()					{ return nKind; }
	BYTE		GetKind() const				{ return nKind; }

    void        SetRightToLeft( BYTE b )    { nRightToLeft = b; }
    BYTE        GetRightToLeft() const      { return nRightToLeft; }
    BOOL        IsRightToLeft() const       { return (nRightToLeft&1); }

	sal_Unicode	GetExtraValue() const		{ return nExtraValue; }
	void		SetExtraValue( sal_Unicode n ) 	{ nExtraValue = n; }

	BOOL		HasValidSize() const		{ return aOutSz.Width() != (-1); }

    ExtraPortionInfo*   GetExtraInfos() const { return pExtraInfos; }
    void                SetExtraInfos( ExtraPortionInfo* p ) { delete pExtraInfos; pExtraInfos = p; }
};

// -------------------------------------------------------------------------
// class TextPortionList
// -------------------------------------------------------------------------
typedef TextPortion* TextPortionPtr;
SV_DECL_PTRARR( TextPortionArray, TextPortionPtr, 0, 8 )//STRIP008 ;

class TextPortionList : public TextPortionArray
{
public:
			TextPortionList();
			~TextPortionList();

	void	Reset();
	USHORT	FindPortion( USHORT nCharPos, USHORT& rPortionStart, BOOL bPreferStartingPortion = FALSE );
	void	DeleteFromPortion( USHORT nDelFrom );
};

class ParaPortion;

SV_DECL_VARARR( CharPosArray, sal_Int32, 0, CHARPOSGROW )//STRIP008 ;

// ------------------------------------------------------------------------
// class EditLine
// -------------------------------------------------------------------------
class EditLine
{
private:
	CharPosArray	aPositions;
    long            nTxtWidth;
	USHORT			nStartPosX;
	USHORT			nStart;		// koennte durch nStartPortion ersetzt werden
	USHORT			nEnd;       // koennte durch nEndPortion ersetzt werden
	USHORT			nStartPortion;
	USHORT 			nEndPortion;
	USHORT			nHeight;	// Gesamthoehe der Zeile
	USHORT			nTxtHeight;	// Reine Texthoehe
	USHORT			nCrsrHeight;	// Bei Konturfluss hohe Zeilen => Cursor zu gro?
	USHORT			nMaxAscent;
	BOOL			bHangingPunctuation;
	BOOL			bInvalid;	// fuer geschickte Formatierung

public:
					EditLine();
					EditLine( const EditLine& );
					~EditLine();

	BOOL			IsIn( USHORT nIndex ) const
						{ return ( (nIndex >= nStart ) && ( nIndex < nEnd ) ); }

	BOOL			IsIn( USHORT nIndex, BOOL bInclEnd ) const
						{ return ( ( nIndex >= nStart ) && ( bInclEnd ? ( nIndex <= nEnd ) : ( nIndex < nEnd ) ) ); }

	void			SetStart( USHORT n )			{ nStart = n; }
	USHORT			GetStart() const				{ return nStart; }
	USHORT&			GetStart() 						{ return nStart; }

	void			SetEnd( USHORT n )				{ nEnd = n; }
	USHORT			GetEnd() const					{ return nEnd; }
	USHORT&			GetEnd() 						{ return nEnd; }

	void			SetStartPortion( USHORT n )		{ nStartPortion = n; }
	USHORT			GetStartPortion() const			{ return nStartPortion; }
	USHORT&			GetStartPortion() 				{ return nStartPortion; }

	void			SetEndPortion( USHORT n )		{ nEndPortion = n; }
	USHORT			GetEndPortion() const			{ return nEndPortion; }
	USHORT&			GetEndPortion() 				{ return nEndPortion; }

	void			SetHeight( USHORT nH, USHORT nTxtH = 0, USHORT nCrsrH = 0 )
					{ 	nHeight = nH;
						nTxtHeight = ( nTxtH ? nTxtH : nH );
						nCrsrHeight = ( nCrsrH ? nCrsrH : nTxtHeight );
					}
	USHORT			GetHeight() const				{ return nHeight; }
	USHORT			GetTxtHeight() const			{ return nTxtHeight; }
	USHORT			GetCrsrHeight() const			{ return nCrsrHeight; }

    void            SetTextWidth( long n )          { nTxtWidth = n; }
    long            GetTextWidth() const            { return nTxtWidth; }

	void			SetMaxAscent( USHORT n )		{ nMaxAscent = n; }
	USHORT			GetMaxAscent() const			{ return nMaxAscent; }

	void			SetHangingPunctuation( BOOL b )		{ bHangingPunctuation = b; }
	BOOL			IsHangingPunctuation() const		{ return bHangingPunctuation; }

	USHORT			GetLen() const					{ return nEnd - nStart; }

	USHORT			GetStartPosX() const			{ return nStartPosX; }
	void			SetStartPosX( USHORT nStart )	{ nStartPosX = nStart; }

	Size			CalcTextSize( ParaPortion& rParaPortion );

	BOOL			IsInvalid()	const				{ return bInvalid; }
	BOOL			IsValid() const					{ return !bInvalid; }
	void			SetInvalid()					{ bInvalid = TRUE; }
	void			SetValid()						{ bInvalid = FALSE; }

	BOOL			IsEmpty() const					{ return (nEnd > nStart) ? FALSE : TRUE; }

	CharPosArray&	GetCharPosArray()				{ return aPositions; }


	EditLine&	operator = ( const EditLine& rLine );
	friend BOOL operator == ( const EditLine& r1,  const EditLine& r2  );
	friend BOOL operator != ( const EditLine& r1,  const EditLine& r2  );
};


// -------------------------------------------------------------------------
// class LineList
// -------------------------------------------------------------------------
typedef EditLine* EditLinePtr;
SV_DECL_PTRARR( LineArray, EditLinePtr, 0, 4 )//STRIP008 ;

class EditLineList : public LineArray
{
public:
			EditLineList();
			~EditLineList();

	void	Reset();
	void	DeleteFromLine( USHORT nDelFrom );
};

// -------------------------------------------------------------------------
// class ParaPortion
// -------------------------------------------------------------------------
class ParaPortion
{
	friend class ImpEditEngine;	// zum Einstellen der Hoehe
private:
	EditLineList		aLineList;
	TextPortionList		aTextPortionList;
	ContentNode*		pNode;
	long				nHeight;

	ScriptTypePosInfos	    aScriptInfos;
    WritingDirectionInfos   aWritingDirectionInfos;

	USHORT				nInvalidPosStart;
	USHORT				nFirstLineOffset;	// Fuer Writer-LineSpacing-Interpretation
	USHORT				nBulletX;
	short				nInvalidDiff;

	BOOL				bInvalid 			: 1;
	BOOL				bSimple				: 1;	// nur lineares Tippen
	BOOL				bVisible			: 1;	// MT 05/00: Gehoert an den Node!!!
	BOOL				bForceRepaint		: 1;

						ParaPortion( const ParaPortion& );

public:
						ParaPortion( ContentNode* pNode );
						~ParaPortion();

	USHORT				GetLineNumber( USHORT nIndex );

	EditLineList&		GetLines()					{ return aLineList; }

	BOOL				IsInvalid()	const			{ return bInvalid; }
	BOOL				IsSimpleInvalid()	const	{ return bSimple; }
	void				SetValid()					{ bInvalid = FALSE; bSimple = TRUE;}

	BOOL				MustRepaint() const			{ return bForceRepaint; }
	void				SetMustRepaint( BOOL bRP )	{ bForceRepaint = bRP; }

	USHORT				GetBulletX() const			{ return nBulletX; }
	void				SetBulletX( USHORT n ) 		{ nBulletX = n; }

	void				MarkInvalid( USHORT nStart, short nDiff);
	void				MarkSelectionInvalid( USHORT nStart, USHORT nEnd );

	BOOL				IsVisible()					{ return bVisible; }

	long				GetHeight() const 			{ return ( bVisible ? nHeight : 0 ); }
	USHORT				GetFirstLineOffset() const 	{ return ( bVisible ? nFirstLineOffset : 0 ); }
	void				ResetHeight()	{ nHeight = 0; nFirstLineOffset = 0; }

	ContentNode*		GetNode() const				{ return pNode; }
	TextPortionList&	GetTextPortions() 			{ return aTextPortionList; }

	USHORT				GetInvalidPosStart() const	{ return nInvalidPosStart; }
	short				GetInvalidDiff() const 		{ return nInvalidDiff; }

	void				CorrectValuesBehindLastFormattedLine( USHORT nLastFormattedLine );

};

typedef ParaPortion* ParaPortionPtr;
SV_DECL_PTRARR( DummyParaPortionList, ParaPortionPtr, 0, 4 )//STRIP008 ;

// -------------------------------------------------------------------------
// class ParaPortionList
// -------------------------------------------------------------------------
class ParaPortionList : public DummyParaPortionList
{
public:
					ParaPortionList();
					~ParaPortionList();

	void			Reset();
	long			GetYOffset( ParaPortion* pPPortion );
	USHORT			FindParagraph( long nYOffset );

	inline ParaPortion*	SaveGetObject( USHORT nPos ) const
		{ return ( nPos < Count() ) ? GetObject( nPos ) : 0; }

	// temporaer:
};

// -------------------------------------------------------------------------
// class EditSelection
// -------------------------------------------------------------------------
class EditSelection
{
private:
	EditPaM 		aStartPaM;
	EditPaM 		aEndPaM;

public:
					EditSelection();	// kein CCTOR und DTOR, geht autom. richtig!
					EditSelection( const EditPaM& rStartAndAnd );
					EditSelection( const EditPaM& rStart, const EditPaM& rEnd );

	EditPaM&		Min()				{ return aStartPaM; }
	EditPaM&		Max()				{ return aEndPaM; }

	const EditPaM&	Min() const			{ return aStartPaM; }
	const EditPaM&	Max() const			{ return aEndPaM; }

	BOOL			HasRange() const	{ return aStartPaM != aEndPaM; }

	BOOL			Adjust( const ContentList& rNodes );

	EditSelection&	operator = ( const EditPaM& r );
	BOOL 			operator == ( const EditSelection& r ) const
					{ return ( ( aStartPaM == r.aStartPaM ) && ( aEndPaM == r.aEndPaM ) )
							? TRUE : FALSE; }
	BOOL 			operator != ( const EditSelection& r ) const { return !( r == *this ); }
};

// -------------------------------------------------------------------------
// class DeletedNodeInfo
// -------------------------------------------------------------------------
class DeletedNodeInfo
{
private:
	ULONG 	nInvalidAdressPtr;
	USHORT	nInvalidParagraph;

public:
			DeletedNodeInfo( ULONG nInvAdr, USHORT nPos )
											{ 	nInvalidAdressPtr = nInvAdr;
												nInvalidParagraph = nPos; }

	ULONG	GetInvalidAdress()				{	return nInvalidAdressPtr; }
	USHORT	GetPosition()					{	return nInvalidParagraph; }
};

typedef DeletedNodeInfo* DeletedNodeInfoPtr;
SV_DECL_PTRARR( DeletedNodesList, DeletedNodeInfoPtr, 0, 4 )//STRIP008 ;

// -------------------------------------------------------------------------
// class EditDoc
// -------------------------------------------------------------------------
class EditDoc : public ContentList
{
private:
	SfxItemPool*	pItemPool;
    Link            aModifyHdl;

	SvxFont			aDefFont;			//schneller, als jedesmal vom Pool!
	USHORT			nDefTab;
	BOOL			bIsVertical;

	BOOL			bOwnerOfPool;
	BOOL			bModified;

protected:
	void			ImplDestroyContents();

public:
					EditDoc( SfxItemPool* pItemPool );
					~EditDoc();

	BOOL			IsModified() const		{ return bModified; }
	void			SetModified( BOOL b );

    void            SetModifyHdl( const Link& rLink ) { aModifyHdl = rLink; }
    Link            GetModifyHdl() const { return aModifyHdl; }

	void			CreateDefFont( BOOL bUseStyles );
	const SvxFont&	GetDefFont() { return aDefFont; }

	void			SetDefTab( USHORT nTab )	{ nDefTab = nTab ? nTab : DEFTAB; }
	USHORT			GetDefTab() const 			{ return nDefTab; }

	void			SetVertical( BOOL bVertical )	{ bIsVertical = bVertical; }
	BOOL			IsVertical() const 				{ return bIsVertical; }

	EditPaM			Clear();
	EditPaM			RemoveText();
	EditPaM			RemoveChars( EditPaM aPaM, USHORT nChars );
	EditPaM			InsertText( EditPaM aPaM, const XubString& rStr );
	EditPaM			InsertParaBreak( EditPaM aPaM, BOOL bKeepEndingAttribs );
	EditPaM			InsertFeature( EditPaM aPaM, const SfxPoolItem& rItem );
	EditPaM			ConnectParagraphs( ContentNode* pLeft, ContentNode* pRight );

	String			GetText( LineEnd eEnd ) const;
	ULONG			GetTextLen() const;

	XubString 		GetParaAsString( USHORT nNode ) const;
	XubString 		GetParaAsString( ContentNode* pNode, USHORT nStartPos = 0, USHORT nEndPos = 0xFFFF, BOOL bResolveFields = TRUE ) const;

	inline EditPaM	GetStartPaM() const;
	inline EditPaM	GetEndPaM() const;

	SfxItemPool&		GetItemPool()					{ return *pItemPool; }
	const SfxItemPool&	GetItemPool() const				{ return *pItemPool; }
	
	void			RemoveItemsFromPool( ContentNode* pNode );

	void			InsertAttrib( const SfxPoolItem& rItem, ContentNode* pNode, USHORT nStart, USHORT nEnd );
	void 			InsertAttrib( ContentNode* pNode, USHORT nStart, USHORT nEnd, const SfxPoolItem& rPoolItem );
	void			InsertAttribInSelection( ContentNode* pNode, USHORT nStart, USHORT nEnd, const SfxPoolItem& rPoolItem );
	BOOL			RemoveAttribs( ContentNode* pNode, USHORT nStart, USHORT nEnd, EditCharAttrib*& rpStarting, EditCharAttrib*& rpEnding, USHORT nWhich = 0 );
	void			FindAttribs( ContentNode* pNode, USHORT nStartPos, USHORT nEndPos, SfxItemSet& rCurSet );

	USHORT			GetPos( ContentNode* pNode ) const { return ContentList::GetPos(pNode); }
	ContentNode*	SaveGetObject( USHORT nPos ) const { return ( nPos < Count() ) ? GetObject( nPos ) : 0; }

	static XubString	GetSepStr( LineEnd eEnd );
};

inline EditPaM EditDoc::GetStartPaM() const
{
	return EditPaM( GetObject( 0 ), 0 );
}

inline EditPaM EditDoc::GetEndPaM() const
{
	ContentNode* pLastNode = GetObject( Count()-1 );
	return EditPaM( pLastNode, pLastNode->Len() );
}

inline EditCharAttrib* GetAttrib( const CharAttribArray& rAttribs, USHORT nAttr )
{
	return ( nAttr < rAttribs.Count() ) ? rAttribs[nAttr] : 0;
}

BOOL CheckOrderedList( CharAttribArray& rAttribs, BOOL bStart );

// -------------------------------------------------------------------------
// class EditEngineItemPool
// -------------------------------------------------------------------------
class EditEngineItemPool : public SfxItemPool
{
public:
						EditEngineItemPool( BOOL bPersistenRefCounts );
						~EditEngineItemPool();

	virtual SvStream&	Store( SvStream& rStream ) const;
};

}//end of namespace binfilter
#endif // _EDITDOC_HXX
