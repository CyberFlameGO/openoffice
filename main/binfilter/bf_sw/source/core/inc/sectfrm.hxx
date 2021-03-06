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


#ifndef _SECTFRM_HXX
#define _SECTFRM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVMEMPOOL_HXX //autogen
#include <tools/mempool.hxx>
#endif

#include "layfrm.hxx"
#include "flowfrm.hxx"
namespace binfilter {

class SwSection;
class SwSectionFmt;
class SwAttrSetChg;
class SwFtnContFrm;
class SwFtnFrm;
class SwLayouter;

#define FINDMODE_ENDNOTE 1
#define FINDMODE_LASTCNT 2
#define FINDMODE_MYLAST  4

class SwSectionFrm: public SwLayoutFrm, public SwFlowFrm
{
	SwSection* pSection;

	void _UpdateAttr( SfxPoolItem*, SfxPoolItem*, BYTE &,
					  SwAttrSetChg *pa = 0, SwAttrSetChg *pb = 0 );
	void _Cut( BOOL bRemove );
	// Is there a FtnContainer?
	// An empty sectionfrm without FtnCont is superfluous
	BOOL IsSuperfluous() const { return !ContainsAny() && !ContainsFtnCont(); }
	void CalcFtnAtEndFlag();
	void CalcEndAtEndFlag();
protected:
	virtual void MakeAll();
	virtual BOOL ShouldBwdMoved( SwLayoutFrm *pNewUpper, BOOL bHead, BOOL &rReformat ){DBG_BF_ASSERT(0, "STRIP"); return FALSE;} //STRIP001 virtual BOOL ShouldBwdMoved( SwLayoutFrm *pNewUpper, BOOL bHead, BOOL &rReformat );
	virtual void Format( const SwBorderAttrs *pAttrs = 0 );
public:
	SwSectionFrm( SwSection & );				 //Inhalt wird nicht erzeugt!
    SwSectionFrm( SwSectionFrm &, BOOL bMaster );//_Nur_ zum Erzeugen von Master/Follows
	~SwSectionFrm();

    void Init();
    virtual void  CheckDirection( BOOL bVert );

	virtual void Cut();
	virtual	void Modify( SfxPoolItem*, SfxPoolItem* );

	inline const SwSectionFrm *GetFollow() const;
	inline		 SwSectionFrm *GetFollow();
	inline const SwSectionFrm *FindMaster() const;
	inline		 SwSectionFrm *FindMaster();
				 SwCntntFrm *FindLastCntnt( BYTE nMode = 0 );
	inline const SwCntntFrm *FindLastCntnt( BYTE nMode = 0 ) const;
	inline SwSection* GetSection() { return pSection; }
	inline const SwSection* GetSection() const { return pSection; }
	inline void ColLock()		{ bColLocked = TRUE; }
	inline void ColUnlock()   	{ bColLocked = FALSE; }

	void CalcFtnCntnt();
	void SimpleFormat();
	void MergeNext( SwSectionFrm* pNxt );
	//Zerlegt den pFrm umgebenden SectionFrm in zwei Teile,
	//pFrm an den Anfang des 2. Teils
	void DelEmpty( BOOL bRemove ); 	// wie Cut(), Follow-Verkettung wird aber mitgepflegt
	BOOL IsToIgnore() const			// Keine Groesse, kein Inhalt, muss ignoriert werden
	{ return !Frm().Height() && !ContainsCntnt(); }
	SwSectionFrm *FindSectionMaster();
	SwFtnContFrm* ContainsFtnCont( const SwFtnContFrm* pCont = NULL ) const;
	BOOL Growable() const;
    SwTwips _Shrink( SwTwips, SZPTR BOOL bTst );
    SwTwips _Grow  ( SwTwips, SZPTR BOOL bTst );

	// A sectionfrm has to maximize, if he has a follow or a ftncontainer at
	// the end of the page. A superfluous follow will be ignored,
	// if bCheckFollow is set.
	BOOL ToMaximize( BOOL bCheckFollow ) const;
	inline BOOL _ToMaximize() const
		{ if( !pSection ) return FALSE; return ToMaximize( FALSE );	}
	BOOL MoveAllowed( const SwFrm* ) const;
	BOOL CalcMinDiff( SwTwips& rMinDiff ) const;
	// Uebergibt man kein bOverSize bzw. FALSE, so ist der Returnwert
	// >0 fuer Undersized-Frames, ==0 sonst.
	// Uebergibt man TRUE, so kann es auch einen negativen Returnwert geben,
	// wenn der SectionFrm nicht vollstaendig ausgefuellt ist, was z.B. bei
	// SectionFrm mit Follow meist vorkommt. Benoetigt wird dies im
	// FormatWidthCols, um "Luft" aus den Spalten zu lassen.
	long Undersize( BOOL bOverSize = FALSE );
	// Groesse an die Umgebung anpassen
	void _CheckClipping( BOOL bGrow, BOOL bMaximize );

	void InvalidateFtnPos();

	static void MoveCntntAndDelete( SwSectionFrm* pDel, BOOL bSave );
	DECL_FIXEDMEMPOOL_NEWDEL(SwSectionFrm)
};

typedef SwSectionFrm* SwSectionFrmPtr;
SV_DECL_PTRARR_SORT( SwDestroyList, SwSectionFrmPtr, 1, 5)

inline const SwSectionFrm *SwSectionFrm::GetFollow() const
{
	return (const SwSectionFrm*)SwFlowFrm::GetFollow();
}
inline SwSectionFrm *SwSectionFrm::GetFollow()
{
	return (SwSectionFrm*)SwFlowFrm::GetFollow();
}

inline const SwSectionFrm *SwSectionFrm::FindMaster() const
{
	return (const SwSectionFrm*)SwFlowFrm::FindMaster();
}
inline SwSectionFrm *SwSectionFrm::FindMaster()
{
	return (SwSectionFrm*)SwFlowFrm::FindMaster();
}

inline const SwCntntFrm *SwSectionFrm::FindLastCntnt( BYTE nMode ) const
{
	return ((SwSectionFrm*)this)->FindLastCntnt( nMode );
}


} //namespace binfilter
#endif	//_SECTFRM_HXX
