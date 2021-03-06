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


#include <bf_svtools/optionsdlg.hxx>

#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#include <hash_map>
#include "itemholder1.hxx"

using namespace utl;
using namespace rtl;
using namespace com::sun::star::beans ;
using namespace com::sun::star::uno;

namespace binfilter
{

#define CFG_FILENAME            OUString( RTL_CONSTASCII_USTRINGPARAM( "Office.OptionsDialog" ) )
#define ROOT_NODE               OUString( RTL_CONSTASCII_USTRINGPARAM( "OptionsDialogGroups" ) )
#define PAGES_NODE              OUString( RTL_CONSTASCII_USTRINGPARAM( "Pages" ) )
#define OPTIONS_NODE            OUString( RTL_CONSTASCII_USTRINGPARAM( "Options" ) )
#define PROPERTY_HIDE           OUString( RTL_CONSTASCII_USTRINGPARAM( "Hide" ) )

static SvtOptionsDlgOptions_Impl*   pOptions = NULL;
static sal_Int32                    nRefCount = 0;

class SvtOptionsDlgOptions_Impl : public utl::ConfigItem
{
private:
    struct OUStringHashCode
    {
        size_t operator()( const ::rtl::OUString& sString ) const
        {
            return sString.hashCode();
        }
    };

    typedef std::hash_map< OUString, sal_Bool, OUStringHashCode, ::std::equal_to< OUString > > OptionNodeList;

    OUString        m_sPathDelimiter;
    OptionNodeList  m_aOptionNodeList;

    enum NodeType{ NT_Group, NT_Page, NT_Option };
    void            ReadNode( const OUString& _rNode, NodeType _eType );

public:
                    SvtOptionsDlgOptions_Impl();

    virtual void    Notify( const com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
    virtual void    Commit();

    static ::osl::Mutex & getInitMutex();
};

::osl::Mutex & SvtOptionsDlgOptions_Impl::getInitMutex()
{
    static ::osl::Mutex *pMutex = 0;

    if( ! pMutex )
    {
        ::osl::MutexGuard guard( ::osl::Mutex::getGlobalMutex() );
        if( ! pMutex )
        {
            static ::osl::Mutex mutex;
            pMutex = &mutex;
        }
    }
    return *pMutex;
}

// -----------------------------------------------------------------------

SvtOptionsDlgOptions_Impl::SvtOptionsDlgOptions_Impl()
    : ConfigItem( OUString( CFG_FILENAME ) ),

    m_sPathDelimiter( RTL_CONSTASCII_USTRINGPARAM( "/" ) ),
    m_aOptionNodeList( OptionNodeList() )

{
    OUString sRootNode( ROOT_NODE );
    Sequence< OUString > aNodeSeq = GetNodeNames( sRootNode );
    OUString sNode( sRootNode + m_sPathDelimiter );
    sal_uInt32 nCount = aNodeSeq.getLength();
    for ( sal_uInt32 n = 0; n < nCount; n++ )
    {
        OUString sSubNode( sNode + aNodeSeq[n] );
        ReadNode( sSubNode, NT_Group );
    }
}

// -----------------------------------------------------------------------

void SvtOptionsDlgOptions_Impl::Commit()
{
    // nothing to commit
}

// -----------------------------------------------------------------------

void SvtOptionsDlgOptions_Impl::Notify( const Sequence< rtl::OUString >& )
{
    // nothing to notify
}

void SvtOptionsDlgOptions_Impl::ReadNode( const OUString& _rNode, NodeType _eType )
{
    OUString sNode( _rNode + m_sPathDelimiter );
    OUString sSet;
    sal_Int32 nLen = 0;
    switch ( _eType )
    {
        case NT_Group :
        {
            sSet = PAGES_NODE;
            nLen = 2;
            break;
        }

        case NT_Page :
        {
            sSet = OPTIONS_NODE;
            nLen = 2;
            break;
        }

        case NT_Option :
        {
            nLen = 1;
            break;
        }
    }

    Sequence< OUString > lResult( nLen );
    lResult[0] = OUString( sNode + PROPERTY_HIDE );
    if ( _eType != NT_Option )
        lResult[1] = OUString( sNode + sSet );

    Sequence< Any > aValues;
    aValues = GetProperties( lResult );
    sal_Bool bHide = sal_False;
    if ( aValues[0] >>= bHide )
        m_aOptionNodeList.insert( OptionNodeList::value_type( sNode, bHide ) );

    if ( _eType != NT_Option )
    {
        OUString sNodes( sNode + sSet );
        Sequence< OUString > aNodes = GetNodeNames( sNodes );
        if ( aNodes.getLength() > 0 )
        {
            for ( sal_uInt32 n = 0; n < (sal_uInt32)aNodes.getLength(); ++n )
            {
                OUString sSubNodeName( sNodes + m_sPathDelimiter + aNodes[n] );
                ReadNode( sSubNodeName, _eType == NT_Group ? NT_Page : NT_Option );
            }
        }
    }
}

// -----------------------------------------------------------------------

SvtOptionsDialogOptions::SvtOptionsDialogOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( SvtOptionsDlgOptions_Impl::getInitMutex() );
    ++nRefCount;
    if ( !pOptions )
    {
        pOptions = new SvtOptionsDlgOptions_Impl;

        ItemHolder1::holdConfigItem( E_OPTIONSDLGOPTIONS );
    }
    m_pImp = pOptions;
}

// -----------------------------------------------------------------------

SvtOptionsDialogOptions::~SvtOptionsDialogOptions()
{
    // Global access, must be guarded (multithreading)
    ::osl::MutexGuard aGuard( SvtOptionsDlgOptions_Impl::getInitMutex() );
    if ( !--nRefCount )
	{
		if ( pOptions->IsModified() )
			pOptions->Commit();
        DELETEZ( pOptions );
	}
}

}
