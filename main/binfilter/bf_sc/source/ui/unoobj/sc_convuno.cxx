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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <i18npool/mslangid.hxx>

#include "convuno.hxx"

#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif

namespace binfilter {

using namespace ::com::sun::star;

//------------------------------------------------------------------------

//	everything is static...

LanguageType ScUnoConversion::GetLanguage( const lang::Locale& rLocale )
{
	//	empty language -> LANGUAGE_SYSTEM
	if ( rLocale.Language.getLength() == 0 )
		return LANGUAGE_SYSTEM;

	LanguageType eRet = MsLangId::convertLocaleToLanguage( rLocale );
	if ( eRet == LANGUAGE_NONE )
		eRet = LANGUAGE_SYSTEM;			//! or throw an exception?

	return eRet;
}

void ScUnoConversion::FillLocale( lang::Locale& rLocale, LanguageType eLang )
{
    MsLangId::convertLanguageToLocale( eLang, rLocale );
}



}
