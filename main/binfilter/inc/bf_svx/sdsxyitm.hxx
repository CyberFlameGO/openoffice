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


#ifndef _SDSXYITM_HXX
#define _SDSXYITM_HXX

#ifndef _SDMETITM_HXX
#include <bf_svx/sdmetitm.hxx>
#endif

#ifndef _SVDDEF_HXX //autogen
#include <bf_svx/svddef.hxx>
#endif
namespace binfilter {

//---------------------
// class SdrShadowXDistItem
//---------------------
class SdrShadowXDistItem: public SdrMetricItem {
public:
	SdrShadowXDistItem(long nDist=0) : SdrMetricItem(SDRATTR_SHADOWXDIST,nDist) {}
	SdrShadowXDistItem(SvStream& rIn): SdrMetricItem(SDRATTR_SHADOWXDIST,rIn)   {}
};

//---------------------
// class SdrShadowYDistItem
//---------------------
class SdrShadowYDistItem: public SdrMetricItem {
public:
	SdrShadowYDistItem(long nDist=0) : SdrMetricItem(SDRATTR_SHADOWYDIST,nDist) {}
	SdrShadowYDistItem(SvStream& rIn): SdrMetricItem(SDRATTR_SHADOWYDIST,rIn)   {}
};

}//end of namespace binfilter
#endif
