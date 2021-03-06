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



#include <iostream>
#include <string.h>
#include <ooxml/resourceids.hxx>
#include "OOXMLFastTokenHandler.hxx"
#include "gperffasttoken.hxx"

namespace writerfilter {
namespace ooxml
{

using namespace ::std;

OOXMLFastTokenHandler::OOXMLFastTokenHandler
(css::uno::Reference< css::uno::XComponentContext > const & context) 
: m_xContext(context)
{}

// ::com::sun::star::xml::sax::XFastTokenHandler:
::sal_Int32 SAL_CALL OOXMLFastTokenHandler::getToken(const ::rtl::OUString & Identifier) 
    throw (css::uno::RuntimeException)
{        
    ::sal_Int32 nResult = OOXML_FAST_TOKENS_END;

    struct tokenmap::token * pToken =
        tokenmap::Perfect_Hash::in_word_set
        (OUStringToOString(Identifier, RTL_TEXTENCODING_ASCII_US).getStr(), 
         Identifier.getLength());

    if (pToken != NULL)
        nResult = pToken->nToken;

#ifdef DEBUG_TOKEN
    clog << "getToken: " 
         << OUStringToOString(Identifier, RTL_TEXTENCODING_ASCII_US).getStr() 
         << ", " << nResult
         << endl;
#endif

    return nResult;
}

::rtl::OUString SAL_CALL OOXMLFastTokenHandler::getIdentifier(::sal_Int32 Token) 
    throw (css::uno::RuntimeException)
{
    ::rtl::OUString sResult;

    if ( Token >= 0 || Token < OOXML_FAST_TOKENS_END )
    {
        static ::rtl::OUString aTokens[OOXML_FAST_TOKENS_END];
        
        if (aTokens[Token].getLength() == 0)
            aTokens[Token] = ::rtl::OUString::createFromAscii
                (tokenmap::wordlist[Token].name);
    }
        
    return sResult;
}

css::uno::Sequence< ::sal_Int8 > SAL_CALL OOXMLFastTokenHandler::getUTF8Identifier(::sal_Int32 Token) 
    throw (css::uno::RuntimeException)
{
	if ( Token < 0  || Token >= OOXML_FAST_TOKENS_END )
		return css::uno::Sequence< ::sal_Int8 >();
		
	return css::uno::Sequence< ::sal_Int8 >(reinterpret_cast< const sal_Int8 *>(tokenmap::wordlist[Token].name), strlen(tokenmap::wordlist[Token].name));
}

::sal_Int32 SAL_CALL OOXMLFastTokenHandler::getTokenFromUTF8
(const css::uno::Sequence< ::sal_Int8 > & Identifier) throw (css::uno::RuntimeException)
{
    ::sal_Int32 nResult = OOXML_FAST_TOKENS_END;
    
    struct tokenmap::token * pToken =
        tokenmap::Perfect_Hash::in_word_set
        (reinterpret_cast<const char *>(Identifier.getConstArray()), 
         Identifier.getLength());

    if (pToken != NULL)
        nResult = pToken->nToken;

#ifdef DEBUG_TOKEN
    clog << "getTokenFromUTF8: " 
         << string(reinterpret_cast<const char *>
                   (Identifier.getConstArray()), Identifier.getLength())
         << ", " << nResult
         << (pToken == NULL ? ", failed" : "") << endl;
#endif

    return nResult;
}

}}
