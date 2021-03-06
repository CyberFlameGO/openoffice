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


#ifndef _SFXHINT_HXX
#define _SFXHINT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

namespace binfilter
{

class  SfxHint
{
public:
	TYPEINFO();

	virtual ~SfxHint();
};

//--------------------------------------------------------------------

#define DECL_PTRHINT(Visibility, Name, Type) \
		class Visibility Name: public ::binfilter::SfxHint \
        { \
            Type* pObj; \
            BOOL  bIsOwner; \
        \
        public: \
			TYPEINFO(); \
            Name( Type* Object, BOOL bOwnedByHint = FALSE ); \
            ~Name(); \
        \
            Type* GetObject() const { return pObj; } \
            BOOL  IsOwner() const { return bIsOwner; } \
        }

#define IMPL_PTRHINT_AUTODELETE(Name, Type) \
		TYPEINIT1(Name, SfxHint);	\
        Name::Name( Type* pObject, BOOL bOwnedByHint ) \
            { pObj = pObject; bIsOwner = bOwnedByHint; } \
        Name::~Name() { if ( bIsOwner ) delete pObj; }

#define IMPL_PTRHINT(Name, Type) \
		TYPEINIT1(Name, SfxHint);	\
        Name::Name( Type* pObject, BOOL bOwnedByHint ) \
            { pObj = pObject; bIsOwner = bOwnedByHint; } \
        Name::~Name() {}


}

#endif

