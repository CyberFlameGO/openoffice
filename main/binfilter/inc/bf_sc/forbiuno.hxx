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



#ifndef SC_FORBIUNO_HXX
#define SC_FORBIUNO_HXX

#ifndef _SVX_UNOFORBIDDENCHARSTABLE_HXX_
#include <bf_svx/UnoForbiddenCharsTable.hxx>
#endif
#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif
namespace binfilter {

class ScDocShell;

//------------------------------------------------------------------------

//	object to set forbidden charaters to document

class ScForbiddenCharsObj : public SvxUnoForbiddenCharsTable, public SfxListener
{
	ScDocShell*		pDocShell;

protected:
	virtual void	onChange();

public:
					ScForbiddenCharsObj( ScDocShell* pDocSh );
	virtual			~ScForbiddenCharsObj();

	virtual void	Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
};

} //namespace binfilter
#endif

