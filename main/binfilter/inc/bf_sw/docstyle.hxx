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


#ifndef _DOCSTYLE_HXX
#define _DOCSTYLE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_STRINGSDTOR
#include <bf_svtools/svstdarr.hxx>
#endif

#ifndef _SFXSTYLE_HXX //autogen
#include <bf_svtools/style.hxx>
#endif

#ifndef _SFXITEMSET_HXX //autogen
#include <bf_svtools/itemset.hxx>
#endif
namespace binfilter {

class SwDoc;
class SwDocStyleSheetPool;
class SwPageDesc;
class SwCharFmt;
class SwTxtFmtColl;
class SwFrmFmt;
class SwNumRule;

/*--------------------------------------------------------------------
	Beschreibung:	Lokale Hilfsklasse
 --------------------------------------------------------------------*/
class SwPoolFmtList : public SvStringsDtor
{
public:
	SwPoolFmtList() {}
	void Append( char cChar, const String& rStr );
	void Erase();
};


/*--------------------------------------------------------------------
	Beschreibung:	temp. StyleSheet
 --------------------------------------------------------------------*/
class SwDocStyleSheet : public SfxStyleSheetBase
{
	friend class SwDocStyleSheetPool;
	friend class SwStyleSheetIterator;

	SwCharFmt*			pCharFmt;
	SwTxtFmtColl* 		pColl;
	SwFrmFmt* 			pFrmFmt;
	const SwPageDesc* 	pDesc;
	const SwNumRule*	pNumRule;

	SwDoc&				rDoc;
	SfxItemSet			aCoreSet;

	BOOL 				bPhysical;


	// leere Huelse zum richtigen StyleSheet (Core) machen
	void				Create();

	// den StyleSheet mit Daten fuellen
	enum FillStyleType {
		FillOnlyName,
		FillAllInfo,
		FillPhysical
	};
	BOOL 				FillStyleSheet( FillStyleType eFType );


public:
	SwDocStyleSheet( SwDoc& 				rDoc,
					 const String& 			rName,
					 SwDocStyleSheetPool& 	rPool,
					 SfxStyleFamily 		eFam,
					 USHORT 				nMask);

	SwDocStyleSheet( const SwDocStyleSheet& );
	virtual ~SwDocStyleSheet();

	void					Reset();

	void 					SetMask(USHORT nMsk)			{ nMask = nMsk;   	}
	void 					SetFamily(SfxStyleFamily eFam)	{ nFamily = eFam;	}

	BOOL		   			IsPhysical() const				{ return bPhysical; }
	void					SetPhysical(BOOL bPhys);

	void					SetItemSet(const SfxItemSet& rSet);

	virtual SfxItemSet& 	GetItemSet();
	virtual const String& 	GetParent() const;
	virtual const String& 	GetFollow() const;


	// Vorbelegen der member ohne physikalischen Zugriff
	// wird vom StyleSheetPool benutzt
	//
	void			  		PresetName(const String& rName)  { aName   = rName; }
	void			  		PresetNameAndFamily(const String& rName);
	void					PresetParent(const String& rName){ aParent = rName; }
	void					PresetFollow(const String& rName){ aFollow = rName; }

	virtual BOOL 			SetParent( const String& rStr);
	virtual BOOL 			SetFollow( const String& rStr);


	SwCharFmt*		   		GetCharFmt();
	SwTxtFmtColl* 			GetCollection();
	SwFrmFmt* 				GetFrmFmt();
	const SwPageDesc* 		GetPageDesc();
	const SwNumRule*		GetNumRule();
	void					SetNumRule(const SwNumRule& rRule);

};

/*--------------------------------------------------------------------
	Beschreibung:	Iterator fuer den Pool
 --------------------------------------------------------------------*/

class SwStyleSheetIterator : public SfxStyleSheetIterator, public SfxListener
{
	SwDocStyleSheet 	aIterSheet;
	SwDocStyleSheet 	aStyleSheet;
	SwPoolFmtList 		aLst;
	USHORT 				nLastPos;
	BOOL 				bFirstCalled;

	void				AppendStyleList(const SvStringsDtor& rLst,
										BOOL 	bUsed,
										USHORT 	nSection,
										char	cType);

public:
	SwStyleSheetIterator( SwDocStyleSheetPool* pBase,
						  SfxStyleFamily eFam, USHORT n=0xFFFF );
	virtual ~SwStyleSheetIterator();

	virtual SfxStyleSheetBase* First();
	virtual SfxStyleSheetBase* Next();

	virtual void Notify( SfxBroadcaster&, const SfxHint& );
};

/*--------------------------------------------------------------------
	Beschreibung:	Pool fuer
 --------------------------------------------------------------------*/

class SwDocStyleSheetPool : public SfxStyleSheetBasePool
{
	SwDocStyleSheet 	aStyleSheet;
	SwDoc&				rDoc;
	BOOL				bOrganizer : 1;		// TRUE: fuer den Organizer


public:
	SwDocStyleSheetPool( SwDoc&, BOOL bOrganizer = FALSE );
	virtual ~SwDocStyleSheetPool();

	virtual SfxStyleSheetBase& Make(const String&, SfxStyleFamily, USHORT nMask, USHORT nPos = 0xffff);
	virtual SfxStyleSheetBase* Find( const String&, SfxStyleFamily eFam,
									USHORT n=0xFFFF );
	virtual BOOL SetParent( SfxStyleFamily eFam, const String &rStyle,
							const String &rParent );

	void	SetItemSet(const SfxItemSet& rSet) { aStyleSheet.SetItemSet(rSet); }

	void	SetOrganizerMode( BOOL bMode )	{ bOrganizer = bMode; }
	BOOL 	IsOrganizerMode() const 		{ return bOrganizer; }

	virtual SfxStyleSheetIterator* CreateIterator( SfxStyleFamily,
													USHORT nMask );

	SwDoc& GetDoc() const { return rDoc; }

	//Fuer die daemlicheren Compiler
private:
	SwDocStyleSheetPool( const SwDocStyleSheetPool& );
};


} //namespace binfilter
#endif
