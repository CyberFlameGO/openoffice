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

PRJ=..$/..

PRJNAME=more_fonts
TARGET=ttf_croscore

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

.IF "$(WITH_CATB_FONTS)" != "YES"

@all:
	@echo "category B licensed CrossCore font is disabled."

.ELSE

# --- Files --------------------------------------------------------

TARFILE_NAME=croscorefonts-1.21.0
TARFILE_MD5=61f59e4110781cbe66b46449eadac231
TARFILE_ROOTDIR=.

PATCH_FILES=

CONFIGURE_DIR=
CONFIGURE_ACTION=

BUILD_DIR=
BUILD_ACTION=
BUILD_FLAGS=

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk

.IF "$(WITH_FONTS)"!="NO"
.INCLUDE : tg_ext.mk
.ENDIF

.ENDIF
