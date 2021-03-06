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



#include "sbintern.hxx"
#include "sbunoobj.hxx"
#include "token.hxx"				// Tokenizer
/*?*/ // #include "symtbl.hxx"				// Symbolverwaltung
/*?*/ // #include "parser.hxx"				// Parser
#include "codegen.hxx" 				// Code-Generator
#include "basmgr.hxx"

namespace binfilter {

SV_IMPL_PTRARR(SbErrorStack, SbErrorStackEntry*)

SbiGlobals* GetSbData()
{
    DBG_TESTSOLARMUTEX();
    static SbiGlobals* s_pGlobals = new SbiGlobals;
    return s_pGlobals;
}

SbiGlobals::SbiGlobals()
{
	pInst = NULL;
	pMod  = NULL;
	pSbFac= NULL;
	pUnoFac = NULL;
	pTypeFac = NULL;
	pOLEFac = NULL;
	pCompMod = NULL; // JSM
	nInst = 0;
	nCode = 0;
	nLine = 0;
	nCol1 = nCol2 = 0;
	bCompiler = FALSE;
	bGlobalInitErr = FALSE;
	bRunInit = FALSE;
	eLanguageMode = SB_LANG_BASIC;
	pErrStack = NULL;
    pTransliterationWrapper = NULL;
    bBlockCompilerError = FALSE;
    pAppBasMgr = NULL;
}

SbiGlobals::~SbiGlobals()
{
	delete pErrStack;
	delete pSbFac;
	delete pUnoFac;
    delete pTransliterationWrapper;
}

}
