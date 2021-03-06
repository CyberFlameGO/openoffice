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
#include "precompiled_sc.hxx"

// INCLUDE ---------------------------------------------------------------

#include <sfx2/app.hxx>
#include <sfx2/objsh.hxx>
#include <basic/sbmeth.hxx>
#include <basic/sbstar.hxx>
#include <svl/zforlist.hxx>
#include <tools/rcid.h>
#include <tools/rc.hxx>
#include <tools/solar.h>
#include <unotools/charclass.hxx>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/sheet/FormulaOpCodeMapEntry.hpp>
#include <com/sun/star/sheet/FormulaLanguage.hpp>
#include <com/sun/star/sheet/FormulaMapGroup.hpp>
#include <comphelper/processfactory.hxx>
#include <unotools/transliterationwrapper.hxx>
#include <tools/urlobj.hxx>
#include <rtl/math.hxx>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "compiler.hxx"
#include "rangenam.hxx"
#include "dbcolect.hxx"
#include "document.hxx"
#include "callform.hxx"
#include "addincol.hxx"
#include "refupdat.hxx"
#include "scresid.hxx"
#include "sc.hrc"
#include "globstr.hrc"
#include "cell.hxx"
#include "dociter.hxx"
#include "docoptio.hxx"
#include <formula/errorcodes.hxx>
#include "parclass.hxx"
#include "autonamecache.hxx"
#include "externalrefmgr.hxx"
#include "rangeutl.hxx"
#include "convuno.hxx"
#include "tokenuno.hxx"
#include "formulaparserpool.hxx"

using namespace formula;
using namespace ::com::sun::star;
using rtl::OUString;
using ::std::vector;

#if OSL_DEBUG_LEVEL > 1
// For some unknown reason the identical dbg_dump utilities in
// tools/source/string/debugprint.cxx tend to crash when called from within
// gdb. Having them here also comes handy as libtl*.so doesn't have to be
// replaced.
const char* dbg_sc_dump( const ByteString & rStr )
{
    static ByteString aStr;
    aStr = rStr;
    aStr.Append(static_cast<char>(0));
    return aStr.GetBuffer();
}
const char* dbg_sc_dump( const UniString & rStr )
{
    return dbg_sc_dump(ByteString(rStr, RTL_TEXTENCODING_UTF8));
}
const char* dbg_sc_dump( const sal_Unicode * pBuf )
{
    return dbg_sc_dump( UniString( pBuf));
}
const char* dbg_sc_dump( const sal_Unicode c )
{
    return dbg_sc_dump( UniString( c));
}
#endif

CharClass*                          ScCompiler::pCharClassEnglish = NULL;
const ScCompiler::Convention*       ScCompiler::pConventions[ ]   = { NULL, NULL, NULL, NULL, NULL, NULL };

enum ScanState
{
    ssGetChar,
    ssGetBool,
    ssGetValue,
    ssGetString,
    ssSkipString,
    ssGetIdent,
    ssGetReference,
    ssSkipReference,
    ssStop
};

static const sal_Char* pInternal[ 1 ] = { "TTT" };

using namespace ::com::sun::star::i18n;

/////////////////////////////////////////////////////////////////////////



class ScCompilerRecursionGuard
{
private:
            short&              rRecursion;
public:
                                ScCompilerRecursionGuard( short& rRec )
                                    : rRecursion( rRec ) { ++rRecursion; }
                                ~ScCompilerRecursionGuard() { --rRecursion; }
};


void ScCompiler::fillFromAddInMap( NonConstOpCodeMapPtr xMap,FormulaGrammar::Grammar _eGrammar  ) const
{
    size_t nSymbolOffset;
    switch( _eGrammar )
    {
        case FormulaGrammar::GRAM_PODF:
            nSymbolOffset = offsetof( AddInMap, pUpper);
            break;
        default:
        case FormulaGrammar::GRAM_ODFF:
            nSymbolOffset = offsetof( AddInMap, pODFF);
            break;
        case FormulaGrammar::GRAM_ENGLISH:
            nSymbolOffset = offsetof( AddInMap, pEnglish);
            break;
    }
    const AddInMap* pMap = GetAddInMap();
    const AddInMap* const pStop = pMap + GetAddInMapCount();
    for ( ; pMap < pStop; ++pMap)
    {
        char const * const * ppSymbol =
            reinterpret_cast< char const * const * >(
                    reinterpret_cast< char const * >(pMap) + nSymbolOffset);
        xMap->putExternal( String::CreateFromAscii( *ppSymbol),
                String::CreateFromAscii( pMap->pOriginal));
    }
}

void ScCompiler::fillFromAddInCollectionUpperName( NonConstOpCodeMapPtr xMap ) const
{
    ScUnoAddInCollection* pColl = ScGlobal::GetAddInCollection();
    long nCount = pColl->GetFuncCount();
    for (long i=0; i < nCount; ++i)
    {
        const ScUnoAddInFuncData* pFuncData = pColl->GetFuncData(i);
        if (pFuncData)
            xMap->putExternalSoftly( pFuncData->GetUpperName(),
                    pFuncData->GetOriginalName());
    }
}

void ScCompiler::fillFromAddInCollectionEnglishName( NonConstOpCodeMapPtr xMap ) const
{
    ScUnoAddInCollection* pColl = ScGlobal::GetAddInCollection();
    long nCount = pColl->GetFuncCount();
    for (long i=0; i < nCount; ++i)
    {
        const ScUnoAddInFuncData* pFuncData = pColl->GetFuncData(i);
        if (pFuncData)
        {
            String aName;
            if (pFuncData->GetExcelName( LANGUAGE_ENGLISH_US, aName))
                xMap->putExternalSoftly( aName, pFuncData->GetOriginalName());
            else
                xMap->putExternalSoftly( pFuncData->GetUpperName(),
                        pFuncData->GetOriginalName());
        }
    }
}


#ifdef erGENERATEMAPPING
// Run in en-US UI by calling from within gdb, edit pODFF entries afterwards.
void dbg_call_generateMappingODFF()
{
    // static ScCompiler members
    fprintf( stdout, "%s", "static struct AddInMap\n{\n    const char* pODFF;\n    const char* pEnglish;\n    bool        bMapDupToInternal;\n    const char* pOriginal;\n    const char* pUpper;\n} maAddInMap[];\n");
    fprintf( stdout, "%s", "static const AddInMap* GetAddInMap();\n");
    fprintf( stdout, "%s", "static size_t GetAddInMapCount();\n");
    fprintf( stdout, "addinfuncdata___:%s", "ScCompiler::AddInMap ScCompiler::maAddInMap[] =\n{\n");
    ScUnoAddInCollection* pColl = ScGlobal::GetAddInCollection();
    long nCount = pColl->GetFuncCount();
    for (long i=0; i < nCount; ++i)
    {
        const ScUnoAddInFuncData* pFuncData = pColl->GetFuncData(i);
        if (pFuncData)
        {
#define out(rStr) (ByteString( rStr, RTL_TEXTENCODING_UTF8).GetBuffer())
            String aL = pFuncData->GetUpperLocal();
            String aP = pFuncData->GetOriginalName();
            String aU = pFuncData->GetUpperName();
            fprintf( stdout, "addinfuncdata%3ld:    { \"%s\", \"%s\", false, \"%s\", \"%s\" },\n",
                    i, out(aL), out(aL), out(aP), out(aU));
#undef out
        }
    }
    fprintf( stdout, "addinfuncdata___:%s", "};\n");
    fprintf( stdout, "%s", "\n// static\nconst ScCompiler::AddInMap* ScCompiler::GetAddInMap()\n{\n    return maAddInMap;\n}\n");
    fprintf( stdout, "%s", "\n// static\nsize_t ScCompiler::GetAddInMapCount()\n{\n    return sizeof(maAddInMap)/sizeof(maAddInMap[0]);\n}\n");
    fflush( stdout);
}
#endif  // erGENERATEMAPPING

#ifdef erGENERATEMAPPINGDIFF
// Run in en-US UI by calling from within gdb.
void dbg_call_generateMappingDiff()
{
    using namespace ::com::sun::star::sheet;
    ScCompiler::OpCodeMapPtr xPODF = ScCompiler::GetOpCodeMap(
            FormulaLanguage::ODF_11);
    ScCompiler::OpCodeMapPtr xODFF = ScCompiler::GetOpCodeMap(
            FormulaLanguage::ODFF);
    ScCompiler::OpCodeMapPtr xENUS = ScCompiler::GetOpCodeMap(
            FormulaLanguage::ENGLISH);
    sal_uInt16 nPODF = xPODF->getSymbolCount();
    sal_uInt16 nODFF = xODFF->getSymbolCount();
    sal_uInt16 nENUS = xENUS->getSymbolCount();
    printf( "%s\n", "This is a semicolon separated file, you may import it as such to Calc.");
    printf( "%s\n", "Spreadsheet functions name differences between PODF (ODF < 1.2) and ODFF (ODF >= 1.2), plus English UI names.");
    printf( "\nInternal OpCodes; PODF: %d; ODFF: %d; ENUS: %d\n",
            (int)nPODF, (int)nODFF, (int)nENUS);
    sal_uInt16 nMax = ::std::max( ::std::max( nPODF, nODFF), nENUS);
#define out(rStr) (ByteString( rStr, RTL_TEXTENCODING_UTF8).GetBuffer())
    for (sal_uInt16 i=0; i < nMax; ++i)
    {
        const String& rPODF = xPODF->getSymbol(static_cast<OpCode>(i));
        const String& rODFF = xODFF->getSymbol(static_cast<OpCode>(i));
        const String& rENUS = xENUS->getSymbol(static_cast<OpCode>(i));
        if (rPODF != rODFF)
            printf( "%d;%s;%s;%s\n", (int)i, out(rPODF), out(rODFF), out(rENUS));
    }
    // Actually they should all differ, so we could simply list them all, but
    // this is correct and we would find odd things, if any.
    const ExternalHashMap* pPODF = xPODF->getReverseExternalHashMap();
    const ExternalHashMap* pODFF = xODFF->getReverseExternalHashMap();
    const ExternalHashMap* pENUS = xENUS->getReverseExternalHashMap();
    printf( "\n%s\n", "Add-In mapping");
    for (ExternalHashMap::const_iterator it = pPODF->begin(); it != pPODF->end(); ++it)
    {
        ExternalHashMap::const_iterator iLookODFF = pODFF->find( (*it).first);
        ExternalHashMap::const_iterator iLookENUS = pENUS->find( (*it).first);
        String aNative( iLookENUS == pENUS->end() ?
                String::CreateFromAscii( "ENGLISH_SYMBOL_NOT_FOUND") :
                (*iLookENUS).second);
        if (iLookODFF == pODFF->end())
            printf( "NOT FOUND;%s;;%s\n", out((*it).first), out(aNative));
        else if((*it).second == (*iLookODFF).second)    // upper equal
            printf( "EQUAL;%s;%s;%s\n", out((*it).first), out((*iLookODFF).second), out(aNative));
        else
            printf( ";%s;%s;%s\n", out((*it).first), out((*iLookODFF).second), out(aNative));
    }
#undef out
    fflush( stdout);
}
#endif  // erGENERATEMAPPINGDIFF

// static
void ScCompiler::DeInit()
{
    if (pCharClassEnglish)
    {
        delete pCharClassEnglish;
        pCharClassEnglish = NULL;
    }
}

bool ScCompiler::IsEnglishSymbol( const String& rName )
{
    // function names are always case-insensitive
    String aUpper( ScGlobal::pCharClass->upper( rName ) );

    // 1. built-in function name
    OpCode eOp = ScCompiler::GetEnglishOpCode( aUpper );
    if ( eOp != ocNone )
    {
        return true;
    }
    // 2. old add in functions
    sal_uInt16 nIndex;
    if ( ScGlobal::GetFuncCollection()->SearchFunc( aUpper, nIndex ) )
    {
        return true;
    }

    // 3. new (uno) add in functions
    String aIntName(ScGlobal::GetAddInCollection()->FindFunction( aUpper, sal_False ));
    if (aIntName.Len())
    {
        return true;
    }
    return false;		// no valid function name
}

// static
void ScCompiler::InitCharClassEnglish()
{
    ::com::sun::star::lang::Locale aLocale(
            OUString( RTL_CONSTASCII_USTRINGPARAM( "en")),
            OUString( RTL_CONSTASCII_USTRINGPARAM( "US")),
            OUString());
    pCharClassEnglish = new CharClass(
            ::comphelper::getProcessServiceFactory(), aLocale);
}


void ScCompiler::SetGrammar( const FormulaGrammar::Grammar eGrammar )
{
    DBG_ASSERT( eGrammar != FormulaGrammar::GRAM_UNSPECIFIED, "ScCompiler::SetGrammar: don't pass FormulaGrammar::GRAM_UNSPECIFIED");
    if (eGrammar == GetGrammar())
        return;     // nothing to be done

    if( eGrammar == FormulaGrammar::GRAM_EXTERNAL )
    {
        meGrammar = eGrammar;
        mxSymbols = GetOpCodeMap( ::com::sun::star::sheet::FormulaLanguage::NATIVE);
    }
    else
    {
        FormulaGrammar::Grammar eMyGrammar = eGrammar;
        const sal_Int32 nFormulaLanguage = FormulaGrammar::extractFormulaLanguage( eMyGrammar);
        OpCodeMapPtr xMap = GetOpCodeMap( nFormulaLanguage);
        DBG_ASSERT( xMap, "ScCompiler::SetGrammar: unknown formula language");
        if (!xMap)
        {
            xMap = GetOpCodeMap( ::com::sun::star::sheet::FormulaLanguage::NATIVE);
            eMyGrammar = xMap->getGrammar();
        }

        // Save old grammar for call to SetGrammarAndRefConvention().
        FormulaGrammar::Grammar eOldGrammar = GetGrammar();
        // This also sets the grammar associated with the map!
        SetFormulaLanguage( xMap);

        // Override if necessary.
        if (eMyGrammar != GetGrammar())
            SetGrammarAndRefConvention( eMyGrammar, eOldGrammar);
    }
}

void ScCompiler::SetEncodeUrlMode( EncodeUrlMode eMode )
{
    meEncodeUrlMode = eMode;
}

ScCompiler::EncodeUrlMode ScCompiler::GetEncodeUrlMode() const
{
    return meEncodeUrlMode;
}

void ScCompiler::SetFormulaLanguage( const ScCompiler::OpCodeMapPtr & xMap )
{
    if (xMap.get())
    {
        mxSymbols = xMap;
        if (mxSymbols->isEnglish())
        {
            if (!pCharClassEnglish)
                InitCharClassEnglish();
            pCharClass = pCharClassEnglish;
        }
        else
            pCharClass = ScGlobal::pCharClass;
        SetGrammarAndRefConvention( mxSymbols->getGrammar(), GetGrammar());
    }
}


void ScCompiler::SetGrammarAndRefConvention(
        const FormulaGrammar::Grammar eNewGrammar, const FormulaGrammar::Grammar eOldGrammar )
{
    meGrammar = eNewGrammar;    //! SetRefConvention needs the new grammar set!
    FormulaGrammar::AddressConvention eConv = FormulaGrammar::extractRefConvention( meGrammar);
    if (eConv == FormulaGrammar::CONV_UNSPECIFIED && eOldGrammar == FormulaGrammar::GRAM_UNSPECIFIED)
    {
        if (pDoc)
            SetRefConvention( pDoc->GetAddressConvention());
        else
            SetRefConvention( pConvOOO_A1);
    }
    else
        SetRefConvention( eConv );
}

String ScCompiler::FindAddInFunction( const String& rUpperName, sal_Bool bLocalFirst ) const
{
    return ScGlobal::GetAddInCollection()->FindFunction(rUpperName, bLocalFirst);    // bLocalFirst=sal_False for english
}


#ifdef erDEBUG
void dbg_call_testcreatemapping()
{
    using namespace ::com::sun::star::sheet;
    ScCompiler::OpCodeMapPtr xMap = ScCompiler::GetOpCodeMap( FormulaLanguage::ODFF);
    xMap->createSequenceOfAvailableMappings( FormulaMapGroup::FUNCTIONS);
}
#endif

//-----------------------------------------------------------------------------

ScCompiler::Convention::~Convention()
{
    delete [] mpCharTable;
    mpCharTable = NULL;
}

ScCompiler::Convention::Convention( FormulaGrammar::AddressConvention eConv )
        :
    meConv( eConv )
{
    int i;
    sal_uLong *t= new sal_uLong [128];

    ScCompiler::pConventions[ meConv ] = this;
    mpCharTable = t;

    for (i = 0; i < 128; i++)
        t[i] = SC_COMPILER_C_ILLEGAL;

/*   */     t[32] = SC_COMPILER_C_CHAR_DONTCARE | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* ! */     t[33] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
    if (FormulaGrammar::CONV_ODF == meConv)
/* ! */     t[33] |= SC_COMPILER_C_ODF_LABEL_OP;
/* " */     t[34] = SC_COMPILER_C_CHAR_STRING | SC_COMPILER_C_STRING_SEP;
/* # */     t[35] = SC_COMPILER_C_WORD_SEP;
/* $ */     t[36] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT;
    if (FormulaGrammar::CONV_ODF == meConv)
/* $ */     t[36] |= SC_COMPILER_C_ODF_NAME_MARKER;
/* % */     t[37] = SC_COMPILER_C_VALUE;
/* & */     t[38] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* ' */     t[39] = SC_COMPILER_C_NAME_SEP;
/* ( */     t[40] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* ) */     t[41] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* * */     t[42] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* + */     t[43] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_EXP | SC_COMPILER_C_VALUE_SIGN;
/* , */     t[44] = SC_COMPILER_C_CHAR_VALUE | SC_COMPILER_C_VALUE;
/* - */     t[45] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_EXP | SC_COMPILER_C_VALUE_SIGN;
/* . */     t[46] = SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_VALUE | SC_COMPILER_C_VALUE | SC_COMPILER_C_IDENT | SC_COMPILER_C_NAME;
/* / */     t[47] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;

    for (i = 48; i < 58; i++)
/* 0-9 */   t[i] = SC_COMPILER_C_CHAR_VALUE | SC_COMPILER_C_WORD | SC_COMPILER_C_VALUE | SC_COMPILER_C_VALUE_EXP | SC_COMPILER_C_VALUE_VALUE | SC_COMPILER_C_IDENT | SC_COMPILER_C_NAME;

/* : */     t[58] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD;
/* ; */     t[59] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* < */     t[60] = SC_COMPILER_C_CHAR_BOOL | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* = */     t[61] = SC_COMPILER_C_CHAR | SC_COMPILER_C_BOOL | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* > */     t[62] = SC_COMPILER_C_CHAR_BOOL | SC_COMPILER_C_BOOL | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* ? */     t[63] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_NAME;
/* @ */     // FREE

    for (i = 65; i < 91; i++)
/* A-Z */   t[i] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT | SC_COMPILER_C_CHAR_NAME | SC_COMPILER_C_NAME;

    if (FormulaGrammar::CONV_ODF == meConv)
    {
/* [ */     t[91] = SC_COMPILER_C_ODF_LBRACKET;
/* \ */     // FREE
/* ] */     t[93] = SC_COMPILER_C_ODF_RBRACKET;
    }
    else
    {
/* [ */     // FREE
/* \ */     // FREE
/* ] */     // FREE
    }
/* ^ */     t[94] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
/* _ */     t[95] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT | SC_COMPILER_C_CHAR_NAME | SC_COMPILER_C_NAME;
/* ` */     // FREE

    for (i = 97; i < 123; i++)
/* a-z */   t[i] = SC_COMPILER_C_CHAR_WORD | SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_IDENT | SC_COMPILER_C_IDENT | SC_COMPILER_C_CHAR_NAME | SC_COMPILER_C_NAME;

/* { */     t[123] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP; // array open
/* | */     t[124] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP; // array row sep (Should be OOo specific)
/* } */     t[125] = SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP; // array close
/* ~ */     t[126] = SC_COMPILER_C_CHAR;        // OOo specific
/* 127 */   // FREE

    if( FormulaGrammar::CONV_XL_A1 == meConv || FormulaGrammar::CONV_XL_R1C1 == meConv || FormulaGrammar::CONV_XL_OOX == meConv )
    {
/*   */     t[32] |=   SC_COMPILER_C_WORD;
/* ! */     t[33] |=   SC_COMPILER_C_IDENT | SC_COMPILER_C_WORD;
/* " */     t[34] |=   SC_COMPILER_C_WORD;
/* # */     t[35] &= (~SC_COMPILER_C_WORD_SEP);
/* # */     t[35] |=   SC_COMPILER_C_WORD;
/* % */     t[37] |=   SC_COMPILER_C_WORD;
/* ' */     t[39] |=   SC_COMPILER_C_WORD;

/* % */     t[37] |=   SC_COMPILER_C_WORD;
/* & */     t[38] |=   SC_COMPILER_C_WORD;
/* ' */     t[39] |=   SC_COMPILER_C_WORD;
/* ( */     t[40] |=   SC_COMPILER_C_WORD;
/* ) */     t[41] |=   SC_COMPILER_C_WORD;
/* * */     t[42] |=   SC_COMPILER_C_WORD;
/* + */     t[43] |=   SC_COMPILER_C_WORD;
#if 0 /* this really needs to be locale specific. */
/* , */     t[44]  =   SC_COMPILER_C_CHAR | SC_COMPILER_C_WORD_SEP | SC_COMPILER_C_VALUE_SEP;
#else
/* , */     t[44] |=   SC_COMPILER_C_WORD;
#endif
/* - */     t[45] |=   SC_COMPILER_C_WORD;

/* ; */     t[59] |=   SC_COMPILER_C_WORD;
/* < */     t[60] |=   SC_COMPILER_C_WORD;
/* = */     t[61] |=   SC_COMPILER_C_WORD;
/* > */     t[62] |=   SC_COMPILER_C_WORD;
/* ? */     // question really is not permitted in sheet name
/* @ */     t[64] |=   SC_COMPILER_C_WORD;
/* [ */     t[91] |=   SC_COMPILER_C_WORD;
/* ] */     t[93] |=   SC_COMPILER_C_WORD;
/* { */     t[123]|=   SC_COMPILER_C_WORD;
/* | */     t[124]|=   SC_COMPILER_C_WORD;
/* } */     t[125]|=   SC_COMPILER_C_WORD;
/* ~ */     t[126]|=   SC_COMPILER_C_WORD;

        if( FormulaGrammar::CONV_XL_R1C1 == meConv )
        {
/* - */     t[45] |= SC_COMPILER_C_IDENT;
/* [ */     t[91] |= SC_COMPILER_C_IDENT;
/* ] */     t[93] |= SC_COMPILER_C_IDENT;
        }
        if( FormulaGrammar::CONV_XL_OOX == meConv )
        {
/* [ */     t[91] |= SC_COMPILER_C_CHAR_IDENT;
/* ] */     t[93] |= SC_COMPILER_C_IDENT;
        }
    }
}

//-----------------------------------------------------------------------------

static bool lcl_isValidQuotedText( const String& rFormula, xub_StrLen nSrcPos, ParseResult& rRes )
{
    // Tokens that start at ' can have anything in them until a final '
    // but '' marks an escaped '
    // We've earlier guaranteed that a string containing '' will be
    // surrounded by '
    if (rFormula.GetChar(nSrcPos) == '\'')
    {
        xub_StrLen nPos = nSrcPos+1;
        while (nPos < rFormula.Len())
        {
            if (rFormula.GetChar(nPos) == '\'')
            {
                if ( (nPos+1 == rFormula.Len()) || (rFormula.GetChar(nPos+1) != '\'') )
                {
                    rRes.TokenType = KParseType::SINGLE_QUOTE_NAME;
                    rRes.EndPos = nPos+1;
                    return true;
                }
                ++nPos;
            }
            ++nPos;
        }
    }

    return false;
}

static bool lcl_parseExternalName(
        const String& rSymbol,
        String& rFile,
        String& rName,
        const sal_Unicode cSep,
        const ScDocument* pDoc = NULL,
        const uno::Sequence< const sheet::ExternalLinkInfo > * pExternalLinks = NULL )
{
    /* TODO: future versions will have to support sheet-local names too, thus
     * return a possible sheet name as well. */
    const sal_Unicode* const pStart = rSymbol.GetBuffer();
    const sal_Unicode* p = pStart;
    xub_StrLen nLen = rSymbol.Len();
    sal_Unicode cPrev = 0;
    String aTmpFile, aTmpName;
    xub_StrLen i = 0;
    bool bInName = false;
    if (cSep == '!')
    {
        // For XL use existing parser that resolves bracketed and quoted and
        // indexed external document names.
        ScRange aRange;
        String aStartTabName, aEndTabName;
        sal_uInt16 nFlags = 0;
        p = aRange.Parse_XL_Header( p, pDoc, aTmpFile, aStartTabName,
                aEndTabName, nFlags, true, pExternalLinks );
        if (!p || p == pStart)
            return false;
        i = xub_StrLen(p - pStart);
        cPrev = *(p-1);
    }
    for ( ; i < nLen; ++i, ++p)
    {
        sal_Unicode c = *p;
        if (i == 0)
        {
            if (c == '.' || c == cSep)
                return false;

            if (c == '\'')
            {
                // Move to the next chart and loop until the second single
                // quote.
                cPrev = c;
                ++i; ++p;
                for (xub_StrLen j = i; j < nLen; ++j, ++p)
                {
                    c = *p;
                    if (c == '\'')
                    {
                        if (j == i)
                        {
                            // empty quote e.g. (=''!Name)
                            return false;
                        }

                        if (cPrev == '\'')
                        {
                            // two consecutive quotes equals a single
                            // quote in the file name.
                            aTmpFile.Append(c);
                            cPrev = 'a';
                        }
                        else
                            cPrev = c;

                        continue;
                    }

                    if (cPrev == '\'' && j != i)
                    {
                        // this is not a quote but the previous one
                        // is.  This ends the parsing of the quoted
                        // segment.

                        i = j;
                        bInName = true;
                        break;
                    }
                    aTmpFile.Append(c);
                    cPrev = c;
                }

                if (!bInName)
                {
                    // premature ending of the quoted segment.
                    return false;
                }

                if (c != cSep)
                {
                    // only the separator is allowed after the closing quote.
                    return false;
                }

                cPrev = c;
                continue;
            }
        }

        if (bInName)
        {
            if (c == cSep)
            {
                // A second separator ?  Not a valid external name.
                return false;
            }
            aTmpName.Append(c);
        }
        else
        {
            if (c == cSep)
            {
                bInName = true;
            }
            else
            {
                do
                {
                    if (CharClass::isAsciiAlphaNumeric(c))
                        // allowed.
                        break;

                    if (c > 128)
                        // non-ASCII character is allowed.
                        break;

                    bool bValid = false;
                    switch (c)
                    {
                        case '_':
                        case '-':
                        case '.':
                            // these special characters are allowed.
                            bValid = true;
                            break;
                    }
                    if (bValid)
                        break;

                    return false;
                }
                while (false);
                aTmpFile.Append(c);
            }
        }
        cPrev = c;
    }

    if (!bInName)
    {
        // No name found - most likely the symbol has no '!'s.
        return false;
    }

    rFile = aTmpFile;
    rName = aTmpName;
    return true;
}

static String lcl_makeExternalNameStr( const String& rFile, const String& rName,
        const sal_Unicode cSep, bool bODF )
{
    String aFile( rFile), aName( rName), aEscQuote( RTL_CONSTASCII_USTRINGPARAM("''"));
    aFile.SearchAndReplaceAllAscii( "'", aEscQuote);
    if (bODF)
        aName.SearchAndReplaceAllAscii( "'", aEscQuote);
    rtl::OUStringBuffer aBuf( aFile.Len() + aName.Len() + 9);
    if (bODF)
        aBuf.append( sal_Unicode( '['));
    aBuf.append( sal_Unicode( '\''));
    aBuf.append( aFile);
    aBuf.append( sal_Unicode( '\''));
    aBuf.append( cSep);
    if (bODF)
        aBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "$$'"));
    aBuf.append( aName);
    if (bODF)
        aBuf.appendAscii( RTL_CONSTASCII_STRINGPARAM( "']"));
    return String( aBuf.makeStringAndClear());
}

static bool lcl_getLastTabName( String& rTabName2, const String& rTabName1,
                                const vector<String>& rTabNames, const ScComplexRefData& rRef )
{
    SCsTAB nTabSpan = rRef.Ref2.nTab - rRef.Ref1.nTab;
    if (nTabSpan > 0)
    {
        size_t nCount = rTabNames.size();
        vector<String>::const_iterator itrBeg = rTabNames.begin(), itrEnd = rTabNames.end();
        vector<String>::const_iterator itr = ::std::find(itrBeg, itrEnd, rTabName1);
        if (itr == rTabNames.end())
        {
            rTabName2 = ScGlobal::GetRscString(STR_NO_REF_TABLE);
            return false;
        }

        size_t nDist = ::std::distance(itrBeg, itr);
        if (nDist + static_cast<size_t>(nTabSpan) >= nCount)
        {
            rTabName2 = ScGlobal::GetRscString(STR_NO_REF_TABLE);
            return false;
        }

        rTabName2 = rTabNames[nDist+nTabSpan];
    }
    else
        rTabName2 = rTabName1;

    return true;
}

struct Convention_A1 : public ScCompiler::Convention
{
    Convention_A1( FormulaGrammar::AddressConvention eConv ) : ScCompiler::Convention( eConv ) { }
    static void MakeColStr( rtl::OUStringBuffer& rBuffer, SCCOL nCol );
    static void MakeRowStr( rtl::OUStringBuffer& rBuffer, SCROW nRow );

    ParseResult parseAnyToken( const String& rFormula,
                               xub_StrLen nSrcPos,
                               const CharClass* pCharClass) const
    {
        ParseResult aRet;
        if ( lcl_isValidQuotedText(rFormula, nSrcPos, aRet) )
            return aRet;

        static const sal_Int32 nStartFlags = KParseTokens::ANY_LETTER_OR_NUMBER |
            KParseTokens::ASC_UNDERSCORE | KParseTokens::ASC_DOLLAR;
        static const sal_Int32 nContFlags = nStartFlags | KParseTokens::ASC_DOT;
        // '?' allowed in range names because of Xcl :-/
        static const String aAddAllowed(String::CreateFromAscii("?#"));
        return pCharClass->parseAnyToken( rFormula,
                nSrcPos, nStartFlags, aAddAllowed, nContFlags, aAddAllowed );
    }
};

void Convention_A1::MakeColStr( rtl::OUStringBuffer& rBuffer, SCCOL nCol )
{
    if ( !ValidCol( nCol) )
        rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
    else
        ::ScColToAlpha( rBuffer, nCol);
}

void Convention_A1::MakeRowStr( rtl::OUStringBuffer& rBuffer, SCROW nRow )
{
    if ( !ValidRow(nRow) )
        rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
    else
        rBuffer.append(sal_Int32(nRow + 1));
}

//-----------------------------------------------------------------------------

struct ConventionOOO_A1 : public Convention_A1
{
    ConventionOOO_A1() : Convention_A1 (FormulaGrammar::CONV_OOO) { }
    ConventionOOO_A1( FormulaGrammar::AddressConvention eConv ) : Convention_A1 (eConv) { }
    static String MakeTabStr( const ScCompiler& rComp, SCTAB nTab, String& aDoc )
    {
        String aString;
        if (!rComp.GetDoc()->GetName( nTab, aString ))
            aString = ScGlobal::GetRscString(STR_NO_REF_TABLE);
        else
        {
            // "'Doc'#Tab"
            xub_StrLen nPos = ScCompiler::GetDocTabPos( aString);
            if (nPos != STRING_NOTFOUND)
            {
                aDoc = aString.Copy( 0, nPos + 1 );
                aString.Erase( 0, nPos + 1 );
                aDoc = INetURLObject::decode( aDoc, INET_HEX_ESCAPE,
                        INetURLObject::DECODE_UNAMBIGUOUS );
            }
            else
                aDoc.Erase();
            ScCompiler::CheckTabQuotes( aString, FormulaGrammar::CONV_OOO );
        }
        aString += '.';
        return aString;
    }

    void MakeRefStrImpl( rtl::OUStringBuffer&   rBuffer,
                         const ScCompiler&      rComp,
                         const ScComplexRefData&    rRef,
                         bool bSingleRef,
                         bool bODF ) const
    {
        if (bODF)
            rBuffer.append(sal_Unicode('['));
        ScComplexRefData aRef( rRef );
        // In case absolute/relative positions weren't separately available:
        // transform relative to absolute!
        //  AdjustReference( aRef.Ref1 );
        //  if( !bSingleRef )
        //      AdjustReference( aRef.Ref2 );
        aRef.Ref1.CalcAbsIfRel( rComp.GetPos() );
        if( !bSingleRef )
            aRef.Ref2.CalcAbsIfRel( rComp.GetPos() );
        if( aRef.Ref1.IsFlag3D() )
        {
            if (aRef.Ref1.IsTabDeleted())
            {
                if (!aRef.Ref1.IsTabRel())
                    rBuffer.append(sal_Unicode('$'));
                rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
                rBuffer.append(sal_Unicode('.'));
            }
            else
            {
                String aDoc;
                String aRefStr( MakeTabStr( rComp, aRef.Ref1.nTab, aDoc ) );
                rBuffer.append(aDoc);
                if (!aRef.Ref1.IsTabRel()) rBuffer.append(sal_Unicode('$'));
                rBuffer.append(aRefStr);
            }
        }
        else if (bODF)
            rBuffer.append(sal_Unicode('.'));
        if (!aRef.Ref1.IsColRel())
            rBuffer.append(sal_Unicode('$'));
        if ( aRef.Ref1.IsColDeleted() )
            rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
        else
            MakeColStr(rBuffer, aRef.Ref1.nCol );
        if (!aRef.Ref1.IsRowRel())
            rBuffer.append(sal_Unicode('$'));
        if ( aRef.Ref1.IsRowDeleted() )
            rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
        else
            MakeRowStr( rBuffer, aRef.Ref1.nRow );
        if (!bSingleRef)
        {
            rBuffer.append(sal_Unicode(':'));
            if (aRef.Ref2.IsFlag3D() || aRef.Ref2.nTab != aRef.Ref1.nTab)
            {
                if (aRef.Ref2.IsTabDeleted())
                {
                    if (!aRef.Ref2.IsTabRel())
                        rBuffer.append(sal_Unicode('$'));
                    rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
                    rBuffer.append(sal_Unicode('.'));
                }
                else
                {
                    String aDoc;
                    String aRefStr( MakeTabStr( rComp, aRef.Ref2.nTab, aDoc ) );
                    rBuffer.append(aDoc);
                    if (!aRef.Ref2.IsTabRel()) rBuffer.append(sal_Unicode('$'));
                    rBuffer.append(aRefStr);
                }
            }
            else if (bODF)
                rBuffer.append(sal_Unicode('.'));
            if (!aRef.Ref2.IsColRel())
                rBuffer.append(sal_Unicode('$'));
            if ( aRef.Ref2.IsColDeleted() )
                rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
            else
                MakeColStr( rBuffer, aRef.Ref2.nCol );
            if (!aRef.Ref2.IsRowRel())
                rBuffer.append(sal_Unicode('$'));
            if ( aRef.Ref2.IsRowDeleted() )
                rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
            else
                MakeRowStr( rBuffer, aRef.Ref2.nRow );
        }
        if (bODF)
            rBuffer.append(sal_Unicode(']'));
    }

    void MakeRefStr( rtl::OUStringBuffer&   rBuffer,
                     const ScCompiler&      rComp,
                     const ScComplexRefData& rRef,
                     sal_Bool bSingleRef ) const
    {
        MakeRefStrImpl( rBuffer, rComp, rRef, bSingleRef, false);
    }

    virtual sal_Unicode getSpecialSymbol( SpecialSymbolType eSymType ) const
    {
        switch (eSymType)
        {
            case ScCompiler::Convention::ABS_SHEET_PREFIX:
                return '$';
            case ScCompiler::Convention::SHEET_SEPARATOR:
                return '.';
        }

        return sal_Unicode(0);
    }

    virtual bool parseExternalName( const String& rSymbol, String& rFile, String& rName,
            const ScDocument* pDoc,
            const ::com::sun::star::uno::Sequence<
                const ::com::sun::star::sheet::ExternalLinkInfo > * pExternalLinks ) const
    {
        return lcl_parseExternalName(rSymbol, rFile, rName, sal_Unicode('#'), pDoc, pExternalLinks);
    }

    virtual String makeExternalNameStr( const String& rFile, const String& rName ) const
    {
        return lcl_makeExternalNameStr( rFile, rName, sal_Unicode('#'), false);
    }

    bool makeExternalSingleRefStr( ::rtl::OUStringBuffer& rBuffer, sal_uInt16 nFileId,
                                   const String& rTabName, const ScSingleRefData& rRef,
                                   ScExternalRefManager* pRefMgr, bool bDisplayTabName, bool bEncodeUrl ) const
    {
        if (bDisplayTabName)
        {
            String aFile;
            const String* p = pRefMgr->getExternalFileName(nFileId);
            if (p)
            {
                if (bEncodeUrl)
                    aFile = *p;
                else
                    aFile = INetURLObject::decode(*p, INET_HEX_ESCAPE, INetURLObject::DECODE_UNAMBIGUOUS);
            }
            aFile.SearchAndReplaceAllAscii("'", String::CreateFromAscii("''"));

            rBuffer.append(sal_Unicode('\''));
            rBuffer.append(aFile);
            rBuffer.append(sal_Unicode('\''));
            rBuffer.append(sal_Unicode('#'));

            if (!rRef.IsTabRel())
                rBuffer.append(sal_Unicode('$'));
            ScRangeStringConverter::AppendTableName(rBuffer, rTabName);

            rBuffer.append(sal_Unicode('.'));
        }

        if (!rRef.IsColRel())
            rBuffer.append(sal_Unicode('$'));
        MakeColStr( rBuffer, rRef.nCol);
        if (!rRef.IsRowRel())
            rBuffer.append(sal_Unicode('$'));
        MakeRowStr( rBuffer, rRef.nRow);

        return true;
    }

    void makeExternalRefStrImpl( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef,
                                     ScExternalRefManager* pRefMgr, bool bODF ) const
    {
        ScSingleRefData aRef(rRef);
        aRef.CalcAbsIfRel(rCompiler.GetPos());

        if (bODF)
            rBuffer.append( sal_Unicode('['));

        bool bEncodeUrl = true;
        switch (rCompiler.GetEncodeUrlMode())
        {
            case ScCompiler::ENCODE_BY_GRAMMAR:
                bEncodeUrl = bODF;
            break;
            case ScCompiler::ENCODE_ALWAYS:
                bEncodeUrl = true;
            break;
            case ScCompiler::ENCODE_NEVER:
                bEncodeUrl = false;
            break;
            default:
                ;
        }
        makeExternalSingleRefStr(rBuffer, nFileId, rTabName, aRef, pRefMgr, true, bEncodeUrl);
        if (bODF)
            rBuffer.append( sal_Unicode(']'));
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        makeExternalRefStrImpl( rBuffer, rCompiler, nFileId, rTabName, rRef, pRefMgr, false);
    }

    void makeExternalRefStrImpl( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef,
                                     ScExternalRefManager* pRefMgr, bool bODF ) const
    {
        ScComplexRefData aRef(rRef);
        aRef.CalcAbsIfRel(rCompiler.GetPos());

        if (bODF)
            rBuffer.append( sal_Unicode('['));
        // Ensure that there's always a closing bracket, no premature returns.
        bool bEncodeUrl = true;
        switch (rCompiler.GetEncodeUrlMode())
        {
            case ScCompiler::ENCODE_BY_GRAMMAR:
                bEncodeUrl = bODF;
            break;
            case ScCompiler::ENCODE_ALWAYS:
                bEncodeUrl = true;
            break;
            case ScCompiler::ENCODE_NEVER:
                bEncodeUrl = false;
            break;
            default:
                ;
        }

        do
        {
            if (!makeExternalSingleRefStr(rBuffer, nFileId, rTabName, aRef.Ref1, pRefMgr, true, bEncodeUrl))
                break;

            rBuffer.append(sal_Unicode(':'));

            String aLastTabName;
            bool bDisplayTabName = (aRef.Ref1.nTab != aRef.Ref2.nTab);
            if (bDisplayTabName)
            {
                // Get the name of the last table.
                vector<String> aTabNames;
                pRefMgr->getAllCachedTableNames(nFileId, aTabNames);
                if (aTabNames.empty())
                {
                    DBG_ERROR1( "ConventionOOO_A1::makeExternalRefStrImpl: no sheet names for document ID %s", nFileId);
                }

                if (!lcl_getLastTabName(aLastTabName, rTabName, aTabNames, aRef))
                {
                    DBG_ERROR( "ConventionOOO_A1::makeExternalRefStrImpl: sheet name not found");
                    // aLastTabName contains #REF!, proceed.
                }
            }
            else if (bODF)
                rBuffer.append( sal_Unicode('.'));      // need at least the sheet separator in ODF
            makeExternalSingleRefStr( rBuffer, nFileId, aLastTabName,
                    aRef.Ref2, pRefMgr, bDisplayTabName, bEncodeUrl);
        } while (0);
        if (bODF)
            rBuffer.append( sal_Unicode(']'));
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        makeExternalRefStrImpl( rBuffer, rCompiler, nFileId, rTabName, rRef, pRefMgr, false);
    }
};


static const ConventionOOO_A1 ConvOOO_A1;
const ScCompiler::Convention * const ScCompiler::pConvOOO_A1 = &ConvOOO_A1;

//-----------------------------------------------------------------------------

struct ConventionOOO_A1_ODF : public ConventionOOO_A1
{
    ConventionOOO_A1_ODF() : ConventionOOO_A1 (FormulaGrammar::CONV_ODF) { }
    void MakeRefStr( rtl::OUStringBuffer&   rBuffer,
                     const ScCompiler&      rComp,
                     const ScComplexRefData& rRef,
                     sal_Bool bSingleRef ) const
    {
        MakeRefStrImpl( rBuffer, rComp, rRef, bSingleRef, true);
    }

    virtual String makeExternalNameStr( const String& rFile, const String& rName ) const
    {
        return lcl_makeExternalNameStr( rFile, rName, sal_Unicode('#'), true);
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        makeExternalRefStrImpl( rBuffer, rCompiler, nFileId, rTabName, rRef, pRefMgr, true);
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        makeExternalRefStrImpl( rBuffer, rCompiler, nFileId, rTabName, rRef, pRefMgr, true);
    }
};

static const ConventionOOO_A1_ODF ConvOOO_A1_ODF;
const ScCompiler::Convention * const ScCompiler::pConvOOO_A1_ODF = &ConvOOO_A1_ODF;

//-----------------------------------------------------------------------------

struct ConventionXL
{
    static bool GetDocAndTab( const ScCompiler& rComp,
                              const ScSingleRefData& rRef,
                              String& rDocName,
                              String& rTabName )
    {
        bool bHasDoc = false;

        rDocName.Erase();
        if (rRef.IsTabDeleted() ||
            !rComp.GetDoc()->GetName( rRef.nTab, rTabName ))
        {
            rTabName = ScGlobal::GetRscString( STR_NO_REF_TABLE );
            return false;
        }

        // Cheesy hack to unparse the OOO style "'Doc'#Tab"
        xub_StrLen nPos = ScCompiler::GetDocTabPos( rTabName);
        if (nPos != STRING_NOTFOUND)
        {
            rDocName = rTabName.Copy( 0, nPos );
            // TODO : More research into how XL escapes the doc path
            rDocName = INetURLObject::decode( rDocName, INET_HEX_ESCAPE,
                    INetURLObject::DECODE_UNAMBIGUOUS );
            rTabName.Erase( 0, nPos + 1 );
            bHasDoc = true;
        }

        // XL uses the same sheet name quoting conventions in both modes
        // it is safe to use A1 here
        ScCompiler::CheckTabQuotes( rTabName, FormulaGrammar::CONV_XL_A1 );
        return bHasDoc;
    }

    static void MakeDocStr( rtl::OUStringBuffer& rBuf,
                            const ScCompiler& rComp,
                            const ScComplexRefData& rRef,
                            bool bSingleRef )
    {
        if( rRef.Ref1.IsFlag3D() )
        {
            String aStartTabName, aStartDocName, aEndTabName, aEndDocName;
            bool bStartHasDoc = false, bEndHasDoc = false;

            bStartHasDoc = GetDocAndTab( rComp, rRef.Ref1,
                                         aStartDocName, aStartTabName);

            if( !bSingleRef && rRef.Ref2.IsFlag3D() )
            {
                bEndHasDoc = GetDocAndTab( rComp, rRef.Ref2,
                                           aEndDocName, aEndTabName);
            }
            else
                bEndHasDoc = bStartHasDoc;

            if( bStartHasDoc )
            {
                // A ref across multipled workbooks ?
                if( !bEndHasDoc )
                    return;

                rBuf.append( sal_Unicode( '[' ) );
                rBuf.append( aStartDocName );
                rBuf.append( sal_Unicode( ']' ) );
            }

            rBuf.append( aStartTabName );
            if( !bSingleRef && rRef.Ref2.IsFlag3D() && aStartTabName != aEndTabName )
            {
                rBuf.append( sal_Unicode( ':' ) );
                rBuf.append( aEndTabName );
            }

            rBuf.append( sal_Unicode( '!' ) );
        }
    }

    static sal_Unicode getSpecialSymbol( ScCompiler::Convention::SpecialSymbolType eSymType )
    {
        switch (eSymType)
        {
            case ScCompiler::Convention::ABS_SHEET_PREFIX:
                return sal_Unicode(0);
            case ScCompiler::Convention::SHEET_SEPARATOR:
                return '!';
        }
        return sal_Unicode(0);
    }

    static bool parseExternalName( const String& rSymbol, String& rFile, String& rName,
            const ScDocument* pDoc,
            const ::com::sun::star::uno::Sequence<
                const ::com::sun::star::sheet::ExternalLinkInfo > * pExternalLinks )
    {
        return lcl_parseExternalName( rSymbol, rFile, rName, sal_Unicode('!'), pDoc, pExternalLinks);
    }

    static String makeExternalNameStr( const String& rFile, const String& rName )
    {
        return lcl_makeExternalNameStr( rFile, rName, sal_Unicode('!'), false);
    }

    static void makeExternalDocStr( ::rtl::OUStringBuffer& rBuffer, const String& rFullName, bool bEncodeUrl )
    {
        // Format that is easier to deal with inside OOo, because we use file
        // URL, and all characetrs are allowed.  Check if it makes sense to do
        // it the way Gnumeric does it.  Gnumeric doesn't use the URL form
        // and allows relative file path.
        //
        //   ['file:///path/to/source/filename.xls']

        rBuffer.append(sal_Unicode('['));
        rBuffer.append(sal_Unicode('\''));
        String aFullName;
        if (bEncodeUrl)
            aFullName = rFullName;
        else
            aFullName = INetURLObject::decode(rFullName, INET_HEX_ESCAPE, INetURLObject::DECODE_UNAMBIGUOUS);

        const sal_Unicode* pBuf = aFullName.GetBuffer();
        xub_StrLen nLen = aFullName.Len();
        for (xub_StrLen i = 0; i < nLen; ++i)
        {
            const sal_Unicode c = pBuf[i];
            if (c == sal_Unicode('\''))
                rBuffer.append(c);
            rBuffer.append(c);
        }
        rBuffer.append(sal_Unicode('\''));
        rBuffer.append(sal_Unicode(']'));
    }

    static void makeExternalTabNameRange( ::rtl::OUStringBuffer& rBuf, const String& rTabName,
                                          const vector<String>& rTabNames,
                                          const ScComplexRefData& rRef )
    {
        String aLastTabName;
        if (!lcl_getLastTabName(aLastTabName, rTabName, rTabNames, rRef))
        {
            ScRangeStringConverter::AppendTableName(rBuf, aLastTabName);
            return;
        }

        ScRangeStringConverter::AppendTableName(rBuf, rTabName);
        if (rTabName != aLastTabName)
        {
            rBuf.append(sal_Unicode(':'));
            ScRangeStringConverter::AppendTableName(rBuf, rTabName);
        }
    }

    static void parseExternalDocName( const String& rFormula, xub_StrLen& rSrcPos )
    {
        xub_StrLen nLen = rFormula.Len();
        const sal_Unicode* p = rFormula.GetBuffer();
        sal_Unicode cPrev = 0;
        for (xub_StrLen i = rSrcPos; i < nLen; ++i)
        {
            sal_Unicode c = p[i];
            if (i == rSrcPos)
            {
                // first character must be '['.
                if (c != '[')
                    return;
            }
            else if (i == rSrcPos + 1)
            {
                // second character must be a single quote.
                if (c != '\'')
                    return;
            }
            else if (c == '\'')
            {
                if (cPrev == '\'')
                    // two successive single quote is treated as a single
                    // valid character.
                    c = 'a';
            }
            else if (c == ']')
            {
                if (cPrev == '\'')
                {
                    // valid source document path found.  Increment the
                    // current position to skip the source path.
                    rSrcPos = i + 1;
                    if (rSrcPos >= nLen)
                        rSrcPos = nLen - 1;
                    return;
                }
                else
                    return;
            }
            else
            {
                // any other character
                if (i > rSrcPos + 2 && cPrev == '\'')
                    // unless it's the 3rd character, a normal character
                    // following immediately a single quote is invalid.
                    return;
            }
            cPrev = c;
        }
    }
};

struct ConventionXL_A1 : public Convention_A1, public ConventionXL
{
    ConventionXL_A1() : Convention_A1( FormulaGrammar::CONV_XL_A1 ) { }
    ConventionXL_A1( FormulaGrammar::AddressConvention eConv ) : Convention_A1( eConv ) { }

    void makeSingleCellStr( ::rtl::OUStringBuffer& rBuf, const ScSingleRefData& rRef ) const
    {
        if (!rRef.IsColRel())
            rBuf.append(sal_Unicode('$'));
        MakeColStr(rBuf, rRef.nCol);
        if (!rRef.IsRowRel())
            rBuf.append(sal_Unicode('$'));
        MakeRowStr(rBuf, rRef.nRow);
    }

    void MakeRefStr( rtl::OUStringBuffer&   rBuf,
                     const ScCompiler&      rComp,
                     const ScComplexRefData& rRef,
                     sal_Bool bSingleRef ) const
    {
        ScComplexRefData aRef( rRef );

        // Play fast and loose with invalid refs.  There is not much point in producing
        // Foo!A1:#REF! versus #REF! at this point
        aRef.Ref1.CalcAbsIfRel( rComp.GetPos() );

        MakeDocStr( rBuf, rComp, aRef, bSingleRef );

        if( aRef.Ref1.IsColDeleted() || aRef.Ref1.IsRowDeleted() )
        {
            rBuf.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
            return;
        }

        if( !bSingleRef )
        {
            aRef.Ref2.CalcAbsIfRel( rComp.GetPos() );
            if( aRef.Ref2.IsColDeleted() || aRef.Ref2.IsRowDeleted() )
            {
                rBuf.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
                return;
            }

            if( aRef.Ref1.nCol == 0 && aRef.Ref2.nCol >= MAXCOL )
            {
                if (!aRef.Ref1.IsRowRel())
                    rBuf.append(sal_Unicode( '$' ));
                MakeRowStr( rBuf, aRef.Ref1.nRow );
                rBuf.append(sal_Unicode( ':' ));
                if (!aRef.Ref2.IsRowRel())
                    rBuf.append(sal_Unicode( '$' ));
                MakeRowStr( rBuf, aRef.Ref2.nRow );
                return;
            }

            if( aRef.Ref1.nRow == 0 && aRef.Ref2.nRow >= MAXROW )
            {
                if (!aRef.Ref1.IsColRel())
                    rBuf.append(sal_Unicode( '$' ));
                MakeColStr(rBuf, aRef.Ref1.nCol );
                rBuf.append(sal_Unicode( ':' ));
                if (!aRef.Ref2.IsColRel())
                    rBuf.append(sal_Unicode( '$' ));
                MakeColStr(rBuf, aRef.Ref2.nCol );
                return;
            }
        }

        makeSingleCellStr(rBuf, aRef.Ref1);
        if (!bSingleRef)
        {
            rBuf.append(sal_Unicode( ':' ));
            makeSingleCellStr(rBuf, aRef.Ref2);
        }
    }

    virtual ParseResult parseAnyToken( const String& rFormula,
                                       xub_StrLen nSrcPos,
                                       const CharClass* pCharClass) const
    {
        ParseResult aRet;
        if ( lcl_isValidQuotedText(rFormula, nSrcPos, aRet) )
            return aRet;

        static const sal_Int32 nStartFlags = KParseTokens::ANY_LETTER_OR_NUMBER |
            KParseTokens::ASC_UNDERSCORE | KParseTokens::ASC_DOLLAR;
        static const sal_Int32 nContFlags = nStartFlags | KParseTokens::ASC_DOT;
        // '?' allowed in range names
        static const String aAddAllowed = String::CreateFromAscii("?!");
        return pCharClass->parseAnyToken( rFormula,
                nSrcPos, nStartFlags, aAddAllowed, nContFlags, aAddAllowed );
    }

    virtual sal_Unicode getSpecialSymbol( SpecialSymbolType eSymType ) const
    {
        return ConventionXL::getSpecialSymbol(eSymType);
    }

    virtual bool parseExternalName( const String& rSymbol, String& rFile, String& rName,
            const ScDocument* pDoc,
            const ::com::sun::star::uno::Sequence<
                const ::com::sun::star::sheet::ExternalLinkInfo > * pExternalLinks ) const
    {
        return ConventionXL::parseExternalName( rSymbol, rFile, rName, pDoc, pExternalLinks);
    }

    virtual String makeExternalNameStr( const String& rFile, const String& rName ) const
    {
        return ConventionXL::makeExternalNameStr(rFile, rName);
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        // ['file:///path/to/file/filename.xls']'Sheet Name'!$A$1
        // This is a little different from the format Excel uses, as Excel
        // puts [] only around the file name.  But we need to enclose the
        // whole file path with [] because the file name can contain any
        // characters.

        const String* pFullName = pRefMgr->getExternalFileName(nFileId);
        if (!pFullName)
            return;

        ScSingleRefData aRef(rRef);
        aRef.CalcAbsIfRel(rCompiler.GetPos());

        ConventionXL::makeExternalDocStr(
            rBuffer, *pFullName, rCompiler.GetEncodeUrlMode() == ScCompiler::ENCODE_ALWAYS);
        ScRangeStringConverter::AppendTableName(rBuffer, rTabName);
        rBuffer.append(sal_Unicode('!'));

        makeSingleCellStr(rBuffer, aRef);
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        const String* pFullName = pRefMgr->getExternalFileName(nFileId);
        if (!pFullName)
            return;

        vector<String> aTabNames;
        pRefMgr->getAllCachedTableNames(nFileId, aTabNames);
        if (aTabNames.empty())
            return;

        ScComplexRefData aRef(rRef);
        aRef.CalcAbsIfRel(rCompiler.GetPos());

        ConventionXL::makeExternalDocStr(
            rBuffer, *pFullName, rCompiler.GetEncodeUrlMode() == ScCompiler::ENCODE_ALWAYS);
        ConventionXL::makeExternalTabNameRange(rBuffer, rTabName, aTabNames, aRef);
        rBuffer.append(sal_Unicode('!'));

        makeSingleCellStr(rBuffer, aRef.Ref1);
        if (aRef.Ref1 != aRef.Ref2)
        {
            rBuffer.append(sal_Unicode(':'));
            makeSingleCellStr(rBuffer, aRef.Ref2);
        }
    }
};

static const ConventionXL_A1 ConvXL_A1;
const ScCompiler::Convention * const ScCompiler::pConvXL_A1 = &ConvXL_A1;


struct ConventionXL_OOX : public ConventionXL_A1
{
    ConventionXL_OOX() : ConventionXL_A1( FormulaGrammar::CONV_XL_OOX ) { }
};

static const ConventionXL_OOX ConvXL_OOX;
const ScCompiler::Convention * const ScCompiler::pConvXL_OOX = &ConvXL_OOX;


//-----------------------------------------------------------------------------

static void
r1c1_add_col( rtl::OUStringBuffer &rBuf, const ScSingleRefData& rRef )
{
    rBuf.append( sal_Unicode( 'C' ) );
    if( rRef.IsColRel() )
    {
        if (rRef.nRelCol != 0)
        {
            rBuf.append( sal_Unicode( '[' ) );
            rBuf.append( String::CreateFromInt32( rRef.nRelCol ) );
            rBuf.append( sal_Unicode( ']' ) );
        }
    }
    else
        rBuf.append( String::CreateFromInt32( rRef.nCol + 1 ) );
}
static void
r1c1_add_row( rtl::OUStringBuffer &rBuf, const ScSingleRefData& rRef )
{
    rBuf.append( sal_Unicode( 'R' ) );
    if( rRef.IsRowRel() )
    {
        if (rRef.nRelRow != 0)
        {
            rBuf.append( sal_Unicode( '[' ) );
            rBuf.append( String::CreateFromInt32( rRef.nRelRow ) );
            rBuf.append( sal_Unicode( ']' ) );
        }
    }
    else
        rBuf.append( String::CreateFromInt32( rRef.nRow + 1 ) );
}

struct ConventionXL_R1C1 : public ScCompiler::Convention, public ConventionXL
{
    ConventionXL_R1C1() : ScCompiler::Convention( FormulaGrammar::CONV_XL_R1C1 ) { }
    void MakeRefStr( rtl::OUStringBuffer&   rBuf,
                     const ScCompiler&      rComp,
                     const ScComplexRefData& rRef,
                     sal_Bool bSingleRef ) const
    {
        ScComplexRefData aRef( rRef );

        MakeDocStr( rBuf, rComp, aRef, bSingleRef );

        // Play fast and loose with invalid refs.  There is not much point in producing
        // Foo!A1:#REF! versus #REF! at this point
        aRef.Ref1.CalcAbsIfRel( rComp.GetPos() );
        if( aRef.Ref1.IsColDeleted() || aRef.Ref1.IsRowDeleted() )
        {
            rBuf.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
            return;
        }

        if( !bSingleRef )
        {
            aRef.Ref2.CalcAbsIfRel( rComp.GetPos() );
            if( aRef.Ref2.IsColDeleted() || aRef.Ref2.IsRowDeleted() )
            {
                rBuf.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
                return;
            }

            if( aRef.Ref1.nCol == 0 && aRef.Ref2.nCol >= MAXCOL )
            {
                r1c1_add_row( rBuf,  rRef.Ref1 );
                if( rRef.Ref1.nRow != rRef.Ref2.nRow ||
                    rRef.Ref1.IsRowRel() != rRef.Ref2.IsRowRel() ) {
                    rBuf.append (sal_Unicode ( ':' ) );
                    r1c1_add_row( rBuf,  rRef.Ref2 );
                }
                return;

            }

            if( aRef.Ref1.nRow == 0 && aRef.Ref2.nRow >= MAXROW )
            {
                r1c1_add_col( rBuf, rRef.Ref1 );
                if( rRef.Ref1.nCol != rRef.Ref2.nCol ||
                    rRef.Ref1.IsColRel() != rRef.Ref2.IsColRel() )
                {
                    rBuf.append (sal_Unicode ( ':' ) );
                    r1c1_add_col( rBuf,  rRef.Ref2 );
                }
                return;
            }
        }

        r1c1_add_row( rBuf, rRef.Ref1 );
        r1c1_add_col( rBuf, rRef.Ref1 );
        if (!bSingleRef)
        {
            rBuf.append (sal_Unicode ( ':' ) );
            r1c1_add_row( rBuf, rRef.Ref2 );
            r1c1_add_col( rBuf, rRef.Ref2 );
        }
    }

    ParseResult parseAnyToken( const String& rFormula,
                               xub_StrLen nSrcPos,
                               const CharClass* pCharClass) const
    {
        ConventionXL::parseExternalDocName(rFormula, nSrcPos);

        ParseResult aRet;
        if ( lcl_isValidQuotedText(rFormula, nSrcPos, aRet) )
            return aRet;

        static const sal_Int32 nStartFlags = KParseTokens::ANY_LETTER_OR_NUMBER |
            KParseTokens::ASC_UNDERSCORE ;
        static const sal_Int32 nContFlags = nStartFlags | KParseTokens::ASC_DOT;
        // '?' allowed in range names
        static const String aAddAllowed = String::CreateFromAscii( "?-[]!" );

        return pCharClass->parseAnyToken( rFormula,
                nSrcPos, nStartFlags, aAddAllowed, nContFlags, aAddAllowed );
    }

    virtual sal_Unicode getSpecialSymbol( SpecialSymbolType eSymType ) const
    {
        return ConventionXL::getSpecialSymbol(eSymType);
    }

    virtual bool parseExternalName( const String& rSymbol, String& rFile, String& rName,
            const ScDocument* pDoc,
            const ::com::sun::star::uno::Sequence<
                const ::com::sun::star::sheet::ExternalLinkInfo > * pExternalLinks ) const
    {
        return ConventionXL::parseExternalName( rSymbol, rFile, rName, pDoc, pExternalLinks);
    }

    virtual String makeExternalNameStr( const String& rFile, const String& rName ) const
    {
        return ConventionXL::makeExternalNameStr(rFile, rName);
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScSingleRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        // ['file:///path/to/file/filename.xls']'Sheet Name'!$A$1
        // This is a little different from the format Excel uses, as Excel
        // puts [] only around the file name.  But we need to enclose the
        // whole file path with [] because the file name can contain any
        // characters.

        const String* pFullName = pRefMgr->getExternalFileName(nFileId);
        if (!pFullName)
            return;

        ScSingleRefData aRef(rRef);
        aRef.CalcAbsIfRel(rCompiler.GetPos());

        ConventionXL::makeExternalDocStr(
            rBuffer, *pFullName, rCompiler.GetEncodeUrlMode() == ScCompiler::ENCODE_ALWAYS);
        ScRangeStringConverter::AppendTableName(rBuffer, rTabName);
        rBuffer.append(sal_Unicode('!'));

        r1c1_add_row(rBuffer, aRef);
        r1c1_add_col(rBuffer, aRef);
    }

    virtual void makeExternalRefStr( ::rtl::OUStringBuffer& rBuffer, const ScCompiler& rCompiler,
                                     sal_uInt16 nFileId, const String& rTabName, const ScComplexRefData& rRef,
                                     ScExternalRefManager* pRefMgr ) const
    {
        const String* pFullName = pRefMgr->getExternalFileName(nFileId);
        if (!pFullName)
            return;

        vector<String> aTabNames;
        pRefMgr->getAllCachedTableNames(nFileId, aTabNames);
        if (aTabNames.empty())
            return;

        ScComplexRefData aRef(rRef);
        aRef.CalcAbsIfRel(rCompiler.GetPos());

        ConventionXL::makeExternalDocStr(
            rBuffer, *pFullName, rCompiler.GetEncodeUrlMode() == ScCompiler::ENCODE_ALWAYS);
        ConventionXL::makeExternalTabNameRange(rBuffer, rTabName, aTabNames, aRef);
        rBuffer.append(sal_Unicode('!'));

        if (aRef.Ref2.IsColDeleted() || aRef.Ref2.IsRowDeleted())
        {
            rBuffer.append(ScGlobal::GetRscString(STR_NO_REF_TABLE));
            return;
        }

        if (aRef.Ref1.nCol == 0 && aRef.Ref2.nCol >= MAXCOL)
        {
            r1c1_add_row(rBuffer, rRef.Ref1);
            if (rRef.Ref1.nRow != rRef.Ref2.nRow || rRef.Ref1.IsRowRel() != rRef.Ref2.IsRowRel())
            {
                rBuffer.append (sal_Unicode(':'));
                r1c1_add_row(rBuffer, rRef.Ref2);
            }
            return;
        }

        if (aRef.Ref1.nRow == 0 && aRef.Ref2.nRow >= MAXROW)
        {
            r1c1_add_col(rBuffer, aRef.Ref1);
            if (aRef.Ref1.nCol != aRef.Ref2.nCol || aRef.Ref1.IsColRel() != aRef.Ref2.IsColRel())
            {
                rBuffer.append (sal_Unicode(':'));
                r1c1_add_col(rBuffer, aRef.Ref2);
            }
            return;
        }

        r1c1_add_row(rBuffer, aRef.Ref1);
        r1c1_add_col(rBuffer, aRef.Ref1);
        rBuffer.append (sal_Unicode (':'));
        r1c1_add_row(rBuffer, aRef.Ref2);
        r1c1_add_col(rBuffer, aRef.Ref2);
    }
};

static const ConventionXL_R1C1 ConvXL_R1C1;
const ScCompiler::Convention * const ScCompiler::pConvXL_R1C1 = &ConvXL_R1C1;

//-----------------------------------------------------------------------------
ScCompiler::ScCompiler( ScDocument* pDocument, const ScAddress& rPos,ScTokenArray& rArr)
        : FormulaCompiler(rArr),
        pDoc( pDocument ),
        aPos( rPos ),
        pCharClass( ScGlobal::pCharClass ),
        mnPredetectedReference(0),
        mnRangeOpPosInSymbol(-1),
        pConv( pConvOOO_A1 ),
        meEncodeUrlMode( ENCODE_BY_GRAMMAR ),
        mbCloseBrackets( true ),
        mbExtendedErrorDetection( false ),
        mbRewind( false )
{
    nMaxTab = pDoc ? pDoc->GetTableCount() - 1 : 0;
}

ScCompiler::ScCompiler( ScDocument* pDocument, const ScAddress& rPos)
        :
        pDoc( pDocument ),
        aPos( rPos ),
        pCharClass( ScGlobal::pCharClass ),
        mnPredetectedReference(0),
        mnRangeOpPosInSymbol(-1),
        pConv( pConvOOO_A1 ),
        meEncodeUrlMode( ENCODE_BY_GRAMMAR ),
        mbCloseBrackets( true ),
        mbExtendedErrorDetection( false ),
        mbRewind( false )
{
    nMaxTab = pDoc ? pDoc->GetTableCount() - 1 : 0;
}

void ScCompiler::CheckTabQuotes( String& rString,
                                 const FormulaGrammar::AddressConvention eConv )
{
    using namespace ::com::sun::star::i18n;
    sal_Int32 nStartFlags = KParseTokens::ANY_LETTER_OR_NUMBER | KParseTokens::ASC_UNDERSCORE;
    sal_Int32 nContFlags = nStartFlags;
    ParseResult aRes = ScGlobal::pCharClass->parsePredefinedToken(
        KParseType::IDENTNAME, rString, 0, nStartFlags, EMPTY_STRING, nContFlags, EMPTY_STRING);
    bool bNeedsQuote = !((aRes.TokenType & KParseType::IDENTNAME) && aRes.EndPos == rString.Len());

    switch ( eConv )
    {
        default :
        case FormulaGrammar::CONV_UNSPECIFIED :
            break;
        case FormulaGrammar::CONV_OOO :
        case FormulaGrammar::CONV_XL_A1 :
        case FormulaGrammar::CONV_XL_R1C1 :
        case FormulaGrammar::CONV_XL_OOX :
            if( bNeedsQuote )
            {
                static const String one_quote = static_cast<sal_Unicode>( '\'' );
                static const String two_quote = String::CreateFromAscii( "''" );
                // escape embedded quotes
                rString.SearchAndReplaceAll( one_quote, two_quote );
            }
            break;
    }

    if ( !bNeedsQuote && CharClass::isAsciiNumeric( rString ) )
    {
        // Prevent any possible confusion resulting from pure numeric sheet names.
        bNeedsQuote = true;
    }

    if( bNeedsQuote )
    {
        rString.Insert( '\'', 0 );
        rString += '\'';
    }
}


xub_StrLen ScCompiler::GetDocTabPos( const String& rString )
{
    if (rString.GetChar(0) != '\'')
        return STRING_NOTFOUND;
    xub_StrLen nPos = ScGlobal::FindUnquoted( rString, SC_COMPILER_FILE_TAB_SEP);
    // it must be 'Doc'#
    if (nPos != STRING_NOTFOUND && rString.GetChar(nPos-1) != '\'')
        nPos = STRING_NOTFOUND;
    return nPos;
}

//---------------------------------------------------------------------------

void ScCompiler::SetRefConvention( FormulaGrammar::AddressConvention eConv )
{
    switch ( eConv ) {
        case FormulaGrammar::CONV_UNSPECIFIED :
            break;
        default :
        case FormulaGrammar::CONV_OOO :      SetRefConvention( pConvOOO_A1 ); break;
        case FormulaGrammar::CONV_ODF :      SetRefConvention( pConvOOO_A1_ODF ); break;
        case FormulaGrammar::CONV_XL_A1 :    SetRefConvention( pConvXL_A1 );  break;
        case FormulaGrammar::CONV_XL_R1C1 :  SetRefConvention( pConvXL_R1C1 ); break;
        case FormulaGrammar::CONV_XL_OOX :   SetRefConvention( pConvXL_OOX ); break;
    }
}

void ScCompiler::SetRefConvention( const ScCompiler::Convention *pConvP )
{
    pConv = pConvP;
    meGrammar = FormulaGrammar::mergeToGrammar( meGrammar, pConv->meConv);
    DBG_ASSERT( FormulaGrammar::isSupported( meGrammar),
            "ScCompiler::SetRefConvention: unsupported grammar resulting");
}

void ScCompiler::SetError(sal_uInt16 nError)
{
    if( !pArr->GetCodeError() )
        pArr->SetCodeError( nError);
}


sal_Unicode* lcl_UnicodeStrNCpy( sal_Unicode* pDst, const sal_Unicode* pSrc, xub_StrLen nMax )
{
    const sal_Unicode* const pStop = pDst + nMax;
    while ( *pSrc && pDst < pStop )
    {
        *pDst++ = *pSrc++;
    }
    *pDst = 0;
    return pDst;
}


//---------------------------------------------------------------------------
// NextSymbol
//---------------------------------------------------------------------------
// Zerlegt die Formel in einzelne Symbole fuer die weitere
// Verarbeitung (Turing-Maschine).
//---------------------------------------------------------------------------
// Ausgangs Zustand = GetChar
//---------------+-------------------+-----------------------+---------------
// Alter Zustand | gelesenes Zeichen | Aktion                | Neuer Zustand
//---------------+-------------------+-----------------------+---------------
// GetChar       | ;()+-*/^=&        | Symbol=Zeichen        | Stop
//               | <>                | Symbol=Zeichen        | GetBool
//               | $ Buchstabe       | Symbol=Zeichen        | GetWord
//               | Ziffer            | Symbol=Zeichen        | GetValue
//               | "                 | Keine                 | GetString
//               | Sonst             | Keine                 | GetChar
//---------------+-------------------+-----------------------+---------------
// GetBool       | =>                | Symbol=Symbol+Zeichen | Stop
//               | Sonst             | Dec(CharPos)          | Stop
//---------------+-------------------+-----------------------+---------------
// GetWord       | SepSymbol         | Dec(CharPos)          | Stop
//               | ()+-*/^=<>&~      |                       |
//               | Leerzeichen       | Dec(CharPos)          | Stop
//               | $_:.              |                       |
//               | Buchstabe,Ziffer  | Symbol=Symbol+Zeichen | GetWord
//               | Sonst             | Fehler                | Stop
//---------------|-------------------+-----------------------+---------------
// GetValue      | ;()*/^=<>&        |                       |
//               | Leerzeichen       | Dec(CharPos)          | Stop
//               | Ziffer E+-%,.     | Symbol=Symbol+Zeichen | GetValue
//               | Sonst             | Fehler                | Stop
//---------------+-------------------+-----------------------+---------------
// GetString     | "                 | Keine                 | Stop
//               | Sonst             | Symbol=Symbol+Zeichen | GetString
//---------------+-------------------+-----------------------+---------------

xub_StrLen ScCompiler::NextSymbol(bool bInArray)
{
    cSymbol[MAXSTRLEN-1] = 0;       // Stopper
    sal_Unicode* pSym = cSymbol;
    const sal_Unicode* const pStart = aFormula.GetBuffer();
    const sal_Unicode* pSrc = pStart + nSrcPos;
    bool bi18n = false;
    sal_Unicode c = *pSrc;
    sal_Unicode cLast = 0;
    bool bQuote = false;
    mnRangeOpPosInSymbol = -1;
    ScanState eState = ssGetChar;
    xub_StrLen nSpaces = 0;
    sal_Unicode cSep = mxSymbols->getSymbol( ocSep).GetChar(0);
    sal_Unicode cArrayColSep = mxSymbols->getSymbol( ocArrayColSep).GetChar(0);
    sal_Unicode cArrayRowSep = mxSymbols->getSymbol( ocArrayRowSep).GetChar(0);
    sal_Unicode cDecSep = (mxSymbols->isEnglish() ? '.' :
            ScGlobal::pLocaleData->getNumDecimalSep().GetChar(0));

    // special symbols specific to address convention used
    sal_Unicode cSheetPrefix = pConv->getSpecialSymbol(ScCompiler::Convention::ABS_SHEET_PREFIX);
    sal_Unicode cSheetSep    = pConv->getSpecialSymbol(ScCompiler::Convention::SHEET_SEPARATOR);

    int nDecSeps = 0;
    bool bAutoIntersection = false;
    int nRefInName = 0;
    mnPredetectedReference = 0;
    // try to parse simple tokens before calling i18n parser
    while ((c != 0) && (eState != ssStop) )
    {
        pSrc++;
        sal_uLong nMask = GetCharTableFlags( c );
        // The parameter separator and the array column and row separators end
        // things unconditionally if not in string or reference.
        if (c == cSep || (bInArray && (c == cArrayColSep || c == cArrayRowSep)))
        {
            switch (eState)
            {
                // these are to be continued
                case ssGetString:
                case ssSkipString:
                case ssGetReference:
                case ssSkipReference:
                    break;
                default:
                    if (eState == ssGetChar)
                        *pSym++ = c;
                    else
                        pSrc--;
                    eState = ssStop;
            }
        }
Label_MaskStateMachine:
        switch (eState)
        {
            case ssGetChar :
            {
                // Order is important!
                if( nMask & SC_COMPILER_C_ODF_LABEL_OP )
                {
                    // '!!' automatic intersection
                    if (GetCharTableFlags( pSrc[0] ) & SC_COMPILER_C_ODF_LABEL_OP)
                    {
                        /* TODO: For now the UI "space operator" is used, this
                         * could be enhanced using a specialized OpCode to get
                         * rid of the space ambiguity, which would need some
                         * places to be adapted though. And we would still need
                         * to support the ambiguous space operator for UI
                         * purposes anyway. However, we then could check for
                         * invalid usage of '!!', which currently isn't
                         * possible. */
                        if (!bAutoIntersection)
                        {
                            ++pSrc;
                            nSpaces += 2;   // must match the character count
                            bAutoIntersection = true;
                        }
                        else
                        {
                            pSrc--;
                            eState = ssStop;
                        }
                    }
                    else
                    {
                        nMask &= ~SC_COMPILER_C_ODF_LABEL_OP;
                        goto Label_MaskStateMachine;
                    }
                }
                else if( nMask & SC_COMPILER_C_ODF_NAME_MARKER )
                {
                    // '$$' defined name marker
                    if (GetCharTableFlags( pSrc[0] ) & SC_COMPILER_C_ODF_NAME_MARKER)
                    {
                        // both eaten, not added to pSym
                        ++pSrc;
                    }
                    else
                    {
                        nMask &= ~SC_COMPILER_C_ODF_NAME_MARKER;
                        goto Label_MaskStateMachine;
                    }
                }
                else if( nMask & SC_COMPILER_C_CHAR )
                {
                    *pSym++ = c;
                    eState = ssStop;
                }
                else if( nMask & SC_COMPILER_C_ODF_LBRACKET )
                {
                    // eaten, not added to pSym
                    eState = ssGetReference;
                    mnPredetectedReference = 1;
                }
                else if( nMask & SC_COMPILER_C_CHAR_BOOL )
                {
                    *pSym++ = c;
                    eState = ssGetBool;
                }
                else if( nMask & SC_COMPILER_C_CHAR_VALUE )
                {
                    *pSym++ = c;
                    eState = ssGetValue;
                }
                else if( nMask & SC_COMPILER_C_CHAR_STRING )
                {
                    *pSym++ = c;
                    eState = ssGetString;
                }
                else if( nMask & SC_COMPILER_C_CHAR_DONTCARE )
                {
                    nSpaces++;
                }
                else if( nMask & SC_COMPILER_C_CHAR_IDENT )
                {   // try to get a simple ASCII identifier before calling
                    // i18n, to gain performance during import
                    *pSym++ = c;
                    eState = ssGetIdent;
                }
                else
                {
                    bi18n = true;
                    eState = ssStop;
                }
            }
            break;
            case ssGetIdent:
            {
                if ( nMask & SC_COMPILER_C_IDENT )
                {   // This catches also $Sheet1.A$1, for example.
                    if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                    {
                        SetError(errStringOverflow);
                        eState = ssStop;
                    }
                    else
                        *pSym++ = c;
                }
                else if (c == ':' && mnRangeOpPosInSymbol < 0)
                {
                    // One range operator may form Sheet1.A:A, which we need to
                    // pass as one entity to IsReference().
                    mnRangeOpPosInSymbol = pSym - &cSymbol[0];
                    if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                    {
                        SetError(errStringOverflow);
                        eState = ssStop;
                    }
                    else
                        *pSym++ = c;
                }
                else if ( 128 <= c || '\'' == c )
                {   // High values need reparsing with i18n,
                    // single quoted $'sheet' names too (otherwise we'd had to
                    // implement everything twice).
                    bi18n = true;
                    eState = ssStop;
                }
                else
                {
                    pSrc--;
                    eState = ssStop;
                }
            }
            break;
            case ssGetBool :
            {
                if( nMask & SC_COMPILER_C_BOOL )
                {
                    *pSym++ = c;
                    eState = ssStop;
                }
                else
                {
                    pSrc--;
                    eState = ssStop;
                }
            }
            break;
            case ssGetValue :
            {
                if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                {
                    SetError(errStringOverflow);
                    eState = ssStop;
                }
                else if (c == cDecSep)
                {
                    if (++nDecSeps > 1)
                    {
                        // reparse with i18n, may be numeric sheet name as well
                        bi18n = true;
                        eState = ssStop;
                    }
                    else
                        *pSym++ = c;
                }
                else if( nMask & SC_COMPILER_C_VALUE )
                    *pSym++ = c;
                else if( nMask & SC_COMPILER_C_VALUE_SEP )
                {
                    pSrc--;
                    eState = ssStop;
                }
                else if (c == 'E' || c == 'e')
                {
                    if (GetCharTableFlags( pSrc[0] ) & SC_COMPILER_C_VALUE_EXP)
                        *pSym++ = c;
                    else
                    {
                        // reparse with i18n
                        bi18n = true;
                        eState = ssStop;
                    }
                }
                else if( nMask & SC_COMPILER_C_VALUE_SIGN )
                {
                    if (((cLast == 'E') || (cLast == 'e')) &&
                            (GetCharTableFlags( pSrc[0] ) & SC_COMPILER_C_VALUE_VALUE))
                    {
                        *pSym++ = c;
                    }
                    else
                    {
                        pSrc--;
                        eState = ssStop;
                    }
                }
                else
                {
                    // reparse with i18n
                    bi18n = true;
                    eState = ssStop;
                }
            }
            break;
            case ssGetString :
            {
                if( nMask & SC_COMPILER_C_STRING_SEP )
                {
                    if ( !bQuote )
                    {
                        if ( *pSrc == '"' )
                            bQuote = true;      // "" => literal "
                        else
                            eState = ssStop;
                    }
                    else
                        bQuote = false;
                }
                if ( !bQuote )
                {
                    if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                    {
                        SetError(errStringOverflow);
                        eState = ssSkipString;
                    }
                    else
                        *pSym++ = c;
                }
            }
            break;
            case ssSkipString:
                if( nMask & SC_COMPILER_C_STRING_SEP )
                    eState = ssStop;
                break;
            case ssGetReference:
                if( pSym == &cSymbol[ MAXSTRLEN-1 ] )
                {
                    SetError( errStringOverflow);
                    eState = ssSkipReference;
                }
                // fall through and follow logic
            case ssSkipReference:
                // ODF reference: ['External'#$'Sheet'.A1:.B2] with dots being
                // mandatory also if no sheet name. 'External'# is optional,
                // sheet name is optional, quotes around sheet name are
                // optional if no quote contained.
                // 2nd usage: ['Sheet'.$$'DefinedName']
                // 3rd usage: ['External'#$$'DefinedName']
                // 4th usage: ['External'#$'Sheet'.$$'DefinedName']
                // Also for all these names quotes are optional if no quote
                // contained.
                {

                    // nRefInName: 0 := not in sheet name yet. 'External'
                    // is parsed as if it was a sheet name and nRefInName
                    // is reset when # is encountered immediately after closing
                    // quote. Same with 'DefinedName', nRefInName is cleared
                    // when : is encountered.

                    // Encountered leading $ before sheet name.
                    static const int kDollar    = (1 << 1);
                    // Encountered ' opening quote, which may be after $ or
                    // not.
                    static const int kOpen      = (1 << 2);
                    // Somewhere in name.
                    static const int kName      = (1 << 3);
                    // Encountered ' in name, will be cleared if double or
                    // transformed to kClose if not, in which case kOpen is
                    // cleared.
                    static const int kQuote     = (1 << 4);
                    // Past ' closing quote.
                    static const int kClose     = (1 << 5);
                    // Encountered # file/sheet separator.
                    static const int kFileSep   = (1 << 6);
                    // Past . sheet name separator.
                    static const int kPast      = (1 << 7);
                    // Marked name $$ follows sheet name separator, detected
                    // while we're still on the separator. Will be cleared when
                    // entering the name.
                    static const int kMarkAhead = (1 << 8);
                    // In marked defined name.
                    static const int kDefName   = (1 << 9);

                    bool bAddToSymbol = true;
                    if ((nMask & SC_COMPILER_C_ODF_RBRACKET) && !(nRefInName & kOpen))
                    {
                        DBG_ASSERT( nRefInName & (kPast | kDefName),
                                "ScCompiler::NextSymbol: reference: "
                                "closing bracket ']' without prior sheet name separator '.' violates ODF spec");
                        // eaten, not added to pSym
                        bAddToSymbol = false;
                        eState = ssStop;
                    }
                    else if (cSheetSep == c && nRefInName == 0)
                    {
                        // eat it, no sheet name [.A1]
                        bAddToSymbol = false;
                        nRefInName |= kPast;
                        if ('$' == pSrc[0] && '$' == pSrc[1])
                            nRefInName |= kMarkAhead;
                    }
                    else if (!(nRefInName & kPast) || (nRefInName & (kMarkAhead | kDefName)))
                    {
                        // Not in col/row yet.

                        if (SC_COMPILER_FILE_TAB_SEP == c && (nRefInName & kFileSep))
                            nRefInName = 0;
                        else if ('$' == c && '$' == pSrc[0] && !(nRefInName & kOpen))
                        {
                            nRefInName &= ~kMarkAhead;
                            if (!(nRefInName & kDefName))
                            {
                                // eaten, not added to pSym (2 chars)
                                bAddToSymbol = false;
                                ++pSrc;
                                nRefInName &= kPast;
                                nRefInName |= kDefName;
                            }
                            else
                            {
                                // ScAddress::Parse() will recognize this as
                                // invalid later.
                                if (eState != ssSkipReference)
                                {
                                    *pSym++ = c;
                                    *pSym++ = *pSrc++;
                                }
                                bAddToSymbol = false;
                            }
                        }
                        else if (cSheetPrefix == c && nRefInName == 0)
                            nRefInName |= kDollar;
                        else if ('\'' == c)
                        {
                            // TODO: The conventions' parseExternalName()
                            // should handle quoted names, but as long as they
                            // don't remove non-embedded quotes here.
                            if (!(nRefInName & kName))
                            {
                                nRefInName |= (kOpen | kName);
                                bAddToSymbol = !(nRefInName & kDefName);
                            }
                            else if (!(nRefInName & kOpen))
                            {
                                DBG_ERRORFILE("ScCompiler::NextSymbol: reference: "
                                        "a ''' without the name being enclosed in '...' violates ODF spec");
                            }
                            else if (nRefInName & kQuote)
                            {
                                // escaped embedded quote
                                nRefInName &= ~kQuote;
                            }
                            else
                            {
                                switch (pSrc[0])
                                {
                                    case '\'':
                                        // escapes embedded quote
                                        nRefInName |= kQuote;
                                        break;
                                    case SC_COMPILER_FILE_TAB_SEP:
                                        // sheet name should follow
                                        nRefInName |= kFileSep;
                                        // fallthru
                                    default:
                                        // quote not followed by quote => close
                                        nRefInName |= kClose;
                                        nRefInName &= ~kOpen;
                                }
                                bAddToSymbol = !(nRefInName & kDefName);
                            }
                        }
                        else if (cSheetSep == c && !(nRefInName & kOpen))
                        {
                            // unquoted sheet name separator
                            nRefInName |= kPast;
                            if ('$' == pSrc[0] && '$' == pSrc[1])
                                nRefInName |= kMarkAhead;
                        }
                        else if (':' == c && !(nRefInName & kOpen))
                        {
                            DBG_ERRORFILE("ScCompiler::NextSymbol: reference: "
                                    "range operator ':' without prior sheet name separator '.' violates ODF spec");
                            nRefInName = 0;
                            ++mnPredetectedReference;
                        }
                        else if (!(nRefInName & kName))
                        {
                            // start unquoted name
                            nRefInName |= kName;
                        }
                    }
                    else if (':' == c)
                    {
                        // range operator
                        nRefInName = 0;
                        ++mnPredetectedReference;
                    }
                    if (bAddToSymbol && eState != ssSkipReference)
                        *pSym++ = c;    // everything is part of reference
                }
                break;
            case ssStop:
                ;   // nothing, prevent warning
                break;
        }
        cLast = c;
        c = *pSrc;
    }
    if ( bi18n )
    {
        nSrcPos = sal::static_int_cast<xub_StrLen>( nSrcPos + nSpaces );
        String aSymbol;
        mnRangeOpPosInSymbol = -1;
        sal_uInt16 nErr = 0;
        do
        {
            bi18n = false;
            // special case  (e.g. $'sheetname' in OOO A1)
            if ( pStart[nSrcPos] == cSheetPrefix && pStart[nSrcPos+1] == '\'' )
                aSymbol += pStart[nSrcPos++];

            ParseResult aRes = pConv->parseAnyToken( aFormula, nSrcPos, pCharClass );

            if ( !aRes.TokenType )
                SetError( nErr = errIllegalChar );      // parsed chars as string
            if ( aRes.EndPos <= nSrcPos )
            {   // ?!?
                SetError( nErr = errIllegalChar );
                nSrcPos = aFormula.Len();
                aSymbol.Erase();
            }
            else
            {
                aSymbol.Append( pStart + nSrcPos, (xub_StrLen)aRes.EndPos - nSrcPos );
                nSrcPos = (xub_StrLen) aRes.EndPos;
                c = pStart[nSrcPos];
                if ( aRes.TokenType & KParseType::SINGLE_QUOTE_NAME )
                {   // special cases (e.g. 'sheetname'. or 'filename'# in OOO A1)
                    bi18n = (c == cSheetSep || c == SC_COMPILER_FILE_TAB_SEP);
                }
                // One range operator restarts parsing for second reference.
                if (c == ':' && mnRangeOpPosInSymbol < 0)
                {
                    mnRangeOpPosInSymbol = aSymbol.Len();
                    bi18n = true;
                }
                if ( bi18n )
                    aSymbol += pStart[nSrcPos++];
            }
        } while ( bi18n && !nErr );
        xub_StrLen nLen = aSymbol.Len();
        if ( nLen >= MAXSTRLEN )
        {
            SetError( errStringOverflow );
            nLen = MAXSTRLEN-1;
        }
        lcl_UnicodeStrNCpy( cSymbol, aSymbol.GetBuffer(), nLen );
    }
    else
    {
        nSrcPos = sal::static_int_cast<xub_StrLen>( pSrc - pStart );
        *pSym = 0;
    }
    if (mnRangeOpPosInSymbol >= 0 && mnRangeOpPosInSymbol == (pSym-1) - &cSymbol[0])
    {
        // This is a trailing range operator, which is nonsense. Will be caught 
        // in next round.
        mnRangeOpPosInSymbol = -1;
        *--pSym = 0;
        --nSrcPos;
    }
    if ( bAutoCorrect )
        aCorrectedSymbol = cSymbol;
    if (bAutoIntersection && nSpaces > 1)
        --nSpaces;  // replace '!!' with only one space
    return nSpaces;
}

//---------------------------------------------------------------------------
// Convert symbol to token
//---------------------------------------------------------------------------

sal_Bool ScCompiler::IsOpCode( const String& rName, bool bInArray )
{
    OpCodeHashMap::const_iterator iLook( mxSymbols->getHashMap()->find( rName));
    sal_Bool bFound = (iLook != mxSymbols->getHashMap()->end());
    if (bFound)
    {
        ScRawToken aToken;
        OpCode eOp = iLook->second;
        if (bInArray)
        {
            if (rName.Equals(mxSymbols->getSymbol(ocArrayColSep)))
                eOp = ocArrayColSep;
            else if (rName.Equals(mxSymbols->getSymbol(ocArrayRowSep)))
                eOp = ocArrayRowSep;
        }
        aToken.SetOpCode(eOp);
        pRawToken = aToken.Clone();
    }
    else if (mxSymbols->isODFF())
    {
        // ODFF names that are not written in the current mapping but to be 
        // recognized. New names will be written in a future relase, then 
        // exchange (!) with the names in 
        // formula/source/core/resource/core_resource.src to be able to still 
        // read the old names as well.
        struct FunctionName
        {
            const sal_Char* pName;
            OpCode          eOp;
        };
        static const FunctionName aOdffAliases[] = {
            // Renamed old names:
            // XXX none yet.
            // Renamed new names:
            { "BINOM.DIST.RANGE",               ocB },              // B -> BINOM.DIST.RANGE
            { "LEGACY.TDIST",                   ocTDist },          // TDIST -> LEGACY.TDIST
            { "ORG.OPENOFFICE.EASTERSUNDAY",    ocEasterSunday }    // EASTERSUNDAY -> ORG.OPENOFFICE.EASTERSUNDAY
        };
        static const size_t nOdffAliases = sizeof(aOdffAliases) / sizeof(aOdffAliases[0]);
        for (size_t i=0; i<nOdffAliases; ++i)
        {
            if (rName.EqualsIgnoreCaseAscii( aOdffAliases[i].pName))
            {
                ScRawToken aToken;
                aToken.SetOpCode( aOdffAliases[i].eOp);
                pRawToken = aToken.Clone();
                bFound = sal_True;
                break;  // for
            }
        }
    }
    if (!bFound)
    {
        String aIntName;
        if (mxSymbols->hasExternals())
        {
            // If symbols are set by filters get mapping to exact name.
            ExternalHashMap::const_iterator iExt(
                    mxSymbols->getExternalHashMap()->find( rName));
            if (iExt != mxSymbols->getExternalHashMap()->end())
            {
                if (ScGlobal::GetAddInCollection()->GetFuncData( (*iExt).second))
                    aIntName = (*iExt).second;
            }
            if (!aIntName.Len())
            {
                // If that isn't found we might continue with rName lookup as a
                // last resort by just falling through to FindFunction(), but
                // it shouldn't happen if the map was setup correctly. Don't
                // waste time and bail out.
                return sal_False;
            }
        }
        if (!aIntName.Len())
        {
            // Old (deprecated) addins first for legacy.
            sal_uInt16 nIndex;
            bFound = ScGlobal::GetFuncCollection()->SearchFunc( cSymbol, nIndex);
            if (bFound)
            {
                ScRawToken aToken;
                aToken.SetExternal( cSymbol );
                pRawToken = aToken.Clone();
            }
            else
                // bLocalFirst=sal_False for (English) upper full original name
                // (service.function)
                aIntName = ScGlobal::GetAddInCollection()->FindFunction(
                        rName, !mxSymbols->isEnglish());
        }
        if (aIntName.Len())
        {
            ScRawToken aToken;
            aToken.SetExternal( aIntName.GetBuffer() );     // international name
            pRawToken = aToken.Clone();
            bFound = sal_True;
        }
    }
    OpCode eOp;
    if (bFound && ((eOp = pRawToken->GetOpCode()) == ocSub || eOp == ocNegSub))
    {
        bool bShouldBeNegSub =
            (eLastOp == ocOpen || eLastOp == ocSep || eLastOp == ocNegSub ||
             (SC_OPCODE_START_BIN_OP <= eLastOp && eLastOp < SC_OPCODE_STOP_BIN_OP) ||
             eLastOp == ocArrayOpen ||
             eLastOp == ocArrayColSep || eLastOp == ocArrayRowSep);
        if (bShouldBeNegSub && eOp == ocSub)
            pRawToken->NewOpCode( ocNegSub );
            //! if ocNegSub had ForceArray we'd have to set it here
        else if (!bShouldBeNegSub && eOp == ocNegSub)
            pRawToken->NewOpCode( ocSub );
    }
    return bFound;
}

sal_Bool ScCompiler::IsOpCode2( const String& rName )
{
    sal_Bool bFound = sal_False;
    sal_uInt16 i;

    for( i = ocInternalBegin; i <= ocInternalEnd && !bFound; i++ )
        bFound = rName.EqualsAscii( pInternal[ i-ocInternalBegin ] );

    if (bFound)
    {
        ScRawToken aToken;
        aToken.SetOpCode( (OpCode) --i );
        pRawToken = aToken.Clone();
    }
    return bFound;
}

sal_Bool ScCompiler::IsValue( const String& rSym )
{
    double fVal;
    sal_uInt32 nIndex = ( mxSymbols->isEnglish() ?
        pDoc->GetFormatTable()->GetStandardIndex( LANGUAGE_ENGLISH_US ) : 0 );
//  sal_uLong nIndex = 0;
////    sal_uLong nIndex = pDoc->GetFormatTable()->GetStandardIndex(ScGlobal::eLnge);
    if (pDoc->GetFormatTable()->IsNumberFormat( rSym, nIndex, fVal ) )
    {
        sal_uInt16 nType = pDoc->GetFormatTable()->GetType(nIndex);

        // Don't accept 3:3 as time, it is a reference to entire row 3 instead.
        // Dates should never be entered directly and automatically converted
        // to serial, because the serial would be wrong if null-date changed.
        // Usually it wouldn't be accepted anyway because the date separator
        // clashed with other separators or operators.
        if (nType & (NUMBERFORMAT_TIME | NUMBERFORMAT_DATE))
            return sal_False;

        if (nType == NUMBERFORMAT_LOGICAL)
        {
            const sal_Unicode* p = aFormula.GetBuffer() + nSrcPos;
            while( *p == ' ' )
                p++;
            if (*p == '(')
                return sal_False;   // Boolean function instead.
        }

        if( aFormula.GetChar(nSrcPos) == '.' )
            // numerical sheet name?
            return sal_False;

        if( nType == NUMBERFORMAT_TEXT )
            // HACK: number too big!
            SetError( errIllegalArgument );
        ScRawToken aToken;
        aToken.SetDouble( fVal );
        pRawToken = aToken.Clone();
        return sal_True;
    }
    else
        return sal_False;
}

sal_Bool ScCompiler::IsString()
{
    register const sal_Unicode* p = cSymbol;
    while ( *p )
        p++;
    xub_StrLen nLen = sal::static_int_cast<xub_StrLen>( p - cSymbol - 1 );
    sal_Bool bQuote = ((cSymbol[0] == '"') && (cSymbol[nLen] == '"'));
    if ((bQuote ? nLen-2 : nLen) > MAXSTRLEN-1)
    {
        SetError(errStringOverflow);
        return sal_False;
    }
    if ( bQuote )
    {
        cSymbol[nLen] = '\0';
        ScRawToken aToken;
        aToken.SetString( cSymbol+1 );
        pRawToken = aToken.Clone();
        return sal_True;
    }
    return sal_False;
}


sal_Bool ScCompiler::IsPredetectedReference( const String& rName )
{
    // Speedup documents with lots of broken references, e.g. sheet deleted.
    xub_StrLen nPos = rName.SearchAscii( "#REF!");
    if (nPos != STRING_NOTFOUND)
    {
        /* TODO: this may be enhanced by reusing scan information from
         * NextSymbol(), the positions of quotes and special characters found
         * there for $'sheet'.A1:... could be stored in a vector. We don't
         * fully rescan here whether found positions are within single quotes
         * for performance reasons. This code does not check for possible
         * occurrences of insane "valid" sheet names like
         * 'haha.#REF!1fooledyou' and will generate an error on such. */
        if (nPos == 0)
            return false;           // #REF!.AB42 or #REF!42 or #REF!#REF!
        sal_Unicode c = rName.GetChar(nPos-1);      // before #REF!
        if ('$' == c)
        {
            if (nPos == 1)
                return false;       // $#REF!.AB42 or $#REF!42 or $#REF!#REF!
            c = rName.GetChar(nPos-2);              // before $#REF!
        }
        sal_Unicode c2 = rName.GetChar(nPos+5);     // after #REF!
        switch (c)
        {
            case '.':
                if ('$' == c2 || '#' == c2 || ('0' <= c2 && c2 <= '9'))
                    return false;   // sheet.#REF!42 or sheet.#REF!#REF!
                break;
            case ':':
                if (mnPredetectedReference > 1 &&
                        ('.' == c2 || '$' == c2 || '#' == c2 ||
                         ('0' <= c2 && c2 <= '9')))
                    return false;   // :#REF!.AB42 or :#REF!42 or :#REF!#REF!
                break;
            default:
                if ((('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) &&
                        ((mnPredetectedReference > 1 && ':' == c2) || 0 == c2))
                    return false;   // AB#REF!: or AB#REF!
        }
    }
    switch (mnPredetectedReference)
    {
        case 1:
            return IsSingleReference( rName);
        case 2:
            return IsDoubleReference( rName);
    }
    return false;
}


sal_Bool ScCompiler::IsDoubleReference( const String& rName )
{
    ScRange aRange( aPos, aPos );
    const ScAddress::Details aDetails( pConv->meConv, aPos );
    ScAddress::ExternalInfo aExtInfo;
    sal_uInt16 nFlags = aRange.Parse( rName, pDoc, aDetails, &aExtInfo, &maExternalLinks );
    if( nFlags & SCA_VALID )
    {
        ScRawToken aToken;
        ScComplexRefData aRef;
        aRef.InitRange( aRange );
        aRef.Ref1.SetColRel( (nFlags & SCA_COL_ABSOLUTE) == 0 );
        aRef.Ref1.SetRowRel( (nFlags & SCA_ROW_ABSOLUTE) == 0 );
        aRef.Ref1.SetTabRel( (nFlags & SCA_TAB_ABSOLUTE) == 0 );
        if ( !(nFlags & SCA_VALID_TAB) )
            aRef.Ref1.SetTabDeleted( sal_True );        // #REF!
        aRef.Ref1.SetFlag3D( ( nFlags & SCA_TAB_3D ) != 0 );
        aRef.Ref2.SetColRel( (nFlags & SCA_COL2_ABSOLUTE) == 0 );
        aRef.Ref2.SetRowRel( (nFlags & SCA_ROW2_ABSOLUTE) == 0 );
        aRef.Ref2.SetTabRel( (nFlags & SCA_TAB2_ABSOLUTE) == 0 );
        if ( !(nFlags & SCA_VALID_TAB2) )
            aRef.Ref2.SetTabDeleted( sal_True );        // #REF!
        aRef.Ref2.SetFlag3D( ( nFlags & SCA_TAB2_3D ) != 0 );
        aRef.CalcRelFromAbs( aPos );
        if (aExtInfo.mbExternal)
        {
            ScExternalRefManager* pRefMgr = pDoc->GetExternalRefManager();
            const String* pRealTab = pRefMgr->getRealTableName(aExtInfo.mnFileId, aExtInfo.maTabName);
            aToken.SetExternalDoubleRef(
                aExtInfo.mnFileId, pRealTab ? *pRealTab : aExtInfo.maTabName, aRef);
        }
        else
        {
            aToken.SetDoubleReference(aRef);
        }
        pRawToken = aToken.Clone();
    }

    return ( nFlags & SCA_VALID ) != 0;
}


sal_Bool ScCompiler::IsSingleReference( const String& rName )
{
    ScAddress aAddr( aPos );
    const ScAddress::Details aDetails( pConv->meConv, aPos );
    ScAddress::ExternalInfo aExtInfo;
    sal_uInt16 nFlags = aAddr.Parse( rName, pDoc, aDetails, &aExtInfo, &maExternalLinks );
    // Something must be valid in order to recognize Sheet1.blah or blah.a1
    // as a (wrong) reference.
    if( nFlags & ( SCA_VALID_COL|SCA_VALID_ROW|SCA_VALID_TAB ) )
    {
        ScRawToken aToken;
        ScSingleRefData aRef;
        aRef.InitAddress( aAddr );
        aRef.SetColRel( (nFlags & SCA_COL_ABSOLUTE) == 0 );
        aRef.SetRowRel( (nFlags & SCA_ROW_ABSOLUTE) == 0 );
        aRef.SetTabRel( (nFlags & SCA_TAB_ABSOLUTE) == 0 );
        aRef.SetFlag3D( ( nFlags & SCA_TAB_3D ) != 0 );
        // the reference is really invalid
        if( !( nFlags & SCA_VALID ) )
        {
            if( !( nFlags & SCA_VALID_COL ) )
                aRef.nCol = MAXCOL+1;
            if( !( nFlags & SCA_VALID_ROW ) )
                aRef.nRow = MAXROW+1;
            if( !( nFlags & SCA_VALID_TAB ) )
                aRef.nTab = MAXTAB+3;
            nFlags |= SCA_VALID;
        }
        aRef.CalcRelFromAbs( aPos );

        if (aExtInfo.mbExternal)
        {
            ScExternalRefManager* pRefMgr = pDoc->GetExternalRefManager();
            const String* pRealTab = pRefMgr->getRealTableName(aExtInfo.mnFileId, aExtInfo.maTabName);
            aToken.SetExternalSingleRef(
                aExtInfo.mnFileId, pRealTab ? *pRealTab : aExtInfo.maTabName, aRef);
        }
        else
            aToken.SetSingleReference(aRef);
        pRawToken = aToken.Clone();
    }

    return ( nFlags & SCA_VALID ) != 0;
}


sal_Bool ScCompiler::IsReference( const String& rName )
{
    // Has to be called before IsValue
    sal_Unicode ch1 = rName.GetChar(0);
    sal_Unicode cDecSep = ( mxSymbols->isEnglish() ? '.' :
        ScGlobal::pLocaleData->getNumDecimalSep().GetChar(0) );
    if ( ch1 == cDecSep )
        return sal_False;
    // Who was that imbecile introducing '.' as the sheet name separator!?!
    if ( CharClass::isAsciiNumeric( ch1 ) )
    {
        // Numerical sheet name is valid.
        // But English 1.E2 or 1.E+2 is value 100, 1.E-2 is 0.01
        // Don't create a #REF! of values. But also do not bail out on
        // something like 3:3, meaning entire row 3.
        do
        {
            const xub_StrLen nPos = ScGlobal::FindUnquoted( rName, '.');
            if ( nPos == STRING_NOTFOUND )
            {
                if (ScGlobal::FindUnquoted( rName, ':') != STRING_NOTFOUND)
                    break;      // may be 3:3, continue as usual
                return sal_False;
            }
            sal_Unicode const * const pTabSep = rName.GetBuffer() + nPos;
            sal_Unicode ch2 = pTabSep[1];   // maybe a column identifier
            if ( !(ch2 == '$' || CharClass::isAsciiAlpha( ch2 )) )
                return sal_False;
            if ( cDecSep == '.' && (ch2 == 'E' || ch2 == 'e')   // E + - digit
                    && (GetCharTableFlags( pTabSep[2] ) & SC_COMPILER_C_VALUE_EXP) )
            {   // #91053#
                // If it is an 1.E2 expression check if "1" is an existent sheet
                // name. If so, a desired value 1.E2 would have to be entered as
                // 1E2 or 1.0E2 or 1.E+2, sorry. Another possibility would be to
                // require numerical sheet names always being entered quoted, which
                // is not desirable (too many 1999, 2000, 2001 sheets in use).
                // Furthermore, XML files created with versions prior to SRC640e
                // wouldn't contain the quotes added by MakeTabStr()/CheckTabQuotes()
                // and would produce wrong formulas if the conditions here are met.
                // If you can live with these restrictions you may remove the
                // check and return an unconditional FALSE.
                String aTabName( rName.Copy( 0, nPos ) );
                SCTAB nTab;
                if ( !pDoc->GetTable( aTabName, nTab ) )
                    return sal_False;
                // If sheet "1" exists and the expression is 1.E+2 continue as
                // usual, the ScRange/ScAddress parser will take care of it.
            }
        } while(0);
    }

    if (IsSingleReference( rName))
        return true;

    // Though the range operator is handled explicitly, when encountering
    // something like Sheet1.A:A we will have to treat it as one entity if it
    // doesn't pass as single cell reference.
    if (mnRangeOpPosInSymbol > 0)   // ":foo" would be nonsense
    {
        if (IsDoubleReference( rName))
            return true;
        // Now try with a symbol up to the range operator, rewind source 
        // position.
        sal_Int32 nLen = mnRangeOpPosInSymbol;
        while (cSymbol[++nLen])
            ;
        cSymbol[mnRangeOpPosInSymbol] = 0;
        nSrcPos -= static_cast<xub_StrLen>(nLen - mnRangeOpPosInSymbol);
        mnRangeOpPosInSymbol = -1;
        mbRewind = true;
        return true;    // end all checks
    }
    else
    {
        // Special treatment for the 'E:\[doc]Sheet1:Sheet3'!D5 Excel sickness, 
        // mnRangeOpPosInSymbol did not catch the range operator as it is 
        // within a quoted name.
        switch (pConv->meConv)
        {
            case FormulaGrammar::CONV_XL_A1:
            case FormulaGrammar::CONV_XL_R1C1:
            case FormulaGrammar::CONV_XL_OOX:
                if (rName.GetChar(0) == '\'' && IsDoubleReference( rName))
                    return true;
                break;
            default:
                ;   // nothing
        }
    }
    return false;
}

sal_Bool ScCompiler::IsMacro( const String& rName )
{
    String aName( rName);
    StarBASIC* pObj = 0;
    SfxObjectShell* pDocSh = pDoc->GetDocumentShell();

    SfxApplication* pSfxApp = SFX_APP();

    if( pDocSh )//XXX
        pObj = pDocSh->GetBasic();
    else
        pObj = pSfxApp->GetBasic();

    // ODFF recommends to store user-defined functions prefixed with "USER.", 
    // use only unprefixed name if encountered. BASIC doesn't allow '.' in a 
    // function name so a function "USER.FOO" could not exist, and macro check 
    // is assigned the lowest priority in function name check.
    if (FormulaGrammar::isODFF( GetGrammar()) && aName.EqualsIgnoreCaseAscii( "USER.", 0, 5))
        aName.Erase( 0, 5);

    SbxMethod* pMeth = (SbxMethod*) pObj->Find( aName, SbxCLASS_METHOD );
    if( !pMeth )
    {
        return sal_False;
    }
    // It really should be a BASIC function!
    if( pMeth->GetType() == SbxVOID
     || ( pMeth->IsFixed() && pMeth->GetType() == SbxEMPTY )
     || !pMeth->ISA(SbMethod) )
    {
        return sal_False;
    }
    ScRawToken aToken;
    aToken.SetExternal( aName.GetBuffer() );
    aToken.eOp = ocMacro;
    pRawToken = aToken.Clone();
    return sal_True;
}

sal_Bool ScCompiler::IsNamedRange( const String& rUpperName )
{
    // IsNamedRange is called only from NextNewToken, with an upper-case string

    sal_uInt16 n;
    ScRangeName* pRangeName = pDoc->GetRangeName();
    if (pRangeName->SearchNameUpper( rUpperName, n ) )
    {
        ScRangeData* pData = (*pRangeName)[n];
        ScRawToken aToken;
        aToken.SetName( pData->GetIndex() );
        pRawToken = aToken.Clone();
        return sal_True;
    }
    else
        return sal_False;
}

bool ScCompiler::IsExternalNamedRange( const String& rSymbol )
{
    /* FIXME: This code currently (2008-12-02T15:41+0100 in CWS mooxlsc)
     * correctly parses external named references in OOo, as required per RFE
     * #i3740#, just that we can't store them in ODF yet. We will need an OASIS
     * spec first. Until then don't pretend to support external names that
     * wouldn't survive a save and reload cycle, return false instead. */

#if 0
    if (!pConv)
        return false;

    String aFile, aName;
    if (!pConv->parseExternalName( rSymbol, aFile, aName, pDoc, &maExternalLinks))
        return false;

    ScRawToken aToken;
    if (aFile.Len() > MAXSTRLEN || aName.Len() > MAXSTRLEN)
        return false;

    ScExternalRefManager* pRefMgr = pDoc->GetExternalRefManager();
    pRefMgr->convertToAbsName(aFile);
    sal_uInt16 nFileId = pRefMgr->getExternalFileId(aFile);
    if (!pRefMgr->getRangeNameTokens(nFileId, aName).get())
        // range name doesn't exist in the source document.
        return false;

    const String* pRealName = pRefMgr->getRealRangeName(nFileId, aName);
    aToken.SetExternalName(nFileId, pRealName ? *pRealName : aName);
    pRawToken = aToken.Clone();
    return true;
#else
    (void)rSymbol;
    return false;
#endif
}

sal_Bool ScCompiler::IsDBRange( const String& rName )
{
    sal_uInt16 n;
    ScDBCollection* pDBColl = pDoc->GetDBCollection();
    if (pDBColl->SearchName( rName, n ) )
    {
        ScDBData* pData = (*pDBColl)[n];
        ScRawToken aToken;
        aToken.SetName( pData->GetIndex() );
        aToken.eOp = ocDBArea;
        pRawToken = aToken.Clone();
        return sal_True;
    }
    else
        return sal_False;
}

sal_Bool ScCompiler::IsColRowName( const String& rName )
{
    sal_Bool bInList = sal_False;
    sal_Bool bFound = sal_False;
    ScSingleRefData aRef;
    String aName( rName );
    DeQuote( aName );
    SCTAB nThisTab = aPos.Tab();
    for ( short jThisTab = 1; jThisTab >= 0 && !bInList; jThisTab-- )
    {   // #50300# first check ranges on this sheet, in case of duplicated names
        for ( short jRow=0; jRow<2 && !bInList; jRow++ )
        {
            ScRangePairList* pRL;
            if ( !jRow )
                pRL = pDoc->GetColNameRanges();
            else
                pRL = pDoc->GetRowNameRanges();
            for ( ScRangePair* pR = pRL->First(); pR && !bInList; pR = pRL->Next() )
            {
                const ScRange& rNameRange = pR->GetRange(0);
                if ( jThisTab && !(rNameRange.aStart.Tab() <= nThisTab &&
                        nThisTab <= rNameRange.aEnd.Tab()) )
                    continue;   // for
                ScCellIterator aIter( pDoc, rNameRange );
                for ( ScBaseCell* pCell = aIter.GetFirst(); pCell && !bInList;
                        pCell = aIter.GetNext() )
                {
                    // Don't crash if cell (via CompileNameFormula) encounters
                    // a formula cell without code and
                    // HasStringData/Interpret/Compile is executed and all that
                    // recursive..
                    // Furthermore, *this* cell won't be touched, since no RPN exists yet.
                    CellType eType = pCell->GetCellType();
                    sal_Bool bOk = sal::static_int_cast<sal_Bool>( (eType == CELLTYPE_FORMULA ?
                        ((ScFormulaCell*)pCell)->GetCode()->GetCodeLen() > 0
                        && ((ScFormulaCell*)pCell)->aPos != aPos    // noIter
                        : sal_True ) );
                    if ( bOk && pCell->HasStringData() )
                    {
                        String aStr;
                        switch ( eType )
                        {
                            case CELLTYPE_STRING:
                                ((ScStringCell*)pCell)->GetString( aStr );
                            break;
                            case CELLTYPE_FORMULA:
                                ((ScFormulaCell*)pCell)->GetString( aStr );
                            break;
                            case CELLTYPE_EDIT:
                                ((ScEditCell*)pCell)->GetString( aStr );
                            break;
                            case CELLTYPE_NONE:
                            case CELLTYPE_VALUE:
                            case CELLTYPE_NOTE:
                            case CELLTYPE_SYMBOLS:
#if DBG_UTIL
                            case CELLTYPE_DESTROYED:
#endif
                                ;   // nothing, prevent compiler warning
                            break;
                        }
                        if ( ScGlobal::GetpTransliteration()->isEqual( aStr, aName ) )
                        {
                            aRef.InitFlags();
                            aRef.nCol = aIter.GetCol();
                            aRef.nRow = aIter.GetRow();
                            aRef.nTab = aIter.GetTab();
                            if ( !jRow )
                                aRef.SetColRel( sal_True );     // ColName
                            else
                                aRef.SetRowRel( sal_True );     // RowName
                            aRef.CalcRelFromAbs( aPos );
                            bInList = bFound = sal_True;
                        }
                    }
                }
            }
        }
    }
    if ( !bInList && pDoc->GetDocOptions().IsLookUpColRowNames() )
    {   // search in current sheet
        long nDistance = 0, nMax = 0;
        long nMyCol = (long) aPos.Col();
        long nMyRow = (long) aPos.Row();
        sal_Bool bTwo = sal_False;
        ScAddress aOne( 0, 0, aPos.Tab() );
        ScAddress aTwo( MAXCOL, MAXROW, aPos.Tab() );

        ScAutoNameCache* pNameCache = pDoc->GetAutoNameCache();
        if ( pNameCache )
        {
            //  #b6355215# use GetNameOccurences to collect all positions of aName on the sheet
            //  (only once), similar to the outer part of the loop in the "else" branch.

            const ScAutoNameAddresses& rAddresses = pNameCache->GetNameOccurences( aName, aPos.Tab() );

            //  Loop through the found positions, similar to the inner part of the loop in the "else" branch.
            //  The order of addresses in the vector is the same as from ScCellIterator.

            ScAutoNameAddresses::const_iterator aEnd(rAddresses.end());
            for ( ScAutoNameAddresses::const_iterator aAdrIter(rAddresses.begin()); aAdrIter != aEnd; ++aAdrIter )
            {
                ScAddress aAddress( *aAdrIter );        // cell address with an equal string

                if ( bFound )
                {   // stop if everything else is further away
                    if ( nMax < (long)aAddress.Col() )
                        break;      // aIter
                }
                if ( aAddress != aPos )
                {
                    // same treatment as in isEqual case below

                    SCCOL nCol = aAddress.Col();
                    SCROW nRow = aAddress.Row();
                    long nC = nMyCol - nCol;
                    long nR = nMyRow - nRow;
                    if ( bFound )
                    {
                        long nD = nC * nC + nR * nR;
                        if ( nD < nDistance )
                        {
                            if ( nC < 0 || nR < 0 )
                            {   // right or below
                                bTwo = sal_True;
                                aTwo.Set( nCol, nRow, aAddress.Tab() );
                                nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                                nDistance = nD;
                            }
                            else if ( !(nRow < aOne.Row() && nMyRow >= (long)aOne.Row()) )
                            {
                                // upper left, only if not further up than the
                                // current entry and nMyRow is below (CellIter
                                // runs column-wise)
                                bTwo = sal_False;
                                aOne.Set( nCol, nRow, aAddress.Tab() );
                                nMax = Max( nMyCol + nC, nMyRow + nR );
                                nDistance = nD;
                            }
                        }
                    }
                    else
                    {
                        aOne.Set( nCol, nRow, aAddress.Tab() );
                        nDistance = nC * nC + nR * nR;
                        nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                    }
                    bFound = sal_True;
                }
            }
        }
        else
        {
            ScCellIterator aIter( pDoc, ScRange( aOne, aTwo ) );
            for ( ScBaseCell* pCell = aIter.GetFirst(); pCell; pCell = aIter.GetNext() )
            {
                if ( bFound )
                {   // stop if everything else is further away
                    if ( nMax < (long)aIter.GetCol() )
                        break;      // aIter
                }
                CellType eType = pCell->GetCellType();
                sal_Bool bOk = sal::static_int_cast<sal_Bool>( (eType == CELLTYPE_FORMULA ?
                    ((ScFormulaCell*)pCell)->GetCode()->GetCodeLen() > 0
                    && ((ScFormulaCell*)pCell)->aPos != aPos    // noIter
                    : sal_True ) );
                if ( bOk && pCell->HasStringData() )
                {
                    String aStr;
                    switch ( eType )
                    {
                        case CELLTYPE_STRING:
                            ((ScStringCell*)pCell)->GetString( aStr );
                        break;
                        case CELLTYPE_FORMULA:
                            ((ScFormulaCell*)pCell)->GetString( aStr );
                        break;
                        case CELLTYPE_EDIT:
                            ((ScEditCell*)pCell)->GetString( aStr );
                        break;
                        case CELLTYPE_NONE:
                        case CELLTYPE_VALUE:
                        case CELLTYPE_NOTE:
                        case CELLTYPE_SYMBOLS:
#if DBG_UTIL
                        case CELLTYPE_DESTROYED:
#endif
                            ;   // nothing, prevent compiler warning
                        break;
                    }
                    if ( ScGlobal::GetpTransliteration()->isEqual( aStr, aName ) )
                    {
                        SCCOL nCol = aIter.GetCol();
                        SCROW nRow = aIter.GetRow();
                        long nC = nMyCol - nCol;
                        long nR = nMyRow - nRow;
                        if ( bFound )
                        {
                            long nD = nC * nC + nR * nR;
                            if ( nD < nDistance )
                            {
                                if ( nC < 0 || nR < 0 )
                                {   // right or below
                                    bTwo = sal_True;
                                    aTwo.Set( nCol, nRow, aIter.GetTab() );
                                    nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                                    nDistance = nD;
                                }
                                else if ( !(nRow < aOne.Row() && nMyRow >= (long)aOne.Row()) )
                                {
                                    // upper left, only if not further up than the
                                    // current entry and nMyRow is below (CellIter
                                    // runs column-wise)
                                    bTwo = sal_False;
                                    aOne.Set( nCol, nRow, aIter.GetTab() );
                                    nMax = Max( nMyCol + nC, nMyRow + nR );
                                    nDistance = nD;
                                }
                            }
                        }
                        else
                        {
                            aOne.Set( nCol, nRow, aIter.GetTab() );
                            nDistance = nC * nC + nR * nR;
                            nMax = Max( nMyCol + Abs( nC ), nMyRow + Abs( nR ) );
                        }
                        bFound = sal_True;
                    }
                }
            }
        }

        if ( bFound )
        {
            ScAddress aAdr;
            if ( bTwo )
            {
                if ( nMyCol >= (long)aOne.Col() && nMyRow >= (long)aOne.Row() )
                    aAdr = aOne;        // upper left takes precedence
                else
                {
                    if ( nMyCol < (long)aOne.Col() )
                    {   // two to the right
                        if ( nMyRow >= (long)aTwo.Row() )
                            aAdr = aTwo;        // directly right
                        else
                            aAdr = aOne;
                    }
                    else
                    {   // two below or below and right, take the nearest
                        long nC1 = nMyCol - aOne.Col();
                        long nR1 = nMyRow - aOne.Row();
                        long nC2 = nMyCol - aTwo.Col();
                        long nR2 = nMyRow - aTwo.Row();
                        if ( nC1 * nC1 + nR1 * nR1 <= nC2 * nC2 + nR2 * nR2 )
                            aAdr = aOne;
                        else
                            aAdr = aTwo;
                    }
                }
            }
            else
                aAdr = aOne;
            aRef.InitAddress( aAdr );
            if ( (aRef.nRow != MAXROW && pDoc->HasStringData(
                    aRef.nCol, aRef.nRow + 1, aRef.nTab ))
              || (aRef.nRow != 0 && pDoc->HasStringData(
                    aRef.nCol, aRef.nRow - 1, aRef.nTab )) )
                aRef.SetRowRel( sal_True );     // RowName
            else
                aRef.SetColRel( sal_True );     // ColName
            aRef.CalcRelFromAbs( aPos );
        }
    }
    if ( bFound )
    {
        ScRawToken aToken;
        aToken.SetSingleReference( aRef );
        aToken.eOp = ocColRowName;
        pRawToken = aToken.Clone();
        return sal_True;
    }
    else
        return sal_False;
}

sal_Bool ScCompiler::IsBoolean( const String& rName )
{
    OpCodeHashMap::const_iterator iLook( mxSymbols->getHashMap()->find( rName ) );
    if( iLook != mxSymbols->getHashMap()->end() &&
        ((*iLook).second == ocTrue ||
         (*iLook).second == ocFalse) )
    {
        ScRawToken aToken;
        aToken.SetOpCode( (*iLook).second );
        pRawToken = aToken.Clone();
        return sal_True;
    }
    else
        return sal_False;
}

//---------------------------------------------------------------------------

void ScCompiler::AutoCorrectParsedSymbol()
{
    xub_StrLen nPos = aCorrectedSymbol.Len();
    if ( nPos )
    {
        nPos--;
        const sal_Unicode cQuote = '\"';
        const sal_Unicode cx = 'x';
        const sal_Unicode cX = 'X';
        sal_Unicode c1 = aCorrectedSymbol.GetChar( 0 );
        sal_Unicode c2 = aCorrectedSymbol.GetChar( nPos );
        if ( c1 == cQuote && c2 != cQuote  )
        {   // "...
            // What's not a word doesn't belong to it.
            // Don't be pedantic: c < 128 should be sufficient here.
            while ( nPos && ((aCorrectedSymbol.GetChar(nPos) < 128) &&
                    ((GetCharTableFlags( aCorrectedSymbol.GetChar(nPos) ) &
                    (SC_COMPILER_C_WORD | SC_COMPILER_C_CHAR_DONTCARE)) == 0)) )
                nPos--;
            if ( nPos == MAXSTRLEN - 2 )
                aCorrectedSymbol.SetChar( nPos, cQuote );   // '"' the 255th character
            else
                aCorrectedSymbol.Insert( cQuote, nPos + 1 );
            bCorrected = sal_True;
        }
        else if ( c1 != cQuote && c2 == cQuote )
        {   // ..."
            aCorrectedSymbol.Insert( cQuote, 0 );
            bCorrected = sal_True;
        }
        else if ( nPos == 0 && (c1 == cx || c1 == cX) )
        {   // x => *
            aCorrectedSymbol = mxSymbols->getSymbol(ocMul);
            bCorrected = sal_True;
        }
        else if ( (GetCharTableFlags( c1 ) & SC_COMPILER_C_CHAR_VALUE)
               && (GetCharTableFlags( c2 ) & SC_COMPILER_C_CHAR_VALUE) )
        {
            xub_StrLen nXcount;
            if ( (nXcount = aCorrectedSymbol.GetTokenCount( cx )) > 1 )
            {   // x => *
                xub_StrLen nIndex = 0;
                sal_Unicode c = mxSymbols->getSymbol(ocMul).GetChar(0);
                while ( (nIndex = aCorrectedSymbol.SearchAndReplace(
                        cx, c, nIndex )) != STRING_NOTFOUND )
                    nIndex++;
                bCorrected = sal_True;
            }
            if ( (nXcount = aCorrectedSymbol.GetTokenCount( cX )) > 1 )
            {   // X => *
                xub_StrLen nIndex = 0;
                sal_Unicode c = mxSymbols->getSymbol(ocMul).GetChar(0);
                while ( (nIndex = aCorrectedSymbol.SearchAndReplace(
                        cX, c, nIndex )) != STRING_NOTFOUND )
                    nIndex++;
                bCorrected = sal_True;
            }
        }
        else
        {
            String aSymbol( aCorrectedSymbol );
            String aDoc;
            xub_StrLen nPosition;
            if ( aSymbol.GetChar(0) == '\''
              && ((nPosition = aSymbol.SearchAscii( "'#" )) != STRING_NOTFOUND) )
            {   // Split off 'Doc'#, may be d:\... or whatever
                aDoc = aSymbol.Copy( 0, nPosition + 2 );
                aSymbol.Erase( 0, nPosition + 2 );
            }
            xub_StrLen nRefs = aSymbol.GetTokenCount( ':' );
            sal_Bool bColons;
            if ( nRefs > 2 )
            {   // duplicated or too many ':'? B:2::C10 => B2:C10
                bColons = sal_True;
                xub_StrLen nIndex = 0;
                String aTmp1( aSymbol.GetToken( 0, ':', nIndex ) );
                xub_StrLen nLen1 = aTmp1.Len();
                String aSym, aTmp2;
                sal_Bool bLastAlp, bNextNum;
                bLastAlp = bNextNum = sal_True;
                xub_StrLen nStrip = 0;
                xub_StrLen nCount = nRefs;
                for ( xub_StrLen j=1; j<nCount; j++ )
                {
                    aTmp2 = aSymbol.GetToken( 0, ':', nIndex );
                    xub_StrLen nLen2 = aTmp2.Len();
                    if ( nLen1 || nLen2 )
                    {
                        if ( nLen1 )
                        {
                            aSym += aTmp1;
                            bLastAlp = CharClass::isAsciiAlpha( aTmp1 );
                        }
                        if ( nLen2 )
                        {
                            bNextNum = CharClass::isAsciiNumeric( aTmp2 );
                            if ( bLastAlp == bNextNum && nStrip < 1 )
                            {
                                // Must be alternating number/string, only
                                // strip within a reference.
                                nRefs--;
                                nStrip++;
                            }
                            else
                            {
                                xub_StrLen nSymLen = aSym.Len();
                                if ( nSymLen
                                  && (aSym.GetChar( nSymLen - 1 ) != ':') )
                                    aSym += ':';
                                nStrip = 0;
                            }
                            bLastAlp = !bNextNum;
                        }
                        else
                        {   // ::
                            nRefs--;
                            if ( nLen1 )
                            {   // B10::C10 ? append ':' on next round
                                if ( !bLastAlp && !CharClass::isAsciiNumeric( aTmp1 ) )
                                    nStrip++;
                            }
                            bNextNum = !bLastAlp;
                        }
                        aTmp1 = aTmp2;
                        nLen1 = nLen2;
                    }
                    else
                        nRefs--;
                }
                aSymbol = aSym;
                aSymbol += aTmp1;
            }
            else
                bColons = sal_False;
            if ( nRefs && nRefs <= 2 )
            {   // reference twisted? 4A => A4 etc.
                String aTab[2], aRef[2];
                const ScAddress::Details aDetails( pConv->meConv, aPos );
                if ( nRefs == 2 )
                {
                    aRef[0] = aSymbol.GetToken( 0, ':' );
                    aRef[1] = aSymbol.GetToken( 1, ':' );
                }
                else
                    aRef[0] = aSymbol;

                sal_Bool bChanged = sal_False;
                sal_Bool bOk = sal_True;
                sal_uInt16 nMask = SCA_VALID | SCA_VALID_COL | SCA_VALID_ROW;
                for ( int j=0; j<nRefs; j++ )
                {
                    xub_StrLen nTmp = 0;
                    xub_StrLen nDotPos = STRING_NOTFOUND;
                    while ( (nTmp = aRef[j].Search( '.', nTmp )) != STRING_NOTFOUND )
                        nDotPos = nTmp++;      // the last one counts
                    if ( nDotPos != STRING_NOTFOUND )
                    {
                        aTab[j] = aRef[j].Copy( 0, nDotPos + 1 );  // with '.'
                        aRef[j].Erase( 0, nDotPos + 1 );
                    }
                    String aOld( aRef[j] );
                    String aStr2;
                    const sal_Unicode* p = aRef[j].GetBuffer();
                    while ( *p && CharClass::isAsciiNumeric( *p ) )
                        aStr2 += *p++;
                    aRef[j] = String( p );
                    aRef[j] += aStr2;
                    if ( bColons || aRef[j] != aOld )
                    {
                        bChanged = sal_True;
                        ScAddress aAdr;
                        bOk &= ((aAdr.Parse( aRef[j], pDoc, aDetails ) & nMask) == nMask);
                    }
                }
                if ( bChanged && bOk )
                {
                    aCorrectedSymbol = aDoc;
                    aCorrectedSymbol += aTab[0];
                    aCorrectedSymbol += aRef[0];
                    if ( nRefs == 2 )
                    {
                        aCorrectedSymbol += ':';
                        aCorrectedSymbol += aTab[1];
                        aCorrectedSymbol += aRef[1];
                    }
                    bCorrected = sal_True;
                }
            }
        }
    }
}

inline bool lcl_UpperAsciiOrI18n( String& rUpper, const String& rOrg, FormulaGrammar::Grammar eGrammar )
{
    if (FormulaGrammar::isODFF( eGrammar ))
    {
        // ODFF has a defined set of English function names, avoid i18n 
        // overhead.
        rUpper = rOrg;
        rUpper.ToUpperAscii();
        return true;
    }
    else
    {
        rUpper = ScGlobal::pCharClass->upper( rOrg );
        return false;
    }
}

sal_Bool ScCompiler::NextNewToken( bool bInArray )
{
    bool bAllowBooleans = bInArray;
    xub_StrLen nSpaces = NextSymbol(bInArray);

#if 0
    fprintf( stderr, "NextNewToken '%s' (spaces = %d)\n",
             rtl::OUStringToOString( cSymbol, RTL_TEXTENCODING_UTF8 ).getStr(), nSpaces );
#endif

    if (!cSymbol[0])
        return false;

    if( nSpaces )
    {
        ScRawToken aToken;
        aToken.SetOpCode( ocSpaces );
        aToken.sbyte.cByte = (sal_uInt8) ( nSpaces > 255 ? 255 : nSpaces );
        if( !static_cast<ScTokenArray*>(pArr)->AddRawToken( aToken ) )
        {
            SetError(errCodeOverflow);
            return false;
        }
    }

    // Short cut for references when reading ODF to speedup things.
    if (mnPredetectedReference)
    {
        String aStr( cSymbol);
        if (!IsPredetectedReference( aStr) && !IsExternalNamedRange( aStr))
        {
            /* TODO: it would be nice to generate a #REF! error here, which
             * would need an ocBad token with additional error value.
             * FormulaErrorToken wouldn't do because we want to preserve the
             * original string containing partial valid address
             * information. */
            ScRawToken aToken;
            aToken.SetString( aStr.GetBuffer() );
            aToken.NewOpCode( ocBad );
            pRawToken = aToken.Clone();
        }
        return true;
    }

    if ( (cSymbol[0] == '#' || cSymbol[0] == '$') && cSymbol[1] == 0 &&
            !bAutoCorrect )
    {   // #101100# special case to speed up broken [$]#REF documents
        /* FIXME: ISERROR(#REF!) would be valid and sal_True and the formula to
         * be processed as usual. That would need some special treatment,
         * also in NextSymbol() because of possible combinations of
         * #REF!.#REF!#REF! parts. In case of reading ODF that is all
         * handled by IsPredetectedReference(), this case here remains for
         * manual/API input. */
        String aBad( aFormula.Copy( nSrcPos-1 ) );
        eLastOp = pArr->AddBad( aBad )->GetOpCode();
        return false;
    }

    if( IsString() )
        return true;

    bool bMayBeFuncName;
    bool bAsciiNonAlnum;    // operators, separators, ...
    if ( cSymbol[0] < 128 )
    {
        bMayBeFuncName = CharClass::isAsciiAlpha( cSymbol[0] );
        bAsciiNonAlnum = !bMayBeFuncName && !CharClass::isAsciiDigit( cSymbol[0] );
    }
    else
    {
        String aTmpStr( cSymbol[0] );
        bMayBeFuncName = ScGlobal::pCharClass->isLetter( aTmpStr, 0 );
        bAsciiNonAlnum = false;
    }
    if ( bMayBeFuncName )
    {
        // a function name must be followed by a parenthesis
        const sal_Unicode* p = aFormula.GetBuffer() + nSrcPos;
        while( *p == ' ' )
            p++;
        bMayBeFuncName = ( *p == '(' );
    }

#if 0
    fprintf( stderr, "Token '%s'\n",
            rtl::OUStringToOString( aUpper, RTL_TEXTENCODING_UTF8 ).getStr() );
#endif

    // #42016# Italian ARCTAN.2 resulted in #REF! => IsOpcode() before
    // IsReference().

    String aUpper;

    do
    {
        mbRewind = false;
        const String aOrg( cSymbol );

        if (bAsciiNonAlnum && IsOpCode( aOrg, bInArray ))
            return true;

        aUpper.Erase();
        bool bAsciiUpper = false;
        if (bMayBeFuncName)
        {
            bAsciiUpper = lcl_UpperAsciiOrI18n( aUpper, aOrg, meGrammar);
            if (IsOpCode( aUpper, bInArray ))
                return true;
        }

        // Column 'DM' ("Deutsche Mark", German currency) couldn't be
        // referred => IsReference() before IsValue().
        // Preserve case of file names in external references.
        if (IsReference( aOrg ))
        {
            if (mbRewind)   // Range operator, but no direct reference.
                continue;   // do; up to range operator.
            return true;
        }

        if (!aUpper.Len())
            bAsciiUpper = lcl_UpperAsciiOrI18n( aUpper, aOrg, meGrammar);

        // IsBoolean() before IsValue() to catch inline bools without the kludge
        //    for inline arrays.
        if (bAllowBooleans && IsBoolean( aUpper ))
            return true;

        if (IsValue( aUpper ))
            return true;

        // User defined names and such do need i18n upper also in ODF.
        if (bAsciiUpper)
            aUpper = ScGlobal::pCharClass->upper( aOrg );

        if (IsNamedRange( aUpper ))
            return true;
        // Preserve case of file names in external references.
        if (IsExternalNamedRange( aOrg ))
            return true;
        if (IsDBRange( aUpper ))
            return true;
        if (IsColRowName( aUpper ))
            return true;
        if (bMayBeFuncName && IsMacro( aUpper ))
            return true;
        if (bMayBeFuncName && IsOpCode2( aUpper ))
            return true;

    } while (mbRewind);

    if ( mbExtendedErrorDetection )
    {
        // set an error and end compilation
        SetError( errNoName );
        return false;
    }

    // Provide single token information and continue. Do not set an error, that 
    // would prematurely end compilation. Simple unknown names are handled by 
    // the interpreter.
    ScGlobal::pCharClass->toLower( aUpper );
    ScRawToken aToken;
    aToken.SetString( aUpper.GetBuffer() );
    aToken.NewOpCode( ocBad );
    pRawToken = aToken.Clone();
    if ( bAutoCorrect )
        AutoCorrectParsedSymbol();
    return true;
}

void ScCompiler::CreateStringFromXMLTokenArray( String& rFormula, String& rFormulaNmsp )
{
    bool bExternal = GetGrammar() == FormulaGrammar::GRAM_EXTERNAL;
    sal_uInt16 nExpectedCount = bExternal ? 2 : 1;
    DBG_ASSERT( pArr->GetLen() == nExpectedCount, "ScCompiler::CreateStringFromXMLTokenArray - wrong number of tokens" );
    if( pArr->GetLen() == nExpectedCount )
    {
        FormulaToken** ppTokens = pArr->GetArray();
        // string tokens expected, GetString() will assert if token type is wrong
        rFormula = ppTokens[ 0 ]->GetString();
        if( bExternal )
            rFormulaNmsp = ppTokens[ 1 ]->GetString();
    }
}

ScTokenArray* ScCompiler::CompileString( const String& rFormula )
{
#if 0
    fprintf( stderr, "CompileString '%s'\n",
             rtl::OUStringToOString( rFormula, RTL_TEXTENCODING_UTF8 ).getStr() );
#endif

    OSL_ENSURE( meGrammar != FormulaGrammar::GRAM_EXTERNAL, "ScCompiler::CompileString - unexpected grammar GRAM_EXTERNAL" );
    if( meGrammar == FormulaGrammar::GRAM_EXTERNAL )
        SetGrammar( FormulaGrammar::GRAM_PODF );

    ScTokenArray aArr;
    pArr = &aArr;
    aFormula = rFormula;

    aFormula.EraseLeadingChars();
    aFormula.EraseTrailingChars();
    nSrcPos = 0;
    bCorrected = sal_False;
    if ( bAutoCorrect )
    {
        aCorrectedFormula.Erase();
        aCorrectedSymbol.Erase();
    }
    sal_uInt8 nForced = 0;   // ==formula forces recalc even if cell is not visible
    if( aFormula.GetChar(nSrcPos) == '=' )
    {
        nSrcPos++;
        nForced++;
        if ( bAutoCorrect )
            aCorrectedFormula += '=';
    }
    if( aFormula.GetChar(nSrcPos) == '=' )
    {
        nSrcPos++;
        nForced++;
        if ( bAutoCorrect )
            aCorrectedFormula += '=';
    }
    struct FunctionStack
    {
        OpCode  eOp;
        short   nPar;
    };
    // FunctionStack only used if PODF!
    bool bPODF = FormulaGrammar::isPODF( meGrammar);
    const size_t nAlloc = 512;
    FunctionStack aFuncs[ nAlloc ];
    FunctionStack* pFunctionStack = (bPODF && rFormula.Len() > nAlloc ?
            new FunctionStack[ rFormula.Len() ] : &aFuncs[0]);
    pFunctionStack[0].eOp = ocNone;
    pFunctionStack[0].nPar = 0;
    size_t nFunction = 0;
    short nBrackets = 0;
    bool bInArray = false;
    eLastOp = ocOpen;
    while( NextNewToken( bInArray ) )
    {
        const OpCode eOp = pRawToken->GetOpCode();
        switch (eOp)
        {
            case ocOpen:
            {
                ++nBrackets;
                if (bPODF)
                {
                    ++nFunction;
                    pFunctionStack[ nFunction ].eOp = eLastOp;
                    pFunctionStack[ nFunction ].nPar = 0;
                }
            }
            break;
            case ocClose:
            {
                if( !nBrackets )
                {
                    SetError( errPairExpected );
                    if ( bAutoCorrect )
                    {
                        bCorrected = sal_True;
                        aCorrectedSymbol.Erase();
                    }
                }
                else
                    nBrackets--;
                if (bPODF && nFunction)
                    --nFunction;
            }
            break;
            case ocSep:
            {
                if (bPODF)
                    ++pFunctionStack[ nFunction ].nPar;
            }
            break;
            case ocArrayOpen:
            {
                if( bInArray )
                    SetError( errNestedArray );
                else
                    bInArray = true;
                // Don't count following column separator as parameter separator.
                if (bPODF)
                {
                    ++nFunction;
                    pFunctionStack[ nFunction ].eOp = eOp;
                    pFunctionStack[ nFunction ].nPar = 0;
                }
            }
            break;
            case ocArrayClose:
            {
                if( bInArray )
                {
                    bInArray = false;
                }
                else
                {
                    SetError( errPairExpected );
                    if ( bAutoCorrect )
                    {
                        bCorrected = sal_True;
                        aCorrectedSymbol.Erase();
                    }
                }
                if (bPODF && nFunction)
                    --nFunction;
            }
            default:
            break;
        }
        if( (eLastOp == ocSep ||
             eLastOp == ocArrayRowSep ||
             eLastOp == ocArrayColSep ||
             eLastOp == ocArrayOpen) &&
            (eOp == ocSep ||
             eOp == ocArrayRowSep ||
             eOp == ocArrayColSep ||
             eOp == ocArrayClose) )
        {
            // FIXME: should we check for known functions with optional empty
            // args so the correction dialog can do better?
            if ( !static_cast<ScTokenArray*>(pArr)->Add( new FormulaMissingToken ) )
            {
                SetError(errCodeOverflow); break;
            }
        }
        if (bPODF)
        {
            /* TODO: for now this is the only PODF adapter. If there were more,
             * factor this out. */
            // Insert ADDRESS() new empty parameter 4 if there is a 4th, now to be 5th.
            if (eOp == ocSep &&
                    pFunctionStack[ nFunction ].eOp == ocAddress &&
                    pFunctionStack[ nFunction ].nPar == 3)
            {
                if (!static_cast<ScTokenArray*>(pArr)->Add( new FormulaToken( svSep,ocSep)) ||
                        !static_cast<ScTokenArray*>(pArr)->Add( new FormulaDoubleToken( 1.0)))
                {
                    SetError(errCodeOverflow); break;
                }
                ++pFunctionStack[ nFunction ].nPar;
            }
        }
        FormulaToken* pNewToken = static_cast<ScTokenArray*>(pArr)->Add( pRawToken->CreateToken());
        if (!pNewToken)
        {
            SetError(errCodeOverflow); break;
        }
        else if (eLastOp == ocRange && pNewToken->GetOpCode() == ocPush &&
                pNewToken->GetType() == svSingleRef)
            static_cast<ScTokenArray*>(pArr)->MergeRangeReference( aPos);
        eLastOp = pRawToken->GetOpCode();
        if ( bAutoCorrect )
            aCorrectedFormula += aCorrectedSymbol;
    }
    if ( mbCloseBrackets )
    {
        if( bInArray )
        {
            FormulaByteToken aToken( ocArrayClose );
            if( !pArr->AddToken( aToken ) )
            {
                SetError(errCodeOverflow);
            }
            else if ( bAutoCorrect )
                aCorrectedFormula += mxSymbols->getSymbol(ocArrayClose);
        }

        FormulaByteToken aToken( ocClose );
        while( nBrackets-- )
        {
            if( !pArr->AddToken( aToken ) )
            {
                SetError(errCodeOverflow); break;
            }
            if ( bAutoCorrect )
                aCorrectedFormula += mxSymbols->getSymbol(ocClose);
        }
    }
    if ( nForced >= 2 )
        pArr->SetRecalcModeForced();

    if (pFunctionStack != &aFuncs[0])
        delete [] pFunctionStack;

    // remember pArr, in case a subsequent CompileTokenArray() is executed.
    ScTokenArray* pNew = new ScTokenArray( aArr );
    pArr = pNew;
    return pNew;
}


ScTokenArray* ScCompiler::CompileString( const String& rFormula, const String& rFormulaNmsp )
{
    DBG_ASSERT( (GetGrammar() == FormulaGrammar::GRAM_EXTERNAL) || (rFormulaNmsp.Len() == 0),
        "ScCompiler::CompileString - unexpected formula namespace for internal grammar" );
    if( GetGrammar() == FormulaGrammar::GRAM_EXTERNAL ) try
    {
        ScFormulaParserPool& rParserPool = pDoc->GetFormulaParserPool();
        uno::Reference< sheet::XFormulaParser > xParser( rParserPool.getFormulaParser( rFormulaNmsp ), uno::UNO_SET_THROW );
        table::CellAddress aReferencePos;
        ScUnoConversion::FillApiAddress( aReferencePos, aPos );
        uno::Sequence< sheet::FormulaToken > aTokenSeq = xParser->parseFormula( rFormula, aReferencePos );
        ScTokenArray aTokenArray;
        if( ScTokenConversion::ConvertToTokenArray( *pDoc, aTokenArray, aTokenSeq ) )
        {
            // remember pArr, in case a subsequent CompileTokenArray() is executed.
            ScTokenArray* pNew = new ScTokenArray( aTokenArray );
            pArr = pNew;
            return pNew;
        }
    }
    catch( uno::Exception& )
    {
    }
    // no success - fallback to some internal grammar and hope the best
    return CompileString( rFormula );
}


sal_Bool ScCompiler::HandleRange()
{
    ScRangeData* pRangeData = pDoc->GetRangeName()->FindIndex( pToken->GetIndex() );
    if (pRangeData)
    {
        sal_uInt16 nErr = pRangeData->GetErrCode();
        if( nErr )
            SetError( errNoName );
        else if ( !bCompileForFAP )
        {
            ScTokenArray* pNew;
            // #35168# put named formula into parentheses.
            // #37680# But only if there aren't any yet, parenthetical
            // ocSep doesn't work, e.g. SUM((...;...))
            // or if not directly between ocSep/parenthesis,
            // e.g. SUM(...;(...;...)) no, SUM(...;(...)*3) yes,
            // in short: if it isn't a self-contained expression.
            FormulaToken* p1 = pArr->PeekPrevNoSpaces();
            FormulaToken* p2 = pArr->PeekNextNoSpaces();
            OpCode eOp1 = (p1 ? p1->GetOpCode() : static_cast<OpCode>( ocSep ) );
            OpCode eOp2 = (p2 ? p2->GetOpCode() : static_cast<OpCode>( ocSep ) );
            sal_Bool bBorder1 = (eOp1 == ocSep || eOp1 == ocOpen);
            sal_Bool bBorder2 = (eOp2 == ocSep || eOp2 == ocClose);
            sal_Bool bAddPair = !(bBorder1 && bBorder2);
            if ( bAddPair )
            {
                pNew = new ScTokenArray();
                pNew->AddOpCode( ocClose );
                PushTokenArray( pNew, sal_True );
                pNew->Reset();
            }
			pNew = pRangeData->GetCode()->Clone();
            PushTokenArray( pNew, sal_True );
            if( pRangeData->HasReferences() )
            {
                SetRelNameReference();
                MoveRelWrap(pRangeData->GetMaxCol(), pRangeData->GetMaxRow());
            }
            pNew->Reset();
            if ( bAddPair )
            {
                pNew = new ScTokenArray();
                pNew->AddOpCode( ocOpen );
                PushTokenArray( pNew, sal_True );
                pNew->Reset();
            }
            return GetToken();
        }
    }
    else
        SetError(errNoName);
    return sal_True;
}
// -----------------------------------------------------------------------------
sal_Bool ScCompiler::HandleExternalReference(const FormulaToken& _aToken)
{
    // Handle external range names.
    switch (_aToken.GetType())
    {
        case svExternalSingleRef:
        case svExternalDoubleRef:
            pArr->IncrementRefs();
        break;
        case svExternalName:
        {
            ScExternalRefManager* pRefMgr = pDoc->GetExternalRefManager();
            const String* pFile = pRefMgr->getExternalFileName(_aToken.GetIndex());
            if (!pFile)
            {
                SetError(errNoName);
                return true;
            }

            const String& rName = _aToken.GetString();
            ScExternalRefCache::TokenArrayRef xNew = pRefMgr->getRangeNameTokens(
                _aToken.GetIndex(), rName, &aPos);

            if (!xNew)
            {
                SetError(errNoName);
                return true;
            }

            ScTokenArray* pNew = xNew->Clone();
            PushTokenArray( pNew, true);
            if (pNew->GetNextReference() != NULL)
            {
                SetRelNameReference();
                MoveRelWrap(MAXCOL, MAXROW);
            }
            pNew->Reset();
            return GetToken();
        }
		default:
			DBG_ERROR("Wrong type for external reference!");
			return sal_False;
    }
    return sal_True;
}


//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Append token to RPN code
//---------------------------------------------------------------------------


//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------
// RPN creation by recursion
//---------------------------------------------------------------------------



//-----------------------------------------------------------------------------

sal_Bool ScCompiler::HasModifiedRange()
{
	pArr->Reset();
    for ( FormulaToken* t = pArr->Next(); t; t = pArr->Next() )
    {
        OpCode eOpCode = t->GetOpCode();
        if ( eOpCode == ocName )
		{
             ScRangeData* pRangeData = pDoc->GetRangeName()->FindIndex(t->GetIndex());

			if (pRangeData && pRangeData->IsModified())
				return sal_True;
		}
        else if ( eOpCode == ocDBArea )
        {
            ScDBData* pDBData = pDoc->GetDBCollection()->FindIndex(t->GetIndex());

            if (pDBData && pDBData->IsModified())
                return sal_True;
        }
    }
    return sal_False;
}


//---------------------------------------------------------------------------

template< typename T, typename S >
S lcl_adjval( S& n, T pos, T max, sal_Bool bRel )
{
    max++;
    if( bRel )
        n = sal::static_int_cast<S>( n + pos );
    if( n < 0 )
        n = sal::static_int_cast<S>( n + max );
    else if( n >= max )
        n = sal::static_int_cast<S>( n - max );
    if( bRel )
        n = sal::static_int_cast<S>( n - pos );
    return n;
}

// reference of named range with relative references

void ScCompiler::SetRelNameReference()
{
    pArr->Reset();
    for( ScToken* t = static_cast<ScToken*>(pArr->GetNextReference()); t;
                  t = static_cast<ScToken*>(pArr->GetNextReference()) )
    {
        ScSingleRefData& rRef1 = t->GetSingleRef();
        if ( rRef1.IsColRel() || rRef1.IsRowRel() || rRef1.IsTabRel() )
            rRef1.SetRelName( sal_True );
        if ( t->GetType() == svDoubleRef )
        {
            ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
            if ( rRef2.IsColRel() || rRef2.IsRowRel() || rRef2.IsTabRel() )
                rRef2.SetRelName( sal_True );
        }
    }
}

// Wrap-adjust relative references of a RangeName to current position,
// don't call for other token arrays!
void ScCompiler::MoveRelWrap( SCCOL nMaxCol, SCROW nMaxRow )
{
    pArr->Reset();
    for( ScToken* t = static_cast<ScToken*>(pArr->GetNextReference()); t;
                  t = static_cast<ScToken*>(pArr->GetNextReference()) )
    {
        if ( t->GetType() == svSingleRef || t->GetType() == svExternalSingleRef )
            ScRefUpdate::MoveRelWrap( pDoc, aPos, nMaxCol, nMaxRow, SingleDoubleRefModifier( t->GetSingleRef() ).Ref() );
        else
            ScRefUpdate::MoveRelWrap( pDoc, aPos, nMaxCol, nMaxRow, t->GetDoubleRef() );
    }
}

// static
// Wrap-adjust relative references of a RangeName to current position,
// don't call for other token arrays!
void ScCompiler::MoveRelWrap( ScTokenArray& rArr, ScDocument* pDoc, const ScAddress& rPos,
                              SCCOL nMaxCol, SCROW nMaxRow )
{
    rArr.Reset();
    for( ScToken* t = static_cast<ScToken*>(rArr.GetNextReference()); t;
                  t = static_cast<ScToken*>(rArr.GetNextReference()) )
    {
        if ( t->GetType() == svSingleRef || t->GetType() == svExternalSingleRef )
            ScRefUpdate::MoveRelWrap( pDoc, rPos, nMaxCol, nMaxRow, SingleDoubleRefModifier( t->GetSingleRef() ).Ref() );
        else
            ScRefUpdate::MoveRelWrap( pDoc, rPos, nMaxCol, nMaxRow, t->GetDoubleRef() );
    }
}

ScRangeData* ScCompiler::UpdateReference(UpdateRefMode eUpdateRefMode,
                                 const ScAddress& rOldPos, const ScRange& r,
                                 SCsCOL nDx, SCsROW nDy, SCsTAB nDz,
                                 sal_Bool& rChanged, sal_Bool& rRefSizeChanged )
{
    rChanged = rRefSizeChanged = sal_False;
    if ( eUpdateRefMode == URM_COPY )
    {   // Normally nothing has to be done here since RelRefs are used, also
        // SharedFormulas don't need any special handling, except if they
        // wrapped around sheet borders.
        // #67383# But ColRowName tokens pointing to a ColRow header which was
        // copied along with this formula need to be updated to point to the
        // copied header instead of the old position's new intersection.
        ScToken* t;
        pArr->Reset();
        while( (t = static_cast<ScToken*>(pArr->GetNextColRowName())) != NULL )
        {
            ScSingleRefData& rRef = t->GetSingleRef();
            rRef.CalcAbsIfRel( rOldPos );
            ScAddress aNewRef( rRef.nCol + nDx, rRef.nRow + nDy, rRef.nTab + nDz );
            if ( r.In( aNewRef ) )
            {   // yes, this is URM_MOVE
                if ( ScRefUpdate::Update( pDoc, URM_MOVE, aPos,
                        r, nDx, nDy, nDz,
                        SingleDoubleRefModifier( rRef ).Ref() )
                        != UR_NOTHING
                    )
                    rChanged = sal_True;
            }
        }
        // Check for SharedFormulas.
        ScRangeData* pRangeData = NULL;
        pArr->Reset();
        for( FormulaToken* j = pArr->GetNextName(); j && !pRangeData;
             j = pArr->GetNextName() )
        {
            if( j->GetOpCode() == ocName )
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex( j->GetIndex() );
                if (pName && pName->HasType(RT_SHARED))
                    pRangeData = pName;
            }
        }
        // Check SharedFormulas for wraps.
        if (pRangeData)
        {
            ScRangeData* pName = pRangeData;
            pRangeData = NULL;
            pArr->Reset();
            for( t = static_cast<ScToken*>(pArr->GetNextReferenceRPN()); t && !pRangeData;
                 t = static_cast<ScToken*>(pArr->GetNextReferenceRPN()) )
            {
                sal_Bool bRelName = (t->GetType() == svSingleRef ?
                        t->GetSingleRef().IsRelName() :
                        (t->GetDoubleRef().Ref1.IsRelName() ||
                         t->GetDoubleRef().Ref2.IsRelName()));
                if (bRelName)
                {
                    t->CalcAbsIfRel( rOldPos);
                    sal_Bool bValid = (t->GetType() == svSingleRef ?
                            t->GetSingleRef().Valid() :
                            t->GetDoubleRef().Valid());
                    // If the reference isn't valid, copying the formula
                    // wrapped it. Replace SharedFormula.
                    if (!bValid)
                    {
                        pRangeData = pName;
                        rChanged = sal_True;
                    }
                }
            }
        }
        return pRangeData;
    }
    else
    {
/*
 * Set SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE to 1 if we wanted to preserve as
 * many shared formulas as possible instead of replacing them with direct code.
 * Note that this may produce shared formula usage Excel doesn't understand,
 * which would have to be adapted for in the export filter. Advisable as a long
 * term goal, since it could decrease memory footprint.
 */
#define SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE 0
        ScRangeData* pRangeData = NULL;
        ScToken* t;
        pArr->Reset();
        while( (t = static_cast<ScToken*>(pArr->GetNextReferenceOrName())) != NULL )
        {
            if( t->GetOpCode() == ocName )
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex( t->GetIndex() );
                if (pName && pName->HasType(RT_SHAREDMOD))
                {
                    pRangeData = pName;     // maybe need a replacement of shared with own code
#if ! SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                    rChanged = sal_True;
#endif
                }
            }
            else if( t->GetType() != svIndex )  // it may be a DB area!!!
            {
                t->CalcAbsIfRel( rOldPos );
                switch (t->GetType())
                {
                    case svExternalSingleRef:
                    case svExternalDoubleRef:
                        // External references never change their positioning
                        // nor point to parts that will be removed or expanded.
                        // In fact, calling ScRefUpdate::Update() for URM_MOVE
                        // may have negative side effects. Simply adapt
                        // relative references to the new position.
                        t->CalcRelFromAbs( aPos);
                        break;
                    case svSingleRef:
                        {
                            if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                                        aPos, r, nDx, nDy, nDz,
                                        SingleDoubleRefModifier(
                                            t->GetSingleRef()).Ref())
                                    != UR_NOTHING)
                                rChanged = sal_True;
                        }
                        break;
                    default:
                        {
                            ScComplexRefData& rRef = t->GetDoubleRef();
                            SCCOL nCols = rRef.Ref2.nCol - rRef.Ref1.nCol;
                            SCROW nRows = rRef.Ref2.nRow - rRef.Ref1.nRow;
                            SCTAB nTabs = rRef.Ref2.nTab - rRef.Ref1.nTab;
                            if ( ScRefUpdate::Update( pDoc, eUpdateRefMode,
                                        aPos, r, nDx, nDy, nDz,
                                        t->GetDoubleRef()) != UR_NOTHING)
                            {
                                rChanged = sal_True;
                                if (rRef.Ref2.nCol - rRef.Ref1.nCol != nCols ||
                                        rRef.Ref2.nRow - rRef.Ref1.nRow != nRows ||
                                        rRef.Ref2.nTab - rRef.Ref1.nTab != nTabs)
                                    rRefSizeChanged = sal_True;
                            }
                        }
                }
            }
        }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
        sal_Bool bEasyShared, bPosInRange;
        if ( !pRangeData )
            bEasyShared = bPosInRange = sal_False;
        else
        {
            bEasyShared = sal_True;
            bPosInRange = r.In( eUpdateRefMode == URM_MOVE ? aPos : rOldPos );
        }
#endif
        pArr->Reset();
        while ( (t = static_cast<ScToken*>(pArr->GetNextReferenceRPN())) != NULL )
        {
            if ( t->GetRef() != 1 )
            {
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                bEasyShared = sal_False;
#endif
            }
            else
            {   // if nRefCnt>1 it's already updated in token code
                if ( t->GetType() == svSingleRef )
                {
                    ScSingleRefData& rRef = t->GetSingleRef();
                    SingleDoubleRefModifier aMod( rRef );
                    if ( rRef.IsRelName() )
                    {
                        ScRefUpdate::MoveRelWrap( pDoc, aPos, MAXCOL, MAXROW, aMod.Ref() );
                        rChanged = sal_True;
                    }
                    else
                    {
                        aMod.Ref().CalcAbsIfRel( rOldPos );
                        if ( ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                    r, nDx, nDy, nDz, aMod.Ref() )
                                != UR_NOTHING
                            )
                            rChanged = sal_True;
                    }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                    if ( bEasyShared )
                    {
                        const ScSingleRefData& rSRD = aMod.Ref().Ref1;
                        ScAddress aRef( rSRD.nCol, rSRD.nRow, rSRD.nTab );
                        if ( r.In( aRef ) != bPosInRange )
                            bEasyShared = sal_False;
                    }
#endif
                }
                else
                {
                    ScComplexRefData& rRef = t->GetDoubleRef();
                    SCCOL nCols = rRef.Ref2.nCol - rRef.Ref1.nCol;
                    SCROW nRows = rRef.Ref2.nRow - rRef.Ref1.nRow;
                    SCTAB nTabs = rRef.Ref2.nTab - rRef.Ref1.nTab;
                    if ( rRef.Ref1.IsRelName() || rRef.Ref2.IsRelName() )
                    {
                        ScRefUpdate::MoveRelWrap( pDoc, aPos, MAXCOL, MAXROW, rRef );
                        rChanged = sal_True;
                    }
                    else
                    {
                        if ( ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                    r, nDx, nDy, nDz, rRef )
                                != UR_NOTHING
                            )
                        {
                            rChanged = sal_True;
                            if (rRef.Ref2.nCol - rRef.Ref1.nCol != nCols ||
                                    rRef.Ref2.nRow - rRef.Ref1.nRow != nRows ||
                                    rRef.Ref2.nTab - rRef.Ref1.nTab != nTabs)
                            {
                                rRefSizeChanged = sal_True;
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                                bEasyShared = sal_False;
#endif
                            }
                        }
                    }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
                    if ( bEasyShared )
                    {
                        ScRange aRef( rRef.Ref1.nCol, rRef.Ref1.nRow,
                                rRef.Ref1.nTab, rRef.Ref2.nCol, rRef.Ref2.nRow,
                                rRef.Ref2.nTab );
                        if ( r.In( aRef ) != bPosInRange )
                            bEasyShared = sal_False;
                    }
#endif
                }
            }
        }
#if SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
        if ( pRangeData )
        {
            if ( bEasyShared )
                pRangeData = 0;
            else
                rChanged = sal_True;
        }
#endif
#undef SC_PRESERVE_SHARED_FORMULAS_IF_POSSIBLE
        return pRangeData;
    }
}

sal_Bool ScCompiler::UpdateNameReference(UpdateRefMode eUpdateRefMode,
                                     const ScRange& r,
                                     SCsCOL nDx, SCsROW nDy, SCsTAB nDz,
                                     sal_Bool& rChanged, sal_Bool bSharedFormula)
{
    sal_Bool bRelRef = sal_False;   // set if relative reference
    rChanged = sal_False;
    pArr->Reset();
    ScToken* t;
    while ( (t = static_cast<ScToken*>(pArr->GetNextReference())) != NULL )
    {
        SingleDoubleRefModifier aMod( *t );
        ScComplexRefData& rRef = aMod.Ref();
        bRelRef = rRef.Ref1.IsColRel() || rRef.Ref1.IsRowRel() ||
            rRef.Ref1.IsTabRel();
        if (!bRelRef && t->GetType() == svDoubleRef)
            bRelRef = rRef.Ref2.IsColRel() || rRef.Ref2.IsRowRel() ||
                rRef.Ref2.IsTabRel();
        bool bUpdate = !rRef.Ref1.IsColRel() || !rRef.Ref1.IsRowRel() ||
            !rRef.Ref1.IsTabRel();
        if (!bUpdate && t->GetType() == svDoubleRef)
            bUpdate = !rRef.Ref2.IsColRel() || !rRef.Ref2.IsRowRel() ||
                !rRef.Ref2.IsTabRel();
        if (!bSharedFormula)
        {
            // We cannot update names with sheet-relative references, they may
            // be used on other sheets as well and the resulting reference
            // would be wrong. This is a dilemma if col/row would need to be
            // updated for the current usage.
            // TODO: seems the only way out of this would be to not allow
            // relative sheet references and have sheet-local names that can be
            // copied along with sheets.
            bUpdate = bUpdate && !rRef.Ref1.IsTabRel() && !rRef.Ref2.IsTabRel();
        }
        if (bUpdate)
        {
            rRef.CalcAbsIfRel( aPos);
            if (ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos, r,
                        nDx, nDy, nDz, rRef, ScRefUpdate::ABSOLUTE)
                    != UR_NOTHING )
                rChanged = sal_True;
        }
    }
    return bRelRef;
}


void ScCompiler::UpdateSharedFormulaReference( UpdateRefMode eUpdateRefMode,
                                  const ScAddress& rOldPos, const ScRange& r,
                                  SCsCOL nDx, SCsROW nDy, SCsTAB nDz )
{
    if ( eUpdateRefMode == URM_COPY )
        return ;
    else
    {
        ScToken* t;
        pArr->Reset();
        while ( (t = static_cast<ScToken*>(pArr->GetNextReference())) != NULL )
        {
            if( t->GetType() != svIndex )   // it may be a DB area!!!
            {
                t->CalcAbsIfRel( rOldPos );
                // Absolute references have been already adjusted in the named
                // shared formula itself prior to breaking the shared formula
                // and calling this function. Don't readjust them again.
                SingleDoubleRefModifier aMod( *t );
                ScComplexRefData& rRef = aMod.Ref();
                ScComplexRefData aBkp = rRef;
                ScRefUpdate::Update( pDoc, eUpdateRefMode, aPos,
                                            r, nDx, nDy, nDz, rRef );
                // restore absolute parts
                if ( !aBkp.Ref1.IsColRel() )
                {
                    rRef.Ref1.nCol = aBkp.Ref1.nCol;
                    rRef.Ref1.nRelCol = aBkp.Ref1.nRelCol;
                    rRef.Ref1.SetColDeleted( aBkp.Ref1.IsColDeleted() );
                }
                if ( !aBkp.Ref1.IsRowRel() )
                {
                    rRef.Ref1.nRow = aBkp.Ref1.nRow;
                    rRef.Ref1.nRelRow = aBkp.Ref1.nRelRow;
                    rRef.Ref1.SetRowDeleted( aBkp.Ref1.IsRowDeleted() );
                }
                if ( !aBkp.Ref1.IsTabRel() )
                {
                    rRef.Ref1.nTab = aBkp.Ref1.nTab;
                    rRef.Ref1.nRelTab = aBkp.Ref1.nRelTab;
                    rRef.Ref1.SetTabDeleted( aBkp.Ref1.IsTabDeleted() );
                }
                if ( t->GetType() == svDoubleRef )
                {
                    if ( !aBkp.Ref2.IsColRel() )
                    {
                        rRef.Ref2.nCol = aBkp.Ref2.nCol;
                        rRef.Ref2.nRelCol = aBkp.Ref2.nRelCol;
                        rRef.Ref2.SetColDeleted( aBkp.Ref2.IsColDeleted() );
                    }
                    if ( !aBkp.Ref2.IsRowRel() )
                    {
                        rRef.Ref2.nRow = aBkp.Ref2.nRow;
                        rRef.Ref2.nRelRow = aBkp.Ref2.nRelRow;
                        rRef.Ref2.SetRowDeleted( aBkp.Ref2.IsRowDeleted() );
                    }
                    if ( !aBkp.Ref2.IsTabRel() )
                    {
                        rRef.Ref2.nTab = aBkp.Ref2.nTab;
                        rRef.Ref2.nRelTab = aBkp.Ref2.nRelTab;
                        rRef.Ref2.SetTabDeleted( aBkp.Ref2.IsTabDeleted() );
                    }
                }
            }
        }
    }
}


ScRangeData* ScCompiler::UpdateInsertTab( SCTAB nTable, sal_Bool bIsName )
{
    ScRangeData* pRangeData = NULL;
    SCTAB nPosTab = aPos.Tab();     // _after_ incremented!
    SCTAB nOldPosTab = ((nPosTab > nTable) ? (nPosTab - 1) : nPosTab);
    sal_Bool bIsRel = sal_False;
    ScToken* t;
    pArr->Reset();
    if (bIsName)
        t = static_cast<ScToken*>(pArr->GetNextReference());
    else
        t = static_cast<ScToken*>(pArr->GetNextReferenceOrName());
    while( t )
    {
        if( t->GetOpCode() == ocName )
        {
            if (!bIsName)
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                if (pName && pName->HasType(RT_SHAREDMOD))
                    pRangeData = pName;
            }
        }
        else if( t->GetType() != svIndex )  // it may be a DB area!!!
        {
            if ( !(bIsName && t->GetSingleRef().IsTabRel()) )
            {   // of names only adjust absolute references
                ScSingleRefData& rRef = t->GetSingleRef();
                if ( rRef.IsTabRel() )
                {
                    rRef.nTab = rRef.nRelTab + nOldPosTab;
                    if ( rRef.nTab < 0 )
                        rRef.nTab = sal::static_int_cast<SCsTAB>( rRef.nTab + pDoc->GetTableCount() );  // was a wrap
                }
                if (nTable <= rRef.nTab)
                    ++rRef.nTab;
                rRef.nRelTab = rRef.nTab - nPosTab;
            }
            else
                bIsRel = sal_True;
            if ( t->GetType() == svDoubleRef )
            {
                if ( !(bIsName && t->GetDoubleRef().Ref2.IsTabRel()) )
                {   // of names only adjust absolute references
                    ScSingleRefData& rRef = t->GetDoubleRef().Ref2;
                    if ( rRef.IsTabRel() )
                    {
                        rRef.nTab = rRef.nRelTab + nOldPosTab;
                        if ( rRef.nTab < 0 )
                            rRef.nTab = sal::static_int_cast<SCsTAB>( rRef.nTab + pDoc->GetTableCount() );  // was a wrap
                    }
                    if (nTable <= rRef.nTab)
                        ++rRef.nTab;
                    rRef.nRelTab = rRef.nTab - nPosTab;
                }
                else
                    bIsRel = sal_True;
            }
            if ( bIsName && bIsRel )
                pRangeData = (ScRangeData*) this;   // not dereferenced in rangenam
        }
        if (bIsName)
            t = static_cast<ScToken*>(pArr->GetNextReference());
        else
            t = static_cast<ScToken*>(pArr->GetNextReferenceOrName());
    }
    if ( !bIsName )
    {
        pArr->Reset();
        while ( (t = static_cast<ScToken*>(pArr->GetNextReferenceRPN())) != NULL )
        {
            if ( t->GetRef() == 1 )
            {
                ScSingleRefData& rRef1 = t->GetSingleRef();
                if ( !(rRef1.IsRelName() && rRef1.IsTabRel()) )
                {   // of names only adjust absolute references
                    if ( rRef1.IsTabRel() )
                    {
                        rRef1.nTab = rRef1.nRelTab + nOldPosTab;
                        if ( rRef1.nTab < 0 )
                            rRef1.nTab = sal::static_int_cast<SCsTAB>( rRef1.nTab + pDoc->GetTableCount() );  // was a wrap
                    }
                    if (nTable <= rRef1.nTab)
                        ++rRef1.nTab;
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                }
                if ( t->GetType() == svDoubleRef )
                {
                    ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( !(rRef2.IsRelName() && rRef2.IsTabRel()) )
                    {   // of names only adjust absolute references
                        if ( rRef2.IsTabRel() )
                        {
                            rRef2.nTab = rRef2.nRelTab + nOldPosTab;
                            if ( rRef2.nTab < 0 )
                                rRef2.nTab = sal::static_int_cast<SCsTAB>( rRef2.nTab + pDoc->GetTableCount() );  // was a wrap
                        }
                        if (nTable <= rRef2.nTab)
                            ++rRef2.nTab;
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                }
            }
        }
    }
    return pRangeData;
}

ScRangeData* ScCompiler::UpdateDeleteTab(SCTAB nTable, sal_Bool /* bIsMove */, sal_Bool bIsName,
                                 sal_Bool& rChanged)
{
    ScRangeData* pRangeData = NULL;
    SCTAB nTab, nTab2;
    SCTAB nPosTab = aPos.Tab();          // _after_ decremented!
    SCTAB nOldPosTab = ((nPosTab >= nTable) ? (nPosTab + 1) : nPosTab);
    rChanged = sal_False;
    sal_Bool bIsRel = sal_False;
    ScToken* t;
    pArr->Reset();
    if (bIsName)
        t = static_cast<ScToken*>(pArr->GetNextReference());
    else
        t = static_cast<ScToken*>(pArr->GetNextReferenceOrName());
    while( t )
    {
        if( t->GetOpCode() == ocName )
        {
            if (!bIsName)
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                if (pName && pName->HasType(RT_SHAREDMOD))
                    pRangeData = pName;
            }
            rChanged = sal_True;
        }
        else if( t->GetType() != svIndex )  // it may be a DB area!!!
        {
            if ( !(bIsName && t->GetSingleRef().IsTabRel()) )
            {   // of names only adjust absolute references
                ScSingleRefData& rRef = t->GetSingleRef();
                if ( rRef.IsTabRel() )
                    nTab = rRef.nTab = rRef.nRelTab + nOldPosTab;
                else
                    nTab = rRef.nTab;
                if ( nTable < nTab )
                {
                    rRef.nTab = nTab - 1;
                    rChanged = sal_True;
                }
                else if ( nTable == nTab )
                {
                    if ( t->GetType() == svDoubleRef )
                    {
                        ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                        if ( rRef2.IsTabRel() )
                            nTab2 = rRef2.nRelTab + nOldPosTab;
                        else
                            nTab2 = rRef2.nTab;
                        if ( nTab == nTab2
                          || (nTab+1) >= pDoc->GetTableCount() )
                        {
                            rRef.nTab = MAXTAB+1;
                            rRef.SetTabDeleted( sal_True );
                        }
                        // else: nTab later points to what's nTable+1 now
                        // => area shrunk
                    }
                    else
                    {
                        rRef.nTab = MAXTAB+1;
                        rRef.SetTabDeleted( sal_True );
                    }
                    rChanged = sal_True;
                }
                rRef.nRelTab = rRef.nTab - nPosTab;
            }
            else
                bIsRel = sal_True;
            if ( t->GetType() == svDoubleRef )
            {
                if ( !(bIsName && t->GetDoubleRef().Ref2.IsTabRel()) )
                {   // of names only adjust absolute references
                    ScSingleRefData& rRef = t->GetDoubleRef().Ref2;
                    if ( rRef.IsTabRel() )
                        nTab = rRef.nTab = rRef.nRelTab + nOldPosTab;
                    else
                        nTab = rRef.nTab;
                    if ( nTable < nTab )
                    {
                        rRef.nTab = nTab - 1;
                        rChanged = sal_True;
                    }
                    else if ( nTable == nTab )
                    {
                        if ( !t->GetDoubleRef().Ref1.IsTabDeleted() )
                            rRef.nTab = nTab - 1;   // shrink area
                        else
                        {
                            rRef.nTab = MAXTAB+1;
                            rRef.SetTabDeleted( sal_True );
                        }
                        rChanged = sal_True;
                    }
                    rRef.nRelTab = rRef.nTab - nPosTab;
                }
                else
                    bIsRel = sal_True;
            }
            if ( bIsName && bIsRel )
                pRangeData = (ScRangeData*) this;   // not dereferenced in rangenam
        }
        if (bIsName)
            t = static_cast<ScToken*>(pArr->GetNextReference());
        else
            t = static_cast<ScToken*>(pArr->GetNextReferenceOrName());
    }
    if ( !bIsName )
    {
        pArr->Reset();
        while ( (t = static_cast<ScToken*>(pArr->GetNextReferenceRPN())) != NULL )
        {
            if ( t->GetRef() == 1 )
            {
                ScSingleRefData& rRef1 = t->GetSingleRef();
                if ( !(rRef1.IsRelName() && rRef1.IsTabRel()) )
                {   // of names only adjust absolute references
                    if ( rRef1.IsTabRel() )
                        nTab = rRef1.nTab = rRef1.nRelTab + nOldPosTab;
                    else
                        nTab = rRef1.nTab;
                    if ( nTable < nTab )
                    {
                        rRef1.nTab = nTab - 1;
                        rChanged = sal_True;
                    }
                    else if ( nTable == nTab )
                    {
                        if ( t->GetType() == svDoubleRef )
                        {
                            ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                            if ( rRef2.IsTabRel() )
                                nTab2 = rRef2.nRelTab + nOldPosTab;
                            else
                                nTab2 = rRef2.nTab;
                            if ( nTab == nTab2
                              || (nTab+1) >= pDoc->GetTableCount() )
                            {
                                rRef1.nTab = MAXTAB+1;
                                rRef1.SetTabDeleted( sal_True );
                            }
                            // else: nTab later points to what's nTable+1 now
                            // => area shrunk
                        }
                        else
                        {
                            rRef1.nTab = MAXTAB+1;
                            rRef1.SetTabDeleted( sal_True );
                        }
                        rChanged = sal_True;
                    }
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                }
                if ( t->GetType() == svDoubleRef )
                {
                    ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( !(rRef2.IsRelName() && rRef2.IsTabRel()) )
                    {   // of names only adjust absolute references
                        if ( rRef2.IsTabRel() )
                            nTab = rRef2.nTab = rRef2.nRelTab + nOldPosTab;
                        else
                            nTab = rRef2.nTab;
                        if ( nTable < nTab )
                        {
                            rRef2.nTab = nTab - 1;
                            rChanged = sal_True;
                        }
                        else if ( nTable == nTab )
                        {
                            if ( !rRef1.IsTabDeleted() )
                                rRef2.nTab = nTab - 1;  // shrink area
                            else
                            {
                                rRef2.nTab = MAXTAB+1;
                                rRef2.SetTabDeleted( sal_True );
                            }
                            rChanged = sal_True;
                        }
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                }
            }
        }
    }
    return pRangeData;
}

// aPos.Tab() must be already adjusted!
ScRangeData* ScCompiler::UpdateMoveTab( SCTAB nOldTab, SCTAB nNewTab,
        sal_Bool bIsName )
{
    ScRangeData* pRangeData = NULL;
    SCsTAB nTab;

    SCTAB nStart, nEnd;
    short nDir;                         // direction in which others move
    if ( nOldTab < nNewTab )
    {
        nDir = -1;
        nStart = nOldTab;
        nEnd = nNewTab;
    }
    else
    {
        nDir = 1;
        nStart = nNewTab;
        nEnd = nOldTab;
    }
    SCTAB nPosTab = aPos.Tab();        // current sheet
    SCTAB nOldPosTab;                  // previously it was this one
    if ( nPosTab == nNewTab )
        nOldPosTab = nOldTab;           // look, it's me!
    else if ( nPosTab < nStart || nEnd < nPosTab )
        nOldPosTab = nPosTab;           // wasn't moved
    else
        nOldPosTab = nPosTab - nDir;    // moved by one

    sal_Bool bIsRel = sal_False;
    ScToken* t;
    pArr->Reset();
    if (bIsName)
        t = static_cast<ScToken*>(pArr->GetNextReference());
    else
        t = static_cast<ScToken*>(pArr->GetNextReferenceOrName());
    while( t )
    {
        if( t->GetOpCode() == ocName )
        {
            if (!bIsName)
            {
                ScRangeData* pName = pDoc->GetRangeName()->FindIndex(t->GetIndex());
                if (pName && pName->HasType(RT_SHAREDMOD))
                    pRangeData = pName;
            }
        }
        else if( t->GetType() != svIndex )  // it may be a DB area!!!
        {
            ScSingleRefData& rRef1 = t->GetSingleRef();
            if ( !(bIsName && rRef1.IsTabRel()) )
            {   // of names only adjust absolute references
                if ( rRef1.IsTabRel() )
                    nTab = rRef1.nRelTab + nOldPosTab;
                else
                    nTab = rRef1.nTab;
                if ( nTab == nOldTab )
                    rRef1.nTab = nNewTab;
                else if ( nStart <= nTab && nTab <= nEnd )
                    rRef1.nTab = nTab + nDir;
                rRef1.nRelTab = rRef1.nTab - nPosTab;
            }
            else
                bIsRel = sal_True;
            if ( t->GetType() == svDoubleRef )
            {
                ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                if ( !(bIsName && rRef2.IsTabRel()) )
                {   // of names only adjust absolute references
                    if ( rRef2.IsTabRel() )
                        nTab = rRef2.nRelTab + nOldPosTab;
                    else
                        nTab = rRef2.nTab;
                    if ( nTab == nOldTab )
                        rRef2.nTab = nNewTab;
                    else if ( nStart <= nTab && nTab <= nEnd )
                        rRef2.nTab = nTab + nDir;
                    rRef2.nRelTab = rRef2.nTab - nPosTab;
                }
                else
                    bIsRel = sal_True;
                SCsTAB nTab1, nTab2;
                if ( rRef1.IsTabRel() )
                    nTab1 = rRef1.nRelTab + nPosTab;
                else
                    nTab1 = rRef1.nTab;
                if ( rRef2.IsTabRel() )
                    nTab2 = rRef2.nRelTab + nPosTab;
                else
                    nTab2 = rRef1.nTab;
                if ( nTab2 < nTab1 )
                {   // PutInOrder
                    rRef1.nTab = nTab2;
                    rRef2.nTab = nTab1;
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                    rRef2.nRelTab = rRef2.nTab - nPosTab;
                }
            }
            if ( bIsName && bIsRel )
                pRangeData = (ScRangeData*) this;   // not dereferenced in rangenam
        }
        if (bIsName)
            t = static_cast<ScToken*>(pArr->GetNextReference());
        else
            t = static_cast<ScToken*>(pArr->GetNextReferenceOrName());
    }
    if ( !bIsName )
    {
        SCsTAB nMaxTabMod = (SCsTAB) pDoc->GetTableCount();
        pArr->Reset();
        while ( (t = static_cast<ScToken*>(pArr->GetNextReferenceRPN())) != NULL )
        {
            if ( t->GetRef() == 1 )
            {
                ScSingleRefData& rRef1 = t->GetSingleRef();
                if ( rRef1.IsRelName() && rRef1.IsTabRel() )
                {   // possibly wrap RelName, like lcl_MoveItWrap in refupdat.cxx
                    nTab = rRef1.nRelTab + nPosTab;
                    if ( nTab < 0 )
                        nTab = sal::static_int_cast<SCsTAB>( nTab + nMaxTabMod );
                    else if ( nTab > nMaxTab )
                        nTab = sal::static_int_cast<SCsTAB>( nTab - nMaxTabMod );
                    rRef1.nRelTab = nTab - nPosTab;
                }
                else
                {
                    if ( rRef1.IsTabRel() )
                        nTab = rRef1.nRelTab + nOldPosTab;
                    else
                        nTab = rRef1.nTab;
                    if ( nTab == nOldTab )
                        rRef1.nTab = nNewTab;
                    else if ( nStart <= nTab && nTab <= nEnd )
                        rRef1.nTab = nTab + nDir;
                    rRef1.nRelTab = rRef1.nTab - nPosTab;
                }
                if( t->GetType() == svDoubleRef )
                {
                    ScSingleRefData& rRef2 = t->GetDoubleRef().Ref2;
                    if ( rRef2.IsRelName() && rRef2.IsTabRel() )
                    {   // possibly wrap RelName, like lcl_MoveItWrap in refupdat.cxx
                        nTab = rRef2.nRelTab + nPosTab;
                        if ( nTab < 0 )
                            nTab = sal::static_int_cast<SCsTAB>( nTab + nMaxTabMod );
                        else if ( nTab > nMaxTab )
                            nTab = sal::static_int_cast<SCsTAB>( nTab - nMaxTabMod );
                        rRef2.nRelTab = nTab - nPosTab;
                    }
                    else
                    {
                        if ( rRef2.IsTabRel() )
                            nTab = rRef2.nRelTab + nOldPosTab;
                        else
                            nTab = rRef2.nTab;
                        if ( nTab == nOldTab )
                            rRef2.nTab = nNewTab;
                        else if ( nStart <= nTab && nTab <= nEnd )
                            rRef2.nTab = nTab + nDir;
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                    SCsTAB nTab1, nTab2;
                    if ( rRef1.IsTabRel() )
                        nTab1 = rRef1.nRelTab + nPosTab;
                    else
                        nTab1 = rRef1.nTab;
                    if ( rRef2.IsTabRel() )
                        nTab2 = rRef2.nRelTab + nPosTab;
                    else
                        nTab2 = rRef1.nTab;
                    if ( nTab2 < nTab1 )
                    {   // PutInOrder
                        rRef1.nTab = nTab2;
                        rRef2.nTab = nTab1;
                        rRef1.nRelTab = rRef1.nTab - nPosTab;
                        rRef2.nRelTab = rRef2.nTab - nPosTab;
                    }
                }
            }
        }
    }
    return pRangeData;
}


void ScCompiler::CreateStringFromExternal(rtl::OUStringBuffer& rBuffer, FormulaToken* pTokenP)
{
    FormulaToken* t = pTokenP;
    ScExternalRefManager* pRefMgr = pDoc->GetExternalRefManager();
    switch (t->GetType())
    {
	    case svExternalName:
	    {
    		const String *pStr = pRefMgr->getExternalFileName(t->GetIndex());
    	    String aFileName = pStr ? *pStr : ScGlobal::GetRscString(STR_NO_NAME_REF);
	        rBuffer.append(pConv->makeExternalNameStr( aFileName, t->GetString()));
        }
        break;
        case svExternalSingleRef:
            pConv->makeExternalRefStr(
                   rBuffer, *this, t->GetIndex(), t->GetString(), static_cast<ScToken*>(t)->GetSingleRef(), pRefMgr);
        break;
        case svExternalDoubleRef:
            pConv->makeExternalRefStr(
                        rBuffer, *this, t->GetIndex(), t->GetString(), static_cast<ScToken*>(t)->GetDoubleRef(), pRefMgr);
   		break;
        default:
            // warning, not error, otherwise we may end up with a never
            // ending message box loop if this was the cursor cell to be redrawn.
            DBG_WARNING("ScCompiler::CreateStringFromToken: unknown type of ocExternalRef");
	}
}

void ScCompiler::CreateStringFromMatrix( rtl::OUStringBuffer& rBuffer,
                                           FormulaToken* pTokenP)
{
    const ScMatrix* pMatrix = static_cast<ScToken*>(pTokenP)->GetMatrix();
    SCSIZE nC, nMaxC, nR, nMaxR;

    pMatrix->GetDimensions( nMaxC, nMaxR);

    rBuffer.append( mxSymbols->getSymbol(ocArrayOpen) );
    for( nR = 0 ; nR < nMaxR ; nR++)
    {
        if( nR > 0)
        {
            rBuffer.append( mxSymbols->getSymbol(ocArrayRowSep) );
        }

        for( nC = 0 ; nC < nMaxC ; nC++)
        {
            if( nC > 0)
            {
                rBuffer.append( mxSymbols->getSymbol(ocArrayColSep) );
            }

            if( pMatrix->IsValue( nC, nR ) )
            {
                ScMatValType nType;
                const ScMatrixValue* pVal = pMatrix->Get( nC, nR, nType);

                if( nType == SC_MATVAL_BOOLEAN )
                    AppendBoolean( rBuffer, pVal->GetBoolean() );
                else
                {
                    sal_uInt16 nErr = pVal->GetError();
                    if( nErr )
                        rBuffer.append( ScGlobal::GetErrorString( nErr ) );
                    else
                        AppendDouble( rBuffer, pVal->fVal );
                }
            }
            else if( pMatrix->IsEmpty( nC, nR ) )
                ;
            else if( pMatrix->IsString( nC, nR ) )
                AppendString( rBuffer, pMatrix->GetString( nC, nR ) );
        }
    }
    rBuffer.append( mxSymbols->getSymbol(ocArrayClose) );
}

void ScCompiler::CreateStringFromSingleRef(rtl::OUStringBuffer& rBuffer,FormulaToken* _pTokenP)
{
    const OpCode eOp = _pTokenP->GetOpCode();
    ScSingleRefData& rRef = static_cast<ScToken*>(_pTokenP)->GetSingleRef();
    ScComplexRefData aRef;
    aRef.Ref1 = aRef.Ref2 = rRef;
    if ( eOp == ocColRowName )
    {
        rRef.CalcAbsIfRel( aPos );
        if ( pDoc->HasStringData( rRef.nCol, rRef.nRow, rRef.nTab ) )
        {
            String aStr;
            pDoc->GetString( rRef.nCol, rRef.nRow, rRef.nTab, aStr );
            EnQuote( aStr );
            rBuffer.append(aStr);
        }
        else
        {
            rBuffer.append(ScGlobal::GetRscString(STR_NO_NAME_REF));
            pConv->MakeRefStr (rBuffer, *this, aRef, sal_True );
        }
    }
    else
        pConv->MakeRefStr( rBuffer, *this, aRef, sal_True );
}
// -----------------------------------------------------------------------------
void ScCompiler::CreateStringFromDoubleRef(rtl::OUStringBuffer& rBuffer,FormulaToken* _pTokenP)
{
    pConv->MakeRefStr( rBuffer, *this, static_cast<ScToken*>(_pTokenP)->GetDoubleRef(), sal_False );
}
// -----------------------------------------------------------------------------
void ScCompiler::CreateStringFromIndex(rtl::OUStringBuffer& rBuffer,FormulaToken* _pTokenP)
{
    const OpCode eOp = _pTokenP->GetOpCode();
    rtl::OUStringBuffer aBuffer;
    switch ( eOp )
    {
		case ocName:
        {
            ScRangeData* pData = pDoc->GetRangeName()->FindIndex(_pTokenP->GetIndex());
            if (pData)
            {
                if (pData->HasType(RT_SHARED))
                    pData->UpdateSymbol( aBuffer, aPos, GetGrammar());
                else
                    aBuffer.append(pData->GetName());
            }
        }
        break;
        case ocDBArea:
        {
            ScDBData* pDBData = pDoc->GetDBCollection()->FindIndex(_pTokenP->GetIndex());
            if (pDBData)
                aBuffer.append(pDBData->GetName());
        }
        break;
        default:
            ;   // nothing
    }
    if ( aBuffer.getLength() )
        rBuffer.append(aBuffer);
    else
        rBuffer.append(ScGlobal::GetRscString(STR_NO_NAME_REF));
}
// -----------------------------------------------------------------------------
void ScCompiler::LocalizeString( String& rName )
{
    ScGlobal::GetAddInCollection()->LocalizeString( rName );
}
// -----------------------------------------------------------------------------
sal_Bool ScCompiler::IsImportingXML() const
{
    return pDoc->IsImportingXML();
}

// Put quotes around string if non-alphanumeric characters are contained,
// quote characters contained within are escaped by '\\'.
sal_Bool ScCompiler::EnQuote( String& rStr )
{
    sal_Int32 nType = ScGlobal::pCharClass->getStringType( rStr, 0, rStr.Len() );
    if ( !CharClass::isNumericType( nType )
            && CharClass::isAlphaNumericType( nType ) )
        return sal_False;

    xub_StrLen nPos = 0;
    while ( (nPos = rStr.Search( '\'', nPos)) != STRING_NOTFOUND )
    {
        rStr.Insert( '\\', nPos );
        nPos += 2;
    }
    rStr.Insert( '\'', 0 );
    rStr += '\'';
    return sal_True;
}

sal_Unicode ScCompiler::GetNativeAddressSymbol( Convention::SpecialSymbolType eType ) const
{
    return pConv->getSpecialSymbol(eType);
}

void ScCompiler::fillAddInToken(::std::vector< ::com::sun::star::sheet::FormulaOpCodeMapEntry >& _rVec,bool _bIsEnglish) const
{
    // All known AddIn functions.
    sheet::FormulaOpCodeMapEntry aEntry;
    aEntry.Token.OpCode = ocExternal;

    ScUnoAddInCollection* pColl = ScGlobal::GetAddInCollection();
    const long nCount = pColl->GetFuncCount();
    for (long i=0; i < nCount; ++i)
    {
        const ScUnoAddInFuncData* pFuncData = pColl->GetFuncData(i);
        if (pFuncData)
        {
            if ( _bIsEnglish )
            {
                String aName;
                if (pFuncData->GetExcelName( LANGUAGE_ENGLISH_US, aName))
                    aEntry.Name = aName;
                else
                    aEntry.Name = pFuncData->GetUpperName();
            }
            else
                aEntry.Name = pFuncData->GetUpperLocal();
            aEntry.Token.Data <<= ::rtl::OUString( pFuncData->GetOriginalName());
            _rVec.push_back( aEntry);
        }
    }
    // FIXME: what about those old non-UNO AddIns?
}
// -----------------------------------------------------------------------------
sal_Bool ScCompiler::HandleSingleRef()
{
    ScSingleRefData& rRef = static_cast<ScToken*>((FormulaToken*)pToken)->GetSingleRef();
    rRef.CalcAbsIfRel( aPos );
    if ( !rRef.Valid() )
    {
        SetError( errNoRef );
        return sal_True;
    }
    SCCOL nCol = rRef.nCol;
    SCROW nRow = rRef.nRow;
    SCTAB nTab = rRef.nTab;
    ScAddress aLook( nCol, nRow, nTab );
    sal_Bool bColName = rRef.IsColRel();
    SCCOL nMyCol = aPos.Col();
    SCROW nMyRow = aPos.Row();
    sal_Bool bInList = sal_False;
    sal_Bool bValidName = sal_False;
    ScRangePairList* pRL = (bColName ?
        pDoc->GetColNameRanges() : pDoc->GetRowNameRanges());
    ScRange aRange;
    for ( ScRangePair* pR = pRL->First(); pR; pR = pRL->Next() )
    {
        if ( pR->GetRange(0).In( aLook ) )
        {
            bInList = bValidName = sal_True;
            aRange = pR->GetRange(1);
            if ( bColName )
            {
                aRange.aStart.SetCol( nCol );
                aRange.aEnd.SetCol( nCol );
            }
            else
            {
                aRange.aStart.SetRow( nRow );
                aRange.aEnd.SetRow( nRow );
            }
            break;  // for
        }
    }
    if ( !bInList && pDoc->GetDocOptions().IsLookUpColRowNames() )
    {   // automagically or created by copying and NamePos isn't in list
        sal_Bool bString = pDoc->HasStringData( nCol, nRow, nTab );
        if ( !bString && !pDoc->GetCell( aLook ) )
            bString = sal_True;     // empty cell is ok
        if ( bString )
        {   //! coresponds with ScInterpreter::ScColRowNameAuto()
            bValidName = sal_True;
            if ( bColName )
            {   // ColName
                SCROW nStartRow = nRow + 1;
                if ( nStartRow > MAXROW )
                    nStartRow = MAXROW;
                SCROW nMaxRow = MAXROW;
                if ( nMyCol == nCol )
                {   // formula cell in same column
                    if ( nMyRow == nStartRow )
                    {   // take remainder under name cell
                        nStartRow++;
                        if ( nStartRow > MAXROW )
                            nStartRow = MAXROW;
                    }
                    else if ( nMyRow > nStartRow )
                    {   // from name cell down to formula cell
                        nMaxRow = nMyRow - 1;
                    }
                }
                for ( ScRangePair* pR = pRL->First(); pR; pR = pRL->Next() )
                {   // next defined ColNameRange below limits row
                    const ScRange& rRange = pR->GetRange(1);
                    if ( rRange.aStart.Col() <= nCol && nCol <= rRange.aEnd.Col() )
                    {   // identical column range
                        SCROW nTmp = rRange.aStart.Row();
                        if ( nStartRow < nTmp && nTmp <= nMaxRow )
                            nMaxRow = nTmp - 1;
                    }
                }
                aRange.aStart.Set( nCol, nStartRow, nTab );
                aRange.aEnd.Set( nCol, nMaxRow, nTab );
            }
            else
            {   // RowName
                SCCOL nStartCol = nCol + 1;
                if ( nStartCol > MAXCOL )
                    nStartCol = MAXCOL;
                SCCOL nMaxCol = MAXCOL;
                if ( nMyRow == nRow )
                {   // formula cell in same row
                    if ( nMyCol == nStartCol )
                    {   // take remainder right from name cell
                        nStartCol++;
                        if ( nStartCol > MAXCOL )
                            nStartCol = MAXCOL;
                    }
                    else if ( nMyCol > nStartCol )
                    {   // from name cell right to formula cell
                        nMaxCol = nMyCol - 1;
                    }
                }
                for ( ScRangePair* pR = pRL->First(); pR; pR = pRL->Next() )
                {   // next defined RowNameRange to the right limits column
                    const ScRange& rRange = pR->GetRange(1);
                    if ( rRange.aStart.Row() <= nRow && nRow <= rRange.aEnd.Row() )
                    {   // identical row range
                        SCCOL nTmp = rRange.aStart.Col();
                        if ( nStartCol < nTmp && nTmp <= nMaxCol )
                            nMaxCol = nTmp - 1;
                    }
                }
                aRange.aStart.Set( nStartCol, nRow, nTab );
                aRange.aEnd.Set( nMaxCol, nRow, nTab );
            }
        }
    }
    if ( bValidName )
    {
        // And now the magic to distinguish between a range and a single
        // cell thereof, which is picked position-dependent of the formula
        // cell. If a direct neighbor is a binary operator (ocAdd, ...) a
        // SingleRef matching the column/row of the formula cell is
        // generated. A ocColRowName or ocIntersect as a neighbor results
        // in a range. Special case: if label is valid for a single cell, a
        // position independent SingleRef is generated.
        sal_Bool bSingle = (aRange.aStart == aRange.aEnd);
        sal_Bool bFound;
        if ( bSingle )
            bFound = sal_True;
        else
        {
            FormulaToken* p1 = pArr->PeekPrevNoSpaces();
            FormulaToken* p2 = pArr->PeekNextNoSpaces();
            // begin/end of a formula => single
            OpCode eOp1 = p1 ? p1->GetOpCode() : static_cast<OpCode>( ocAdd );
            OpCode eOp2 = p2 ? p2->GetOpCode() : static_cast<OpCode>( ocAdd );
            if ( eOp1 != ocColRowName && eOp1 != ocIntersect
                && eOp2 != ocColRowName && eOp2 != ocIntersect )
            {
                if (    (SC_OPCODE_START_BIN_OP <= eOp1 && eOp1 < SC_OPCODE_STOP_BIN_OP) ||
                        (SC_OPCODE_START_BIN_OP <= eOp2 && eOp2 < SC_OPCODE_STOP_BIN_OP))
                    bSingle = sal_True;
            }
            if ( bSingle )
            {   // column and/or row must match range
                if ( bColName )
                {
                    bFound = (aRange.aStart.Row() <= nMyRow
                        && nMyRow <= aRange.aEnd.Row());
                    if ( bFound )
                        aRange.aStart.SetRow( nMyRow );
                }
                else
                {
                    bFound = (aRange.aStart.Col() <= nMyCol
                        && nMyCol <= aRange.aEnd.Col());
                    if ( bFound )
                        aRange.aStart.SetCol( nMyCol );
                }
            }
            else
                bFound = sal_True;
        }
        if ( !bFound )
            SetError(errNoRef);
        else if ( !bCompileForFAP )
        {
            ScTokenArray* pNew = new ScTokenArray();
            if ( bSingle )
            {
                ScSingleRefData aRefData;
                aRefData.InitAddress( aRange.aStart );
                if ( bColName )
                    aRefData.SetColRel( sal_True );
                else
                    aRefData.SetRowRel( sal_True );
                aRefData.CalcRelFromAbs( aPos );
                pNew->AddSingleReference( aRefData );
            }
            else
            {
                ScComplexRefData aRefData;
                aRefData.InitRange( aRange );
                if ( bColName )
                {
                    aRefData.Ref1.SetColRel( sal_True );
                    aRefData.Ref2.SetColRel( sal_True );
                }
                else
                {
                    aRefData.Ref1.SetRowRel( sal_True );
                    aRefData.Ref2.SetRowRel( sal_True );
                }
                aRefData.CalcRelFromAbs( aPos );
                if ( bInList )
                    pNew->AddDoubleReference( aRefData );
                else
                {   // automagically
                    pNew->Add( new ScDoubleRefToken( aRefData, ocColRowNameAuto ) );
                }
            }
            PushTokenArray( pNew, sal_True );
            pNew->Reset();
            return GetToken();
        }
    }
    else
        SetError(errNoName);
    return sal_True;
}
// -----------------------------------------------------------------------------
sal_Bool ScCompiler::HandleDbData()
{
    ScDBData* pDBData = pDoc->GetDBCollection()->FindIndex( pToken->GetIndex() );
    if ( !pDBData )
        SetError(errNoName);
    else if ( !bCompileForFAP )
    {
        ScComplexRefData aRefData;
        aRefData.InitFlags();
        pDBData->GetArea(   (SCTAB&) aRefData.Ref1.nTab,
                            (SCCOL&) aRefData.Ref1.nCol,
                            (SCROW&) aRefData.Ref1.nRow,
                            (SCCOL&) aRefData.Ref2.nCol,
                            (SCROW&) aRefData.Ref2.nRow);
        aRefData.Ref2.nTab    = aRefData.Ref1.nTab;
        aRefData.CalcRelFromAbs( aPos );
        ScTokenArray* pNew = new ScTokenArray();
        pNew->AddDoubleReference( aRefData );
        PushTokenArray( pNew, sal_True );
        pNew->Reset();
        return GetToken();
    }
    return sal_True;
}

String GetScCompilerNativeSymbol( OpCode eOp )
{
    return ScCompiler::GetNativeSymbol( eOp );
}
// -----------------------------------------------------------------------------
FormulaTokenRef ScCompiler::ExtendRangeReference( FormulaToken & rTok1, FormulaToken & rTok2, bool bReuseDoubleRef )
{
    return ScToken::ExtendRangeReference( rTok1, rTok2, aPos,bReuseDoubleRef );
}
