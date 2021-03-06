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


#ifndef _SCH_ADDINCOLLECTION_HXX_
#define _SCH_ADDINCOLLECTION_HXX_

#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/util/XRefreshable.hpp>
namespace rtl {
	class OUString;
}

namespace binfilter {

//STRIP008 namespace rtl {
//STRIP008 	class OUString;
//STRIP008 }

class SchAddInCollection // : public
{
private:
	::com::sun::star::uno::Sequence< ::rtl::OUString > maServiceNames;
	sal_Bool mbInitialized;

protected:
	void Initialize();

public:
	SchAddInCollection();
	virtual ~SchAddInCollection();

	::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshable > GetAddInByName( const ::rtl::OUString& rName );
	::com::sun::star::uno::Sequence< ::rtl::OUString > GetAddInNames();
};

} //namespace binfilter
#endif	// _SCH_ADDINCOLLECTION_HXX_
