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


#ifndef _PORGLUE_HXX
#define _PORGLUE_HXX


//#include <stdlib.h>

#include "porlin.hxx"
namespace binfilter {

class SwRect;
class SwLineLayout;
struct SwPosition;

/*************************************************************************
 *						class SwGluePortion
 *************************************************************************/

class SwGluePortion : public SwLinePortion
{
private:
	KSHORT nFixWidth;
public:
				SwGluePortion( const KSHORT nInitFixWidth );

	void Join( SwGluePortion *pVictim );

	inline short GetPrtGlue() const;
	inline KSHORT GetFixWidth() const { return nFixWidth; }
	inline void SetFixWidth( const KSHORT nNew ) { nFixWidth = nNew; }
	void MoveGlue( SwGluePortion *pTarget, const short nPrtGlue );
	inline void MoveAllGlue( SwGluePortion *pTarget );
	inline void MoveHalfGlue( SwGluePortion *pTarget );
	inline void AdjFixWidth();
	virtual void Paint( const SwTxtPaintInfo &rInf ) const{DBG_BF_ASSERT(0, "STRIP");};//STRIP001 	virtual void Paint( const SwTxtPaintInfo &rInf ) const;

	OUTPUT_OPERATOR
};

/*************************************************************************
 *						class SwFixPortion
 *************************************************************************/

class SwFixPortion : public SwGluePortion
{
	KSHORT nFix;		// der Width-Offset in der Zeile
public:
		SwFixPortion( const SwRect &rFlyRect );
		SwFixPortion( const KSHORT nFixWidth, const KSHORT nFixPos );
	inline void   Fix( const KSHORT nNewFix ) { nFix = nNewFix; }
	inline KSHORT Fix() const { return nFix; }
	OUTPUT_OPERATOR
};

/*************************************************************************
 *				  class SwMarginPortion
 *************************************************************************/

class SwMarginPortion : public SwGluePortion
{
public:
		SwMarginPortion( const KSHORT nFixWidth );
		void AdjustRight( const SwLineLayout* pCurr );
	OUTPUT_OPERATOR
};

/*************************************************************************
 *				  inline SwGluePortion::GetPrtGlue()
 *************************************************************************/

inline short SwGluePortion::GetPrtGlue() const
{ return Width() - nFixWidth; }

/*************************************************************************
 *				inline SwGluePortion::AdjFixWidth()
 * Die FixWidth darf niemals groesser sein als die Gesamtbreite !
 *************************************************************************/

inline void SwGluePortion::AdjFixWidth()
{
	if( nFixWidth > PrtWidth() )
		nFixWidth = PrtWidth();
}

/*************************************************************************
 *				   inline SwGluePortion::MoveGlue()
 *************************************************************************/

inline void SwGluePortion::MoveAllGlue( SwGluePortion *pTarget )
{
	MoveGlue( pTarget, GetPrtGlue() );
}

/*************************************************************************
 *				  inline SwGluePortion::MoveHalfGlue()
 *************************************************************************/

inline void SwGluePortion::MoveHalfGlue( SwGluePortion *pTarget )
{
	MoveGlue( pTarget, GetPrtGlue() / 2 );
}

CLASSIO( SwGluePortion )
CLASSIO( SwFixPortion )
CLASSIO( SwMarginPortion )


} //namespace binfilter
#endif

