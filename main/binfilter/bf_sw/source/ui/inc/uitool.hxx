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



#ifndef _UITOOL_HXX
#define _UITOOL_HXX

#include <tools/wintypes.hxx>
#ifndef _SV_FIELD_HXX
#include <vcl/field.hxx>
#endif

/*N*/ #include <tools/debug.hxx> //for stripping
class Printer; 
class ListBox; 
class DateTime; 
namespace binfilter {
class SfxItemSet; 
class MetricFormatter;
class SfxMedium;
class SwPageDesc;

class SvxTabStopItem;
class SwWrtShell;

class SwDocShell;
class SwFrmFmt;
class SwTabCols;


// Umschalten einer Metric

// BoxInfoAttribut fuellen

// SfxItemSets <-> PageDesc
void ItemSetToPageDesc( const SfxItemSet& rSet, SwPageDesc& rPageDesc );
void PageDescToItemSet( const SwPageDesc& rPageDesc, SfxItemSet& rSet);

} //namespace binfilter
#endif // _UITOOL_HXX
