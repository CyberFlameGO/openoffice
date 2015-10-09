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





# --- Settings -----------------------------------------------------
# Has to be included AFTER settings.mk !


# Precompiled header:


.IF "$(GUI)"=="WNT"

# RTTI
CFLAGS+= -GR

.IF "$(NP_LOCALBUILD)"!=""


# Precompiled Headers
PCH_NAME=  udm
.IF "$(DBG_LEVEL)"<"2"
CFLAGS+= -YX"precomp.h" /Fp$(PRJ)$/$(INPATH)$/misc$/$(PCH_NAME).pch
.ELSE
CFLAGS+= -YX"precomp.h" /Fp$(PRJ)$/$(INPATH)$/misc$/$(PCH_NAME).pcd -DNP_LOCALBUILD
.ENDIF

.ENDIF

.ENDIF

.IF "$(OS)"=="LINUX" || "$(OS)"=="FREEBSD" || "$(OS)"=="NETBSD"
CFLAGSCXX+= -frtti
.ENDIF
