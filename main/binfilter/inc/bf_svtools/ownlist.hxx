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



#ifndef _OWNLIST_HXX
#define _OWNLIST_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _TOOLS_OWNLIST_HXX //autogen
#include <tools/ownlist.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

namespace com { namespace sun { namespace star {
	namespace beans {
		struct PropertyValue;
	}
}}}

namespace binfilter
{


//=========================================================================
class SvCommand
/*	[Beschreibung]

	Enth"alt einen String, welcher das Kommando angibt und eine weiteren
	String, der das Argument des Kommandos bildet. W"urde solch ein
	Kommando "uber die Kommandozeile angegeben werden, s"ahe es wie folgt
	aus: Kommando = Argument.
*/
{
	String	aCommand;
	String	aArgument;
public:
					SvCommand() {}
					SvCommand( const String & rCommand, const String & rArg )
					{
						aCommand = rCommand;
						aArgument = rArg;
					}
	const String & GetCommand() const { return aCommand; }
	const String & GetArgument() const { return aArgument; }

	friend SvStream& operator >> ( SvStream& rStm, SvCommand & rThis )
					{
						rStm.ReadByteString( rThis.aCommand, gsl_getSystemTextEncoding() );
						rStm.ReadByteString( rThis.aArgument, gsl_getSystemTextEncoding() );
						return rStm;
					}
	friend SvStream& operator << ( SvStream& rStm, const SvCommand & rThis )
					{
						rStm.WriteByteString( rThis.aCommand, gsl_getSystemTextEncoding() );
						rStm.WriteByteString( rThis.aArgument, gsl_getSystemTextEncoding() );
						return rStm;
					}
};

//=========================================================================
class SvCommandList
/*	[Beschreibung]

	Die Liste enth"alt Objekte vom Typ SvCommand. Wird ein Objekt
	eingef"ugt, dann wird es kopiert und das neue Objekt wird
	in die Liste gestellt.
*/
{
					PRV_SV_DECL_OWNER_LIST(SvCommandList,SvCommand);
	SvCommand &		Append( const String & rCommand, const String & rArg );

	 friend SvStream& operator >> ( SvStream& rStm, SvCommandList & );
	 friend SvStream& operator << ( SvStream&, const SvCommandList & );
};

}

#endif // _OWNLIST_HXX
