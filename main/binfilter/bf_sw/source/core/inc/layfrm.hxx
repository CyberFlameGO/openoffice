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


#ifndef _LAYFRM_HXX
#define _LAYFRM_HXX

#include <bf_svtools/bf_solar.h>

#include "frame.hxx"
namespace binfilter {

class SwCntntFrm;
class SwFlowFrm;
class SwFmtCol;
struct SwCrsrMoveState;
class SwFrmFmt;
class SwBorderAttrs;
class SwFmtFrmSize;
class SwCellFrm;

class SwLayoutFrm: public SwFrm
{
	//Der verkappte SwFrm
	friend class SwFlowFrm;
	friend class SwFrm;

		//Hebt die Lower waehrend eines Spaltenumbaus auf.
	friend SwFrm* SaveCntnt( SwLayoutFrm *, SwFrm * );
	friend void   RestoreCntnt( SwFrm *, SwLayoutFrm *, SwFrm *pSibling );

	//entfernt leere SwSectionFrms aus einer Kette
	friend SwFrm* SwClearDummies( SwFrm* pFrm );

	void CopySubtree( const SwLayoutFrm *pDest );
protected:
	virtual void Format( const SwBorderAttrs *pAttrs = 0 );
	virtual void MakeAll();

	SwFrm			*pLower;

    virtual SwTwips ShrinkFrm( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );
    virtual SwTwips GrowFrm  ( SwTwips, SZPTR
							   BOOL bTst = FALSE, BOOL bInfo = FALSE );

	long CalcRel( const SwFmtFrmSize &rSz, BOOL bWidth ) const;

public:
	void PaintSubsidiaryLines( const SwPageFrm*, const SwRect& ) const;

		//Proportionale Groessenanpassung der untergeordneten.
	void ChgLowersProp( const Size& rOldSize );

    void AdjustColumns( const SwFmtCol *pCol, BOOL bAdjustAttributes,
                        BOOL bAutoWidth = FALSE);

	void ChgColumns( const SwFmtCol &rOld, const SwFmtCol &rNew,
		const BOOL bChgFtn = FALSE );


		//Painted die Column-Trennlinien fuer die innenliegenden Columns.


	virtual void Cut();
	virtual void Paste( SwFrm* pParent, SwFrm* pSibling = 0 );

		//sucht den dichtesten Cntnt zum SPoint, wird bei Seiten, Flys und Cells
		//benutzt wenn GetCrsrOfst versagt hat.
	const SwCntntFrm* GetCntntPos( Point &rPoint, const BOOL bDontLeave,
								   const BOOL bBodyOnly = FALSE,
								   const BOOL bCalc = FALSE,
								   const SwCrsrMoveState *pCMS = 0,
								   const BOOL bDefaultExpand = TRUE ) const;

	SwLayoutFrm( SwFrmFmt* );
	~SwLayoutFrm();

	const SwFrm *Lower() const { return pLower; }
		  SwFrm *Lower()	   { return pLower; }
	const SwCntntFrm *ContainsCntnt() const;
	inline SwCntntFrm *ContainsCntnt();
	const SwCellFrm *FirstCell() const;
	inline SwCellFrm *FirstCell();
	const SwFrm *ContainsAny() const;
	inline SwFrm *ContainsAny();
	BOOL IsAnLower( const SwFrm * ) const;

	const SwFrmFmt *GetFmt() const { return (const SwFrmFmt*)GetDep(); }
		  SwFrmFmt *GetFmt()	   { return (SwFrmFmt*)GetDep(); }
	void 			SetFrmFmt( SwFrmFmt* );

	//Verschieben der Ftns aller Lower - ab dem StartCntnt.
	//TRUE wenn mindestens eine Ftn verschoben wurde.
	//Ruft das Update der Seitennummer wenn bFtnNums gesetzt ist.
	BOOL MoveLowerFtns( SwCntntFrm *pStart, SwFtnBossFrm *pOldBoss,
						SwFtnBossFrm *pNewBoss, const BOOL bFtnNums );

	//Sorgt dafuer, dass innenliegende Flys noetigenfalls zum clippen bzw.
	//reformatieren invalidiert werden.
	void NotifyFlys();

	//Invalidiert diejenigen innenliegenden Frames, deren Breite und/oder
	//Hoehe Prozentual berechnet werden. Auch Rahmen, die an this oder an
	//innenliegenden verankert sind werden ggf. invalidiert.
    void InvaPercentLowers( SwTwips nDiff = 0 );

	//Gerufen von Format fuer Rahmen und Bereichen mit Spalten.
	void FormatWidthCols( const SwBorderAttrs &, const SwTwips nBorder,
						  const SwTwips nMinHeight );

	// InnerHeight returns the height of the content and may be bigger or
	// less than the PrtArea-Height of the layoutframe himself
	SwTwips InnerHeight() const;

    /** method to check relative position of layout frame to
        a given layout frame.

        OD 08.11.2002 - refactoring of pseudo-local method <lcl_Apres(..)> in
        <txtftn.cxx> for #104840#.

        @param _aCheckRefLayFrm
        constant reference of an instance of class <SwLayoutFrm> which
        is used as the reference for the relative position check.

        @author OD

        @return true, if <this> is positioned before the layout frame <p>
    */
    bool IsBefore( const SwLayoutFrm* _pCheckRefLayFrm ) const;
};

//Um doppelte Implementierung zu sparen wird hier ein bischen gecasted
inline SwCntntFrm* SwLayoutFrm::ContainsCntnt()
{
	return (SwCntntFrm*)(((const SwLayoutFrm*)this)->ContainsCntnt());
}

inline SwCellFrm* SwLayoutFrm::FirstCell()
{
	return (SwCellFrm*)(((const SwLayoutFrm*)this)->FirstCell());
}

inline SwFrm* SwLayoutFrm::ContainsAny()
{
	return (SwFrm*)(((const SwLayoutFrm*)this)->ContainsAny());
}

// Diese SwFrm-inlines sind hier, damit frame.hxx nicht layfrm.hxx includen muss
inline BOOL SwFrm::IsColBodyFrm() const
{
    return nType == FRMC_BODY && GetUpper()->IsColumnFrm();
}

inline BOOL SwFrm::IsPageBodyFrm() const
{
    return nType == FRMC_BODY && GetUpper()->IsPageFrm();
}

} //namespace binfilter
#endif	//_LAYFRM_HXX
