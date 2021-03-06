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



$(eval $(call gb_Library_Library,textconversiondlgs))

$(eval $(call gb_Library_set_componentfile,textconversiondlgs,svx/util/textconversiondlgs))

$(eval $(call gb_Library_set_include,textconversiondlgs,\
	-I$(SRCDIR)/svx/inc \
	-I$(SRCDIR)/svx/inc/pch \
	$$(INCLUDE) \
	-I$(OUTDIR)/inc/offuh \
))

$(eval $(call gb_Library_set_defs,textconversiondlgs,\
	$$(DEFS) \
))

$(eval $(call gb_Library_add_linked_libs,textconversiondlgs,\
	comphelper \
	cppuhelper \
	cppu \
	sal \
	svl \
	svt \
    stl \
	tk \
	tl \
	utl \
	vcl \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,textconversiondlgs,\
    svx/source/unodialogs/textconversiondlgs/services \
    svx/source/unodialogs/textconversiondlgs/chinese_dictionarydialog \
    svx/source/unodialogs/textconversiondlgs/chinese_translationdialog \
    svx/source/unodialogs/textconversiondlgs/chinese_translation_unodialog \
    svx/source/unodialogs/textconversiondlgs/resid \
))

# vim: set noet sw=4 ts=4:
