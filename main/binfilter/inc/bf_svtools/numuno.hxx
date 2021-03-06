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


#ifndef _NUMUNO_HXX
#define _NUMUNO_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATSSUPPLIER_HPP_
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif

namespace binfilter
{

class SvNumberFormatter;
class SvNumFmtSuppl_Impl;

//------------------------------------------------------------------

//	SvNumberFormatterServiceObj must be registered as service somewhere

com::sun::star::uno::Reference<com::sun::star::uno::XInterface> SAL_CALL
	SvNumberFormatterServiceObj_NewInstance(
		const com::sun::star::uno::Reference<
			com::sun::star::lang::XMultiServiceFactory>& rSMgr );

//------------------------------------------------------------------

//	SvNumberFormatsSupplierObj: aggregate to document,
//	construct with SvNumberFormatter

class  SvNumberFormatsSupplierObj : public cppu::WeakAggImplHelper2<
									com::sun::star::util::XNumberFormatsSupplier,
									com::sun::star::lang::XUnoTunnel>
{
private:
	SvNumFmtSuppl_Impl*	pImpl;

public:
								SvNumberFormatsSupplierObj();
								SvNumberFormatsSupplierObj(SvNumberFormatter* pForm);
	virtual						~SvNumberFormatsSupplierObj();

	void						SetNumberFormatter(SvNumberFormatter* pNew);
	SvNumberFormatter*			GetNumberFormatter() const;

								// ueberladen, um Attribute im Dokument anzupassen
	virtual void				NumberFormatDeleted(sal_uInt32 nKey);
								// ueberladen, um evtl. neu zu formatieren
	virtual void				SettingsChanged();

								// XNumberFormatsSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL
								getNumberFormatSettings()
									throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormats > SAL_CALL
								getNumberFormats()
									throw(::com::sun::star::uno::RuntimeException);

								// XUnoTunnel
	virtual sal_Int64 SAL_CALL	getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier )
										throw(::com::sun::star::uno::RuntimeException);

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static SvNumberFormatsSupplierObj* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::util::XNumberFormatsSupplier> xObj );
};

}

#endif // #ifndef _NUMUNO_HXX


