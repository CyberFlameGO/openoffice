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


#ifndef _SYLAYITM_HXX
#define _SYLAYITM_HXX

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif

#ifndef _SFXSTRITEM_HXX //autogen
#include <bf_svtools/stritem.hxx>
#endif

#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif

#ifndef _SVDTYPES_HXX //autogen
#include <bf_svx/svdtypes.hxx>
#endif
namespace binfilter {

//------------------------------
// class SdrLayerIdItem
//------------------------------
class SdrLayerIdItem: public SfxUInt16Item {
public:
	SdrLayerIdItem(USHORT nId=0): SfxUInt16Item(SDRATTR_LAYERID,nId)  {}
	SdrLayerIdItem(SvStream& rIn): SfxUInt16Item(SDRATTR_LAYERID,rIn) {}
	SdrLayerID GetValue() const { return (SdrLayerID)SfxUInt16Item::GetValue(); }
};

//------------------------------
// class SdrLayerNameItem
//------------------------------

class SdrLayerNameItem: public SfxStringItem {
public:
	SdrLayerNameItem()                   : SfxStringItem() { SetWhich(SDRATTR_LAYERNAME); }
	SdrLayerNameItem(const String& rStr) : SfxStringItem(SDRATTR_LAYERNAME,rStr) {}
	SdrLayerNameItem(SvStream& rIn)      : SfxStringItem(SDRATTR_LAYERNAME,rIn)  {}
};

}//end of namespace binfilter
#endif
