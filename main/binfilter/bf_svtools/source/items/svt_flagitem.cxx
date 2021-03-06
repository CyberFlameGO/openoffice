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



// MARKER(update_precomp.py): autogen include statement, do not remove


#include <bf_svtools/flagitem.hxx>

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

namespace binfilter
{

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxFlagItem)

USHORT nSfxFlagVal[16] =
{
	0x0001, 0x0002, 0x0004, 0x0008,
	0x0010, 0x0020, 0x0040, 0x0080,
	0x0100, 0x0200, 0x0400, 0x0800,
	0x1000, 0x2000, 0x4000, 0x8000
};


// -----------------------------------------------------------------------

TYPEINIT1(SfxFlagItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxFlagItem::SfxFlagItem( USHORT nW, USHORT nV ) :
	SfxPoolItem( nW ),
	nVal(nV)
{
	DBG_CTOR(SfxFlagItem, 0);
}

// -----------------------------------------------------------------------

SfxFlagItem::SfxFlagItem( const SfxFlagItem& rItem ) :
	SfxPoolItem( rItem ),
	nVal( rItem.nVal )
{
	DBG_CTOR(SfxFlagItem, 0);
}

// -----------------------------------------------------------------------

SvStream& SfxFlagItem::Store(SvStream &rStream, USHORT) const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	rStream << nVal;
	return rStream;
}

// -----------------------------------------------------------------------

SfxItemPresentation SfxFlagItem::GetPresentation
(
	SfxItemPresentation 	/*ePresentation*/,
	SfxMapUnit				/*eCoreMetric*/,
	SfxMapUnit				/*ePresentationMetric*/,
	XubString& 				rText,
    const ::IntlWrapper *
)	const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	rText.Erase();
	for ( BYTE nFlag = 0; nFlag < GetFlagCount(); ++nFlag )
		rText += XubString::CreateFromInt32( GetFlag(nFlag) );
	return SFX_ITEM_PRESENTATION_NAMELESS;
}

// -----------------------------------------------------------------------

XubString SfxFlagItem::GetFlagText( BYTE ) const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	DBG_WARNING( "calling GetValueText(USHORT) on SfxFlagItem -- overload!" );
	return XubString();
}

// -----------------------------------------------------------------------

BYTE SfxFlagItem::GetFlagCount() const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	DBG_WARNING( "calling GetValueText(USHORT) on SfxFlagItem -- overload!" );
	return 0;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxFlagItem::Create(SvStream &, USHORT) const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	DBG_WARNING( "calling Create() on SfxFlagItem -- overload!" );
	return 0;
}

// -----------------------------------------------------------------------

int SfxFlagItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
	return (((SfxFlagItem&)rItem).nVal == nVal);
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxFlagItem::Clone(SfxItemPool *) const
{
	DBG_CHKTHIS(SfxFlagItem, 0);
	return new SfxFlagItem( *this );
}

}
