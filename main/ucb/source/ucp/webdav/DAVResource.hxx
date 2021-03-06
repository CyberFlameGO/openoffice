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



#ifndef _DAVRESOURCE_HXX_
#define _DAVRESOURCE_HXX_

#include <vector>

#include "rtl/ustring.hxx"
#include "com/sun/star/uno/Any.hxx"

namespace http_dav_ucp
{

struct DAVPropertyValue
{
	rtl::OUString            Name;  
    com::sun::star::uno::Any Value;
    bool                     IsCaseSensitive;

    DAVPropertyValue() : IsCaseSensitive( true ) {}
};

struct DAVResource
{
	::rtl::OUString uri;
	std::vector< DAVPropertyValue > properties;

	DAVResource() {}
	DAVResource( const ::rtl::OUString & inUri ) : uri( inUri ) {}
};

struct DAVResourceInfo
{
	::rtl::OUString uri;
	std::vector < ::rtl::OUString > properties;

	DAVResourceInfo( const ::rtl::OUString & inUri ) : uri( inUri ) {}
};

} // namespace http_dav_ucp

#endif // _DAVRESOURCE_HXX_
