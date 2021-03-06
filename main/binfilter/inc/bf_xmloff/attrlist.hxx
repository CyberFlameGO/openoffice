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



#ifndef _XMLOFF_ATTRLIST_HXX
#define _XMLOFF_ATTRLIST_HXX

#ifndef _COM_SUN_STAR_UTIL_XCLONEABLE_HPP_
#include <com/sun/star/util/XCloneable.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#include <cppuhelper/implbase3.hxx>
namespace binfilter {

struct SvXMLAttributeList_Impl;

class SvXMLAttributeList : public ::cppu::WeakImplHelper3<
		::com::sun::star::xml::sax::XAttributeList,
		::com::sun::star::util::XCloneable,
		::com::sun::star::lang::XUnoTunnel>
{
	SvXMLAttributeList_Impl *m_pImpl;

public:
	SvXMLAttributeList();
	SvXMLAttributeList( const SvXMLAttributeList& );
	SvXMLAttributeList( const ::com::sun::star::uno::Reference< 
		::com::sun::star::xml::sax::XAttributeList> & rAttrList );
	~SvXMLAttributeList();

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();
	static SvXMLAttributeList* getImplementation( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ) throw();

	// XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);			 

	// ::com::sun::star::xml::sax::XAttributeList
	virtual sal_Int16 SAL_CALL getLength(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::rtl::OUString SAL_CALL getNameByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::rtl::OUString SAL_CALL getTypeByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::rtl::OUString SAL_CALL getTypeByName(const ::rtl::OUString& aName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::rtl::OUString SAL_CALL getValueByIndex(sal_Int16 i) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::rtl::OUString SAL_CALL getValueByName(const ::rtl::OUString& aName) throw( ::com::sun::star::uno::RuntimeException );

	// ::com::sun::star::util::XCloneable
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone()	throw( ::com::sun::star::uno::RuntimeException );

	// methods that are not contained in any interface
	void AddAttribute( const ::rtl::OUString &sName , const ::rtl::OUString &sValue );
	void Clear();
	void RemoveAttribute( const ::rtl::OUString sName );
	void AppendAttributeList( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & );

 private:
    const ::rtl::OUString sType; // "CDATA"
};

}//end of namespace binfilter
#endif	//  _XMLOFF_ATTRLIST_HXX
