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


#ifndef _SVX_FLSTITEM_HXX
#define _SVX_FLSTITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef   ITEMID_FONTLIST
#define   ITEMID_FONTLIST 0
#endif
namespace binfilter {
class FontList;

// class SvxFontListItem -------------------------------------------------


/*
	[Beschreibung]

	Dieses Item dient als Transport-Medium fuer eine FontListe.
	Die Fontliste wird nicht kopiert und nicht geloescht!
*/

class SvxFontListItem : public SfxPoolItem
{
private:
	const FontList*         pFontList;

public:
	TYPEINFO();

	SvxFontListItem( const FontList* pFontLst,
					 const USHORT nId = ITEMID_FONTLIST );
	SvxFontListItem( const SvxFontListItem& rItem );

	virtual int				operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;


	const FontList*			GetFontList() const { return pFontList; }
};

}//end of namespace binfilter
#endif

