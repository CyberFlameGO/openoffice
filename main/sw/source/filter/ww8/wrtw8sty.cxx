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
#include "precompiled_sw.hxx"

/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#include <algorithm>
#include <functional>

#include <boost/scoped_array.hpp>

#include <com/sun/star/i18n/ScriptType.hdl>
#include <rtl/tencinfo.h>
#include <hintids.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/fontitem.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdotext.hxx>
#include <svx/svdotext.hxx>
#include <svx/fmglob.hxx>
#include <editeng/frmdiritem.hxx>
#include <editeng/lrspitem.hxx>
#include <editeng/ulspitem.hxx>
#include <doc.hxx>
#include <wrtww8.hxx>
#include <docary.hxx>
#include <poolfmt.hxx>
#include <fmtpdsc.hxx>
#include <pagedesc.hxx>
#include <ndtxt.hxx>
#include <ftninfo.hxx>
#include <fmthdft.hxx>
#include <section.hxx>
#include <fmtcntnt.hxx>
#include <fmtftn.hxx>
#include <ndindex.hxx>
#include <txtftn.hxx>
#include <charfmt.hxx>
#include <docufld.hxx>
#include <dcontact.hxx>
#include <fmtcnct.hxx>
#include <ftnidx.hxx>
#include <fmtclds.hxx>
#include <lineinfo.hxx>
#include <fmtline.hxx>
#include <swtable.hxx>
#include <msfilter.hxx>
#include <swmodule.hxx>

#include <writerfilter/doctok/sprmids.hxx>

#include "writerhelper.hxx"
#include "writerwordglue.hxx"
#include "../inc/wwstyles.hxx"
#include "ww8par.hxx"
#include "ww8attributeoutput.hxx"
#include "docxattributeoutput.hxx"
#include "rtfattributeoutput.hxx"

using namespace sw::util;
using namespace nsHdFtFlags;

/// For the output of sections.
struct WW8_PdAttrDesc
{
    ::boost::scoped_array<sal_uInt8> m_pData;
    sal_uInt16 m_nLen;
    WW8_FC m_nSepxFcPos;
    WW8_PdAttrDesc() : m_nLen(0), m_nSepxFcPos(0xffffffff) /*default: none*/
        { }
};

struct WW8_SED
{
    SVBT16 aBits1;      // orientation change + internal, Default: 6
    SVBT32 fcSepx;      //  FC  file offset to beginning of SEPX for section.
                        //  0xFFFFFFFF for no Sprms
    SVBT16 fnMpr;       //  used internally by Windows Word, Default: 0
    SVBT32 fcMpr;       //  FC, points to offset in FC space for MacWord
                        // Default: 0xffffffff ( nothing )
                        //  cbSED is 12 (decimal)), C (hex).
};

SV_IMPL_VARARR( WW8_WrSepInfoPtrs, WW8_SepInfo )

// class WW8_WrPlc0 ist erstmal nur fuer Header / Footer-Positionen, d.h. es
// gibt keine inhaltstragende Struktur.
class WW8_WrPlc0
{
private:
    SvULongs aPos;      // PTRARR von CPs / FCs
    sal_uLong nOfs;

    //No copying
    WW8_WrPlc0(const WW8_WrPlc0&);
    WW8_WrPlc0 &operator=(const WW8_WrPlc0&);
public:
    WW8_WrPlc0( sal_uLong nOffset );
    sal_uInt16 Count() const                { return aPos.Count(); }
    void Append( sal_uLong nStartCpOrFc );
    void Write( SvStream& rStrm );
};

//------------------------------------------------------------
//  Styles
//------------------------------------------------------------

#define WW8_RESERVED_SLOTS 15

// GetId( SwCharFmt ) zur Benutzung im Text -> nil verboten,
// "Default Char Style" stattdessen
sal_uInt16 MSWordExportBase::GetId( const SwCharFmt& rFmt ) const
{
    sal_uInt16 nRet = pStyles->GetSlot( rFmt );
    return ( nRet != 0x0fff ) ? nRet : 10;      // Default Char Style
}

// GetId( SwTxtFmtColl ) zur Benutzung an TextNodes -> nil verboten,
// "Standard" stattdessen
sal_uInt16 MSWordExportBase::GetId( const SwTxtFmtColl& rColl ) const
{
    sal_uInt16 nRet = pStyles->GetSlot( rColl );
    return ( nRet != 0xfff ) ? nRet : 0;        // Default TxtFmtColl
}



//typedef pFmtT
MSWordStyles::MSWordStyles( MSWordExportBase& rExport )
    : m_rExport( rExport )
{
    // if exist any Foot-/End-Notes then get from the EndNoteInfo struct
    // the CharFormats. They will create it!
    if ( m_rExport.pDoc->GetFtnIdxs().Count() )
    {
        m_rExport.pDoc->GetEndNoteInfo().GetAnchorCharFmt( *m_rExport.pDoc );
        m_rExport.pDoc->GetEndNoteInfo().GetCharFmt( *m_rExport.pDoc );
        m_rExport.pDoc->GetFtnInfo().GetAnchorCharFmt( *m_rExport.pDoc );
        m_rExport.pDoc->GetFtnInfo().GetCharFmt( *m_rExport.pDoc );
    }
    sal_uInt16 nAlloc = WW8_RESERVED_SLOTS + m_rExport.pDoc->GetCharFmts()->Count() - 1 +
                                         m_rExport.pDoc->GetTxtFmtColls()->Count() - 1;

    // etwas grosszuegig ( bis zu 15 frei )
    pFmtA = new SwFmt*[ nAlloc ];
    memset( pFmtA, 0, nAlloc * sizeof( SwFmt* ) );

    BuildStylesTable();
}

MSWordStyles::~MSWordStyles()
{
    delete[] pFmtA;
}

// Sty_SetWWSlot() fuer Abhaengigkeiten der Styles -> nil ist erlaubt
sal_uInt16 MSWordStyles::GetSlot( const SwFmt& rFmt ) const
{
    sal_uInt16 n;
    for ( n = 0; n < nUsedSlots; n++ )
        if ( pFmtA[n] == &rFmt )
            return n;
    return 0xfff;                   // 0xfff: WW: nil
}

sal_uInt16 MSWordStyles::BuildGetSlot( const SwFmt& rFmt )
{
    sal_uInt16 nRet;
    switch ( nRet = rFmt.GetPoolFmtId() )
    {
        case RES_POOLCOLL_STANDARD:
            nRet = 0;
            break;

        case RES_POOLCOLL_HEADLINE1:
        case RES_POOLCOLL_HEADLINE2:
        case RES_POOLCOLL_HEADLINE3:
        case RES_POOLCOLL_HEADLINE4:
        case RES_POOLCOLL_HEADLINE5:
        case RES_POOLCOLL_HEADLINE6:
        case RES_POOLCOLL_HEADLINE7:
        case RES_POOLCOLL_HEADLINE8:
        case RES_POOLCOLL_HEADLINE9:
            nRet -= RES_POOLCOLL_HEADLINE1-1;
            break;

        //case RES_POOLCHR_FOOTNOTE_ANCHOR:   nRet =
        //case RES_POOLCHR_ENDNOTE_ANCHOR:
        default:
            nRet = nUsedSlots++;
            break;
    }
    return nRet;
}

sal_uInt16 MSWordStyles::GetWWId( const SwFmt& rFmt ) const
{
    sal_uInt16 nRet = ww::stiUser;    // User-Style als default
    sal_uInt16 nPoolId = rFmt.GetPoolFmtId();
    if( nPoolId == RES_POOLCOLL_STANDARD )
        nRet = 0;
    else if( nPoolId >= RES_POOLCOLL_HEADLINE1 &&
             nPoolId <= RES_POOLCOLL_HEADLINE9 )
        nRet = static_cast< sal_uInt16 >(nPoolId + 1 - RES_POOLCOLL_HEADLINE1);
    else if( nPoolId >= RES_POOLCOLL_TOX_IDX1 &&
             nPoolId <= RES_POOLCOLL_TOX_IDX3 )
        nRet = static_cast< sal_uInt16 >(nPoolId + 10 - RES_POOLCOLL_TOX_IDX1);
    else if( nPoolId >= RES_POOLCOLL_TOX_CNTNT1 &&
             nPoolId <= RES_POOLCOLL_TOX_CNTNT5 )
        nRet = static_cast< sal_uInt16 >(nPoolId + 19 - RES_POOLCOLL_TOX_CNTNT1);
    else if( nPoolId >= RES_POOLCOLL_TOX_CNTNT6 &&
             nPoolId <= RES_POOLCOLL_TOX_CNTNT9 )
        nRet = static_cast< sal_uInt16 >(nPoolId + 24 - RES_POOLCOLL_TOX_CNTNT6);
    else
        switch( nPoolId )
        {
        case RES_POOLCOLL_FOOTNOTE:         nRet = 29;  break;
        case RES_POOLCOLL_HEADER:           nRet = 31;  break;
        case RES_POOLCOLL_FOOTER:           nRet = 32;  break;
        case RES_POOLCOLL_TOX_IDXH:         nRet = 33;  break;
        case RES_POOLCOLL_JAKETADRESS:      nRet = 36;  break;
        case RES_POOLCOLL_SENDADRESS:       nRet = 37;  break;
        case RES_POOLCOLL_ENDNOTE:          nRet = 43;  break;
        case RES_POOLCOLL_LISTS_BEGIN:      nRet = 47;  break;
        case RES_POOLCOLL_DOC_TITEL:        nRet = 62;  break;
        case RES_POOLCOLL_SIGNATURE:        nRet = 64;  break;
        case RES_POOLCOLL_TEXT:             nRet = 66;  break;
        case RES_POOLCOLL_TEXT_MOVE:        nRet = 67;  break;
        case RES_POOLCOLL_DOC_SUBTITEL:     nRet = 74;  break;
        case RES_POOLCOLL_TEXT_IDENT:       nRet = 77;  break;

        case RES_POOLCHR_FOOTNOTE_ANCHOR:   nRet = 38;  break;
        case RES_POOLCHR_ENDNOTE_ANCHOR:    nRet = 42;  break;
        case RES_POOLCHR_INET_NORMAL:       nRet = 85;  break;
        case RES_POOLCHR_INET_VISIT:        nRet = 86;  break;
        case RES_POOLCHR_HTML_STRONG:       nRet = 87;  break;
        case RES_POOLCHR_HTML_EMPHASIS:     nRet = 88;  break;
        case RES_POOLCHR_LINENUM:           nRet = 40;  break;
        case RES_POOLCHR_PAGENO:            nRet = 41;  break;
        }
    return nRet;
}

void MSWordStyles::BuildStylesTable()
{
    nUsedSlots = WW8_RESERVED_SLOTS;    // soviele sind reserviert fuer
                                        // Standard und HeadingX u.a.
    SwFmt* pFmt;
    sal_uInt16 n;
    const SvPtrarr& rArr = *m_rExport.pDoc->GetCharFmts();       // erst CharFmt
    // das Default-ZeichenStyle ( 0 ) wird nicht mit ausgegeben !
    for( n = 1; n < rArr.Count(); n++ )
    {
        pFmt = (SwFmt*)rArr[n];
        pFmtA[ BuildGetSlot( *pFmt ) ] = pFmt;
    }
    
    const SvPtrarr& rArr2 = *m_rExport.pDoc->GetTxtFmtColls();   // dann TxtFmtColls
    // das Default-TextStyle ( 0 ) wird nicht mit ausgegeben !
    for( n = 1; n < rArr2.Count(); n++ )
    {
        pFmt = (SwFmt*)rArr2[n];
        pFmtA[ BuildGetSlot( *pFmt ) ] = pFmt;
    }
}

/// For WW8 only - extend pO so that the size of pTableStrm is even.
static void impl_SkipOdd( WW8Bytes* pO, sal_Size nTableStrmTell )
{
    if ( ( nTableStrmTell + pO->Count() ) & 1 )     // Start auf gerader
        pO->Insert( (sal_uInt8)0, pO->Count() );         // Adresse
}

void WW8AttributeOutput::EndStyle()
{
    impl_SkipOdd( m_rWW8Export.pO, m_rWW8Export.pTableStrm->Tell() );

    short nLen = m_rWW8Export.pO->Count() - 2;            // Laenge des Styles
    sal_uInt8* p = (sal_uInt8*)m_rWW8Export.pO->GetData() + nPOPosStdLen1;
    ShortToSVBT16( nLen, p );               // nachtragen
    p = (sal_uInt8*)m_rWW8Export.pO->GetData() + nPOPosStdLen2;
    ShortToSVBT16( nLen, p );               // dito

    m_rWW8Export.pTableStrm->Write( m_rWW8Export.pO->GetData(), m_rWW8Export.pO->Count() );      // ins File damit
    m_rWW8Export.pO->Remove( 0, m_rWW8Export.pO->Count() );                   // leeren fuer naechsten
}

void WW8AttributeOutput::StartStyle( const String& rName, bool bPapFmt, sal_uInt16 nWwBase,
    sal_uInt16 nWwNext, sal_uInt16 nWwId, sal_uInt16 /*nId*/ )
{
    sal_uInt8 aWW8_STD[ sizeof( WW8_STD ) ];
    sal_uInt8* pData = aWW8_STD;
    memset( &aWW8_STD, 0, sizeof( WW8_STD ) );

    sal_uInt16 nBit16 = 0x1000;         // fInvalHeight
    nBit16 |= (ww::stiNil & nWwId);
    Set_UInt16( pData, nBit16 );

    nBit16 = nWwBase << 4;          // istdBase
    nBit16 |= bPapFmt ? 1 : 2;      // sgc
    Set_UInt16( pData, nBit16 );

    nBit16 = nWwNext << 4;          // istdNext
    nBit16 |= bPapFmt ? 2 : 1;      // cupx
    Set_UInt16( pData, nBit16 );

    pData += sizeof( sal_uInt16 );      // bchUpe

    if( m_rWW8Export.bWrtWW8 )
    {
        //-------- jetzt neu:
        // ab Ver8 gibts zwei Felder mehr:
        //sal_uInt16    fAutoRedef : 1;    /* auto redefine style when appropriate */
        //sal_uInt16    fHidden : 1;       /* hidden from UI? */
        //sal_uInt16    : 14;              /* unused bits */
        pData += sizeof( sal_uInt16 );
    }


    sal_uInt16 nLen = static_cast< sal_uInt16 >( ( pData - aWW8_STD ) + 1 +
                ((m_rWW8Export.bWrtWW8 ? 2 : 1 ) * (rName.Len() + 1)) );  // vorlaeufig

    WW8Bytes* pO = m_rWW8Export.pO;
    nPOPosStdLen1 = pO->Count();        // Adr1 zum nachtragen der Laenge

    SwWW8Writer::InsUInt16( *pO, nLen );
    pO->Insert( aWW8_STD, static_cast< sal_uInt16 >( pData - aWW8_STD ), pO->Count() );

    nPOPosStdLen2 = nPOPosStdLen1 + 8;  // Adr2 zum nachtragen von "end of upx"

    // Namen schreiben
    if( m_rWW8Export.bWrtWW8 )
    {
        SwWW8Writer::InsUInt16( *pO, rName.Len() ); // Laenge
        SwWW8Writer::InsAsString16( *pO, rName );
    }
    else
    {
        pO->Insert( (sal_uInt8)rName.Len(), pO->Count() );       // Laenge
        SwWW8Writer::InsAsString8( *pO, rName, RTL_TEXTENCODING_MS_1252 );
    }
    pO->Insert( (sal_uInt8)0, pO->Count() );             // Trotz P-String 0 am Ende!
}

void MSWordStyles::SetStyleDefaults( const SwFmt& rFmt, bool bPap )
{
    const SwModify* pOldMod = m_rExport.pOutFmtNode;
    m_rExport.pOutFmtNode = &rFmt;
    bool aFlags[ static_cast< sal_uInt16 >(RES_FRMATR_END) - RES_CHRATR_BEGIN ];
    sal_uInt16 nStt, nEnd, n;
    if( bPap )
       nStt = RES_PARATR_BEGIN, nEnd = RES_FRMATR_END;
    else
       nStt = RES_CHRATR_BEGIN, nEnd = RES_TXTATR_END;

    // dynamic defaults
    const SfxItemPool& rPool = *rFmt.GetAttrSet().GetPool();
    for( n = nStt; n < nEnd; ++n )
        aFlags[ n - RES_CHRATR_BEGIN ] = 0 != rPool.GetPoolDefaultItem( n );

    // static defaults, that differs between WinWord and SO
    if( bPap )
    {
        aFlags[ static_cast< sal_uInt16 >(RES_PARATR_WIDOWS) - RES_CHRATR_BEGIN ] = 1;
        aFlags[ static_cast< sal_uInt16 >(RES_PARATR_HYPHENZONE) - RES_CHRATR_BEGIN ] = 1;
    }
    else
    {
        aFlags[ RES_CHRATR_FONTSIZE - RES_CHRATR_BEGIN ] = 1;
        aFlags[ RES_CHRATR_LANGUAGE - RES_CHRATR_BEGIN ] = 1;
    }

    const SfxItemSet* pOldI = m_rExport.GetCurItemSet();
    m_rExport.SetCurItemSet( &rFmt.GetAttrSet() );

    const bool* pFlags = aFlags + ( nStt - RES_CHRATR_BEGIN );
    for ( n = nStt; n < nEnd; ++n, ++pFlags )
    {
        if ( *pFlags && SFX_ITEM_SET != rFmt.GetItemState(n, false))
        {
            //If we are a character property then see if it is one of the
            //western/asian ones that must be collapsed together for export to
            //word. If so default to the western varient.
            if ( bPap || m_rExport.CollapseScriptsforWordOk(
                i18n::ScriptType::LATIN, n) )
            {
                m_rExport.AttrOutput().OutputItem( rFmt.GetFmtAttr( n, true ) );
            }
        }
    }

    m_rExport.SetCurItemSet( pOldI );
    m_rExport.pOutFmtNode = pOldMod;
}

void WW8AttributeOutput::StartStyleProperties( bool bParProp, sal_uInt16 nStyle )
{
    WW8Bytes* pO = m_rWW8Export.pO;

    impl_SkipOdd( pO, m_rWW8Export.pTableStrm->Tell() );

    sal_uInt16 nLen = ( bParProp ) ? 2 : 0;             // Default-Laenge
    m_nStyleLenPos = pO->Count();               // Laenge zum Nachtragen
                                    // Keinen Pointer merken, da sich bei
                                    // _grow der Pointer aendert !

    SwWW8Writer::InsUInt16( *pO, nLen );        // Style-Len

    m_nStyleStartSize = pO->Count();

    if ( bParProp )
        SwWW8Writer::InsUInt16( *pO, nStyle );     // Style-Nummer
}

void MSWordStyles::WriteProperties( const SwFmt* pFmt, bool bParProp, sal_uInt16 nPos,
    bool bInsDefCharSiz )
{
    m_rExport.AttrOutput().StartStyleProperties( bParProp, nPos );

    ASSERT( m_rExport.pCurrentStyle == NULL, "Current style not NULL" ); // set current style before calling out
    m_rExport.pCurrentStyle = pFmt;

    m_rExport.OutputFormat( *pFmt, bParProp, !bParProp );

    ASSERT( m_rExport.pCurrentStyle == pFmt, "current style was changed" );
    // reset current style...
    m_rExport.pCurrentStyle = NULL;

    if ( bInsDefCharSiz  )                   // nicht abgeleitet v. anderem Style
        SetStyleDefaults( *pFmt, bParProp );

    m_rExport.AttrOutput().EndStyleProperties( bParProp );
}

void WW8AttributeOutput::EndStyleProperties( bool /*bParProp*/ )
{
    WW8Bytes* pO = m_rWW8Export.pO;

    sal_uInt16 nLen = pO->Count() - m_nStyleStartSize;
    sal_uInt8* pUpxLen = (sal_uInt8*)pO->GetData() + m_nStyleLenPos; // Laenge zum Nachtragen
    ShortToSVBT16( nLen, pUpxLen );                 // Default-Laenge eintragen
}

void MSWordStyles::GetStyleData( SwFmt* pFmt, bool& bFmtColl, sal_uInt16& nBase, sal_uInt16& nNext )
{
    bFmtColl = pFmt->Which() == RES_TXTFMTCOLL || pFmt->Which() == RES_CONDTXTFMTCOLL;

    // Default: none
    nBase = 0xfff;

    // Derived from?
    if ( !pFmt->IsDefault() )
        nBase = GetSlot( *pFmt->DerivedFrom() );

    SwFmt* pNext;
    if ( bFmtColl )
        pNext = &((SwTxtFmtColl*)pFmt)->GetNextTxtFmtColl();
    else
        pNext = pFmt; // CharFmt: next CharFmt == self

    nNext = GetSlot( *pNext );
}

void WW8AttributeOutput::DefaultStyle( sal_uInt16 nStyle )
{
    if ( nStyle == 10 )           // Default Char-Style ( nur WW )
    {
        if ( m_rWW8Export.bWrtWW8 )
        {
            sal_uInt16 n = 0;
            m_rWW8Export.pTableStrm->Write( &n , 2 );   // empty Style
        }
        else
        {
            static sal_uInt8 __READONLY_DATA aDefCharSty[] = {
                0x26, 0x00,
                0x41, 0x40, 0xF2, 0xFF, 0xA1, 0x00, 0x26, 0x00,
                0x19, 0x41, 0x62, 0x73, 0x61, 0x74, 0x7A, 0x2D,
                0x53, 0x74, 0x61, 0x6E, 0x64, 0x61, 0x72, 0x64,
                0x73, 0x63, 0x68, 0x72, 0x69, 0x66, 0x74, 0x61,
                0x72, 0x74, 0x00, 0x00, 0x00, 0x00 };
            m_rWW8Export.pTableStrm->Write( &aDefCharSty, sizeof( aDefCharSty ) );
        }
    }
    else
    {
        sal_uInt16 n = 0;
        m_rWW8Export.pTableStrm->Write( &n , 2 );   // empty Style
    }
}

// OutputStyle geht fuer TxtFmtColls und CharFmts
void MSWordStyles::OutputStyle( SwFmt* pFmt, sal_uInt16 nPos )
{
    if ( !pFmt )
        m_rExport.AttrOutput().DefaultStyle( nPos );
    else
    {
        bool bFmtColl;
        sal_uInt16 nBase, nWwNext;
        
        GetStyleData( pFmt, bFmtColl, nBase, nWwNext );

        m_rExport.AttrOutput().StartStyle( pFmt->GetName(), bFmtColl,
                nBase, nWwNext, GetWWId( *pFmt ), nPos );
        
        if ( bFmtColl )
            WriteProperties( pFmt, true, nPos, nBase==0xfff );           // UPX.papx

        WriteProperties( pFmt, false, nPos, bFmtColl && nBase==0xfff );  // UPX.chpx

        m_rExport.AttrOutput().EndStyle();
    }
}

void WW8AttributeOutput::StartStyles()
{
    WW8Fib& rFib = *m_rWW8Export.pFib;

    sal_uLong nCurPos = m_rWW8Export.pTableStrm->Tell();
    if ( nCurPos & 1 )                   // Start auf gerader
    {
        *m_rWW8Export.pTableStrm << (char)0;        // Adresse
        ++nCurPos;
    }
    rFib.fcStshfOrig = rFib.fcStshf = nCurPos;
    m_nStyAnzPos = nCurPos + 2;     // Anzahl wird nachgetragen

    if ( m_rWW8Export.bWrtWW8 )
    {
        static sal_uInt8 __READONLY_DATA aStShi[] = {
            0x12, 0x00,
            0x0F, 0x00, 0x0A, 0x00, 0x01, 0x00, 0x5B, 0x00,
            0x0F, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00 };

        m_rWW8Export.pTableStrm->Write( &aStShi, sizeof( aStShi ) );
    }
    else
    {
        static sal_uInt8 __READONLY_DATA aStShi[] = {
            0x0E, 0x00,
            0x0F, 0x00, 0x08, 0x00, 0x01, 0x00, 0x4B, 0x00,
            0x0F, 0x00, 0x00, 0x00, 0x00, 0x00 };
        m_rWW8Export.pTableStrm->Write( &aStShi, sizeof( aStShi ) );
    }
}

void WW8AttributeOutput::EndStyles( sal_uInt16 nNumberOfStyles )
{
    WW8Fib& rFib = *m_rWW8Export.pFib;

    rFib.lcbStshfOrig = rFib.lcbStshf = m_rWW8Export.pTableStrm->Tell() - rFib.fcStshf;
    SwWW8Writer::WriteShort( *m_rWW8Export.pTableStrm, m_nStyAnzPos, nNumberOfStyles );
}

void MSWordStyles::OutputStylesTable()
{
    m_rExport.bStyDef = true;

    m_rExport.AttrOutput().StartStyles();

    sal_uInt16 n;
    for ( n = 0; n < nUsedSlots; n++ )
        OutputStyle( pFmtA[n], n );

    m_rExport.AttrOutput().EndStyles( nUsedSlots );

    m_rExport.bStyDef = false;
}

/*  */

//---------------------------------------------------------------------------
//          Fonts
//---------------------------------------------------------------------------
wwFont::wwFont(const String &rFamilyName, FontPitch ePitch, FontFamily eFamily,
    rtl_TextEncoding eChrSet, bool bWrtWW8) : mbAlt(false), mbWrtWW8(bWrtWW8), mePitch(ePitch), meFamily(eFamily), meChrSet(eChrSet)
{
    FontMapExport aResult(rFamilyName);
    msFamilyNm = aResult.msPrimary;
    msAltNm = aResult.msSecondary;
    if (msAltNm.Len() && msAltNm != msFamilyNm &&
        (msFamilyNm.Len() + msAltNm.Len() + 2 <= 65) )
    {
        //max size of szFfn in 65 chars
        mbAlt = true;
    }

    memset(maWW8_FFN, 0, sizeof(maWW8_FFN));

    if (bWrtWW8)
    {
        maWW8_FFN[0] = (sal_uInt8)( 6 - 1 + 0x22 + ( 2 * ( 1 + msFamilyNm.Len() ) ));
        if (mbAlt)
            maWW8_FFN[0] = static_cast< sal_uInt8 >(maWW8_FFN[0] + 2 * ( 1 + msAltNm.Len()));
    }
    else
    {
        maWW8_FFN[0] = (sal_uInt8)( 6 - 1 + 1 + msFamilyNm.Len() );
        if (mbAlt)
            maWW8_FFN[0] = static_cast< sal_uInt8 >(maWW8_FFN[0] + 1 + msAltNm.Len());
    }

    sal_uInt8 aB = 0;
    switch(ePitch)
    {
        case PITCH_VARIABLE:
            aB |= 2;    // aF.prg = 2
            break;
        case PITCH_FIXED:
            aB |= 1;
            break;
        default:        // aF.prg = 0 : DEFAULT_PITCH (windows.h)
            break;
    }
    aB |= 1 << 2;   // aF.fTrueType = 1; weiss ich nicht besser;

    switch(eFamily)
    {
        case FAMILY_ROMAN:
            aB |= 1 << 4;   // aF.ff = 1;
            break;
        case FAMILY_SWISS:
            aB |= 2 << 4;   // aF.ff = 2;
            break;
        case FAMILY_MODERN:
            aB |= 3 << 4;   // aF.ff = 3;
            break;
        case FAMILY_SCRIPT:
            aB |= 4 << 4;   // aF.ff = 4;
            break;
        case FAMILY_DECORATIVE:
            aB |= 5 << 4;   // aF.ff = 5;
            break;
        default:            // aF.ff = 0; FF_DONTCARE (windows.h)
            break;
    }
    maWW8_FFN[1] = aB;

    ShortToSVBT16( 400, &maWW8_FFN[2] );        // weiss ich nicht besser
                                                // 400 == FW_NORMAL (windows.h)
    maWW8_FFN[4] = sw::ms::rtl_TextEncodingToWinCharset(eChrSet);

    if (mbAlt)
        maWW8_FFN[5] = static_cast< sal_uInt8 >(msFamilyNm.Len() + 1);
}

bool wwFont::Write(SvStream *pTableStrm) const
{
    pTableStrm->Write(maWW8_FFN, sizeof(maWW8_FFN));    // fixed part
    if (mbWrtWW8)
    {
        // ab Ver8 sind folgende beiden Felder eingeschoben,
        // werden von uns ignoriert.
        //char  panose[ 10 ];       //  0x6   PANOSE
        //char  fs[ 24     ];       //  0x10  FONTSIGNATURE
        SwWW8Writer::FillCount(*pTableStrm, 0x22);
        SwWW8Writer::WriteString16(*pTableStrm, msFamilyNm, true);
        if (mbAlt)
            SwWW8Writer::WriteString16(*pTableStrm, msAltNm, true);
    }
    else
    {
        SwWW8Writer::WriteString8(*pTableStrm, msFamilyNm, true,
            RTL_TEXTENCODING_MS_1252);
        if (mbAlt)
        {
            SwWW8Writer::WriteString8( *pTableStrm, msAltNm, true,
                RTL_TEXTENCODING_MS_1252);
        }
    }
    return true;
}

#ifdef DOCX
void wwFont::WriteDocx( const DocxAttributeOutput* rAttrOutput ) const
{
    // no font embedding, panose id, subsetting, ... implemented

    rAttrOutput->StartFont( msFamilyNm );

    if ( mbAlt )
        rAttrOutput->FontAlternateName( msAltNm );
    rAttrOutput->FontCharset( sw::ms::rtl_TextEncodingToWinCharset( meChrSet ) );
    rAttrOutput->FontFamilyType( meFamily );
    rAttrOutput->FontPitchType( mePitch );

    rAttrOutput->EndFont();
}
#endif

void wwFont::WriteRtf( const RtfAttributeOutput* rAttrOutput ) const
{
    rAttrOutput->FontFamilyType( meFamily, *this );
    rAttrOutput->FontPitchType( mePitch );
    rAttrOutput->FontCharset( sw::ms::rtl_TextEncodingToWinCharset( meChrSet ) );
    rAttrOutput->StartFont( msFamilyNm );
    if ( mbAlt )
        rAttrOutput->FontAlternateName( msAltNm );
    rAttrOutput->EndFont();
}

bool operator<(const wwFont &r1, const wwFont &r2)
{
    int nRet = memcmp(r1.maWW8_FFN, r2.maWW8_FFN, sizeof(r1.maWW8_FFN));
    if (nRet == 0)
    {
        StringCompare eRet = r1.msFamilyNm.CompareTo(r2.msFamilyNm);
        if (eRet == COMPARE_EQUAL)
            eRet = r1.msAltNm.CompareTo(r2.msAltNm);
        nRet = eRet;
    }
    return nRet < 0;
}


sal_uInt16 wwFontHelper::GetId(const wwFont &rFont)
{
    sal_uInt16 nRet;
    ::std::map<wwFont, sal_uInt16>::const_iterator aIter = maFonts.find(rFont);
    if (aIter != maFonts.end())
        nRet = aIter->second;
    else
    {
        nRet = static_cast< sal_uInt16 >(maFonts.size());
        maFonts[rFont] = nRet;
    }
    return nRet;
}

void wwFontHelper::InitFontTable(bool bWrtWW8,const SwDoc& rDoc)
{
    mbWrtWW8 = bWrtWW8;

    GetId(wwFont(CREATE_CONST_ASC("Times New Roman"), PITCH_VARIABLE,
        FAMILY_ROMAN, RTL_TEXTENCODING_MS_1252,bWrtWW8));

    GetId(wwFont(CREATE_CONST_ASC("Symbol"), PITCH_VARIABLE, FAMILY_ROMAN,
        RTL_TEXTENCODING_SYMBOL,bWrtWW8));

    GetId(wwFont(CREATE_CONST_ASC("Arial"), PITCH_VARIABLE, FAMILY_SWISS,
        RTL_TEXTENCODING_MS_1252,bWrtWW8));

    const SvxFontItem* pFont = (const SvxFontItem*)GetDfltAttr(RES_CHRATR_FONT);

    GetId(wwFont(pFont->GetFamilyName(), pFont->GetPitch(),
        pFont->GetFamily(), pFont->GetCharSet(),bWrtWW8));

    const SfxItemPool& rPool = rDoc.GetAttrPool();
    if (0 != (pFont = (const SvxFontItem*)rPool.GetPoolDefaultItem(RES_CHRATR_FONT)))
    {
        GetId(wwFont(pFont->GetFamilyName(), pFont->GetPitch(),
            pFont->GetFamily(), pFont->GetCharSet(),bWrtWW8));
    }

    if (!bLoadAllFonts)
        return;

    const sal_uInt16 aTypes[] = { RES_CHRATR_FONT, RES_CHRATR_CJK_FONT, RES_CHRATR_CTL_FONT, 0 };
    for (const sal_uInt16* pId = aTypes; *pId; ++pId)
    {
        sal_uInt32 const nMaxItem = rPool.GetItemCount2( *pId );
        for (sal_uInt32 nGet = 0; nGet < nMaxItem; ++nGet)
        {
            pFont = (const SvxFontItem*)rPool.GetItem2( *pId, nGet );
            if (0 != pFont)
            {
                GetId(wwFont(pFont->GetFamilyName(), pFont->GetPitch(),
                            pFont->GetFamily(), pFont->GetCharSet(),bWrtWW8));
            }
        }
    }
}

sal_uInt16 wwFontHelper::GetId(const Font& rFont)
{
    wwFont aFont(rFont.GetName(), rFont.GetPitch(), rFont.GetFamily(),
        rFont.GetCharSet(), mbWrtWW8);
    return GetId(aFont);
}

sal_uInt16 wwFontHelper::GetId(const SvxFontItem& rFont)
{
    wwFont aFont(rFont.GetFamilyName(), rFont.GetPitch(), rFont.GetFamily(),
        rFont.GetCharSet(), mbWrtWW8);
    return GetId(aFont);
}

::std::vector< const wwFont* > wwFontHelper::AsVector() const
{
    ::std::vector<const wwFont *> aFontList( maFonts.size() );

    typedef ::std::map<wwFont, sal_uInt16>::const_iterator myiter;
    myiter aEnd = maFonts.end();
    for ( myiter aIter = maFonts.begin(); aIter != aEnd; ++aIter )
        aFontList[aIter->second] = &aIter->first;

    return aFontList;
}

void wwFontHelper::WriteFontTable(SvStream *pTableStream, WW8Fib& rFib)
{
    rFib.fcSttbfffn = pTableStream->Tell();
    /*
     * Reserve some space to fill in the len after we know how big it is
     */
    if (mbWrtWW8)
        SwWW8Writer::WriteLong(*pTableStream, 0);
    else
        SwWW8Writer::WriteShort(*pTableStream, 0);

    /*
     * Convert from fast insertion map to linear vector in the order that we
     * want to write.
     */
    ::std::vector<const wwFont *> aFontList( AsVector() );

    /*
     * Write them all to pTableStream
     */
    ::std::for_each(aFontList.begin(), aFontList.end(),
        ::std::bind2nd(::std::mem_fun(&wwFont::Write),pTableStream));

    /*
     * Write the position and len in the FIB
     */
    rFib.lcbSttbfffn = pTableStream->Tell() - rFib.fcSttbfffn;
    if (mbWrtWW8)
        SwWW8Writer::WriteLong( *pTableStream, rFib.fcSttbfffn, maFonts.size());
    else
    {
        SwWW8Writer::WriteShort( *pTableStream, rFib.fcSttbfffn,
            (sal_Int16)rFib.lcbSttbfffn );
    }
}

#ifdef DOCX
void wwFontHelper::WriteFontTable( const DocxAttributeOutput& rAttrOutput )
{
    ::std::vector<const wwFont *> aFontList( AsVector() );

    ::std::for_each( aFontList.begin(), aFontList.end(),
        ::std::bind2nd( ::std::mem_fun( &wwFont::WriteDocx ), &rAttrOutput ) );
}
#endif

void wwFontHelper::WriteFontTable( const RtfAttributeOutput& rAttrOutput )
{
    ::std::vector<const wwFont *> aFontList( AsVector() );

    ::std::for_each( aFontList.begin(), aFontList.end(),
        ::std::bind2nd( ::std::mem_fun( &wwFont::WriteRtf ), &rAttrOutput ) );
}

/*  */

WW8_WrPlc0::WW8_WrPlc0( sal_uLong nOffset )
    : aPos( 4, 4 ), nOfs( nOffset )
{
}

void WW8_WrPlc0::Append( sal_uLong nStartCpOrFc )
{
    aPos.Insert( nStartCpOrFc - nOfs, aPos.Count() );
}

void WW8_WrPlc0::Write( SvStream& rStrm )
{
    sal_uInt16 nLen = aPos.Count();
    for( sal_uInt16 i = 0; i < nLen; ++i )
    {
        SVBT32 nP;
        UInt32ToSVBT32( aPos[i], nP );
        rStrm.Write( nP, 4 );
    }
}

//------------------------------------------------------------------------------

/*  */
//------------------------------------------------------------------------------
// class MSWordSections : Uebersetzung PageDescs in Sections
//      behandelt auch Header und Footer
//------------------------------------------------------------------------------

MSWordSections::MSWordSections( MSWordExportBase& rExport )
    : mbDocumentIsProtected( false ),
      aSects( 4, 4 )
{
    const SwSectionFmt *pFmt = 0;
    rExport.pAktPageDesc = &const_cast<const SwDoc *>(rExport.pDoc)->GetPageDesc( 0 );

    const SfxPoolItem* pI;
    const SwNode* pNd = rExport.pCurPam->GetCntntNode();
    const SfxItemSet* pSet = pNd ? &((SwCntntNode*)pNd)->GetSwAttrSet() : 0;

    sal_uLong nRstLnNum =  pSet ? ((SwFmtLineNumber&)pSet->Get( RES_LINENUMBER )).GetStartValue() : 0;

    const SwTableNode* pTblNd = rExport.pCurPam->GetNode()->FindTableNode();
    const SwSectionNode* pSectNd;
    if ( pTblNd )
    {
        pSet = &pTblNd->GetTable().GetFrmFmt()->GetAttrSet();
        pNd = pTblNd;
    }
    else if ( 0 != ( pSectNd = pNd->FindSectionNode() ) )
    {
        if ( TOX_HEADER_SECTION == pSectNd->GetSection().GetType() &&
             pSectNd->StartOfSectionNode()->IsSectionNode() )
        {
            pSectNd = pSectNd->StartOfSectionNode()->GetSectionNode();
        }

        if ( TOX_CONTENT_SECTION == pSectNd->GetSection().GetType() )
        {
            pNd = pSectNd;
            rExport.pCurPam->GetPoint()->nNode = *pNd;
        }

        if ( CONTENT_SECTION == pSectNd->GetSection().GetType() )
            pFmt = pSectNd->GetSection().GetFmt();
    }

    // Hole evtl. Pagedesc des 1. Nodes
    if ( pSet &&
         SFX_ITEM_ON == pSet->GetItemState( RES_PAGEDESC, true, &pI ) &&
         ( (SwFmtPageDesc*)pI )->GetPageDesc() )
    {
        AppendSection( *(SwFmtPageDesc*)pI, *pNd, pFmt, nRstLnNum );
    }
    else
        AppendSection( rExport.pAktPageDesc, pFmt, nRstLnNum );
}

WW8_WrPlcSepx::WW8_WrPlcSepx( MSWordExportBase& rExport )
    : MSWordSections( rExport )
    , aCps( 4, 4 )
    , m_bHeaderFooterWritten(false)
    , pTxtPos( 0 )
{
    // to be in sync with the AppendSection() call in the MSWordSections
    // constructor
    aCps.Insert( sal_uLong( 0 ), aCps.Count() );
}

MSWordSections::~MSWordSections()
{
}

WW8_WrPlcSepx::~WW8_WrPlcSepx()
{
    delete pTxtPos;
}

bool MSWordSections::HeaderFooterWritten()
{
    return false; // only relevant for WW8
}

bool WW8_WrPlcSepx::HeaderFooterWritten()
{
    return m_bHeaderFooterWritten;
}

sal_uInt16 MSWordSections::CurrentNumberOfColumns( const SwDoc &rDoc ) const
{
    ASSERT( aSects.Count(), "no segement inserted yet" );
    if ( !aSects.Count() )
        return 1;

    return NumberOfColumns( rDoc, aSects[aSects.Count() - 1] );
}

sal_uInt16 MSWordSections::NumberOfColumns( const SwDoc &rDoc, const WW8_SepInfo& rInfo ) const
{
    const SwPageDesc* pPd = rInfo.pPageDesc;
    if ( !pPd )
        pPd = &rDoc.GetPageDesc( 0 );

    if ( !pPd )
    {
        ASSERT( pPd, "totally impossible" );
        return 1;
    }

    const SfxItemSet &rSet = pPd->GetMaster().GetAttrSet();
    SfxItemSet aSet( *rSet.GetPool(), RES_COL, RES_COL );
    aSet.SetParent( &rSet );

    //0xffffffff, what the hell is going on with that!, fixme most terribly
    if ( rInfo.pSectionFmt && (SwSectionFmt*)0xFFFFFFFF != rInfo.pSectionFmt )
        aSet.Put( rInfo.pSectionFmt->GetFmtAttr( RES_COL ) );

    const SwFmtCol& rCol = (const SwFmtCol&)aSet.Get( RES_COL );
    const SwColumns& rColumns = rCol.GetColumns();
    return rColumns.Count();
}

const WW8_SepInfo* MSWordSections::CurrentSectionInfo()
{
    if ( aSects.Count() > 0 )
        return &aSects[aSects.Count() - 1];

    return NULL;
}

void MSWordSections::AppendSection( const SwPageDesc* pPd,
    const SwSectionFmt* pSectionFmt, sal_uLong nLnNumRestartNo )
{
    if (HeaderFooterWritten()) {
        return; // #i117955# prevent new sections in endnotes
    }
    aSects.Insert( WW8_SepInfo( pPd, pSectionFmt, nLnNumRestartNo ),
            aSects.Count() );
    NeedsDocumentProtected( aSects[aSects.Count()-1] );
}

void WW8_WrPlcSepx::AppendSep( WW8_CP nStartCp, const SwPageDesc* pPd,
    const SwSectionFmt* pSectionFmt, sal_uLong nLnNumRestartNo )
{
    if (HeaderFooterWritten()) {
        return; // #i117955# prevent new sections in endnotes
    }
    aCps.Insert( nStartCp, aCps.Count() );

    AppendSection( pPd, pSectionFmt, nLnNumRestartNo );
}

void MSWordSections::AppendSection( const SwFmtPageDesc& rPD,
    const SwNode& rNd, const SwSectionFmt* pSectionFmt, sal_uLong nLnNumRestartNo )
{
    if (HeaderFooterWritten()) {
        return; // #i117955# prevent new sections in endnotes
    }
    WW8_SepInfo aI( rPD.GetPageDesc(), pSectionFmt, nLnNumRestartNo,
            rPD.GetNumOffset(), &rNd );
    aSects.Insert( aI, aSects.Count() );
    NeedsDocumentProtected( aI );
}

void WW8_WrPlcSepx::AppendSep( WW8_CP nStartCp, const SwFmtPageDesc& rPD,
    const SwNode& rNd, const SwSectionFmt* pSectionFmt, sal_uLong nLnNumRestartNo )
{
    if (HeaderFooterWritten()) {
        return; // #i117955# prevent new sections in endnotes
    }
    aCps.Insert(nStartCp, aCps.Count());

    AppendSection( rPD, rNd, pSectionFmt, nLnNumRestartNo );
}

// MSWordSections::SetNum() setzt in jeder Section beim 1. Aufruf den
// Num-Pointer, alle folgenden Aufrufe werden ignoriert. Damit wird
// die erste Aufzaehlung einer Section uebernommen.

void MSWordSections::SetNum( const SwTxtNode* pNumNd )
{
    WW8_SepInfo& rInfo = aSects[ aSects.Count() - 1 ];
    if ( !rInfo.pNumNd ) // noch nicht belegt
        rInfo.pNumNd = pNumNd;
}

void WW8_WrPlcSepx::WriteFtnEndTxt( WW8Export& rWrt, sal_uLong nCpStt )
{
    sal_uInt8 nInfoFlags = 0;
    const SwFtnInfo& rInfo = rWrt.pDoc->GetFtnInfo();
    if( rInfo.aErgoSum.Len() )  nInfoFlags |= 0x02;
    if( rInfo.aQuoVadis.Len() ) nInfoFlags |= 0x04;

    sal_uInt8 nEmptyStt = rWrt.bWrtWW8 ? 0 : 6;
    if( nInfoFlags )
    {
        if( rWrt.bWrtWW8 )
            pTxtPos->Append( nCpStt );  // empty footenote separator

        if( 0x02 & nInfoFlags )         // Footenote contiunation separator
        {
            pTxtPos->Append( nCpStt );
            rWrt.WriteStringAsPara( rInfo.aErgoSum );
            rWrt.WriteStringAsPara( aEmptyStr );
            nCpStt = rWrt.Fc2Cp( rWrt.Strm().Tell() );
        }
        else if( rWrt.bWrtWW8 )
            pTxtPos->Append( nCpStt );

        if( 0x04 & nInfoFlags )         // Footenote contiunation notice
        {
            pTxtPos->Append( nCpStt );
            rWrt.WriteStringAsPara( rInfo.aQuoVadis );
            rWrt.WriteStringAsPara( aEmptyStr );
            nCpStt = rWrt.Fc2Cp( rWrt.Strm().Tell() );
        }
        else if( rWrt.bWrtWW8 )
            pTxtPos->Append( nCpStt );

        if( rWrt.bWrtWW8 )
            nEmptyStt = 3;
        else
            rWrt.pDop->grpfIhdt = nInfoFlags;
    }

    while( 6 > nEmptyStt++ )
        pTxtPos->Append( nCpStt );

    // gleich die Flags am Dop setzen
    WW8Dop& rDop = *rWrt.pDop;
    // Footnote Info
    switch( rInfo.eNum )
    {
    case FTNNUM_PAGE:       rDop.rncFtn = 2; break;
    case FTNNUM_CHAPTER:    rDop.rncFtn  = 1; break;
    default: rDop.rncFtn  = 0; break;
    }                                   // rncFtn
    rDop.nfcFtnRef = WW8Export::GetNumId( rInfo.aFmt.GetNumberingType() );
    rDop.nFtn = rInfo.nFtnOffset + 1;
    rDop.fpc = rWrt.bFtnAtTxtEnd ? 2 : 1;

    // Endnote Info
    rDop.rncEdn = 0;                        // rncEdn: Don't Restart
    const SwEndNoteInfo& rEndInfo = rWrt.pDoc->GetEndNoteInfo();
    rDop.nfcEdnRef = WW8Export::GetNumId( rEndInfo.aFmt.GetNumberingType() );
    rDop.nEdn = rEndInfo.nFtnOffset + 1;
    rDop.epc = rWrt.bEndAtTxtEnd ? 3 : 0;
}

void MSWordSections::SetHeaderFlag( sal_uInt8& rHeadFootFlags, const SwFmt& rFmt,
    sal_uInt8 nFlag )
{
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rFmt.GetItemState(RES_HEADER, true, &pItem)
        && ((SwFmtHeader*)pItem)->IsActive() &&
        ((SwFmtHeader*)pItem)->GetHeaderFmt() )
        rHeadFootFlags |= nFlag;
}

void MSWordSections::SetFooterFlag( sal_uInt8& rHeadFootFlags, const SwFmt& rFmt,
    sal_uInt8 nFlag )
{
    const SfxPoolItem* pItem;
    if( SFX_ITEM_SET == rFmt.GetItemState(RES_FOOTER, true, &pItem)
        && ((SwFmtFooter*)pItem)->IsActive() &&
        ((SwFmtFooter*)pItem)->GetFooterFmt() )
        rHeadFootFlags |= nFlag;
}

void WW8_WrPlcSepx::OutHeaderFooter( WW8Export& rWrt, bool bHeader,
				     const SwFmt& rFmt, sal_uLong& rCpPos, sal_uInt8 nHFFlags, 
				     sal_uInt8 nFlag,  sal_uInt8 nBreakCode)
{
    if ( nFlag & nHFFlags )
    {
        pTxtPos->Append( rCpPos );
        rWrt.WriteHeaderFooterText( rFmt, bHeader);
        rWrt.WriteStringAsPara( aEmptyStr ); // CR ans Ende ( sonst mault WW )
        rCpPos = rWrt.Fc2Cp( rWrt.Strm().Tell() );
    }
    else if ( rWrt.bWrtWW8 )
    {
        pTxtPos->Append( rCpPos );
        if (rWrt.bHasHdr && nBreakCode!=0)
        {
            rWrt.WriteStringAsPara( aEmptyStr ); // Empty paragraph for empty header/footer
            rWrt.WriteStringAsPara( aEmptyStr ); // a CR that WW8 needs for end of the stream
            rCpPos = rWrt.Fc2Cp( rWrt.Strm().Tell() );
        }
    }
}

void MSWordSections::NeedsDocumentProtected(const WW8_SepInfo &rInfo)
{
    if (rInfo.IsProtected())
        mbDocumentIsProtected = true;
}

bool WW8_SepInfo::IsProtected() const
{
    bool bRet = false;
    if (
         pSectionFmt &&
         ((SwSectionFmt*)0xFFFFFFFF != pSectionFmt)
       )
    {
        const SwSection *pSection = pSectionFmt->GetSection();
        if (pSection && pSection->IsProtect())
        {
            bRet = true;
        }
    }
    return bRet;
}


void MSWordSections::CheckForFacinPg( WW8Export& rWrt ) const
{
    // 2 Werte werden gesetzt
    //      Dop.fFacingPages            == Kopf-/Fusszeilen unterschiedlich
    //      Dop.fSwapBordersFacingPgs   == gespiegelte Raender
    for( sal_uInt16 i = 0, nEnde = 0; i < aSects.Count(); ++i )
    {
        WW8_SepInfo& rSepInfo = aSects[i];
        if( !rSepInfo.pSectionFmt )
        {
            const SwPageDesc* pPd = rSepInfo.pPageDesc;
            if( pPd->GetFollow() && pPd != pPd->GetFollow() &&
                pPd->GetFollow()->GetFollow() == pPd->GetFollow() &&
                rSepInfo.pPDNd &&
                pPd->IsFollowNextPageOfNode( *rSepInfo.pPDNd ) )
                // das ist also 1.Seite und nachfolgende, also nur den
                // follow beachten
                pPd = pPd->GetFollow();

            // left-/right chain of pagedescs ?
            else if( !( 1 & nEnde ) &&
                pPd->GetFollow() && pPd != pPd->GetFollow() &&
                pPd->GetFollow()->GetFollow() == pPd &&
                (( nsUseOnPage::PD_LEFT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) &&
                   nsUseOnPage::PD_RIGHT == ( nsUseOnPage::PD_ALL & pPd->GetFollow()->ReadUseOn() )) ||
                 ( nsUseOnPage::PD_RIGHT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) &&
                   nsUseOnPage::PD_LEFT == ( nsUseOnPage::PD_ALL & pPd->GetFollow()->ReadUseOn() )) ))
            {
                rWrt.pDop->fFacingPages = rWrt.pDop->fMirrorMargins = true;
                nEnde |= 1;
            }

            if( !( 1 & nEnde ) &&
                ( !pPd->IsHeaderShared() || !pPd->IsFooterShared() ))
            {
                rWrt.pDop->fFacingPages = true;
                nEnde |= 1;
            }
            if( !( 2 & nEnde ) &&
                nsUseOnPage::PD_MIRROR == ( nsUseOnPage::PD_MIRROR & pPd->ReadUseOn() ))
            {
                rWrt.pDop->fSwapBordersFacingPgs =
                    rWrt.pDop->fMirrorMargins = true;
                nEnde |= 2;
            }

            if( 3 == nEnde )
                break;      // weiter brauchen wird nicht
        }
    }
}

int MSWordSections::HasBorderItem( const SwFmt& rFmt )
{
    const SfxPoolItem* pItem;
    return SFX_ITEM_SET == rFmt.GetItemState(RES_BOX, true, &pItem) &&
            (   ((SvxBoxItem*)pItem)->GetTop() ||
                ((SvxBoxItem*)pItem)->GetBottom()  ||
                ((SvxBoxItem*)pItem)->GetLeft()  ||
                ((SvxBoxItem*)pItem)->GetRight() );
}

void WW8AttributeOutput::StartSection()
{
    m_rWW8Export.pO->Remove( 0, m_rWW8Export.pO->Count() );       // leeren
}

void WW8AttributeOutput::SectionFormProtection( bool bProtected )
{
    //If the document is to be exported as protected, then if a segment
    //is not protected, set the unlocked flag
    if ( m_rWW8Export.pSepx->DocumentIsProtected() && !bProtected )
    {
        if ( m_rWW8Export.bWrtWW8 )
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SFProtected );
        else
            m_rWW8Export.pO->Insert( 139, m_rWW8Export.pO->Count() );
        m_rWW8Export.pO->Insert( 1 , m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::SectionLineNumbering( sal_uLong nRestartNo, const SwLineNumberInfo& rLnNumInfo )
{
    // sprmSNLnnMod - activate Line Numbering and define Modulo
    if ( m_rWW8Export.bWrtWW8 )
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SNLnnMod );
    else
        m_rWW8Export.pO->Insert( 154, m_rWW8Export.pO->Count() );
    SwWW8Writer::InsUInt16( *m_rWW8Export.pO, (sal_uInt16)rLnNumInfo.GetCountBy() );

    // sprmSDxaLnn - xPosition of Line Number
    if ( m_rWW8Export.bWrtWW8 )
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SDxaLnn );
    else
        m_rWW8Export.pO->Insert( 155, m_rWW8Export.pO->Count() );
    SwWW8Writer::InsUInt16( *m_rWW8Export.pO, (sal_uInt16)rLnNumInfo.GetPosFromLeft() );

    // sprmSLnc - restart number: 0 per page, 1 per section, 2 never restart
    if ( nRestartNo || !rLnNumInfo.IsRestartEachPage() )
    {
        if ( m_rWW8Export.bWrtWW8 )
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SLnc );
        else
            m_rWW8Export.pO->Insert( 152, m_rWW8Export.pO->Count() );
        m_rWW8Export.pO->Insert( nRestartNo ? 1 : 2, m_rWW8Export.pO->Count() );
    }

    // sprmSLnnMin - Restart the Line Number with given value
    if ( nRestartNo )
    {
        if ( m_rWW8Export.bWrtWW8 )
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SLnnMin );
        else
            m_rWW8Export.pO->Insert( 160, m_rWW8Export.pO->Count() );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, (sal_uInt16)nRestartNo - 1 );
    }
}

void WW8AttributeOutput::SectionTitlePage()
{
    // sprmSFTitlePage
    if ( m_rWW8Export.bWrtWW8 )
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SFTitlePage );
    else
        m_rWW8Export.pO->Insert( 143, m_rWW8Export.pO->Count() );
    m_rWW8Export.pO->Insert( 1, m_rWW8Export.pO->Count() );
}

void WW8AttributeOutput::SectionPageBorders( const SwFrmFmt* pPdFmt, const SwFrmFmt* pPdFirstPgFmt )
{
    if ( m_rWW8Export.bWrtWW8 )              // Seitenumrandung schreiben
    {
        sal_uInt16 nPgBorder = MSWordSections::HasBorderItem( *pPdFmt ) ? 0 : USHRT_MAX;
        if ( pPdFmt != pPdFirstPgFmt )
        {
            if ( MSWordSections::HasBorderItem( *pPdFirstPgFmt ) )
            {
                if ( USHRT_MAX == nPgBorder )
                {
                    nPgBorder = 1;
                    // nur die 1. Seite umrandet -> BoxItem aus dem
                    // richtigen Format besorgen
                    m_rWW8Export.pISet = &pPdFirstPgFmt->GetAttrSet();
                    OutputItem( pPdFirstPgFmt->GetFmtAttr( RES_BOX ) );
                }
            }
            else if ( !nPgBorder )
                nPgBorder = 2;
        }

        if ( USHRT_MAX != nPgBorder )
        {
            // Flag und das Border Attribut schreiben
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SPgbProp );
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, nPgBorder );
        }
    }
}

void WW8AttributeOutput::SectionBiDi( bool bBiDi )
{
    if ( m_rWW8Export.bWrtWW8 )
    {
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SFBiDi );
        m_rWW8Export.pO->Insert( bBiDi? 1: 0, m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::SectionPageNumbering( sal_uInt16 nNumType, sal_uInt16 nPageRestartNumber )
{
    // sprmSNfcPgn
    sal_uInt8 nb = WW8Export::GetNumId( nNumType );
    if ( m_rWW8Export.bWrtWW8 )
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SNfcPgn );
    else
        m_rWW8Export.pO->Insert( 147, m_rWW8Export.pO->Count() );
    m_rWW8Export.pO->Insert( nb, m_rWW8Export.pO->Count() );

    if ( nPageRestartNumber )
    {
        // sprmSFPgnRestart
        if ( m_rWW8Export.bWrtWW8 )
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SFPgnRestart );
        else
            m_rWW8Export.pO->Insert( 150, m_rWW8Export.pO->Count() );
        m_rWW8Export.pO->Insert( 1, m_rWW8Export.pO->Count() );

        // sprmSPgnStart
        if ( m_rWW8Export.bWrtWW8 )
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SPgnStart );
        else
            m_rWW8Export.pO->Insert( 161, m_rWW8Export.pO->Count() );
        SwWW8Writer::InsUInt16( *m_rWW8Export.pO, nPageRestartNumber );
    }
}

void WW8AttributeOutput::SectionType( sal_uInt8 nBreakCode )
{
    if ( 2 != nBreakCode ) // new page is the default
    {
        if ( m_rWW8Export.bWrtWW8 )
            SwWW8Writer::InsUInt16( *m_rWW8Export.pO, NS_sprm::LN_SBkc );
        else
            m_rWW8Export.pO->Insert( 142, m_rWW8Export.pO->Count() );
        m_rWW8Export.pO->Insert( nBreakCode, m_rWW8Export.pO->Count() );
    }
}

void WW8AttributeOutput::SectionWW6HeaderFooterFlags( sal_uInt8 nHeadFootFlags )
{
    if ( nHeadFootFlags && !m_rWW8Export.bWrtWW8 )
    {
        sal_uInt8 nTmpFlags = nHeadFootFlags;
        if ( m_rWW8Export.pDop->fFacingPages )
        {
            if ( !(nTmpFlags & WW8_FOOTER_EVEN) && (nTmpFlags & WW8_FOOTER_ODD ) )
                nTmpFlags |= WW8_FOOTER_EVEN;

            if ( !(nTmpFlags & WW8_HEADER_EVEN) && (nTmpFlags & WW8_HEADER_ODD ) )
                nTmpFlags |= WW8_HEADER_EVEN;
        }

        // sprmSGprfIhdt, wird nur noch im WW95 benoetigt
        m_rWW8Export.pO->Insert( 153, m_rWW8Export.pO->Count() );
        m_rWW8Export.pO->Insert( nTmpFlags, m_rWW8Export.pO->Count() );
    }
}

void WW8Export::SetupSectionPositions( WW8_PdAttrDesc* pA )
{
    if ( !pA )
        return;

    if (pO->Count()) // are there attributes?
    {
        pA->m_nLen = pO->Count();
        pA->m_pData.reset(new sal_uInt8[pO->Count()]);
        // store for later
        memcpy( pA->m_pData.get(), pO->GetData(), pO->Count() );
        pO->Remove( 0, pO->Count() );       // leeren fuer HdFt-Text
    }
    else
    {
        pA->m_pData.reset();
        pA->m_nLen = 0;
    }
}

void WW8Export::WriteHeadersFooters( sal_uInt8 nHeadFootFlags,
        const SwFrmFmt& rFmt, const SwFrmFmt& rLeftFmt, const SwFrmFmt& rFirstPageFmt, sal_uInt8 nBreakCode )
{
    sal_uLong nCpPos = Fc2Cp( Strm().Tell() );

    IncrementHdFtIndex();
    if ( !(nHeadFootFlags & WW8_HEADER_EVEN) && pDop->fFacingPages )
        pSepx->OutHeaderFooter( *this, true, rFmt, nCpPos, nHeadFootFlags, WW8_HEADER_ODD, nBreakCode );
    else
        pSepx->OutHeaderFooter( *this, true, rLeftFmt, nCpPos, nHeadFootFlags, WW8_HEADER_EVEN, nBreakCode );
    IncrementHdFtIndex();
    pSepx->OutHeaderFooter( *this, true, rFmt, nCpPos, nHeadFootFlags, WW8_HEADER_ODD, nBreakCode );

    IncrementHdFtIndex();
    if ( !(nHeadFootFlags & WW8_FOOTER_EVEN) && pDop->fFacingPages )
        pSepx->OutHeaderFooter( *this, false, rFmt, nCpPos, nHeadFootFlags, WW8_FOOTER_ODD, nBreakCode );
    else
        pSepx->OutHeaderFooter( *this, false, rLeftFmt, nCpPos, nHeadFootFlags, WW8_FOOTER_EVEN, nBreakCode );
    IncrementHdFtIndex();
    pSepx->OutHeaderFooter( *this, false, rFmt, nCpPos, nHeadFootFlags, WW8_FOOTER_ODD, nBreakCode );

    //#i24344# Drawing objects cannot be directly shared between main hd/ft
    //and title hd/ft so we need to differenciate them
    IncrementHdFtIndex();
    pSepx->OutHeaderFooter( *this, true, rFirstPageFmt, nCpPos, nHeadFootFlags, WW8_HEADER_FIRST, nBreakCode );
    pSepx->OutHeaderFooter( *this, false, rFirstPageFmt, nCpPos, nHeadFootFlags, WW8_FOOTER_FIRST, nBreakCode );
}

void MSWordExportBase::SectionProperties( const WW8_SepInfo& rSepInfo, WW8_PdAttrDesc* pA )
{
    const SwPageDesc* pPd = rSepInfo.pPageDesc;

    if ( rSepInfo.pSectionFmt && !pPd )
        pPd = &const_cast<const SwDoc *>( pDoc )->GetPageDesc( 0 );

    pAktPageDesc = pPd;
    
    if ( !pPd )
        return;
    
    bool bOldPg = bOutPageDescs;
    bOutPageDescs = true;

    AttrOutput().StartSection();

    // forms
    AttrOutput().SectionFormProtection( rSepInfo.IsProtected() );

    // line numbers
    const SwLineNumberInfo& rLnNumInfo = pDoc->GetLineNumberInfo();
    if ( rLnNumInfo.IsPaintLineNumbers() )
        AttrOutput().SectionLineNumbering( rSepInfo.nLnNumRestartNo, rLnNumInfo );

    /*  sprmSBkc, break code:   0 No break, 1 New column
        2 New page, 3 Even page, 4 Odd page
        */
    sal_uInt8 nBreakCode = 2;            // default neue Seite beginnen
    bool bOutPgDscSet = true, bLeftRightPgChain = false;
    const SwFrmFmt* pPdFmt = &pPd->GetMaster();
    const SwFrmFmt* pPdFirstPgFmt = pPdFmt;
    if ( rSepInfo.pSectionFmt )
    {
        // ist pSectionFmt gesetzt, dann gab es einen SectionNode
        //  gueltiger Pointer -> Section beginnt,
        //  0xfff -> Section wird beendet
        nBreakCode = 0;         // fortlaufender Abschnitt

        if ( rSepInfo.pPDNd && rSepInfo.pPDNd->IsCntntNode() )
        {
            if ( !NoPageBreakSection( &rSepInfo.pPDNd->GetCntntNode()->GetSwAttrSet() ) )
            {
                nBreakCode = 2;
            }
        }

        if ( (SwSectionFmt*)0xFFFFFFFF != rSepInfo.pSectionFmt )
        {
            if ( nBreakCode == 0 )
                bOutPgDscSet = false;

            // Itemset erzeugen, das das PgDesk-AttrSet beerbt:
            // als Nachkomme wird bei 'deep'-OutputItemSet
            // auch der Vorfahr abgeklappert
            const SfxItemSet* pPdSet = &pPdFmt->GetAttrSet();
            SfxItemSet aSet( *pPdSet->GetPool(), pPdSet->GetRanges() );
            aSet.SetParent( pPdSet );

            // am Nachkommen NUR  die Spaltigkeit gemaess Sect-Attr.
            // umsetzen
            aSet.Put( rSepInfo.pSectionFmt->GetFmtAttr( RES_COL ) );

            const SvxLRSpaceItem &rSectionLR =
                ItemGet<SvxLRSpaceItem>( *(rSepInfo.pSectionFmt), RES_LR_SPACE );
            const SvxLRSpaceItem &rPageLR =
                ItemGet<SvxLRSpaceItem>( *pPdFmt, RES_LR_SPACE );

            SvxLRSpaceItem aResultLR( rPageLR.GetLeft() +
                    rSectionLR.GetLeft(), rPageLR.GetRight() +
                    rSectionLR.GetRight(), 0, 0, RES_LR_SPACE );

            aSet.Put( aResultLR );

            // und raus damit ins WW-File
            const SfxItemSet* pOldI = pISet;
            pISet = &aSet;
            // --> OD 2007-06-12 #TESTING#
            // Switch off test on default item values, if page description
            // set (value of <bOutPgDscSet>) isn't written.
            AttrOutput().OutputStyleItemSet( aSet, true, bOutPgDscSet );
            // <--

            //Cannot export as normal page framedir, as continous sections
            //cannot contain any grid settings like proper sections
            AttrOutput().SectionBiDi( FRMDIR_HORI_RIGHT_TOP == TrueFrameDirection( *rSepInfo.pSectionFmt ) );

            pISet = pOldI;
        }
    }

    if ( bOutPgDscSet )
    {
        // es ist ein Follow gesetzt und dieser zeigt nicht auf sich
        // selbst, so liegt eine Seitenverkettung vor.
        // Falls damit eine "Erste Seite" simuliert werden soll, so
        // koennen wir das auch als solches schreiben.
        // Anders sieht es mit Links/Rechts wechseln aus. Dafuer muss
        // erkannt werden, wo der Seitenwechsel statt findet. Hier ist
        // es aber dafuer zuspaet!
        if ( pPd->GetFollow() && pPd != pPd->GetFollow() &&
             pPd->GetFollow()->GetFollow() == pPd->GetFollow() &&
             ( !rSepInfo.pPDNd || pPd->IsFollowNextPageOfNode( *rSepInfo.pPDNd ) ) )
        {
            const SwPageDesc *pFollow = pPd->GetFollow();
            const SwFrmFmt& rFollowFmt = pFollow->GetMaster();
            if ( sw::util::IsPlausableSingleWordSection( *pPdFmt, rFollowFmt ) )
            {
                if (rSepInfo.pPDNd)
                    pPdFirstPgFmt = pPd->GetPageFmtOfNode( *rSepInfo.pPDNd );
                else
                    pPdFirstPgFmt = &pPd->GetMaster();

                pAktPageDesc = pPd = pFollow;
                pPdFmt = &rFollowFmt;

                // has different headers/footers for the title page
                AttrOutput().SectionTitlePage();
            }
        }

        const SfxItemSet* pOldI = pISet;

        AttrOutput().SectionPageBorders( pPdFmt, pPdFirstPgFmt );

        const SfxPoolItem* pItem;
        if ( pPdFmt != pPdFirstPgFmt && SFX_ITEM_SET ==
                pPdFirstPgFmt->GetItemState( RES_PAPER_BIN, true, &pItem ) )
        {
            pISet = &pPdFirstPgFmt->GetAttrSet();
            bOutFirstPage = true;
            AttrOutput().OutputItem( *pItem );
            bOutFirstPage = false;
        }


        // left-/right chain of pagedescs ?
        if ( pPd->GetFollow() && pPd != pPd->GetFollow() &&
                pPd->GetFollow()->GetFollow() == pPd &&
                (( nsUseOnPage::PD_LEFT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) &&
                   nsUseOnPage::PD_RIGHT == ( nsUseOnPage::PD_ALL & pPd->GetFollow()->ReadUseOn() )) ||
                 ( nsUseOnPage::PD_RIGHT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) &&
                   nsUseOnPage::PD_LEFT == ( nsUseOnPage::PD_ALL & pPd->GetFollow()->ReadUseOn() )) ))
        {
            bLeftRightPgChain = true;

            // welches ist der Bezugspunkt ????? (links oder rechts?)
            // annahme die rechte Seite!
            if ( nsUseOnPage::PD_LEFT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) )
            {
                nBreakCode = 3;
                pPd = pPd->GetFollow();
                pPdFmt = &pPd->GetMaster();
            }
            else
                nBreakCode = 4;
        }

        pISet = &pPdFmt->GetAttrSet();
        AttrOutput().OutputStyleItemSet( pPdFmt->GetAttrSet(), true, false );
        pISet = pOldI;

        // dann noch die restlichen Einstellungen aus dem PageDesc

        AttrOutput().SectionPageNumbering( pPd->GetNumType().GetNumberingType(), rSepInfo.nPgRestartNo );

        // werden es nur linke oder nur rechte Seiten?
        if ( 2 == nBreakCode )
        {
            if ( nsUseOnPage::PD_LEFT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) )
                nBreakCode = 3;
            else if ( nsUseOnPage::PD_RIGHT == ( nsUseOnPage::PD_ALL & pPd->ReadUseOn() ) )
                nBreakCode = 4;
        }
    }

    AttrOutput().SectionType( nBreakCode );

    const SwTxtNode* pNd = rSepInfo.pNumNd;
    if ( pNd )
    {
        const SwNumRule* pRule = pNd->GetNumRule();
        if ( pRule )
            OutputOlst( *pRule );
    }

    // Header oder Footer
    sal_uInt8 nHeadFootFlags = 0;

    const SwFrmFmt* pPdLeftFmt = bLeftRightPgChain
        ? &pPd->GetFollow()->GetMaster()
        : &pPd->GetLeft();

    if ( nBreakCode != 0 )
    {
        MSWordSections::SetHeaderFlag( nHeadFootFlags, *pPdFmt, WW8_HEADER_ODD );
        MSWordSections::SetFooterFlag( nHeadFootFlags, *pPdFmt, WW8_FOOTER_ODD );

        if ( !pPd->IsHeaderShared() || bLeftRightPgChain )
            MSWordSections::SetHeaderFlag( nHeadFootFlags, *pPdLeftFmt, WW8_HEADER_EVEN );

        if ( !pPd->IsFooterShared() || bLeftRightPgChain )
            MSWordSections::SetFooterFlag( nHeadFootFlags, *pPdLeftFmt, WW8_FOOTER_EVEN );

        if ( pPdFmt != pPdFirstPgFmt )
        {
            // es gibt eine ErsteSeite:
            MSWordSections::SetHeaderFlag( nHeadFootFlags, *pPdFirstPgFmt, WW8_HEADER_FIRST );
            MSWordSections::SetFooterFlag( nHeadFootFlags, *pPdFirstPgFmt, WW8_FOOTER_FIRST );
        }

        AttrOutput().SectionWW6HeaderFooterFlags( nHeadFootFlags );
    }

    // binary filters only
    SetupSectionPositions( pA );

    /*
       !!!!!!!!!!!
    // Umrandungen an Kopf- und Fusstexten muessten etwa so gehen:
    // Dabei muss etwas wie pOut eingebaut werden,
    // das bei jeder Spezialtext-Zeile wiederholt wird.
    const SwFrmFmt* pFFmt = rFt.GetFooterFmt();
    const SvxBoxItem& rBox = pFFmt->GetBox(false);
    OutWW8_SwFmtBox1( m_rWW8Export.pOut, rBox, false);
    !!!!!!!!!!!
    Man koennt daraus Absatzattribute machen, die dann bei jedem Absatz
    beachtet werden. Gilt fuer Hintergrund/Umrandung
    !!!!!!!!!!!
    */

    const SwTxtNode *pOldPageRoot = GetHdFtPageRoot();
    SetHdFtPageRoot( rSepInfo.pPDNd ? rSepInfo.pPDNd->GetTxtNode() : 0 );

    WriteHeadersFooters( nHeadFootFlags, *pPdFmt, *pPdLeftFmt, *pPdFirstPgFmt, nBreakCode );

    SetHdFtPageRoot( pOldPageRoot );

    AttrOutput().EndSection();

    // outside of the section properties again
    bOutPageDescs = bOldPg;
}

bool WW8_WrPlcSepx::WriteKFTxt( WW8Export& rWrt )
{
    sal_uLong nCpStart = rWrt.Fc2Cp( rWrt.Strm().Tell() );

    ASSERT( !pTxtPos, "wer hat den Pointer gesetzt?" );
    pTxtPos = new WW8_WrPlc0( nCpStart );

    WriteFtnEndTxt( rWrt, nCpStart );
    CheckForFacinPg( rWrt );

    unsigned int nOldIndex = rWrt.GetHdFtIndex();
    rWrt.SetHdFtIndex( 0 );
    // FIXME: this writes the section properties, but not of all sections;
    // it's possible that later in the document (e.g. in endnotes) sections
    // are added, but they won't have their properties written here!
    m_bHeaderFooterWritten = true;
    for ( sal_uInt16 i = 0; i < aSects.Count(); ++i )
    {
        ::boost::shared_ptr<WW8_PdAttrDesc> const pAttrDesc(new WW8_PdAttrDesc);
        m_SectionAttributes.push_back(pAttrDesc);

        WW8_SepInfo& rSepInfo = aSects[i];
        rWrt.SectionProperties( rSepInfo, pAttrDesc.get() );
    }
    rWrt.SetHdFtIndex( nOldIndex ); //0

    if ( pTxtPos->Count() )
    {
        // HdFt vorhanden ?
        sal_uLong nCpEnd = rWrt.Fc2Cp( rWrt.Strm().Tell() );
        pTxtPos->Append( nCpEnd );  // Ende letzter Hd/Ft fuer PlcfHdd

        if ( nCpEnd > nCpStart )
        {
            ++nCpEnd;
            pTxtPos->Append( nCpEnd + 1 );  // Ende letzter Hd/Ft fuer PlcfHdd

            rWrt.WriteStringAsPara( aEmptyStr ); // CR ans Ende ( sonst mault WW )
        }
        rWrt.pFldHdFt->Finish( nCpEnd, rWrt.pFib->ccpText + rWrt.pFib->ccpFtn );
        rWrt.pFib->ccpHdr = nCpEnd - nCpStart;
    }
    else
        delete pTxtPos, pTxtPos = 0;

    return rWrt.pFib->ccpHdr != 0;
}

void WW8_WrPlcSepx::WriteSepx( SvStream& rStrm ) const
{
    OSL_ENSURE(m_SectionAttributes.size() == static_cast<size_t>(aSects.Count())
        , "WriteSepx(): arrays out of sync!");
    for (size_t i = 0; i < m_SectionAttributes.size(); i++) // all sections
    {
        WW8_PdAttrDesc *const pA = m_SectionAttributes[i].get();
        if (pA->m_nLen && pA->m_pData != NULL)
        {
            SVBT16 nL;
            pA->m_nSepxFcPos = rStrm.Tell();
            ShortToSVBT16( pA->m_nLen, nL );
            rStrm.Write( nL, 2 );
            rStrm.Write( pA->m_pData.get(), pA->m_nLen );
        }
    }
}

void WW8_WrPlcSepx::WritePlcSed( WW8Export& rWrt ) const
{
    OSL_ENSURE(m_SectionAttributes.size() == static_cast<size_t>(aSects.Count())
        , "WritePlcSed(): arrays out of sync!");
    ASSERT( aCps.Count() == aSects.Count() + 1, "WrPlcSepx: DeSync" );
    sal_uLong nFcStart = rWrt.pTableStrm->Tell();

    sal_uInt16 i;
    // ( ueber alle Sections )
    for( i = 0; i <= aSects.Count(); i++ )
    {
        sal_uInt32 nP = aCps[i];
        SVBT32 nPos;
        UInt32ToSVBT32( nP, nPos );
        rWrt.pTableStrm->Write( nPos, 4 );
    }

    static WW8_SED aSed = {{4, 0},{0, 0, 0, 0},{0, 0},{0xff, 0xff, 0xff, 0xff}};

    // ( ueber alle Sections )
    for (size_t j = 0; j < m_SectionAttributes.size(); j++ )
    {
        // Sepx-Pos
        UInt32ToSVBT32( m_SectionAttributes[j]->m_nSepxFcPos, aSed.fcSepx );
        rWrt.pTableStrm->Write( &aSed, sizeof( aSed ) );
    }
    rWrt.pFib->fcPlcfsed = nFcStart;
    rWrt.pFib->lcbPlcfsed = rWrt.pTableStrm->Tell() - nFcStart;
}


void WW8_WrPlcSepx::WritePlcHdd( WW8Export& rWrt ) const
{
    if( pTxtPos && pTxtPos->Count() )
    {
        rWrt.pFib->fcPlcfhdd = rWrt.pTableStrm->Tell();
        pTxtPos->Write( *rWrt.pTableStrm );             // Plc0
        rWrt.pFib->lcbPlcfhdd = rWrt.pTableStrm->Tell() -
                                rWrt.pFib->fcPlcfhdd;
    }
}

void MSWordExportBase::WriteHeaderFooterText( const SwFmt& rFmt, bool bHeader )
{
    const SwFmtCntnt *pCntnt;
    if ( bHeader )
    {
        bHasHdr = true;
        const SwFmtHeader& rHd = rFmt.GetHeader();
        ASSERT( rHd.GetHeaderFmt(), "Header text is not here" );
        pCntnt = &rHd.GetHeaderFmt()->GetCntnt();
    }
    else
    {
        bHasFtr = true;
        const SwFmtFooter& rFt = rFmt.GetFooter();
        ASSERT( rFt.GetFooterFmt(), "Footer text is not here" );
        pCntnt = &rFt.GetFooterFmt()->GetCntnt();
    }
    
    const SwNodeIndex* pSttIdx = pCntnt->GetCntntIdx();

    if ( pSttIdx )
    {
        SwNodeIndex aIdx( *pSttIdx, 1 ),
        aEnd( *pSttIdx->GetNode().EndOfSectionNode() );
        sal_uLong nStart = aIdx.GetIndex();
        sal_uLong nEnd = aEnd.GetIndex();

        // Bereich also gueltiger Node
        if ( nStart < nEnd )
        {
            bool bOldKF = bOutKF;
            bOutKF = true;
            WriteSpecialText( nStart, nEnd, TXT_HDFT );
            bOutKF = bOldKF;
        }
        else
            pSttIdx = 0;
    }

    if ( !pSttIdx )
    {
        // es gibt keine Kopf-/Fusszeile, aber ein CR ist immer noch noetig
        ASSERT( pSttIdx, "K/F-Text nicht richtig da" );
        AttrOutput().EmptyParagraph(); // CR ans Ende ( sonst mault WW )
    }
}

/*  */
//------------------------------------------------------------------------------
// class WW8_WrPlcFtnEdn : Sammeln der Fuss/Endnoten und Ausgeben der Texte
// und Plcs am Ende des Docs.
// WW8_WrPlcFtnEdn ist die Klasse fuer Fuss- und Endnoten
//------------------------------------------------------------------------------
WW8_WrPlcSubDoc::WW8_WrPlcSubDoc()
    : aCps( 0, 16 ), aCntnt( 0, 16 ), pTxtPos( 0 )
{
}

WW8_WrPlcSubDoc::~WW8_WrPlcSubDoc()
{
    delete pTxtPos;
}

void WW8_WrPlcFtnEdn::Append( WW8_CP nCp, const SwFmtFtn& rFtn )
{
    aCps.Insert( nCp, aCps.Count() );
    void* p = (void*)&rFtn;
    aCntnt.Insert( p, aCntnt.Count() );
}

WW8_Annotation::WW8_Annotation(const SwPostItField* pPostIt)
{
    mpRichText = pPostIt->GetTextObject();
    if (!mpRichText)
        msSimpleText = pPostIt->GetTxt();
    msOwner = pPostIt->GetPar1();
    maDateTime = DateTime(pPostIt->GetDate(), pPostIt->GetTime());
}

WW8_Annotation::WW8_Annotation(const SwRedlineData* pRedline) : mpRichText(0)
{
    msSimpleText = pRedline->GetComment();
    msOwner = SW_MOD()->GetRedlineAuthor(pRedline->GetAuthor());
    maDateTime = pRedline->GetTimeStamp();
}

void WW8_WrPlcAnnotations::Append( WW8_CP nCp, const SwPostItField *pPostIt )
{
    aCps.Insert( nCp, aCps.Count() );
    WW8_Annotation* p = new WW8_Annotation(pPostIt);
    aCntnt.Insert( p, aCntnt.Count() );
}

void WW8_WrPlcAnnotations::Append( WW8_CP nCp, const SwRedlineData *pRedline )
{
    maProcessedRedlines.insert(pRedline);
    aCps.Insert( nCp, aCps.Count() );
    WW8_Annotation* p = new WW8_Annotation(pRedline);
    aCntnt.Insert( p, aCntnt.Count() );
}

bool WW8_WrPlcAnnotations::IsNewRedlineComment( const SwRedlineData *pRedline )
{
    return maProcessedRedlines.find(pRedline) == maProcessedRedlines.end();
}

WW8_WrPlcAnnotations::~WW8_WrPlcAnnotations()
{
    for( sal_uInt16 n=0; n < aCntnt.Count(); n++ )
        delete (WW8_Annotation*)aCntnt[n];
}

bool WW8_WrPlcSubDoc::WriteGenericTxt( WW8Export& rWrt, sal_uInt8 nTTyp,
    WW8_CP& rCount )
{
    sal_uInt16 nLen = aCntnt.Count();
    if ( !nLen )
        return false;

    sal_uLong nCpStart = rWrt.Fc2Cp( rWrt.Strm().Tell() );
    pTxtPos = new WW8_WrPlc0( nCpStart );
    sal_uInt16 i;

    switch ( nTTyp )
    {
        case TXT_ATN:
            for ( i = 0; i < nLen; i++ )
            {
                // Anfaenge fuer PlcfAtnTxt
                pTxtPos->Append( rWrt.Fc2Cp( rWrt.Strm().Tell() ));

                rWrt.WritePostItBegin();
                const WW8_Annotation& rAtn = *(const WW8_Annotation*)aCntnt[i];
                if (rAtn.mpRichText)
                    rWrt.WriteOutliner(*rAtn.mpRichText, nTTyp);
                else
                {
                    String sTxt(rAtn.msSimpleText);
                    sTxt.SearchAndReplaceAll(0x0A, 0x0B);
                    rWrt.WriteStringAsPara( sTxt );
                }
            }
            break;

        case TXT_TXTBOX:
        case TXT_HFTXTBOX:
            for ( i = 0; i < nLen; i++ )
            {
                // textbox - content
                WW8_CP nCP = rWrt.Fc2Cp( rWrt.Strm().Tell() );
                aCps.Insert( nCP, i );
                pTxtPos->Append( nCP );

                // is it an writer or sdr - textbox?
                const SdrObject& rObj = *(SdrObject*)aCntnt[ i ];
                if (rObj.GetObjInventor() == FmFormInventor)
                {
                    sal_uInt8 nOldTyp = rWrt.nTxtTyp;
                    rWrt.nTxtTyp = nTTyp;
                    rWrt.GetOCXExp().ExportControl(rWrt,&rObj);
                    rWrt.nTxtTyp = nOldTyp;
                }
                else if( rObj.ISA( SdrTextObj ) )
                    rWrt.WriteSdrTextObj(rObj, nTTyp);
                else
                {
                    const SwFrmFmt* pFmt = ::FindFrmFmt( &rObj );
                    ASSERT( pFmt, "wo ist das Format geblieben?" );

                    const SwNodeIndex* pNdIdx = pFmt->GetCntnt().GetCntntIdx();
                    ASSERT( pNdIdx, "wo ist der StartNode der Textbox?" );
                    rWrt.WriteSpecialText( pNdIdx->GetIndex() + 1,
                                           pNdIdx->GetNode().EndOfSectionIndex(),
                                           nTTyp );
                    // --> OD 2008-08-07 #156757#
                    {
                        SwNodeIndex aContentIdx = *pNdIdx;
                        aContentIdx++;
                        if ( aContentIdx.GetNode().IsTableNode() )
                        {
                            bool bContainsOnlyTables = true;
                            do {
                                aContentIdx = *(aContentIdx.GetNode().EndOfSectionNode());
                                aContentIdx++;
                                if ( !aContentIdx.GetNode().IsTableNode() &&
                                     aContentIdx.GetIndex() != pNdIdx->GetNode().EndOfSectionIndex() )
                                {
                                    bContainsOnlyTables = false;
                                }
                            } while ( aContentIdx.GetNode().IsTableNode() );
                            if ( bContainsOnlyTables )
                            {
                                // Additional paragraph containing a space to
                                // assure that by WW created RTF from written WW8
                                // does not crash WW.
                                rWrt.WriteStringAsPara( String::CreateFromAscii( " " ) );
                            }
                        }
                    }
                    // <--
                }

                // CR at end of one textbox text ( otherwise WW gpft :-( )
                rWrt.WriteStringAsPara( aEmptyStr );
            }
            break;

        case TXT_EDN:
        case TXT_FTN:
            for ( i = 0; i < nLen; i++ )
            {
                // Anfaenge fuer PlcfFtnTxt/PlcfEdnTxt
                pTxtPos->Append( rWrt.Fc2Cp( rWrt.Strm().Tell() ));

                // Noten-Inhalt
                const SwFmtFtn* pFtn = (SwFmtFtn*)aCntnt[ i ];
                rWrt.WriteFtnBegin( *pFtn );
                const SwNodeIndex* pIdx = pFtn->GetTxtFtn()->GetStartNode();
                ASSERT( pIdx, "wo ist der StartNode der Fuss-/EndNote?" );
                rWrt.WriteSpecialText( pIdx->GetIndex() + 1,
                                       pIdx->GetNode().EndOfSectionIndex(),
                                       nTTyp );
            }
            break;

        default:
            ASSERT( !this, "was ist das fuer ein SubDocType?" );
    }

    pTxtPos->Append( rWrt.Fc2Cp( rWrt.Strm().Tell() ));
    // CR ans Ende ( sonst mault WW )
    rWrt.WriteStringAsPara( aEmptyStr );

    WW8_CP nCpEnd = rWrt.Fc2Cp( rWrt.Strm().Tell() );
    pTxtPos->Append( nCpEnd );
    rCount = nCpEnd - nCpStart;

    return ( rCount != 0 );
}

void WW8_WrPlcSubDoc::WriteGenericPlc( WW8Export& rWrt, sal_uInt8 nTTyp,
    WW8_FC& rTxtStart, sal_Int32& rTxtCount, WW8_FC& rRefStart, sal_Int32& rRefCount ) const
{
    typedef ::std::vector<String>::iterator myiter;

    sal_uLong nFcStart = rWrt.pTableStrm->Tell();
    sal_uInt16 nLen = aCps.Count();
    if ( !nLen )
        return;

    ASSERT( aCps.Count() + 2 == pTxtPos->Count(), "WritePlc: DeSync" );

    ::std::vector<String> aStrArr;
    WW8Fib& rFib = *rWrt.pFib;              // n+1-te CP-Pos nach Handbuch
    sal_uInt16 i;
    bool bWriteCP = true;

    switch ( nTTyp )
    {
        case TXT_ATN:
            {
                // then write first the GrpXstAtnOwners
                for ( i = 0; i < nLen; ++i )
                {
                    const WW8_Annotation& rAtn = *(const WW8_Annotation*)aCntnt[i];
                    aStrArr.push_back(rAtn.msOwner);
                }

                //sort and remove duplicates
                ::std::sort(aStrArr.begin(), aStrArr.end());
                myiter aIter = ::std::unique(aStrArr.begin(), aStrArr.end());
                aStrArr.erase(aIter, aStrArr.end());

                if ( rWrt.bWrtWW8 )
                {
                    for ( i = 0; i < aStrArr.size(); ++i )
                    {
                        const String& rStr = aStrArr[i];
                        SwWW8Writer::WriteShort(*rWrt.pTableStrm, rStr.Len());
                        SwWW8Writer::WriteString16(*rWrt.pTableStrm, rStr,
                                false);
                    }
                }
                else
                {
                    for ( i = 0; i < aStrArr.size(); ++i )
                    {
                        const String& rStr = aStrArr[i];
                        *rWrt.pTableStrm << (sal_uInt8)rStr.Len();
                        SwWW8Writer::WriteString8(*rWrt.pTableStrm, rStr, false,
                                RTL_TEXTENCODING_MS_1252);
                    }
                }

                rFib.fcGrpStAtnOwners = nFcStart;
                nFcStart = rWrt.pTableStrm->Tell();
                rFib.lcbGrpStAtnOwners = nFcStart - rFib.fcGrpStAtnOwners;

                // Write the extended >= Word XP ATLD records
                if( rWrt.bWrtWW8 )
                {
                    for( i = 0; i < nLen; ++i )
                    {
                        const WW8_Annotation& rAtn = *(const WW8_Annotation*)aCntnt[i];

                        sal_uInt32 nDTTM = sw::ms::DateTime2DTTM(rAtn.maDateTime);

                        SwWW8Writer::WriteLong( *rWrt.pTableStrm, nDTTM );
                        SwWW8Writer::WriteShort( *rWrt.pTableStrm, 0 );
                        SwWW8Writer::WriteLong( *rWrt.pTableStrm, 0 );
                        SwWW8Writer::WriteLong( *rWrt.pTableStrm, 0 );
                        SwWW8Writer::WriteLong( *rWrt.pTableStrm, 0 );
                    }

                    rFib.fcAtrdExtra = nFcStart;
                    nFcStart = rWrt.pTableStrm->Tell();
                    rFib.lcbAtrdExtra = nFcStart - rFib.fcAtrdExtra;
                    rFib.fcHplxsdr = 0x01010002;  //WTF, but apparently necessary
                    rFib.lcbHplxsdr = 0;
                }
            }
            break;
        case TXT_TXTBOX:
        case TXT_HFTXTBOX:
            {
                pTxtPos->Write( *rWrt.pTableStrm );
                const SvULongs* pShapeIds = GetShapeIdArr();
                ASSERT( pShapeIds, "wo sind die ShapeIds?" );

                // nLen = pTxtPos->Count();
                for ( i = 0; i < nLen; ++i )
                {
                    // write textbox story - FTXBXS
                    // is it an writer or sdr - textbox?
                    const SdrObject* pObj = (SdrObject*)aCntnt[ i ];
                    sal_Int32 nCnt = 1;
                    if ( !pObj->ISA( SdrTextObj ) )
                    {
                        // find the "highest" SdrObject of this
                        const SwFrmFmt& rFmt = *::FindFrmFmt( pObj );

                        const SwFmtChain* pChn = &rFmt.GetChain();
                        while ( pChn->GetNext() )
                        {
                            // has a chain?
                            // then calc the cur pos in the chain
                            ++nCnt;
                            pChn = &pChn->GetNext()->GetChain();
                        }
                    }
                    // long cTxbx / iNextReuse
                    SwWW8Writer::WriteLong( *rWrt.pTableStrm, nCnt );
                    // long cReusable
                    SwWW8Writer::WriteLong( *rWrt.pTableStrm, 0 );
                    // short fReusable
                    SwWW8Writer::WriteShort( *rWrt.pTableStrm, 0 );
                    // long reserved
                    SwWW8Writer::WriteLong( *rWrt.pTableStrm, -1 );
                    // long lid
                    SwWW8Writer::WriteLong( *rWrt.pTableStrm,
                            (*pShapeIds)[i]);
                    // long txidUndo
                    SwWW8Writer::WriteLong( *rWrt.pTableStrm, 0 );
                }
                SwWW8Writer::FillCount( *rWrt.pTableStrm, 22 );
                bWriteCP = false;
            }
            break;
    }

    if ( bWriteCP )
    {
        // Schreibe CP-Positionen
        for ( i = 0; i < nLen; i++ )
            SwWW8Writer::WriteLong( *rWrt.pTableStrm, aCps[ i ] );

        // n+1-te CP-Pos nach Handbuch
        SwWW8Writer::WriteLong( *rWrt.pTableStrm,
                rFib.ccpText + rFib.ccpFtn + rFib.ccpHdr + rFib.ccpEdn +
                rFib.ccpTxbx + rFib.ccpHdrTxbx + 1 );

        if ( TXT_ATN == nTTyp )
        {
            for ( i = 0; i < nLen; ++i )
            {
                const WW8_Annotation& rAtn = *(const WW8_Annotation*)aCntnt[i];

                //aStrArr is sorted
                myiter aIter = ::std::lower_bound(aStrArr.begin(),
                        aStrArr.end(), rAtn.msOwner);
                ASSERT(aIter != aStrArr.end() && *aIter == rAtn.msOwner,
                        "Impossible");
                sal_uInt16 nFndPos = static_cast< sal_uInt16 >(aIter - aStrArr.begin());
                String sAuthor(*aIter);
                sal_uInt8 nNameLen = (sal_uInt8)sAuthor.Len();
                if ( nNameLen > 9 )
                {
                    sAuthor.Erase( 9 );
                    nNameLen = 9;
                }

                // xstUsrInitl[ 10 ] pascal-style String holding initials
                // of annotation author
                if ( rWrt.bWrtWW8 )
                {
                    SwWW8Writer::WriteShort(*rWrt.pTableStrm, nNameLen);
                    SwWW8Writer::WriteString16(*rWrt.pTableStrm, sAuthor,
                            false);
                    SwWW8Writer::FillCount( *rWrt.pTableStrm,
                            (9 - nNameLen) * 2 );

                }
                else
                {
                    *rWrt.pTableStrm << nNameLen;
                    SwWW8Writer::WriteString8(*rWrt.pTableStrm, sAuthor,
                            false, RTL_TEXTENCODING_MS_1252);
                    SwWW8Writer::FillCount(*rWrt.pTableStrm, 9 - nNameLen);
                }

                //SVBT16 ibst;      // index into GrpXstAtnOwners
                //SVBT16 ak;        // not used
                //SVBT16 grfbmc;    // not used
                //SVBT32 ITagBkmk;  // when not -1, this tag identifies the

                SwWW8Writer::WriteShort( *rWrt.pTableStrm, nFndPos );
                SwWW8Writer::WriteShort( *rWrt.pTableStrm, 0 );
                SwWW8Writer::WriteShort( *rWrt.pTableStrm, 0 );
                SwWW8Writer::WriteLong( *rWrt.pTableStrm, -1 );
            }
        }
        else
        {
            sal_uInt16 nNo = 0;
            for ( i = 0; i < nLen; ++i )             // Schreibe Flags
            {
                const SwFmtFtn* pFtn = (SwFmtFtn*)aCntnt[ i ];
                SwWW8Writer::WriteShort( *rWrt.pTableStrm,
                        pFtn->GetNumStr().Len() ? 0 : ++nNo );
            }
        }
    }
    rRefStart = nFcStart;
    nFcStart = rWrt.pTableStrm->Tell();
    rRefCount = nFcStart - rRefStart;

    pTxtPos->Write( *rWrt.pTableStrm );

    switch ( nTTyp )
    {
        case TXT_TXTBOX:
        case TXT_HFTXTBOX:
            for ( i = 0; i < nLen; ++i )
            {
                // write break descriptor (BKD)
                // short itxbxs
                SwWW8Writer::WriteShort( *rWrt.pTableStrm, i );
                // short dcpDepend
                SwWW8Writer::WriteShort( *rWrt.pTableStrm, 0 );
                // short flags : icol/fTableBreak/fColumnBreak/fMarked/
                //               fUnk/fTextOverflow
                SwWW8Writer::WriteShort( *rWrt.pTableStrm, 0x800 );
            }
            SwWW8Writer::FillCount( *rWrt.pTableStrm, 6 );
            break;
    }

    rTxtStart = nFcStart;
    rTxtCount = rWrt.pTableStrm->Tell() - nFcStart;
}

const SvULongs* WW8_WrPlcSubDoc::GetShapeIdArr() const
{
    return 0;
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */
