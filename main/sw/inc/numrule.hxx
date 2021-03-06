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


#ifndef _NUMRULE_HXX
#define _NUMRULE_HXX

#include <tools/link.hxx>
#include <tools/gen.hxx>
#include <tools/string.hxx>
#include <editeng/svxenum.hxx>
#include <editeng/numitem.hxx>
#include "swdllapi.h"
#include <swtypes.hxx>
#include <calbck.hxx>
#include <errhdl.hxx>
#include <error.h>	
#include <hints.hxx>
#include <hash_map>
#include <stringhash.hxx>
#include <SwNumberTreeTypes.hxx>
#include <vector>

class SwTxtFmtColl;
class IDocumentListsAccess;
class SwNodeNum;
class Font;
class SvxBrushItem;
class SvxNumRule;
class SwCharFmt;
class SwDoc;
class SwFmtVertOrient;
class SwTxtNode;

const sal_Unicode cBulletChar	= 0x2022;	// Charakter fuer Aufzaehlungen

class SW_DLLPUBLIC SwNumFmt : public SvxNumberFormat, public SwClient
{
	SwFmtVertOrient* pVertOrient;

	SW_DLLPRIVATE void UpdateNumNodes( SwDoc* pDoc );
    SW_DLLPRIVATE virtual void NotifyGraphicArrived();

    using SvxNumberFormat::operator ==;
    using SvxNumberFormat::operator !=;

protected:
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem* pNew );

public:
	SwNumFmt();
	SwNumFmt( const SwNumFmt& );
	SwNumFmt( const SvxNumberFormat&, SwDoc* pDoc);

	virtual ~SwNumFmt();

	SwNumFmt& operator=( const SwNumFmt& );

	sal_Bool operator==( const SwNumFmt& ) const;
	sal_Bool operator!=( const SwNumFmt& r ) const { return !(*this == r); }

	SwCharFmt* GetCharFmt() const { return (SwCharFmt*)GetRegisteredIn(); }
	void SetCharFmt( SwCharFmt* );
    void ForgetCharFmt();

	virtual void			SetCharFmtName(const String& rSet);
	virtual const String&	GetCharFmtName()const;

    virtual void    SetGraphicBrush( const SvxBrushItem* pBrushItem, const Size* pSize = 0, const sal_Int16* pOrient = 0);

    virtual void                SetVertOrient(sal_Int16 eSet);
    virtual sal_Int16   GetVertOrient() const;
    const SwFmtVertOrient*      GetGraphicOrientation() const;

    sal_Bool IsEnumeration() const; // #i22362#
    sal_Bool IsItemize() const; // #i29560#
};

class SwPaM;
enum SwNumRuleType { OUTLINE_RULE = 0, NUM_RULE = 1, RULE_END = 2 };
class SW_DLLPUBLIC SwNumRule
{
// --> OD 2008-02-19 #refactorlists#
public:
    typedef std::vector< SwTxtNode* > tTxtNodeList;
    typedef std::vector< SwTxtFmtColl* > tParagraphStyleList;
// <--
private:
	friend void _FinitCore();

#ifdef DBG_UTIL
    long int nSerial;
    static long int nInstances;
#endif

	static SwNumFmt* aBaseFmts [ RULE_END ][ MAXLEVEL ];
	static sal_uInt16 aDefNumIndents[ MAXLEVEL ];
    // --> OD 2008-02-11 #newlistlevelattrs#
    // default list level properties for position-and-space mode LABEL_ALIGNMENT
    static SwNumFmt* aLabelAlignmentBaseFmts [ RULE_END ][ MAXLEVEL ];
    // <--
	static sal_uInt16 nRefCount;
	static char* pDefOutlineName;

	SwNumFmt* aFmts[ MAXLEVEL ];

    /** container for associated text nodes

    */
    // --> OD 2008-02-19 #refactorlists#
//    SwTxtNodeTable* pTxtNodeList;
    tTxtNodeList maTxtNodeList;
    // <--

    /** container for associated paragraph styles

        OD 2008-03-03 #refactorlists#
    */
    tParagraphStyleList maParagraphStyleList;

    // #i36749#
    /**
       hash_map containing "name->rule" relation
     */
    std::hash_map<String, SwNumRule *, StringHash> * pNumRuleMap;

	String sName;
	SwNumRuleType eRuleType;
	sal_uInt16 nPoolFmtId;		// Id-fuer "automatich" erzeugte NumRules
	sal_uInt16 nPoolHelpId;		// HelpId fuer diese Pool-Vorlage
	sal_uInt8 nPoolHlpFileId; 	// FilePos ans Doc auf die Vorlagen-Hilfen
	sal_Bool bAutoRuleFlag : 1;
	sal_Bool bInvalidRuleFlag : 1;
	sal_Bool bContinusNum : 1;	// Fortlaufende Numerierung - ohne Ebenen
	sal_Bool bAbsSpaces : 1;	// die Ebenen repraesentieren absol. Einzuege
    bool mbCountPhantoms;

    // --> OD 2008-02-11 #newlistlevelattrs#
    const SvxNumberFormat::SvxNumPositionAndSpaceMode meDefaultNumberFormatPositionAndSpaceMode;
    // <--

    // --> OD 2008-04-03 #refactorlists#
    String msDefaultListId;
    // <--

public:
    // --> OD 2008-02-08 #newlistlevelattrs#
    // add parameter <eDefaultNumberFormatPositionAndSpaceMode>
    SwNumRule( const String& rNm,
               const SvxNumberFormat::SvxNumPositionAndSpaceMode eDefaultNumberFormatPositionAndSpaceMode,
               SwNumRuleType = NUM_RULE,
               sal_Bool bAutoFlg = sal_True );

	SwNumRule( const SwNumRule& );
	~SwNumRule();

	SwNumRule& operator=( const SwNumRule& );
	sal_Bool operator==( const SwNumRule& ) const;
	sal_Bool operator!=( const SwNumRule& r ) const { return !(*this == r); }

	const SwNumFmt* GetNumFmt( sal_uInt16 i ) const;
	const SwNumFmt& Get( sal_uInt16 i ) const;

	void Set( sal_uInt16 i, const SwNumFmt* );
	void Set( sal_uInt16 i, const SwNumFmt& );
	String MakeNumString( const SwNodeNum&, sal_Bool bInclStrings = sal_True,
							sal_Bool bOnlyArabic = sal_False ) const;
    // --> OD 2005-10-17 #126238#
    // - add optional parameter <_nRestrictToThisLevel> in order to
    //   restrict returned string to this level.
    String MakeNumString( const SwNumberTree::tNumberVector & rNumVector,
                          const sal_Bool bInclStrings = sal_True,
                          const sal_Bool bOnlyArabic = sal_False,
                          const unsigned int _nRestrictToThisLevel = MAXLEVEL ) const;
    // <--
    // --> OD 2007-08-24 #i81002#
    String MakeRefNumString( const SwNodeNum& rNodeNum,
                             const bool bInclSuperiorNumLabels = false,
                             const sal_uInt8 nRestrictInclToThisLevel = 0 ) const;
    // <--

    /** Returns list of associated text nodes.

       OD 2008-02-19 #refactorlists#

       @return list of associated text nodes
    */
//    const SwTxtNodeTable * GetTxtNodeList() const { return pTxtNodeList; }
    void GetTxtNodeList( SwNumRule::tTxtNodeList& rTxtNodeList ) const;
    SwNumRule::tTxtNodeList::size_type GetTxtNodeListSize() const;

    // --> OD 2008-02-19 #refactorlists#
    void AddTxtNode( SwTxtNode& rTxtNode );
    void RemoveTxtNode( SwTxtNode& rTxtNode );
    // <--

    // --> OD 2008-03-03 #refactorlists#
    SwNumRule::tParagraphStyleList::size_type GetParagraphStyleListSize() const;
    void AddParagraphStyle( SwTxtFmtColl& rTxtFmtColl );
    void RemoveParagraphStyle( SwTxtFmtColl& rTxtFmtColl );
    // <--

    // --> OD 2008-04-03 #refactorlists#
    inline void SetDefaultListId( const String sDefaultListId )
    {
        msDefaultListId = sDefaultListId;
    }
    inline String GetDefaultListId() const
    {
        return msDefaultListId;
    }
    // <--
    // #i36749#
    /**
       Register this rule in a "name->numrule" map.

       @param pNumRuleMap      map to register in
     */
    void SetNumRuleMap(
                std::hash_map<String, SwNumRule *, StringHash>* pNumRuleMap );

	static char* GetOutlineRuleName() { return pDefOutlineName; }

	static sal_uInt16 GetNumIndent( sal_uInt8 nLvl );
	static sal_uInt16 GetBullIndent( sal_uInt8 nLvl );

	SwNumRuleType GetRuleType() const 			{ return eRuleType; }
	void SetRuleType( SwNumRuleType eNew ) 		{ eRuleType = eNew;
												  bInvalidRuleFlag = sal_True; }

	// eine Art Copy-Constructor, damit die Num-Formate auch an den
	// richtigen CharFormaten eines Dokumentes haengen !!
	// (Kopiert die NumFormate und returnt sich selbst)
	SwNumRule& CopyNumRule( SwDoc*, const SwNumRule& );

	// testet ob die CharFormate aus dem angegeben Doc sind und kopiert
	// die gegebenfalls
	void CheckCharFmts( SwDoc* pDoc );

    const String& GetName() const       { return sName; }
    // --> OD 2008-07-08 #i91400#
    void SetName( const String& rNm,
                  IDocumentListsAccess& rDocListAccess ); // #i36749#
    // <--

	sal_Bool IsAutoRule() const 			{ return bAutoRuleFlag; }
	void SetAutoRule( sal_Bool bFlag )		{ bAutoRuleFlag = bFlag; }

	sal_Bool IsInvalidRule() const 			{ return bInvalidRuleFlag; }
	void SetInvalidRule( sal_Bool bFlag );

	sal_Bool IsContinusNum() const 			{ return bContinusNum; }
	void SetContinusNum( sal_Bool bFlag )	{ bContinusNum = bFlag; }

	sal_Bool IsAbsSpaces() const 			{ return bAbsSpaces; }
	void SetAbsSpaces( sal_Bool bFlag )		{ bAbsSpaces = bFlag; }

    // #115901#
    sal_Bool IsOutlineRule() const { return eRuleType == OUTLINE_RULE; }

    bool IsCountPhantoms() const;
    void SetCountPhantoms(bool bCountPhantoms);

	// erfragen und setzen der Poolvorlagen-Id's
	sal_uInt16 GetPoolFmtId() const			{ return nPoolFmtId; }
	void SetPoolFmtId( sal_uInt16 nId ) 	{ nPoolFmtId = nId; }

	// erfragen und setzen der Hilfe-Id's fuer die Document-Vorlagen
	sal_uInt16 GetPoolHelpId() const 		{ return nPoolHelpId; }
	void SetPoolHelpId( sal_uInt16 nId ) 	{ nPoolHelpId = nId; }
	sal_uInt8 GetPoolHlpFileId() const 		{ return nPoolHlpFileId; }
	void SetPoolHlpFileId( sal_uInt8 nId ) 	{ nPoolHlpFileId = nId; }

	void		SetSvxRule(const SvxNumRule&, SwDoc* pDoc);
	SvxNumRule	MakeSvxNumRule() const;

    // #i23726#, #i23725#
    // --> OD 2008-06-09 #i90078#
    // refactoring: provide certain method for certain purpose
//    void        Indent(short aAmount, int nLevel = -1,
//                       int nReferenceLevel = -1, sal_Bool bRelative = sal_True,
//                       sal_Bool bFirstLine = sal_True, sal_Bool bCheckGtZero = sal_True);
    // change indent of all list levels by given difference
    void ChangeIndent( const short nDiff );
    // set indent of certain list level to given value
    void SetIndent( const short nNewIndent,
                    const sal_uInt16 nListLevel );
    // set indent of first list level to given value and change other list level's
    // indents accordingly
    void SetIndentOfFirstListLevelAndChangeOthers( const short nNewIndent );
    // <--

    void Validate();
};

// --> OD 2006-06-27 #b6440955#
// namespace for static functions and methods for numbering and bullets
namespace numfunc
{
    /** retrieve font family name used for the default bullet list characters

        @author OD
    */
    const String& GetDefBulletFontname();

    /** determine if default bullet font is user defined

        OD 2008-06-06 #i63395#
        The default bullet font is user defined, if it is given in the user configuration

        @author OD
    */
    bool IsDefBulletFontUserDefined();

    /** retrieve font used for the default bullet list characters

        @author OD
    */
    SW_DLLPUBLIC const Font& GetDefBulletFont();

    /** retrieve unicode of character used for the default bullet list for the given list level

        @author OD
    */
    sal_Unicode GetBulletChar( sal_uInt8 nLevel );

    /** configuration, if at first position of the first list item the <TAB>-key
        increased the indent of the complete list or only demotes this list item.
        The same for <SHIFT-TAB>-key at the same position for decreasing the
        indent of the complete list or only promotes this list item.

        OD 2007-10-01 #b6600435#

        @author OD
    */
    sal_Bool ChangeIndentOnTabAtFirstPosOfFirstListItem();

    /**
        OD 2008-06-06 #i89178#

        @author OD
    */
    SvxNumberFormat::SvxNumPositionAndSpaceMode GetDefaultPositionAndSpaceMode();
}

#endif	// _NUMRULE_HXX
