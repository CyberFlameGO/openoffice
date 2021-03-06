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



#ifndef _SVDTYPES_HXX
#define _SVDTYPES_HXX

#include <bf_svtools/bf_solar.h>

namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
//  @@@@  @@@@@  @@@@@    @@@@  @@@@@  @@@@@@ @@@@@  @@@@  @@@@@@
// @@  @@ @@  @@ @@  @@  @@  @@ @@  @@     @@ @@    @@  @@   @@
// @@     @@  @@ @@  @@  @@  @@ @@  @@     @@ @@    @@       @@
//  @@@@  @@  @@ @@@@@   @@  @@ @@@@@      @@ @@@@  @@       @@
//     @@ @@  @@ @@  @@  @@  @@ @@  @@     @@ @@    @@       @@
// @@  @@ @@  @@ @@  @@  @@  @@ @@  @@ @@  @@ @@    @@  @@   @@
//  @@@@  @@@@@  @@  @@   @@@@  @@@@@   @@@@  @@@@@  @@@@    @@
////////////////////////////////////////////////////////////////////////////////////////////////////

const UINT32 SdrInventor=UINT32('S')*0x00000001+
						 UINT32('V')*0x00000100+
						 UINT32('D')*0x00010000+
						 UINT32('r')*0x01000000;

// Kommandos fuer EndCreate()
enum SdrCreateCmd {SDRCREATE_NEXTPOINT,     // Naechster Polygonpunkt, Kreissegment: Naechste Koordinate
				   SDRCREATE_NEXTOBJECT,    // Naechstes Polygon im PolyPolygon
				   SDRCREATE_FORCEEND};     // Ende erzwungen

enum SdrDragMode 
{
	SDRDRAG_MOVE,		// Verschieben
	SDRDRAG_RESIZE,		// Groesse aendern
	SDRDRAG_ROTATE,		// Drehen
	SDRDRAG_MIRROR,		// Spiegeln
	SDRDRAG_SHEAR,		// Schraegstellen
	SDRDRAG_CROOK,		// Kreisfoermig verbiegen
	SDRDRAG_DISTORT,	// Freies verzerren (Rect in beliebiges Viereck transformieren)

	// new modes for interactive transparence and gradient tools
	SDRDRAG_TRANSPARENCE,
	SDRDRAG_GRADIENT
};

// fuer SdrObject::ConvertToPoly()
enum SdrConvertType {SDRCONVERT_POLY,   // reines Polygon erzeugen
					 SDRCONVERT_BEZIER, // alles Beziersegmente
					 SDRCONVERT_MIXED}; // Gemischtes Objekt (optimal)

// fuer SdrObject::TakeContour()
enum SdrContourType {SDRCONTOUR_NORMAL,   // fuer Dragging
					 SDRCONTOUR_ALIGNED,  // fuer Fontwork
					 SDRCONTOUR_DETAILED, // fuer ?
					 SDRCONTOUR_OUTER,    // fuer Writer Kontourfluss
					 SDRCONTOUR_INNER,    // fuer KontourFrames
					 SDRCONTOUR_INNERUNROTATED}; // fuer KontourFrames

////////////////////////////////////////////////////////////////////////////////////////////////////
//  @@     @@@@  @@  @@ @@@@@ @@@@@
//  @@    @@  @@ @@  @@ @@    @@  @@
//  @@    @@  @@ @@  @@ @@    @@  @@
//  @@    @@@@@@  @@@@  @@@@  @@@@@
//  @@    @@  @@   @@   @@    @@  @@
//  @@    @@  @@   @@   @@    @@  @@
//  @@@@@ @@  @@   @@   @@@@@ @@  @@
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// Bei der Identifikation eines Layer kann es vorkommen, das dieser nicht
// vorhanden ist. SdrLayerAdmin::GetLayerID(const String&) liefert
// dann diesen Wert:
#define SDRLAYER_NOTFOUND 0xFF
// Man kann diesen Wert jodoch ohne Bedenken den Methoden des SdrLayerSet
// zuwerfen, bekommt dann jedoch immer FALSE, bzw. tut die Methode nix.
////////////////////////////////////////////////////////////////////////////////////////////////////
// Typdeklaration fuer Layer-IDs
typedef BYTE SdrLayerID;
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// @@@@@   @@@@   @@@@  @@@@@         @@@@  @@@@@  @@@@@@ @@    @@  @@@@  @@@@@@
// @@  @@ @@  @@ @@  @@ @@           @@  @@ @@  @@     @@ @@    @@ @@  @@   @@
// @@  @@ @@  @@ @@     @@      @@   @@  @@ @@  @@     @@ @@    @@ @@       @@
// @@@@@  @@@@@@ @@ @@@ @@@@   @@@@  @@  @@ @@@@@      @@ @@    @@  @@@@    @@
// @@     @@  @@ @@  @@ @@      @@   @@  @@ @@  @@     @@ @@    @@     @@   @@
// @@     @@  @@ @@  @@ @@           @@  @@ @@  @@ @@  @@ @@    @@ @@  @@   @@
// @@     @@  @@  @@@@@ @@@@@         @@@@  @@@@@   @@@@  @@@@@ @@  @@@@    @@
////////////////////////////////////////////////////////////////////////////////////////////////////
enum SdrObjListKind {SDROBJLIST_UNKNOWN    =0x00,    // Unbekannt
					 // reine Objektlisten:
					 SDROBJLIST_GROUPOBJ   =0x01,    // Objektliste eines Gruppenobjekts
					 SDROBJLIST_VIRTOBJECTS=0x02,    // Liste ist die Liste der virtuellen Objekte
					 SDROBJLIST_SYMBOLTABLE=0x03,    // Liste ist die Symboltabelle
					 // Hier haben zur Not noch 12 weitere Listentypen Platz
					 // Pages:
					 SDROBJLIST_DRAWPAGE   =0x10,    // Liste ist eine Zeichenseite
					 SDROBJLIST_MASTERPAGE =0x11,    // Liste ist eine Masterpage
					 // Hier haben zur Not noch 8 weitere Pagetypen Platz
					 // Spezialwerte fuer SdrObjSurrogate:
					 SDROBJLIST_SAMELIST   =0x1E,    // Fuer SdrObjSurrogate
					 SDROBJLIST_SAMEPAGE   =0x1F,    // Fuer SdrObjSurrogate
					 SDROBJLIST_RESERVEDBEG=0xE0,    // Die oberen 3 Bits sind ebenfalls
					 SDROBJLIST_RESERVEDEND=0xFF};   // fuer die Surrogate reserviert

inline FASTBOOL SdrIsPageKind(SdrObjListKind eK) { return eK>=SDROBJLIST_DRAWPAGE && eK<=0x1A; }
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//  @@@@@  @@@@@ @@@@@  @@@@@  @@@@  @@@@@@
//  @@  @@ @@    @@  @@ @@    @@  @@   @@
//  @@  @@ @@    @@  @@ @@    @@  @@   @@
//  @@@@@  @@@@  @@@@@  @@@@  @@@@@@   @@
//  @@  @@ @@    @@     @@    @@  @@   @@
//  @@  @@ @@    @@     @@    @@  @@   @@
//  @@  @@ @@@@@ @@     @@@@@ @@  @@   @@
////////////////////////////////////////////////////////////////////////////////////////////////////

enum SdrRepeatFunc {SDRREPFUNC_OBJ_NONE,
					// Mit Parameter
					SDRREPFUNC_OBJ_MOVE,         // Distanz
					SDRREPFUNC_OBJ_RESIZE,       // Faktor, Referenz(%) bezogen auf MarkRect
					SDRREPFUNC_OBJ_SETSNAPRECT,  // Rectangle
					SDRREPFUNC_OBJ_SETLOGICRECT, // Rectangle
					SDRREPFUNC_OBJ_ROTATE,       // Drehwinkel, Referenz(%) bezogen auf MarkRect
					SDRREPFUNC_OBJ_SHEAR,        // Winkel,Faktor, Referenz(%) bezogen auf MarkRect
					SDRREPFUNC_OBJ_CROOKROTATE,
					SDRREPFUNC_OBJ_CROOKSLANT,
					SDRREPFUNC_OBJ_CROOKSTRETCH,
					SDRREPFUNC_OBJ_ALIGN,        // Hor/Vert/bBound
					SDRREPFUNC_OBJ_SETATTRIBUTES,// ItemSet
					SDRREPFUNC_OBJ_SETSTYLESHEET,// StyleSheet*
					// Ohne Parameter
					SDRREPFUNC_OBJ_DELETE,
					SDRREPFUNC_OBJ_COMBINE_POLYPOLY,
					SDRREPFUNC_OBJ_COMBINE_ONEPOLY,
					SDRREPFUNC_OBJ_DISMANTLE_POLYS,
					SDRREPFUNC_OBJ_DISMANTLE_LINES,
					SDRREPFUNC_OBJ_CONVERTTOPOLY,
					SDRREPFUNC_OBJ_CONVERTTOPATH,
					SDRREPFUNC_OBJ_GROUP,
					SDRREPFUNC_OBJ_UNGROUP,
					SDRREPFUNC_OBJ_PUTTOTOP,
					SDRREPFUNC_OBJ_PUTTOBTM,
					SDRREPFUNC_OBJ_MOVTOTOP,
					SDRREPFUNC_OBJ_MOVTOBTM,
					SDRREPFUNC_OBJ_REVORDER,
					SDRREPFUNC_OBJ_IMPORTMTF,
					SDRREPFUNC_PAGE_DELETE,
					SDRREPFUNC_LAST};

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_SVDTYPES_HXX

