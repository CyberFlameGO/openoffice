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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "notesuno.hxx"
#include "textuno.hxx"
#include "cellsuno.hxx"		// getParent
#include "docsh.hxx"
#include "docfunc.hxx"
#include "hints.hxx"
#include "editsrc.hxx"
#include "miscuno.hxx"

// setVisible:
#include <bf_svx/svdundo.hxx>
#include "drwlayer.hxx"
#include "detfunc.hxx"
#include "undocell.hxx"
#include "unoguard.hxx"
namespace binfilter {

using namespace ::com::sun::star;

//------------------------------------------------------------------------

//	keine Properties fuer Text in Notizen
const SfxItemPropertyMap* lcl_GetAnnotationPropertyMap()
{
	static SfxItemPropertyMap aAnnotationPropertyMap_Impl[] =
	{
		{0,0,0,0}
	};
	return aAnnotationPropertyMap_Impl;
}

//------------------------------------------------------------------------

SC_SIMPLE_SERVICE_INFO( ScAnnotationObj, "ScAnnotationObj", "com.sun.star.sheet.CellAnnotation" )

//------------------------------------------------------------------------

ScAnnotationObj::ScAnnotationObj(ScDocShell* pDocSh, const ScAddress& rPos) :
	pDocShell( pDocSh ),
	aCellPos( rPos ),
	pUnoText( NULL )
{
	pDocShell->GetDocument()->AddUnoObject(*this);

	//	pUnoText is allocated on demand (GetUnoText)
	//	can't be aggregated because getString/setString is handled here
}

SvxUnoText&	ScAnnotationObj::GetUnoText()
{
	if (!pUnoText)
	{
		ScAnnotationEditSource aEditSource( pDocShell, aCellPos );
		pUnoText = new SvxUnoText( &aEditSource, lcl_GetAnnotationPropertyMap(),
									uno::Reference<text::XText>() );
		pUnoText->acquire();
	}
	return *pUnoText;
}

ScAnnotationObj::~ScAnnotationObj()
{
	if (pDocShell)
		pDocShell->GetDocument()->RemoveUnoObject(*this);

	if (pUnoText)
		pUnoText->release();
}

void ScAnnotationObj::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
{
	if ( rHint.ISA( ScUpdateRefHint ) )
	{
		const ScUpdateRefHint& rRef = (const ScUpdateRefHint&)rHint;

		//!	Ref-Update
	}
	else if ( rHint.ISA( SfxSimpleHint ) &&
			((const SfxSimpleHint&)rHint).GetId() == SFX_HINT_DYING )
	{
		pDocShell = NULL;		// ungueltig geworden
	}
}


// XChild

uno::Reference<uno::XInterface> SAL_CALL ScAnnotationObj::getParent() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;

	//	Parent der Notiz ist die zugehoerige Zelle
	//!	existierendes Objekt finden und zurueckgeben ???

	if (pDocShell)
		return (cppu::OWeakObject*)new ScCellObj( pDocShell, aCellPos );

	return NULL;
}

void SAL_CALL ScAnnotationObj::setParent( const uno::Reference<uno::XInterface>& Parent )
									throw(lang::NoSupportException, uno::RuntimeException)
{
	//	hamma nich
	//!	Exception oder so ??!
}

// XSimpleText

uno::Reference<text::XTextCursor> SAL_CALL ScAnnotationObj::createTextCursor()
													throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	//	Notizen brauchen keine Extrawurst
	return GetUnoText().createTextCursor();
}

uno::Reference<text::XTextCursor> SAL_CALL ScAnnotationObj::createTextCursorByRange(
									const uno::Reference<text::XTextRange>& aTextPosition )
													throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	//	Notizen brauchen keine Extrawurst
	return GetUnoText().createTextCursorByRange(aTextPosition);
}

::rtl::OUString SAL_CALL ScAnnotationObj::getString() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	ScPostIt aNote;
	if ( pDocShell )
	{
		ScDocument* pDoc = pDocShell->GetDocument();
		pDoc->GetNote( aCellPos.Col(), aCellPos.Row(), aCellPos.Tab(), aNote );
	}
	return aNote.GetText();
}

void SAL_CALL ScAnnotationObj::setString( const ::rtl::OUString& aText ) throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	if ( pDocShell )
	{
		String aString = aText;
		ScDocFunc aFunc(*pDocShell);
		aFunc.SetNoteText( aCellPos, aString, TRUE );

		// don't create pUnoText here if not there
		if (pUnoText)
			pUnoText->SetSelection(ESelection( 0,0, 0,aString.Len() ));
	}
}

void SAL_CALL ScAnnotationObj::insertString( const uno::Reference<text::XTextRange>& xRange,
											const ::rtl::OUString& aString, sal_Bool bAbsorb )
								throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	GetUnoText().insertString( xRange, aString, bAbsorb );
}

void SAL_CALL ScAnnotationObj::insertControlCharacter( const uno::Reference<text::XTextRange>& xRange,
											sal_Int16 nControlCharacter, sal_Bool bAbsorb )
								throw(lang::IllegalArgumentException, uno::RuntimeException)
{
	ScUnoGuard aGuard;
	GetUnoText().insertControlCharacter( xRange, nControlCharacter, bAbsorb );
}

uno::Reference<text::XText> SAL_CALL ScAnnotationObj::getText() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return GetUnoText().getText();
}

uno::Reference<text::XTextRange> SAL_CALL ScAnnotationObj::getStart() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return GetUnoText().getStart();
}

uno::Reference<text::XTextRange> SAL_CALL ScAnnotationObj::getEnd() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	return GetUnoText().getEnd();
}

// XSheetAnnotation

table::CellAddress SAL_CALL ScAnnotationObj::getPosition() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	table::CellAddress aAdr;
	aAdr.Sheet	= aCellPos.Tab();
	aAdr.Column	= aCellPos.Col();
	aAdr.Row	= aCellPos.Row();
	return aAdr;
}

::rtl::OUString SAL_CALL ScAnnotationObj::getAuthor() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	ScPostIt aNote;
	if ( pDocShell )
	{
		ScDocument* pDoc = pDocShell->GetDocument();
		pDoc->GetNote( aCellPos.Col(), aCellPos.Row(), aCellPos.Tab(), aNote );
	}
	return aNote.GetAuthor();
}

::rtl::OUString SAL_CALL ScAnnotationObj::getDate() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	ScPostIt aNote;
	if ( pDocShell )
	{
		ScDocument* pDoc = pDocShell->GetDocument();
		pDoc->GetNote( aCellPos.Col(), aCellPos.Row(), aCellPos.Tab(), aNote );
	}
	return aNote.GetDate();
}

sal_Bool SAL_CALL ScAnnotationObj::getIsVisible() throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	ScPostIt aNote;
	if ( pDocShell )
	{
		ScDocument* pDoc = pDocShell->GetDocument();
		pDoc->GetNote( aCellPos.Col(), aCellPos.Row(), aCellPos.Tab(), aNote );
	}
	return aNote.IsShown();
}

void SAL_CALL ScAnnotationObj::setIsVisible( sal_Bool bIsVisible ) throw(uno::RuntimeException)
{
	ScUnoGuard aGuard;
	BOOL bDone = FALSE;
	if ( pDocShell )
	{
		//!	Funktion an docfunc oder so

		BOOL bSet = bIsVisible ? TRUE : FALSE;
		ScDocument* pDoc = pDocShell->GetDocument();
		USHORT nCol = aCellPos.Col();
		USHORT nRow = aCellPos.Row();
		USHORT nTab = aCellPos.Tab();
		ScPostIt aNote;
		if ( pDoc->GetNote( nCol, nRow, nTab, aNote ) )
		{
			BOOL bHad = pDoc->HasNoteObject( nCol, nRow, nTab );
			if ( bHad != bSet )
			{
				pDocShell->MakeDrawLayer();
				ScDrawLayer* pModel = pDoc->GetDrawLayer();

				ScDetectiveFunc aFunc( pDoc,nTab );
				if ( bSet )
					bDone = ( aFunc.ShowComment( nCol, nRow, FALSE ) != NULL );
				else
					bDone = aFunc.HideComment( nCol, nRow );
				if (bDone)
				{
					aNote.SetShown( bSet );
					pDoc->SetNote( nCol, nRow, nTab, aNote );

					pDocShell->SetDocumentModified();
				}
			}
		}
	}
}

//------------------------------------------------------------------------




}
