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



#ifndef SC_UNOWIDS_HXX
#define SC_UNOWIDS_HXX

#include <bf_svtools/bf_solar.h>

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"
#endif
namespace binfilter {


//	WIDs for uno property maps,
//	never stored in files

#define SC_WID_UNO_START	1200

#define SC_WID_UNO_CELLSTYL			( SC_WID_UNO_START + 0 )
#define SC_WID_UNO_CHCOLHDR			( SC_WID_UNO_START + 1 )
#define SC_WID_UNO_CHROWHDR			( SC_WID_UNO_START + 2 )
#define SC_WID_UNO_CONDFMT			( SC_WID_UNO_START + 3 )
#define SC_WID_UNO_CONDLOC			( SC_WID_UNO_START + 4 )
#define SC_WID_UNO_CONDXML			( SC_WID_UNO_START + 5 )
#define SC_WID_UNO_TBLBORD			( SC_WID_UNO_START + 6 )
#define SC_WID_UNO_VALIDAT			( SC_WID_UNO_START + 7 )
#define SC_WID_UNO_VALILOC			( SC_WID_UNO_START + 8 )
#define SC_WID_UNO_VALIXML			( SC_WID_UNO_START + 9 )
#define SC_WID_UNO_POS				( SC_WID_UNO_START + 10 )
#define SC_WID_UNO_SIZE				( SC_WID_UNO_START + 11 )
#define SC_WID_UNO_FORMLOC			( SC_WID_UNO_START + 12 )
#define SC_WID_UNO_FORMRT			( SC_WID_UNO_START + 13 )
#define SC_WID_UNO_PAGESTL			( SC_WID_UNO_START + 14 )
#define SC_WID_UNO_CELLVIS			( SC_WID_UNO_START + 15 )
#define SC_WID_UNO_LINKDISPBIT		( SC_WID_UNO_START + 16 )
#define SC_WID_UNO_LINKDISPNAME		( SC_WID_UNO_START + 17 )
#define SC_WID_UNO_CELLWID			( SC_WID_UNO_START + 18 )
#define SC_WID_UNO_OWIDTH			( SC_WID_UNO_START + 19 )
#define SC_WID_UNO_NEWPAGE			( SC_WID_UNO_START + 20 )
#define SC_WID_UNO_MANPAGE			( SC_WID_UNO_START + 21 )
#define SC_WID_UNO_CELLHGT			( SC_WID_UNO_START + 22 )
#define SC_WID_UNO_CELLFILT			( SC_WID_UNO_START + 23 )
#define SC_WID_UNO_OHEIGHT			( SC_WID_UNO_START + 24 )
#define SC_WID_UNO_DISPNAME			( SC_WID_UNO_START + 25 )
#define SC_WID_UNO_HEADERSET		( SC_WID_UNO_START + 26 )
#define SC_WID_UNO_FOOTERSET		( SC_WID_UNO_START + 27 )
#define SC_WID_UNO_NUMRULES			( SC_WID_UNO_START + 28 )

#define SC_WID_UNO_END				( SC_WID_UNO_START + 28 )


inline BOOL IsScUnoWid( USHORT nWid )
{
	return nWid >= SC_WID_UNO_START && nWid <= SC_WID_UNO_END;
}

inline BOOL IsScItemWid( USHORT nWid )
{
	return nWid >= ATTR_STARTINDEX && nWid <= ATTR_ENDINDEX;	// incl. page
}

} //namespace binfilter
#endif

