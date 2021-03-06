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



#ifndef _SVX_UNONAMEITEMTABLE_HXX_
#define _SVX_UNONAMEITEMTABLE_HXX_

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif

#include <cppuhelper/implbase2.hxx>

#include <vector>

#ifndef _SFXLSTNER_HXX 
#include <bf_svtools/lstner.hxx>
#endif

#ifndef _SVX_XIT_HXX
#include "xit.hxx"
#endif

namespace binfilter {

class SfxItemPool;
class SfxItemSet;
class SdrModel;

typedef std::vector< SfxItemSet* > ItemPoolVector;
class SvxUnoNameItemTable : public cppu::WeakImplHelper2< ::com::sun::star::container::XNameContainer, ::com::sun::star::lang::XServiceInfo >,
							public SfxListener
{
private:
	SdrModel*		mpModel;
	SfxItemPool*	mpModelPool;
	USHORT			mnWhich;
	BYTE			mnMemberId;

	ItemPoolVector maItemSetVector;

	void SAL_CALL ImplInsertByName( const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement );

public:
	SvxUnoNameItemTable( SdrModel* pModel, USHORT nWhich, BYTE nMemberId = 0 ) throw();
	virtual	~SvxUnoNameItemTable() throw();

	virtual NameOrIndex* createItem() const throw() = 0;
	void dispose();

	// SfxListener
	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) throw ();

    // XServiceInfo
    virtual sal_Bool SAL_CALL supportsService( const  ::rtl::OUString& ServiceName ) throw( ::com::sun::star::uno::RuntimeException);

	// XNameContainer
	virtual void SAL_CALL insertByName( const  ::rtl::OUString& aName, const  ::com::sun::star::uno::Any& aElement ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::ElementExistException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL removeByName( const  ::rtl::OUString& Name ) throw( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	// XNameReplace
    virtual void SAL_CALL replaceByName( const  ::rtl::OUString& aName, const  ::com::sun::star::uno::Any& aElement ) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	// XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const  ::rtl::OUString& aName ) throw( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence<  ::rtl::OUString > SAL_CALL getElementNames(  ) throw( ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasByName( const  ::rtl::OUString& aName ) throw( ::com::sun::star::uno::RuntimeException);

	// XElementAccess
    virtual sal_Bool SAL_CALL hasElements(  ) throw( ::com::sun::star::uno::RuntimeException);
};

}//end of namespace binfilter
#endif // _SVX_UNONAMEITEMTABLE_HXX_
