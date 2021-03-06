#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..$/..$/..$/..
BFPRJ=..$/..$/..

PRJNAME=binfilter
TARGET=sw_xml

#GEN_HID=TRUE
#GEN_HID_OTHER=TRUE
NO_HIDS=TRUE

# --- Settings -----------------------------------------------------

#.INCLUDE :  $(PRJ)$/inc$/swpre.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/swpre.mk
.INCLUDE :  settings.mk
#.INCLUDE :  $(PRJ)$/inc$/sw.mk
.INCLUDE :  $(PRJ)$/inc$/bf_sw$/sw.mk
INC+= -I$(PRJ)$/inc$/bf_sw
.IF "$(mydebug)" != ""
CDEFS=$(CDEFS) -Dmydebug
.ENDIF

# --- Files --------------------------------------------------------

CXXFILES = \
        sw_swxml.cxx      \
        sw_xmlimp.cxx      \
        sw_wrtxml.cxx      \
        sw_xmlexp.cxx      \
        sw_xmltext.cxx      \
        sw_xmltexte.cxx      \
        sw_xmltexti.cxx      \
        sw_xmltbli.cxx      \
        sw_xmltble.cxx      \
        sw_xmlfmt.cxx      \
        sw_xmlfmte.cxx      \
        sw_xmlmeta.cxx      \
		sw_xmlitemm.cxx	\
		sw_xmlitemi.cxx	\
		sw_xmliteme.cxx	\
		sw_xmlbrsh.cxx		\
		sw_xmlfonte.cxx	\
		sw_XMLRedlineImportHelper.cxx \
		sw_xmlitem.cxx	\
		sw_xmlithlp.cxx \
		sw_xmlitmpr.cxx	\
		sw_xmlimpit.cxx \
		sw_xmlexpit.cxx 

SLOFILES =  \
        $(SLO)$/sw_swxml.obj \
        $(SLO)$/sw_xmlimp.obj \
        $(SLO)$/sw_wrtxml.obj \
        $(SLO)$/sw_xmlexp.obj \
        $(SLO)$/sw_xmltext.obj \
        $(SLO)$/sw_xmltexte.obj \
        $(SLO)$/sw_xmltexti.obj \
        $(SLO)$/sw_xmltbli.obj \
        $(SLO)$/sw_xmltble.obj \
        $(SLO)$/sw_xmlfmt.obj \
        $(SLO)$/sw_xmlfmte.obj \
        $(SLO)$/sw_xmlmeta.obj \
        $(SLO)$/sw_xmlscript.obj \
        $(SLO)$/sw_xmlitemm.obj \
        $(SLO)$/sw_xmlitemi.obj \
        $(SLO)$/sw_xmliteme.obj \
		$(SLO)$/sw_xmlithlp.obj \
		$(SLO)$/sw_xmlbrsh.obj \
		$(SLO)$/sw_xmlfonte.obj \
		$(SLO)$/sw_XMLRedlineImportHelper.obj \
		$(SLO)$/sw_xmlitem.obj	\
		$(SLO)$/sw_xmlitmpr.obj	\
		$(SLO)$/sw_xmlimpit.obj \
		$(SLO)$/sw_xmlexpit.obj

EXCEPTIONSFILES= \
        $(SLO)$/sw_swxml.obj \
        $(SLO)$/sw_xmlimp.obj \
        $(SLO)$/sw_wrtxml.obj \
        $(SLO)$/sw_xmlexp.obj \
        $(SLO)$/sw_xmltext.obj \
        $(SLO)$/sw_xmltexti.obj \
        $(SLO)$/sw_xmltbli.obj \
        $(SLO)$/sw_xmltble.obj \
        $(SLO)$/sw_xmlfmt.obj \
        $(SLO)$/sw_xmlfmte.obj \
        $(SLO)$/sw_xmlmeta.obj \
        $(SLO)$/sw_xmlscript.obj \
        $(SLO)$/sw_xmlitemi.obj \
        $(SLO)$/sw_xmliteme.obj \
		$(SLO)$/sw_xmlithlp.obj \
		$(SLO)$/sw_xmlbrsh.obj \
		$(SLO)$/sw_xmlfonte.obj \
		$(SLO)$/sw_XMLRedlineImportHelper.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk

