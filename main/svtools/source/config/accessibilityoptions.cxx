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
#include "precompiled_svtools.hxx"

#include <svtools/accessibilityoptions.hxx>
#include "configitems/accessibilityoptions_const.hxx"

#include <unotools/configmgr.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
#ifndef _COMPHELPER_CONFIGURATIONHELPER_HXX_
#include <comphelper/configurationhelper.hxx>
#endif
#ifndef _UNOTOOLS_PROCESSFACTORY_HXX_
#include <unotools/processfactory.hxx>
#endif
#ifndef _SVT_LOGHELPER_HXX_
#include <unotools/loghelper.hxx>
#endif

#include <svl/smplhint.hxx>

#include <vcl/settings.hxx>
#include <vcl/svapp.hxx>
#include <rtl/instance.hxx>

#include <itemholder2.hxx>

using namespace utl;
using namespace rtl;
using namespace com::sun::star::uno;
namespace css = com::sun::star;

#define HELP_TIP_TIMEOUT 0xffff     // max. timeout setting to pretend a non-timeout


// class SvtAccessibilityOptions_Impl ---------------------------------------------

class SvtAccessibilityOptions_Impl
{
private:
	css::uno::Reference< css::container::XNameAccess > m_xCfg;
	sal_Bool										   bIsModified;

public:
	SvtAccessibilityOptions_Impl();
	~SvtAccessibilityOptions_Impl();

	void		SetVCLSettings();
	sal_Bool	GetAutoDetectSystemHC();
	sal_Bool	GetIsForPagePreviews() const;
	sal_Bool	GetIsHelpTipsDisappear() const;
	sal_Bool	GetIsAllowAnimatedGraphics() const;
	sal_Bool	GetIsAllowAnimatedText() const;
	sal_Bool	GetIsAutomaticFontColor() const;
	sal_Bool	GetIsSystemFont() const;
	sal_Int16	GetHelpTipSeconds() const;
	sal_Bool	IsSelectionInReadonly() const;

	void		SetAutoDetectSystemHC(sal_Bool bSet);
	void		SetIsForPagePreviews(sal_Bool bSet);
	void		SetIsHelpTipsDisappear(sal_Bool bSet);
	void		SetIsAllowAnimatedGraphics(sal_Bool bSet);
	void		SetIsAllowAnimatedText(sal_Bool bSet);
	void		SetIsAutomaticFontColor(sal_Bool bSet);
	void		SetIsSystemFont(sal_Bool bSet);
	void		SetHelpTipSeconds(sal_Int16 nSet);
	void		SetSelectionInReadonly(sal_Bool bSet);

	sal_Bool	IsModified() const { return bIsModified; };
};

// initialization of static members --------------------------------------

SvtAccessibilityOptions_Impl* volatile 	SvtAccessibilityOptions::sm_pSingleImplConfig =NULL;
sal_Int32					  volatile 	SvtAccessibilityOptions::sm_nAccessibilityRefCount(0);

namespace
{
	struct SingletonMutex
		: public rtl::Static< ::osl::Mutex, SingletonMutex > {};
}

// -----------------------------------------------------------------------
// class SvtAccessibilityOptions_Impl ---------------------------------------------

SvtAccessibilityOptions_Impl::SvtAccessibilityOptions_Impl()
{
	try
	{
		m_xCfg = css::uno::Reference< css::container::XNameAccess >(
			::comphelper::ConfigurationHelper::openConfig(
			utl::getProcessServiceFactory(),
			s_sAccessibility,
			::comphelper::ConfigurationHelper::E_STANDARD),
			css::uno::UNO_QUERY);

		bIsModified = sal_False;
	}
	catch(const css::uno::Exception& ex)
	{
		m_xCfg.clear();
		LogHelper::logIt(ex);
	}
}

SvtAccessibilityOptions_Impl::~SvtAccessibilityOptions_Impl()
{
}

// -----------------------------------------------------------------------
sal_Bool SvtAccessibilityOptions_Impl::GetAutoDetectSystemHC()
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_True;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sAutoDetectSystemHC) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Bool SvtAccessibilityOptions_Impl::GetIsForPagePreviews() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_True;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsForPagePreviews) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
	return bRet;
}

sal_Bool SvtAccessibilityOptions_Impl::GetIsHelpTipsDisappear() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_True;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsHelpTipsDisappear) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Bool SvtAccessibilityOptions_Impl::GetIsAllowAnimatedGraphics() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_True;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsAllowAnimatedGraphics) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Bool SvtAccessibilityOptions_Impl::GetIsAllowAnimatedText() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_True;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsAllowAnimatedText) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Bool SvtAccessibilityOptions_Impl::GetIsAutomaticFontColor() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_False;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsAutomaticFontColor) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Bool SvtAccessibilityOptions_Impl::GetIsSystemFont() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_True;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsSystemFont) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

sal_Int16 SvtAccessibilityOptions_Impl::GetHelpTipSeconds() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Int16										nRet = 4;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sHelpTipSeconds) >>= nRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return nRet;
}

sal_Bool SvtAccessibilityOptions_Impl::IsSelectionInReadonly() const
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);
	sal_Bool										bRet = sal_False;

	try
	{
		if(xNode.is())
			xNode->getPropertyValue(s_sIsSelectionInReadonly) >>= bRet;
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}

	return bRet;
}

void SvtAccessibilityOptions_Impl::SetAutoDetectSystemHC(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sAutoDetectSystemHC)!=bSet)
		{
			xNode->setPropertyValue(s_sAutoDetectSystemHC, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetIsForPagePreviews(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsForPagePreviews)!=bSet)
		{
			xNode->setPropertyValue(s_sIsForPagePreviews, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetIsHelpTipsDisappear(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsHelpTipsDisappear)!=bSet)
		{
			xNode->setPropertyValue(s_sIsHelpTipsDisappear, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetIsAllowAnimatedGraphics(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsAllowAnimatedGraphics)!=bSet)
		{
			xNode->setPropertyValue(s_sIsAllowAnimatedGraphics, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetIsAllowAnimatedText(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsAllowAnimatedText)!=bSet)
		{
			xNode->setPropertyValue(s_sIsAllowAnimatedText, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetIsAutomaticFontColor(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsAutomaticFontColor)!=bSet)
		{
			xNode->setPropertyValue(s_sIsAutomaticFontColor, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetIsSystemFont(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsSystemFont)!=bSet)
		{
			xNode->setPropertyValue(s_sIsSystemFont, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetHelpTipSeconds(sal_Int16 nSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sHelpTipSeconds)!=nSet)
		{
			xNode->setPropertyValue(s_sHelpTipSeconds, css::uno::makeAny(nSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetSelectionInReadonly(sal_Bool bSet)
{
	css::uno::Reference< css::beans::XPropertySet > xNode(m_xCfg, css::uno::UNO_QUERY);

	try
	{
		if(xNode.is() && xNode->getPropertyValue(s_sIsSelectionInReadonly)!=bSet)
		{
			xNode->setPropertyValue(s_sIsSelectionInReadonly, css::uno::makeAny(bSet));
			::comphelper::ConfigurationHelper::flush(m_xCfg);

			bIsModified = sal_True;
		}
	}
	catch(const css::uno::Exception& ex)
	{
		LogHelper::logIt(ex);
	}
}

void SvtAccessibilityOptions_Impl::SetVCLSettings()
{
	AllSettings aAllSettings = Application::GetSettings();
	HelpSettings aHelpSettings = aAllSettings.GetHelpSettings();
	aHelpSettings.SetTipTimeout( GetIsHelpTipsDisappear() ? GetHelpTipSeconds() * 1000 : HELP_TIP_TIMEOUT);
	aAllSettings.SetHelpSettings(aHelpSettings);
	if(aAllSettings.GetStyleSettings().GetUseSystemUIFonts() != GetIsSystemFont() )
	{
		StyleSettings aStyleSettings = aAllSettings.GetStyleSettings();
		aStyleSettings.SetUseSystemUIFonts( GetIsSystemFont()  );
		aAllSettings.SetStyleSettings(aStyleSettings);
		Application::MergeSystemSettings( aAllSettings );
	}

	Application::SetSettings(aAllSettings);
}

// -----------------------------------------------------------------------
// class SvtAccessibilityOptions --------------------------------------------------

SvtAccessibilityOptions::SvtAccessibilityOptions()
{
	{
		::osl::MutexGuard aGuard( SingletonMutex::get() );
		if(!sm_pSingleImplConfig)
		{
			sm_pSingleImplConfig = new SvtAccessibilityOptions_Impl;
			ItemHolder2::holdConfigItem(E_ACCESSIBILITYOPTIONS);
		}
		++sm_nAccessibilityRefCount;
	}
	//StartListening( *sm_pSingleImplConfig, sal_True );
}

// -----------------------------------------------------------------------

SvtAccessibilityOptions::~SvtAccessibilityOptions()
{
	//EndListening( *sm_pSingleImplConfig, sal_True );
	::osl::MutexGuard aGuard( SingletonMutex::get() );
	if( !--sm_nAccessibilityRefCount )
	{
		//if( sm_pSingleImplConfig->IsModified() )
		//	sm_pSingleImplConfig->Commit();
		DELETEZ( sm_pSingleImplConfig );
	}
}

// -----------------------------------------------------------------------

void SvtAccessibilityOptions::Notify( SfxBroadcaster&, const SfxHint& rHint )
{
	NotifyListeners(0);
	if ( rHint.IsA(TYPE(SfxSimpleHint)) )
	{
		if ( ((SfxSimpleHint&)rHint).GetId()  == SFX_HINT_ACCESSIBILITY_CHANGED )
			SetVCLSettings();
	}
}

// -----------------------------------------------------------------------

sal_Bool SvtAccessibilityOptions::IsModified() const
{
	return sm_pSingleImplConfig->IsModified();
}
void SvtAccessibilityOptions::Commit()
{
	//sm_pSingleImplConfig->Commit();
}

// -----------------------------------------------------------------------

sal_Bool SvtAccessibilityOptions::GetIsForDrawings() const
{
	DBG_ERROR( "SvtAccessibilityOptions::GetIsForDrawings: is obsolete!" );
    return sal_False;
}
sal_Bool SvtAccessibilityOptions::GetIsForBorders() const
{
	DBG_ERROR( "SvtAccessibilityOptions::GetIsForBorders: is obsolete!" );
    return sal_False;
}
sal_Bool SvtAccessibilityOptions::GetAutoDetectSystemHC() const
{
	return sm_pSingleImplConfig->GetAutoDetectSystemHC();
}
sal_Bool SvtAccessibilityOptions::GetIsForPagePreviews() const
{
	return sm_pSingleImplConfig->GetIsForPagePreviews();
}
sal_Bool SvtAccessibilityOptions::GetIsHelpTipsDisappear() const
{
	return sm_pSingleImplConfig->GetIsHelpTipsDisappear();
}
sal_Bool SvtAccessibilityOptions::GetIsAllowAnimatedGraphics() const
{
	return sm_pSingleImplConfig->GetIsAllowAnimatedGraphics();
}
sal_Bool SvtAccessibilityOptions::GetIsAllowAnimatedText() const
{
	return sm_pSingleImplConfig->GetIsAllowAnimatedText();
}
sal_Bool SvtAccessibilityOptions::GetIsAutomaticFontColor() const
{
	return sm_pSingleImplConfig->GetIsAutomaticFontColor();
}
sal_Bool SvtAccessibilityOptions::GetIsSystemFont() const
{
	return sm_pSingleImplConfig->GetIsSystemFont();
}
sal_Int16 SvtAccessibilityOptions::GetHelpTipSeconds() const
{
	return sm_pSingleImplConfig->GetHelpTipSeconds();
}
sal_Bool SvtAccessibilityOptions::IsSelectionInReadonly() const
{
	return sm_pSingleImplConfig->IsSelectionInReadonly();
}

// -----------------------------------------------------------------------
void SvtAccessibilityOptions::SetAutoDetectSystemHC(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetAutoDetectSystemHC(bSet);
}
void SvtAccessibilityOptions::SetIsForPagePreviews(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetIsForPagePreviews(bSet);
}
void SvtAccessibilityOptions::SetIsHelpTipsDisappear(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetIsHelpTipsDisappear(bSet);
}
void SvtAccessibilityOptions::SetIsAllowAnimatedGraphics(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetIsAllowAnimatedGraphics(bSet);
}
void SvtAccessibilityOptions::SetIsAllowAnimatedText(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetIsAllowAnimatedText(bSet);
}
void SvtAccessibilityOptions::SetIsAutomaticFontColor(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetIsAutomaticFontColor(bSet);
}
void SvtAccessibilityOptions::SetIsSystemFont(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetIsSystemFont(bSet);
}
void SvtAccessibilityOptions::SetHelpTipSeconds(sal_Int16 nSet)
{
	sm_pSingleImplConfig->SetHelpTipSeconds(nSet);
}
void SvtAccessibilityOptions::SetSelectionInReadonly(sal_Bool bSet)
{
	sm_pSingleImplConfig->SetSelectionInReadonly(bSet);
}

void SvtAccessibilityOptions::SetVCLSettings()
{
	sm_pSingleImplConfig->SetVCLSettings();
}
// -----------------------------------------------------------------------
