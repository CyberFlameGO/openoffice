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



#ifndef _SFXSTRITEM_HXX
#define _SFXSTRITEM_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SVTOOLS_CUSTRITM_HXX
#include <bf_svtools/custritm.hxx>
#endif

namespace binfilter
{

//============================================================================
class  SfxStringItem: public CntUnencodedStringItem
{
public:
	TYPEINFO();

	SfxStringItem() {}

	SfxStringItem(USHORT which, const XubString & rValue):
		CntUnencodedStringItem(which, rValue) {}

	SfxStringItem(USHORT nWhich, SvStream & rStream);

	virtual SfxPoolItem * Create(SvStream & rStream, USHORT) const;

	virtual SvStream & Store(SvStream & rStream, USHORT) const;

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const;
};

}

#endif // _SFXSTRITEM_HXX

