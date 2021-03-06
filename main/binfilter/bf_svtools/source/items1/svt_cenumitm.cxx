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


#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _SVTOOLS_CENUMITM_HXX
#include <bf_svtools/cenumitm.hxx>
#endif

#ifndef _SFX_WHASSERT_HXX
#include <whassert.hxx>
#endif

#include <comphelper/extract.hxx>

namespace binfilter
{

//============================================================================
//
//  class SfxEnumItemInterface
//
//============================================================================

DBG_NAME(SfxEnumItemInterface)

//============================================================================
TYPEINIT1(SfxEnumItemInterface, SfxPoolItem)

//============================================================================
// virtual
int SfxEnumItemInterface::operator ==(const SfxPoolItem & rItem) const
{
	SFX_ASSERT(SfxPoolItem::operator ==(rItem), Which(), "unequal type");
	return GetEnumValue()
		       == static_cast< const SfxEnumItemInterface * >(&rItem)->
		              GetEnumValue();
}

//============================================================================
// virtual
SfxItemPresentation
SfxEnumItemInterface::GetPresentation(SfxItemPresentation, SfxMapUnit,
									  SfxMapUnit, XubString & rText,
                                      const ::IntlWrapper *) const
{
	rText = XubString::CreateFromInt32(GetEnumValue());
	return SFX_ITEM_PRESENTATION_NAMELESS;
}

//============================================================================
// virtual
BOOL SfxEnumItemInterface::QueryValue(::com::sun::star::uno::Any& rVal, BYTE)
	const
{
	rVal <<= sal_Int32(GetEnumValue());
	return true;
}

//============================================================================
// virtual
BOOL SfxEnumItemInterface::PutValue(const ::com::sun::star::uno::Any& rVal,
									BYTE)
{
	sal_Int32 nTheValue = 0;

	if ( ::cppu::enum2int( nTheValue, rVal ) )
	{
		SetEnumValue(USHORT(nTheValue));
		return true;
	}
	DBG_ERROR("SfxEnumItemInterface::PutValue(): Wrong type");
	return false;
}

//============================================================================
XubString SfxEnumItemInterface::GetValueTextByPos(USHORT) const
{
	DBG_WARNING("SfxEnumItemInterface::GetValueTextByPos(): Pure virtual");
	return XubString();
}

//============================================================================
// virtual
USHORT SfxEnumItemInterface::GetValueByPos(USHORT nPos) const
{
	return nPos;
}

//============================================================================
// virtual
USHORT SfxEnumItemInterface::GetPosByValue(USHORT nValue) const
{
	USHORT nCount = GetValueCount();
	for (USHORT i = 0; i < nCount; ++i)
		if (GetValueByPos(i) == nValue)
			return i;
	return USHRT_MAX;
}

BOOL SfxEnumItemInterface::IsEnabled(USHORT) const
{
	return TRUE;
}

//============================================================================
// virtual
int SfxEnumItemInterface::HasBoolValue() const
{
	return false;
}

//============================================================================
// virtual
BOOL SfxEnumItemInterface::GetBoolValue() const
{
	return false;
}

//============================================================================
// virtual
void SfxEnumItemInterface::SetBoolValue(BOOL)
{}

//============================================================================
//
//  class CntEnumItem
//
//============================================================================

DBG_NAME(CntEnumItem)

//============================================================================
CntEnumItem::CntEnumItem(USHORT which, SvStream & rStream):
	SfxEnumItemInterface(which)
{
	m_nValue = 0;
	rStream >> m_nValue;
}

//============================================================================
TYPEINIT1(CntEnumItem, SfxEnumItemInterface)

//============================================================================
// virtual
SvStream & CntEnumItem::Store(SvStream & rStream, USHORT) const
{
	rStream << m_nValue;
	return rStream;
}

//============================================================================
// virtual
USHORT CntEnumItem::GetEnumValue() const
{
	return GetValue();
}

//============================================================================
// virtual
void CntEnumItem::SetEnumValue(USHORT nTheValue)
{
	SetValue(nTheValue);
}

//============================================================================
//
//  class CntBoolItem
//
//============================================================================

DBG_NAME(CntBoolItem)

//============================================================================
TYPEINIT1_AUTOFACTORY(CntBoolItem, SfxPoolItem)

//============================================================================
CntBoolItem::CntBoolItem(USHORT which, SvStream & rStream):
	SfxPoolItem(which)
{
	m_bValue = false;
	rStream >> m_bValue;
}

//============================================================================
// virtual
int CntBoolItem::operator ==(const SfxPoolItem & rItem) const
{
	DBG_ASSERT(rItem.ISA(CntBoolItem),
			   "CntBoolItem::operator ==(): Bad type");
	return m_bValue == static_cast< CntBoolItem const * >(&rItem)->m_bValue;
}

//============================================================================
// virtual
int CntBoolItem::Compare(const SfxPoolItem & rWith) const
{
	DBG_ASSERT(rWith.ISA(CntBoolItem), "CntBoolItem::Compare(): Bad type");
	return m_bValue == static_cast< CntBoolItem const * >(&rWith)->m_bValue ?
		       0 : m_bValue ? -1 : 1;
}

//============================================================================
// virtual
SfxItemPresentation CntBoolItem::GetPresentation(SfxItemPresentation,
												 SfxMapUnit, SfxMapUnit,
												 UniString & rText,
                                                 const ::IntlWrapper *) const
{
	rText = GetValueTextByVal(m_bValue);
	return SFX_ITEM_PRESENTATION_NAMELESS;
}

//============================================================================
// virtual
BOOL CntBoolItem::QueryValue(com::sun::star::uno::Any& rVal, BYTE) const
{
	rVal <<= sal_Bool(m_bValue);
	return true;
}

//============================================================================
// virtual
BOOL CntBoolItem::PutValue(const com::sun::star::uno::Any& rVal, BYTE)
{
	sal_Bool bTheValue = sal_Bool();
	if (rVal >>= bTheValue)
	{
		m_bValue = bTheValue;
		return true;
	}
	DBG_ERROR("CntBoolItem::PutValue(): Wrong type");
	return false;
}

//============================================================================
// virtual
SfxPoolItem * CntBoolItem::Create(SvStream & rStream, USHORT) const
{
	return new CntBoolItem(Which(), rStream);
}

//============================================================================
// virtual
SvStream & CntBoolItem::Store(SvStream & rStream, USHORT) const
{
	rStream << m_bValue;
	return rStream;
}

//============================================================================
// virtual
SfxPoolItem * CntBoolItem::Clone(SfxItemPool *) const
{
	return new CntBoolItem(*this);
}

//============================================================================
// virtual
USHORT CntBoolItem::GetValueCount() const
{
	return 2;
}

//============================================================================
// virtual
UniString CntBoolItem::GetValueTextByVal(BOOL bTheValue) const
{
	return
		bTheValue ?
		    UniString::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("TRUE")) :
		    UniString::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM("FALSE"));
}

}
