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



#ifndef _TOKEN_HXX
#define _TOKEN_HXX

#include "scanner.hxx"
#ifndef _SBDEF_HXX
#include <basic/sbdef.hxx>
#endif

#if defined( SHARED )
#define SbiTokenSHAREDTMPUNDEF
#undef SHARED
#endif

// Der Tokenizer ist stand-alone, d.h. er kann von ueberallher verwendet
// werden. Eine BASIC-Instanz ist fuer Fehlermeldungen notwendig. Ohne
// BASIC werden die Fehler nur gezaehlt. Auch ist Basic notwendig, wenn
// eine erweiterte SBX-Variable zur Erkennung von Datentypen etc. verwendet
// werden soll.

enum SbiToken {
	NIL = 0,
	// Token zwischen 0x20 und 0x3F sind Literale:
	LPAREN = '(', RPAREN = ')', COMMA = ',', DOT = '.', EXCLAM = '!',
	HASH = '#', SEMICOLON = ';',

	// Anweisungen:
	FIRSTKWD = 0x40,
	AS = FIRSTKWD, ALIAS, ASSIGN,
	CALL, CASE, CLOSE, COMPARE, _CONST_,
	DECLARE, DIM, DO,

	// in der Reihenfolge der Datentyp-Enums!
	DEFINT, DEFLNG, DEFSNG, DEFDBL, DEFCUR, DEFDATE, DEFSTR, DEFOBJ,
	DEFERR, DEFBOOL, DEFVAR,
	// in der Reihenfolge der Datentyp-Enums!
	DATATYPE1,
	TINTEGER = DATATYPE1,
	TLONG, TSINGLE, TDOUBLE, TCURRENCY, TDATE, TSTRING, TOBJECT,
	_ERROR_, TBOOLEAN, TVARIANT, TBYTE,
	DATATYPE2 = TBYTE,

	EACH, ELSE,	ELSEIF, END, ERASE, EXIT,
	FOR, FUNCTION,
	GET, GLOBAL, GOSUB, GOTO,
	IF, _IN_, INPUT,
	LET, LINE, LINEINPUT, LOCAL, LOOP, LPRINT, LSET,
	NAME, NEW, NEXT,
	ON, OPEN, OPTION, IMPLEMENTS,
	PRINT, PRIVATE, PROPERTY, PUBLIC,
	REDIM, REM, RESUME, RETURN, RSET,
	SELECT, SET, SHARED, STATIC, STEP, STOP, SUB,
	TEXT, THEN, TO, TYPE, ENUM,
	UNTIL,
	WEND, WHILE, WITH, WRITE,
	ENDENUM, ENDIF, ENDFUNC, ENDPROPERTY, ENDSUB, ENDTYPE, ENDSELECT, ENDWITH,
	// Ende aller Keywords
	LASTKWD = ENDWITH,
	// Statement-Ende
	EOS, EOLN,
	// Operatoren:
	EXPON, NEG, MUL,
	DIV, IDIV, MOD, PLUS, MINUS,
	EQ, NE, LT, GT, LE, GE,
	NOT, AND, OR, XOR, EQV,
	IMP, CAT, LIKE, IS, TYPEOF,
	// Sonstiges:
	FIRSTEXTRA,
	NUMBER=FIRSTEXTRA, FIXSTRING, SYMBOL, _CDECL_, BYVAL, BYREF,
	OUTPUT, RANDOM, APPEND, BINARY, ACCESS,
	LOCK, READ, PRESERVE, BASE, ANY, LIB, _OPTIONAL_,
	EXPLICIT, COMPATIBLE, CLASSMODULE, PARAMARRAY, WITHEVENTS,

	// Ab hier kommen JavaScript-Tokens (gleiches enum, damit gleicher Typ)
	FIRSTJAVA,
	JS_BREAK=FIRSTJAVA, JS_CONTINUE, JS_FOR, JS_FUNCTION, JS_IF, JS_NEW,
	JS_RETURN, JS_THIS, JS_VAR, JS_WHILE, JS_WITH,

	// JavaScript-Operatoren
	// _ASS_ = Assignment
	JS_COMMA, JS_ASSIGNMENT, JS_ASS_PLUS, JS_ASS_MINUS, JS_ASS_MUL,
	JS_ASS_DIV, JS_ASS_MOD, JS_ASS_LSHIFT, JS_ASS_RSHIFT, JS_ASS_RSHIFT_Z,
	JS_ASS_AND, JS_ASS_XOR, JS_ASS_OR,
	JS_COND_QUEST, JS_COND_SEL, JS_LOG_OR, JS_LOG_AND, JS_BIT_OR,
	JS_BIT_XOR, JS_BIT_AND,	JS_EQ, JS_NE, JS_LT, JS_LE,
	JS_GT, JS_GE, JS_LSHIFT, JS_RSHIFT, JS_RSHIFT_Z,
	JS_PLUS, JS_MINUS, JS_MUL, JS_DIV, JS_MOD, JS_LOG_NOT, JS_BIT_NOT,
	JS_INC, JS_DEC, JS_LPAREN, JS_RPAREN, JS_LINDEX, JS_RINDEX
	, VBASUPPORT
};

#ifdef SbiTokenSHAREDTMPUNDEF
#define SHARED
#undef SbiTokenSHAREDTMPUNDEF
#endif

// #i109076
class TokenLabelInfo
{
	bool* m_pTokenCanBeLabelTab;

public:
	TokenLabelInfo( void );
	TokenLabelInfo( const TokenLabelInfo& rInfo )
		: m_pTokenCanBeLabelTab( NULL )
			{ (void)rInfo; }
	~TokenLabelInfo();

	bool canTokenBeLabel( SbiToken eTok )
		{ return m_pTokenCanBeLabelTab[eTok]; }
};

class SbiTokenizer : public SbiScanner {
	TokenLabelInfo	m_aTokenLabelInfo;

protected:
	SbiToken eCurTok;				// aktuelles Token
	SbiToken ePush;					// Pushback-Token
	sal_uInt16  nPLine, nPCol1, nPCol2; // Pushback-Location
	sal_Bool bEof;						// sal_True bei Dateiende
	sal_Bool bEos;						// sal_True bei Statement-Ende
	sal_Bool bKeywords;					// sal_True, falls Keywords geparst werden
	sal_Bool bAs;						// letztes Keyword war AS
	sal_Bool bErrorIsSymbol;			// Handle Error token as Symbol, not keyword
public:
    SbiTokenizer( const ::rtl::OUString&, StarBASIC* = NULL );
   ~SbiTokenizer();

	inline sal_Bool IsEof()				{ return bEof; }
	inline sal_Bool IsEos()				{ return bEos; }

	void  Push( SbiToken ); 			// Pushback eines Tokens
	const String& Symbol( SbiToken );// Rueckumwandlung

	SbiToken Peek();				 	// das naechste Token lesen
	SbiToken Next();				  	// Ein Token lesen
	sal_Bool MayBeLabel( sal_Bool= sal_False );	// Kann es ein Label sein?

	void Hilite( SbTextPortions& );	// Syntax-Highlighting

	void Error( SbError c ) { GenError( c ); }
	void Error( SbError, SbiToken );
	void Error( SbError, const char* );
	void Error( SbError, String );

	void Keywords( sal_Bool b ) { bKeywords = b; }

	static sal_Bool IsEoln( SbiToken t )
		{ return sal_Bool( t == EOS || t == EOLN || t == REM ); }
	static sal_Bool IsKwd( SbiToken t )
		{ return sal_Bool( t >= FIRSTKWD && t <= LASTKWD ); }
	static sal_Bool IsExtra( SbiToken t )
		{ return sal_Bool( t >= FIRSTEXTRA ); }
};


#endif
