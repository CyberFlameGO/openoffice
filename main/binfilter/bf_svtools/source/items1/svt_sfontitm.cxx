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


#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _VCOMPAT_HXX
#include <tools/vcompat.hxx>
#endif

#ifndef _SFONTITM_HXX
#include <bf_svtools/sfontitm.hxx>
#endif

namespace binfilter
{

//============================================================================
//
//  class SfxFontItem
//
//============================================================================

TYPEINIT1(SfxFontItem, SfxPoolItem);

//============================================================================
// virtual
int SfxFontItem::operator ==(const SfxPoolItem & rItem) const
{
	const SfxFontItem * pFontItem = PTR_CAST(SfxFontItem, &rItem);
	return pFontItem && m_bHasFont == pFontItem->m_bHasFont
	       && m_bHasColor == pFontItem->m_bHasColor
	       && m_bHasFillColor == pFontItem->m_bHasFillColor
	       && (!m_bHasColor || m_aColor == pFontItem->m_aColor)
	       && (!m_bHasFillColor || m_aFillColor == pFontItem->m_aFillColor)
	       && (!m_bHasFont || (m_bKerning == pFontItem->m_bKerning
			                  && m_bShadow == pFontItem->m_bShadow
			                  && m_bOutline == pFontItem->m_bOutline
			                  && m_bWordLine == pFontItem->m_bWordLine
			                  && m_nOrientation == pFontItem->m_nOrientation
			                  && m_nStrikeout == pFontItem->m_nStrikeout
			                  && m_nUnderline == pFontItem->m_nUnderline
			                  && m_nItalic == pFontItem->m_nItalic
			                  && m_nWidthType == pFontItem->m_nWidthType
			                  && m_nWeight == pFontItem->m_nWeight
			                  && m_nPitch == pFontItem->m_nPitch
			                  && m_nFamily == pFontItem->m_nFamily
			                  && m_nLanguage == pFontItem->m_nLanguage
			                  && m_nCharSet == pFontItem->m_nCharSet
			                  && m_aFillColor == pFontItem->m_aFillColor
			                  && m_aColor == pFontItem->m_aColor
			                  && m_aSize == pFontItem->m_aSize
			                  && m_aStyleName == pFontItem->m_aStyleName
			                  && m_aName == pFontItem->m_aName));
}

//============================================================================
// virtual
SfxPoolItem * SfxFontItem::Create(SvStream & rStream, USHORT) const
{
	VersionCompat aItemCompat(rStream, STREAM_READ);
	SfxFontItem * pItem = new SfxFontItem(Which());
	{
		VersionCompat aFontCompat(rStream, STREAM_READ);
		readByteString(rStream, pItem->m_aName);
		readByteString(rStream, pItem->m_aStyleName);
		rStream >> pItem->m_aSize;
		sal_Int16 nCharSet = 0;
		rStream >> nCharSet;
		pItem->m_nCharSet = rtl_TextEncoding(nCharSet);
		rStream >> pItem->m_nFamily >> pItem->m_nPitch >> pItem->m_nWeight
				>> pItem->m_nUnderline >> pItem->m_nStrikeout
				>> pItem->m_nItalic;
		sal_Int16 nLanguage = 0;
		rStream >> nLanguage;
		pItem->m_nLanguage = LanguageType(nLanguage);
		rStream >> pItem->m_nWidthType >> pItem->m_nOrientation;
		sal_Int8 nWordLine = 0;
		rStream >> nWordLine;
		pItem->m_bWordLine = nWordLine != 0;
		sal_Int8 nOutline = 0;
		rStream >> nOutline;
		pItem->m_bOutline = nOutline != 0;
		sal_Int8 nShadow = 0;
		rStream >> nShadow;
		pItem->m_bShadow = nShadow != 0;
		sal_Int8 nKerning = 0;
		rStream >> nKerning;
		pItem->m_bKerning = nKerning != 0;
	}
	pItem->m_aColor.Read(rStream, TRUE);
	pItem->m_aFillColor.Read(rStream, TRUE);
	sal_Int16 nFlags = 0;
	rStream >> nFlags;
	pItem->m_bHasFont = (nFlags & 4) != 0;
	pItem->m_bHasColor = (nFlags & 1) != 0;
	pItem->m_bHasFillColor = (nFlags & 2) != 0;
	return pItem;
}

//============================================================================
// virtual
SvStream & SfxFontItem::Store(SvStream & rStream, USHORT) const
{
	VersionCompat aItemCompat(rStream, STREAM_WRITE, 1);
	{
		VersionCompat aFontCompat(rStream, STREAM_WRITE, 1);
		writeByteString(rStream, m_aName);
		writeByteString(rStream, m_aStyleName);
		rStream << m_aSize << sal_Int16(m_nCharSet)
				<< m_nFamily << m_nPitch << m_nWeight << m_nUnderline
				<< m_nStrikeout << m_nItalic << sal_Int16(m_nLanguage)
				<< m_nWidthType << m_nOrientation << sal_Int8(m_bWordLine)
				<< sal_Int8(m_bOutline) << sal_Int8(m_bShadow)
				<< sal_Int8(m_bKerning);
	}
	SAL_CONST_CAST(Color &, m_aColor).Write(rStream, TRUE);
	SAL_CONST_CAST(Color &, m_aFillColor).Write(rStream, TRUE);
	rStream << sal_Int16(m_bHasFont << 2 | m_bHasColor
						  | m_bHasFillColor << 1);
	return rStream;
}

}
