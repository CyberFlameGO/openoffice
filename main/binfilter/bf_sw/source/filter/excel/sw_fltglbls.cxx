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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "doc.hxx"
#include "pam.hxx"
#include "swtable.hxx"
#include "fltglbls.hxx"
namespace binfilter {




//------------------------------------------------------------------------
FilterGlobals::FilterGlobals( SwDoc& rDoc, const SwPaM& rPam )
	: aStandard( String::CreateFromAscii("General") ),
	aTblFmts( 0, 10 )
{
	pD = &rDoc;			// "globaler" Dokument-Pointer
	pPam = new SwPaM( *rPam.GetPoint() );
	pTable = 0;
	pNumFormatter = 0;

	eDefLanguage = LANGUAGE_SYSTEM;
	nDefFormat = 0xFFFFFFFF;

	nColStart = nRowStart = 1;
	nColEnd = nRowEnd = 0;
	nAnzCols = nAnzRows = 0;
}

FilterGlobals::~FilterGlobals()
{
	// JP 13.08.98: TabellenUmrandungen optimieren - Bug 53525
	for( USHORT n = aTblFmts.Count(); n; )
	{
		SwTable* pTbl = SwTable::FindTable( (SwFrmFmt*)aTblFmts[ --n ] );
		if( pTbl )
			pTbl->GCBorderLines();
	}

	delete pPam;
}

//------------------------------------------------------------------------
void FilterGlobals::SetRange( USHORT nCS, USHORT nCE, USHORT nRS, USHORT nRE )
{
	if( nCE < nCS )
	{
		if( nCS == 0 )
			nCE = 0;
		else
			nCE = nCS - 1;
	}

	if( nRE < nRS )
	{
		if( nRS == 0 )
			nRE = 0;
		else
			nRE = nRS - 1;
	}

	nColStart = nCS;
	nColEnd = nCE;
	nRowStart = nRS;
	nRowEnd = nRE;
	nAnzCols = 1 + nCE - nCS;
	nAnzRows = 1 + nRE - nRS;
}



BOOL FilterGlobals::ColRangeLimitter( USHORT &rCS, USHORT &rCE )
{
	//	PREC:	rCS <= rCE
	//	POST:	wenn [rCS, rCE] und [nColStart,nColEnd] disjunkte Intervalle,
	//				return = FALSE;
	//			sonst
	//				return = TRUE
	//				rCS = MAX(rCS,nColStart)
	//				rCE = MIN(rCE,nColEnd)
	BOOL bRet;
	DBG_ASSERT( rCS <= rCE,
		"-FilterGlobals::ColRangeLimitter(): Startspalte > Endspalte!" );
	if( rCS > nColEnd || rCE < nColStart )
		bRet = FALSE;
	else
		{
		bRet = TRUE;
		if( rCS < nColStart )	rCS = nColStart;
		if( rCE > nColEnd )		rCE = nColEnd;
		}
	return bRet;
}

void FilterGlobals::InsertText( USHORT nCol, USHORT nRow, const String& rStr )
{
	pPam->GetPoint()->nNode = *pTable->GetTabLines()[ nRow ]->
										GetTabBoxes()[ nCol ]->GetSttNd();
	pPam->Move( fnMoveForward, fnGoCntnt );

	pD->Insert( *pPam, rStr );
}

void FilterGlobals::CreateTable()
{
	pTable = pD->InsertTable( *pPam->GetPoint(),
								AnzRows(),	// nRows
								AnzCols(),	// nCols
								HORI_LEFT );		// SwHoriOrient

	// JP 13.08.98: TabellenUmrandungen optimieren - Bug 53525
	void* p = pTable->GetFrmFmt();
	aTblFmts.Insert( p, aTblFmts.Count() );
}

void FilterGlobals::InsertAttr( const SfxPoolItem& rItem )
{
	SfxItemSet aTxtAttr( pD->GetAttrPool(), rItem.Which(), rItem.Which() );
	aTxtAttr.Put( rItem );
	pD->Insert( *pLotGlob->pPam, aTxtAttr );
}


}
