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


#ifndef INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX
#define INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX

#include "svtools/svtdllapi.h"
#include <unotools/configitem.hxx>
#include <tools/string.hxx>
#include <svl/brdcst.hxx>
#include <svl/lstner.hxx>
#include <unotools/options.hxx>

class SvtAccessibilityOptions_Impl;

class SVT_DLLPUBLIC SvtAccessibilityOptions:
    public utl::detail::Options, private SfxListener
{
private:
	static SvtAccessibilityOptions_Impl* volatile sm_pSingleImplConfig;
	static sal_Int32					 volatile sm_nAccessibilityRefCount;

public:
	SvtAccessibilityOptions();
	virtual ~SvtAccessibilityOptions();

	// get & set config entries
	sal_Bool 	GetIsForDrawings() const;		// obsolete!
    sal_Bool    GetIsForBorders() const;		// obsolete!
    sal_Bool    GetIsForPagePreviews() const;
	sal_Bool 	GetIsHelpTipsDisappear() const;
	sal_Bool 	GetIsAllowAnimatedGraphics() const;
	sal_Bool 	GetIsAllowAnimatedText() const;
	sal_Bool 	GetIsAutomaticFontColor() const;
	sal_Bool 	GetIsSystemFont() const;
	sal_Int16	GetHelpTipSeconds() const;
    sal_Bool    IsSelectionInReadonly() const;
	sal_Bool	GetAutoDetectSystemHC() const;

    void        SetIsForPagePreviews(sal_Bool bSet);
	void 		SetIsHelpTipsDisappear(sal_Bool bSet);
	void 		SetIsAllowAnimatedGraphics(sal_Bool bSet);
	void 		SetIsAllowAnimatedText(sal_Bool bSet);
	void 		SetIsAutomaticFontColor(sal_Bool bSet);
	void 		SetIsSystemFont(sal_Bool bSet);
	void		SetHelpTipSeconds(sal_Int16 nSet);
    void        SetSelectionInReadonly(sal_Bool bSet);
	void        SetAutoDetectSystemHC(sal_Bool bSet);

	sal_Bool                IsModified() const;
	void		            Commit();

	//SfxListener:
	virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
	void 		SetVCLSettings();
};

#endif // #ifndef INCLUDED_SVTOOLS_ACCESSIBILITYOPTIONS_HXX

