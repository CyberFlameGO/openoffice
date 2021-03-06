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



#ifndef FORMS_COMPONENT_CLONEABLE_HXX
#define FORMS_COMPONENT_CLONEABLE_HXX

#ifndef _COM_SUN_STAR_UNO_XAGGREGATION_HPP_
#include <com/sun/star/uno/XAggregation.hpp>
#endif
namespace binfilter {

//.........................................................................
namespace frm
{
//.........................................................................

	//====================================================================
	//= OCloneableAggregation
	//====================================================================
	class OCloneableAggregation
	{
	protected:
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation>	m_xAggregate;

	protected:
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation > createAggregateClone( const OCloneableAggregation* _pOriginal );
	};

//.........................................................................
}	// namespace frm
//.........................................................................

}//end of namespace binfilter
#endif // FORMS_COMPONENT_CLONEABLE_HXX

