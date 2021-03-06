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


#ifndef _DRAWDOC_HXX
#define _DRAWDOC_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FM_FMMODEL_HXX
#include <bf_svx/fmmodel.hxx>
#endif
class Window; 
class SbxValue; 
class SbxObject; 
namespace binfilter {

class SwDoc;
class SwDocShell;
class SjJSbxObject;


//==================================================================

class SwDrawDocument : public FmFormModel
{
	SwDoc* pDoc;
public:
	SwDrawDocument( SwDoc* pDoc );
    SwDrawDocument( SfxItemPool *pPool, SwDocShell *pDocSh );
	~SwDrawDocument();

	const SwDoc& GetDoc() const	{ return *pDoc; }
		  SwDoc& GetDoc()      	{ return *pDoc; }

	virtual SdrPage* AllocPage(FASTBOOL bMasterPage);

	// fuers "load on demand" von Grafiken im DrawingLayer
	virtual SvStream* GetDocumentStream( SdrDocumentStreamInfo& rInfo ) const;

	// fuers Speicher von Rechtecken als Control-Ersatz fuker Versionen < 5.0
};


} //namespace binfilter
#endif
