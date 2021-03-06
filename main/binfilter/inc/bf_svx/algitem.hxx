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


#ifndef _SVX_ALGITEM_HXX
#define _SVX_ALGITEM_HXX

// include ---------------------------------------------------------------

#include <bf_svx/svxids.hrc>

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <bf_svtools/eitem.hxx>
#endif
#ifndef _SVX_SVXENUM_HXX
#include <bf_svx/svxenum.hxx>
#endif
class SvStream;
namespace binfilter {


//------------------------------------------------------------------------

#ifdef ITEMID_HORJUSTIFY

class SvxHorJustifyItem: public SfxEnumItem
{
public:
	TYPEINFO();

	SvxHorJustifyItem(
		const SvxCellHorJustify eJustify = SVX_HOR_JUSTIFY_STANDARD,
		const USHORT nId = ITEMID_HORJUSTIFY );


	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	virtual USHORT          GetValueCount() const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create( SvStream& rStream, USHORT nVer ) const;

	inline  SvxHorJustifyItem& operator=(const SvxHorJustifyItem& rHorJustify)
			{
				SetValue( rHorJustify.GetValue() );
				return *this;
			}
};

#endif

//------------------------------------------------------------------------

#ifdef ITEMID_VERJUSTIFY

class SvxVerJustifyItem: public SfxEnumItem
{
public:
	TYPEINFO();

	SvxVerJustifyItem(
		const SvxCellVerJustify eJustify = SVX_VER_JUSTIFY_STANDARD,
		const USHORT nId = ITEMID_VERJUSTIFY );


	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	virtual USHORT          GetValueCount() const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create( SvStream& rStream, USHORT nVer ) const;

	inline  SvxVerJustifyItem& operator=(const SvxVerJustifyItem& rVerJustify)
			{
				SetValue( rVerJustify.GetValue() );
				return *this;
			}
};

#endif

//------------------------------------------------------------------------

#ifdef ITEMID_ORIENTATION

class SvxOrientationItem: public SfxEnumItem
{
public:
	TYPEINFO();

	SvxOrientationItem(
		const SvxCellOrientation eOrientation = SVX_ORIENTATION_STANDARD,
		const USHORT nId = ITEMID_ORIENTATION );


	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	virtual USHORT          GetValueCount() const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*    Create( SvStream& rStream, USHORT nVer ) const;

	inline  SvxOrientationItem& operator=(const SvxOrientationItem& rOrientation)
			{
				SetValue( rOrientation.GetValue() );
				return *this;
			}
};

#endif

//------------------------------------------------------------------------

#ifdef ITEMID_MARGIN

class SvxMarginItem: public SfxPoolItem
{
	INT16       nLeftMargin;
	INT16       nTopMargin;
	INT16       nRightMargin;
	INT16       nBottomMargin;
public:
	TYPEINFO();
	SvxMarginItem( const USHORT nId = ITEMID_MARGIN );
	SvxMarginItem( INT16 nLeft, INT16 nTop = 0,
				   INT16 nRight = 0, INT16 nBottom = 0,
				   const USHORT nId = ITEMID_MARGIN );
	SvxMarginItem( const SvxMarginItem& );


	virtual int              operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*     Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*     Create( SvStream& rStream, USHORT nVer ) const;
	virtual SvStream&		 Store( SvStream&, USHORT nItemVersion ) const;

	virtual	sal_Bool        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	sal_Bool			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

			INT16           GetLeftMargin() const {return nLeftMargin; }
			INT16           GetTopMargin() const {return nTopMargin; }
			INT16           GetRightMargin() const {return nRightMargin; }
			INT16           GetBottomMargin() const {return nBottomMargin; }

	inline  SvxMarginItem& operator=(const SvxMarginItem& rMargin)
			{
				nLeftMargin = rMargin.nLeftMargin;
				nTopMargin = rMargin.nTopMargin;
				nRightMargin = rMargin.nRightMargin;
				nBottomMargin = rMargin.nBottomMargin;
				return *this;
			}
};

#endif

}//end of namespace binfilter
#endif

