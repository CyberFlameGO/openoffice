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



#ifndef SC_FUEDIPO_HXX
#define SC_FUEDIPO_HXX

#ifndef _SV_HXX
#endif

#ifndef SC_FUDRAW_HXX
#include "fudraw.hxx"
#endif
namespace binfilter {


/*************************************************************************
|*
|* Basisklasse fuer alle Funktionen
|*
\************************************************************************/

class FuEditPoints : public FuDraw
{
 public:
	FuEditPoints(ScTabViewShell* pViewSh, Window* pWin, SdrView* pView,
		   SdrModel* pDoc, SfxRequest& rReq);

	virtual ~FuEditPoints();
									   // Mouse- & Key-Events
	virtual BOOL KeyInput(const KeyEvent& rKEvt);
	virtual BOOL MouseMove(const MouseEvent& rMEvt);
	virtual BOOL MouseButtonUp(const MouseEvent& rMEvt);
	virtual BOOL MouseButtonDown(const MouseEvent& rMEvt);

	virtual void Activate();		   // Function aktivieren
	virtual void Deactivate();		   // Function deaktivieren
};



} //namespace binfilter
#endif		// _SD_FUEDIPO_HXX

