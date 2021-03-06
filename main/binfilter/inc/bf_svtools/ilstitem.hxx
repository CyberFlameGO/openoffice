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



#ifndef _SFXILSTITEM_HXX
#define _SFXILSTITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_SVLDLLAPI_H

#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

namespace binfilter
{
class SvULongs;

class  SfxIntegerListItem : public SfxPoolItem
{
    ::com::sun::star::uno::Sequence < sal_Int32 > m_aList;

public:
    TYPEINFO();

    SfxIntegerListItem();
    SfxIntegerListItem( const SfxIntegerListItem& rItem );
    ~SfxIntegerListItem();

    ::com::sun::star::uno::Sequence < sal_Int32 > GetSequence()
    { return m_aList; }
    ::com::sun::star::uno::Sequence < sal_Int32 > GetConstSequence() const
    { return SAL_CONST_CAST(SfxIntegerListItem *, this)->GetSequence(); }

	virtual int 			operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*    Clone( SfxItemPool *pPool = 0 ) const;
    virtual BOOL            PutValue  ( const com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );
    virtual BOOL            QueryValue( com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
};

}

#endif // _SFXINTITEM_HXX

