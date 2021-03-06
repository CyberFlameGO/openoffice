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



#ifndef SC_SIZEDEV_HXX
#define SC_SIZEDEV_HXX

#ifndef _SV_MAPMOD_HXX 
#include <vcl/mapmod.hxx>
#endif
class OutputDevice;
namespace binfilter {

class ScDocShell;

class ScSizeDeviceProvider
{
	OutputDevice*	pDevice;
	BOOL			bOwner;
	double			nPPTX;
	double			nPPTY;
	MapMode			aOldMapMode;

public:
				ScSizeDeviceProvider( ScDocShell* pDocSh );
				~ScSizeDeviceProvider();

	OutputDevice* 	GetDevice() const	{ return pDevice; }
	double			GetPPTX() const		{ return nPPTX; }
	double			GetPPTY() const		{ return nPPTY; }
	BOOL			IsPrinter() const	{ return !bOwner; }
};

} //namespace binfilter
#endif

