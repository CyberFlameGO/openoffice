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

#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <rtl/uuid.h>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/accessibility/AccessibleRelation.hpp>
#include <com/sun/star/accessibility/AccessibleRelationType.hpp>
#include <com/sun/star/accessibility/XAccessibleRelationSet.hpp>
#include <com/sun/star/accessibility/AccessibleEventId.hpp>
#include <unotools/accessiblestatesethelper.hxx>
#include <frmfmt.hxx>
#include <flyfrm.hxx>
#include <accmap.hxx>
#include <unotools/accessiblerelationsethelper.hxx>
// --> OD 2009-07-14 #i73249#
#include <hints.hxx>
// <--
#include "acctextframe.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::accessibility;
using ::rtl::OUString;

using utl::AccessibleRelationSetHelper;
using ::com::sun::star::accessibility::XAccessibleContext;

const sal_Char sServiceName[] = "com.sun.star.text.AccessibleTextFrameView";
const sal_Char sImplementationName[] = "com.sun.star.comp.Writer.SwAccessibleTextFrameView";

SwAccessibleTextFrame::SwAccessibleTextFrame(
        SwAccessibleMap* pInitMap,
        const SwFlyFrm* pFlyFrm  ) :
    SwAccessibleFrameBase( pInitMap, AccessibleRole::TEXT_FRAME, pFlyFrm ),
    msTitle(),
    msDesc()
{
    if ( pFlyFrm )
    {
        const SwFlyFrmFmt* pFlyFrmFmt =
                        dynamic_cast<const SwFlyFrmFmt*>( pFlyFrm->GetFmt() );
        msTitle = pFlyFrmFmt->GetObjTitle();

        msDesc = pFlyFrmFmt->GetObjDescription();
        if ( msDesc.getLength() == 0 &&
             msTitle != GetName() )
        {
            msDesc = msTitle;
        }
    }
}

SwAccessibleTextFrame::~SwAccessibleTextFrame()
{
}

void SwAccessibleTextFrame::Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew)
{
    const sal_uInt16 nWhich = pOld ? pOld->Which() : pNew ? pNew->Which() : 0 ;
    // --> OD 2009-07-14 #i73249#
    // suppress handling of RES_NAME_CHANGED in case that attribute Title is
    // used as the accessible name.
    if ( nWhich != RES_NAME_CHANGED ||
         msTitle.getLength() == 0 )
    {
        SwAccessibleFrameBase::Modify( pOld, pNew );
    }

    const SwFlyFrm *pFlyFrm = static_cast< const SwFlyFrm * >( GetFrm() );
	switch( nWhich )
	{
        // --> OD 2009-07-14 #i73249#
        case RES_TITLE_CHANGED:
        {
            const String& sOldTitle(
                        dynamic_cast<const SwStringMsgPoolItem*>(pOld)->GetString() );
            const String& sNewTitle(
                        dynamic_cast<const SwStringMsgPoolItem*>(pNew)->GetString() );
            if ( sOldTitle == sNewTitle )
            {
                break;
            }
            msTitle = sNewTitle;
            AccessibleEventObject aEvent;
            aEvent.EventId = AccessibleEventId::NAME_CHANGED;
            aEvent.OldValue <<= OUString( sOldTitle );
            aEvent.NewValue <<= msTitle;
            FireAccessibleEvent( aEvent );

            const SwFlyFrmFmt* pFlyFrmFmt =
                            dynamic_cast<const SwFlyFrmFmt*>( pFlyFrm->GetFmt() );
            if ( pFlyFrmFmt->GetObjDescription().Len() != 0 )
            {
                break;
            }
        }
        // intentional no break here
        case RES_DESCRIPTION_CHANGED:
        {
            if ( pFlyFrm )
            {
                const OUString sOldDesc( msDesc );

                const SwFlyFrmFmt* pFlyFrmFmt =
                                dynamic_cast<const SwFlyFrmFmt*>( pFlyFrm->GetFmt() );
                const String& rDesc = pFlyFrmFmt->GetObjDescription();
                msDesc = rDesc;
                if ( msDesc.getLength() == 0 &&
                     msTitle != GetName() )
                {
                    msDesc = msTitle;
                }

                if ( msDesc != sOldDesc )
                {
                    AccessibleEventObject aEvent;
                    aEvent.EventId = AccessibleEventId::DESCRIPTION_CHANGED;
                    aEvent.OldValue <<= sOldDesc;
                    aEvent.NewValue <<= msDesc;
                    FireAccessibleEvent( aEvent );
                }
            }
        }
        break;
        // <--
	}
}

// --> OD 2009-07-14 #i73249#
OUString SAL_CALL SwAccessibleTextFrame::getAccessibleName (void)
        throw (uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    if ( msTitle.getLength() != 0 )
    {
        return msTitle;
    }

    return SwAccessibleFrameBase::getAccessibleName();
}
// <--
OUString SAL_CALL SwAccessibleTextFrame::getAccessibleDescription (void)
        throw (uno::RuntimeException)
{
	vos::OGuard aGuard(Application::GetSolarMutex());

    CHECK_FOR_DEFUNC( XAccessibleContext )

    return msDesc;

}

OUString SAL_CALL SwAccessibleTextFrame::getImplementationName()
        throw( uno::RuntimeException )
{
	return OUString(RTL_CONSTASCII_USTRINGPARAM(sImplementationName));
}

sal_Bool SAL_CALL SwAccessibleTextFrame::supportsService(
        const OUString& sTestServiceName)
    throw (uno::RuntimeException)
{
	return sTestServiceName.equalsAsciiL( sServiceName,
										  sizeof(sServiceName)-1 ) ||
		   sTestServiceName.equalsAsciiL( sAccessibleServiceName,
				   						  sizeof(sAccessibleServiceName)-1 );
}

uno::Sequence< OUString > SAL_CALL SwAccessibleTextFrame::getSupportedServiceNames()
		throw( uno::RuntimeException )
{
	uno::Sequence< OUString > aRet(2);
	OUString* pArray = aRet.getArray();
	pArray[0] = OUString( RTL_CONSTASCII_USTRINGPARAM(sServiceName) );
	pArray[1] = OUString( RTL_CONSTASCII_USTRINGPARAM(sAccessibleServiceName) );
	return aRet;
}

uno::Sequence< sal_Int8 > SAL_CALL SwAccessibleTextFrame::getImplementationId()
		throw(uno::RuntimeException)
{
    vos::OGuard aGuard(Application::GetSolarMutex());
    static uno::Sequence< sal_Int8 > aId( 16 );
    static sal_Bool bInit = sal_False;
    if(!bInit)
    {
        rtl_createUuid( (sal_uInt8 *)(aId.getArray() ), 0, sal_True );
        bInit = sal_True;
    }
    return aId;
}


//
// XAccessibleRelationSet
//


SwFlyFrm* SwAccessibleTextFrame::getFlyFrm() const
{
	SwFlyFrm* pFlyFrm = NULL;

	const SwFrm* pFrm = GetFrm();
	DBG_ASSERT( pFrm != NULL, "frame expected" );
	if( pFrm->IsFlyFrm() )
	{
		pFlyFrm = static_cast<SwFlyFrm*>( const_cast<SwFrm*>( pFrm ) );
	}

	return pFlyFrm;
}

AccessibleRelation SwAccessibleTextFrame::makeRelation( sal_Int16 nType, const SwFlyFrm* pFrm )
{
    uno::Sequence<uno::Reference<XInterface> > aSequence(1);
    aSequence[0] = GetMap()->GetContext( pFrm );
	return AccessibleRelation( nType, aSequence );
}


uno::Reference<XAccessibleRelationSet> SAL_CALL SwAccessibleTextFrame::getAccessibleRelationSet( )
    throw ( uno::RuntimeException )
{
	vos::OGuard aGuard(Application::GetSolarMutex());
    CHECK_FOR_DEFUNC( XAccessibleContext );

    // get the frame, and insert prev/next relations into helper

    AccessibleRelationSetHelper* pHelper = new AccessibleRelationSetHelper();

	SwFlyFrm* pFlyFrm = getFlyFrm();
	DBG_ASSERT( pFlyFrm != NULL, "fly frame expected" );

	const SwFlyFrm* pPrevFrm = pFlyFrm->GetPrevLink();
    if( pPrevFrm != NULL )
        pHelper->AddRelation( makeRelation(
            AccessibleRelationType::CONTENT_FLOWS_FROM, pPrevFrm ) );

	const SwFlyFrm* pNextFrm = pFlyFrm->GetNextLink();
    if( pNextFrm != NULL )
        pHelper->AddRelation( makeRelation(
            AccessibleRelationType::CONTENT_FLOWS_TO, pNextFrm ) );

	return pHelper;
}
