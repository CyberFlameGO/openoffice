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


#ifndef _SVX_RECTENUM_HXX
#define _SVX_RECTENUM_HXX
namespace binfilter {

// enum ------------------------------------------------------------------

enum RECT_POINT
{
	RP_LT, RP_MT, RP_RT, RP_LM, RP_MM, RP_RM, RP_LB, RP_MB, RP_RB
};

enum CTL_STYLE
{
	CS_RECT,
	CS_LINE,
	CS_ANGLE,
	CS_SHADOW
};

}//end of namespace binfilter
#endif
