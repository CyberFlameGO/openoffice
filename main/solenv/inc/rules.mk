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



MKFILENAME:=RULES.MK

$(OBJ)/%.obj : %.cxx
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXOBJ) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(CFLAGSINCXX)$(PWD)/$*.cxx
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
.IF "$(noadjust)"==""   
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(noadjust)"==""   
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cxx 
.ELSE
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cxx
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(OBJ)/%.obj : %.cpp
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXOBJ) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(CFLAGSINCXX)$(PWD)/$*.cpp
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cpp 
.ELSE
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cpp
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(OBJ)/%.obj : %.cc
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXOBJ) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(CFLAGSINCXX)$(PWD)/$*.cc
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@+$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE
	@@$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cc 
.ELSE
	$(COMMAND_ECHO)$(CXX) @$(mktmp $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cc )
.ENDIF
.ENDIF

.IF "$(ENABLE_PCH)"!=""
# workaround for file locking problems on network volumes
.IF "$(NETWORK_BUILD)"==""
PCHOUTDIR=$(SLO)/pch
PCHEXOUTDIR=$(SLO)/pch_ex
.ELSE			# "$(NETWORK_BUILD)"==""
PCHOUTDIR=$(TMP)/$(BUILD)$(CWS_WORK_STAMP)$(PRJNAME)$(PROEXT)
PCHEXOUTDIR=$(TMP)/$(BUILD)$(CWS_WORK_STAMP)$(PRJNAME)_ex$(PROEXT)
.ENDIF			# "$(NETWORK_BUILD)"==""
$(SLO)/precompiled.% .PHONY:
	-$(MKDIRHIER) $(SLO)/pch
.IF "$(COM)"=="MSC"
.IF "$(NETWORK_BUILD)"!=""
	-$(MKDIRHIER) $(PCHOUTDIR)
.IF "$(HAVE_BIG_TMP)"==""
	$(COMMAND_ECHO)-$(COPY) $(SLO)/pch/precompiled_$(PRJNAME).hxx$(PCHPOST) $(PCHOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST) 
.ENDIF			# "$(HAVE_BIG_TMP)"==""
.ENDIF			# "$(NETWORK_BUILD)"!=""
	$(COMMAND_ECHO)$(CXX) @$(mktmp -Fp$(PCHOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST) $(CFLAGS_CREATE_PCH) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGS_NO_EXCEPTIONS) -DEXCEPTIONS_OFF $(CFLAGSAPPEND) $(INCPCH)/precompiled_$(PRJNAME).cxx)
.IF "$(NETWORK_BUILD)"!=""
	$(COMMAND_ECHO)$(COPY) $(PCHOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST) $(SLO)/pch/precompiled_$(PRJNAME).hxx$(PCHPOST)
.IF "$(HAVE_BIG_TMP)"==""
	$(COMMAND_ECHO)$(RM) $(PCHOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST)
	$(COMMAND_ECHO)$(RMDIR) $(PCHOUTDIR)
.ENDIF			# "$(HAVE_BIG_TMP)"==""
.ENDIF			# "$(NETWORK_BUILD)"!=""
.ELIF "$(COM)"=="GCC" && "$(CCNUMVER)">="000300040000"
	$(COMMAND_ECHO)$(CXX) -o$(SLO)/pch/precompiled_$(PRJNAME).hxx$(PCHPOST) $(CFLAGS_CREATE_PCH) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGS_NO_EXCEPTIONS) -DEXCEPTIONS_OFF $(CFLAGSAPPEND) $(INCPCH)/precompiled_$(PRJNAME).hxx
	@echo "#error Tried to use wrong precompiled header" > $(SLO)/pch/precompiled_$(PRJNAME).hxx
.ELSE			# "$(COM)"=="MSC"
	@$(TOUCH) $(SLO)/pch/precompiled_$(PRJNAME).hxx$(PCHPOST)
.ENDIF			# "$(COM)"=="MSC"
	@echo USED_PCHFLAGS=$(CFLAGS)$(CFLAGSCXX)$(CFLAGSCXXSLO)$(CFLAGSSLO)$(CDEFS)$(CDEFSSLO)$(CDEFSMT)$(CFLAGS_NO_EXCEPTIONS) -DEXCEPTIONS_OFF$(CFLAGSAPPEND)> $(INCCOM)/pch.mk

$(SLO)/precompiled_ex.% .PHONY:
	-$(MKDIRHIER) $(SLO)/pch_ex
.IF "$(COM)"=="MSC"
.IF "$(NETWORK_BUILD)"!=""
	-$(MKDIRHIER) $(PCHEXOUTDIR)
.IF "$(HAVE_BIG_TMP)"==""
	$(COMMAND_ECHO)-$(COPY) $(SLO)/pch_ex/precompiled_$(PRJNAME).hxx$(PCHPOST) $(PCHEXOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST) 
.ENDIF			# "$(HAVE_BIG_TMP)"==""
.ENDIF			# "$(NETWORK_BUILD)"!=""
	$(COMMAND_ECHO)$(CXX) @$(mktmp -Fp$(PCHEXOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST) $(CFLAGS_CREATE_PCH:s/pchname/pchname_ex/) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSEXCEPTIONS) -DEXCEPTIONS_ON $(CFLAGSAPPEND) $(INCPCH)/precompiled_$(PRJNAME).cxx)
.IF "$(NETWORK_BUILD)"!=""
	$(COMMAND_ECHO)$(COPY) $(PCHEXOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST) $(SLO)/pch_ex/precompiled_$(PRJNAME).hxx$(PCHPOST)
.IF "$(HAVE_BIG_TMP)"==""
	$(COMMAND_ECHO)$(RM) $(PCHEXOUTDIR)/precompiled_$(PRJNAME).hxx$(PCHPOST)
	$(COMMAND_ECHO)$(RMDIR) $(PCHEXOUTDIR)
.ENDIF			# "$(HAVE_BIG_TMP)"==""
.ENDIF			# "$(NETWORK_BUILD)"!=""
.ELIF "$(COM)"=="GCC" && "$(CCNUMVER)">="000300040000"
	$(COMMAND_ECHO)$(CXX) -o$(SLO)/pch_ex/precompiled_$(PRJNAME).hxx$(PCHPOST) $(CFLAGS_CREATE_PCH) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSEXCEPTIONS) -DEXCEPTIONS_ON $(CFLAGSAPPEND) $(INCPCH)/precompiled_$(PRJNAME).hxx
	@echo "#error Tried to use wrong precompiled header" > $(SLO)/pch_ex/precompiled_$(PRJNAME).hxx
.ELSE			# "$(COM)"=="MSC"
	@$(TOUCH) $(SLO)/pch_ex/precompiled_$(PRJNAME).hxx$(PCHPOST)
.ENDIF			# "$(COM)"=="MSC"
	@echo USED_EXCEPTIONS_PCHFLAGS=$(subst,$(CFLAGSDEBUG), $(CFLAGS))$(CFLAGSCXX)$(CFLAGSCXXSLO)$(CFLAGSSLO)$(CDEFS)$(CDEFSSLO)$(CDEFSMT)$(CFLAGSEXCEPTIONS) -DEXCEPTIONS_ON$(CFLAGSAPPEND)> $(INCCOM)/pch_ex.mk

.INCLUDE .IGNORE : $(INCCOM)/pch.mk
.INCLUDE .IGNORE : $(INCCOM)/pch_ex.mk
.ENDIF			# "$(ENABLE_PCH)"!=""

$(SLO)/%.obj : %.cxx
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(ENABLE_PCH)"!=""
# just a helper var	
	@noop $(assign used_exc_switches=$(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)))
# cleanup first
	@noop $(assign ACT_PCH_SWITCHES:=$(NULL))
# eq: first string is a copy of the compile line,
# second generated by pch creation
# use pch with exception support
	@noop $(assign ACT_PCH_SWITCHES+=$(eq,$(strip $(USED_EXCEPTIONS_PCHFLAGS)),$(strip $(subst,$(CFLAGSDEBUG), $(CFLAGS))$(CFLAGSCXX)$(CFLAGSCXXSLO)$(CFLAGSSLO)$(CDEFS:s/\//)$(CDEFSSLO)$(CDEFSMT)$(used_exc_switches)$(CFLAGSAPPEND)) $(CFLAGS_USE_EXCEPTIONS_PCH) $(NULL)))
# use pch without exception support
	@noop $(assign ACT_PCH_SWITCHES+=$(eq,$(strip $(USED_PCHFLAGS)),$(strip $(subst,$(CFLAGSDEBUG), $(CFLAGS))$(CFLAGSCXX)$(CFLAGSCXXSLO)$(CFLAGSSLO)$(CDEFS:s/\//)$(CDEFSSLO)$(CDEFSMT)$(used_exc_switches)$(CFLAGSAPPEND)) $(CFLAGS_USE_PCH) $(NULL)))
.ENDIF			# "$(ENABLE_PCH)"!=""
.IF "$(GUI)"=="UNX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) -E  $(CFLAGSINCXX)$(PWD)/$*.cxx
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CAPTURE_COMMAND) $(CXX) $(ACT_PCH_SWITCHES) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(CFLAGSINCXX)$(PWD)/$*.cxx $(CAPTURE_OUTPUT)
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"=="" && "$(CAPTURE_COMMAND)"==""
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE			# "$(GUI)"=="UNX"
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CAPTURE_COMMAND) $(CXX) $(ACT_PCH_SWITCHES) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cxx $(CAPTURE_OUTPUT)
.ELSE
	$(COMMAND_ECHO)$(CAPTURE_COMMAND) $(CXX) $(ACT_PCH_SWITCHES) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cxx $(CAPTURE_OUTPUT)
.ENDIF		    # "$(COM)"=="GCC"	
.ENDIF			# "$(GUI)"=="UNX"
.IF "$(nodep)"==""
# fixme: write to file
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(SLO)/%.obj : %.cpp
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) -E  $(CFLAGSINCXX)$(PWD)/$*.cpp
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(CFLAGSINCXX)$(PWD)/$*.cpp
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cpp
.ELSE
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(PWD)/$*.cpp
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(SLO)/%.obj : $(MISC)/%.cxx
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) -E  $(CFLAGSINCXX)$(MISC)/$*.cxx
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(CFLAGSINCXX)$(MISC)/$*.cxx
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(MISC)/$*.cxx
.ELSE
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(MISC)/$*.cxx
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $@ ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(SLO)/%.obj : $(MISC)/%.cc
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) -E  $(CFLAGSINCXX)$(MISC)/$*.cc
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXSLO) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(CFLAGSINCXX)$(MISC)/$*.cc
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@+$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE
	$(COMMAND_ECHO)$(CXX) @$(mktmp $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(CFLAGSINCXX)$(MISC)/$*.cc )
.ENDIF

$(OBJ)/%.obj : $(MISC)/%.cxx
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXOBJ) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) -E  $(CFLAGSINCXX)$(MISC)/$*.cxx
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSCXXOBJ) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(CFLAGSINCXX)$(MISC)/$*.cxx
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(MISC)/$*.cxx
.ELSE
	$(COMMAND_ECHO)$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $(!eq,$(EXCEPTIONSFILES),$(subst,$@, $(EXCEPTIONSFILES)) $(LOCAL_EXCEPTIONS_FLAGS) $(GLOBAL_EXCEPTIONS_FLAGS)) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(CFLAGSINCXX)$(MISC)/$*.cxx
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(OBJ)/%.obj : %.c
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) -E $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $*.c
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $*.c
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
.IF "$(noadjust)"==""   
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF			# "$(noadjust)"==""   
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $*.c
.ELSE
  .IF "$(VERBOSE)" == "TRUE"
	-@echo Cflags: $(CFLAGS) $(INCLUDE)
  .ENDIF
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $*.c
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(OBJ)/%.obj : $(MISC)/%.c
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(MISC)/$*.c
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(MISC)/$*.c
.ELSE
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(OBJ)/$*.obj $(MISC)/$*.c
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

$(SLO)/%.obj : $(MISC)/%.c
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(MISC)/$*.c
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(MISC)/$*.c 
.ELSE
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $(MISC)/$*.c
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
# avoid _version objects (see tg_shl.mk) - quite hacky...
	@noop $(!eq,$@,$(@:s/_version.obj/.obj/) $(NULL) $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/)))
.ENDIF			# "$(nodep)"==""

$(SLO)/%.obj : %.c
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(GUI)"=="UNX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $*.c
.IF "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
    $(COMMAND_ECHO)$(ADJUSTVISIBILITY) -p $(@:s/.obj/.o/)
.ENDIF          # "$(OS)"=="SOLARIS" && "$(product)"=="full" && "$(debug)"==""    
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE
	@@-$(RM) $@
.IF "$(COM)"=="GCC"
	   $(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $*.c 
.ELSE
	   $(COMMAND_ECHO)$(CC) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ)$(SLO)/$*.obj $*.c
.ENDIF
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""

# Objective-C files
$(OBJ)/%.obj : %.m
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(objc) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(OBJCFLAGS) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) -E $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $*.m
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objc) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(OBJCFLAGS) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $*.m
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C files is available for this platform"
.ENDIF		"$(OS)"=="MACOSX"

# Objective-C files
$(OBJ)/%.obj : $(MISC)/%.m
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objc) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(OBJCFLAGS) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(MISC)/$*.m
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C files is available for this platform"
.ENDIF		"$(OS)"=="MACOSX"

# Objective-C files
$(SLO)/%.obj : $(MISC)/%.m
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objc) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(OBJCFLAGS) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(MISC)/$*.m
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C files is available for this platform"
.ENDIF		"$(OS)"=="MACOSX"

# Objective-C files
$(SLO)/%.obj : %.m
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objc) $(CFLAGS) $(INCLUDE_C) $(CFLAGSCC) $(OBJCFLAGS) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $*.m
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C files is available for this platform"
.ENDIF		"$(OS)"=="MACOSX"

# dependencies c / c++

not_existing/s_%.dpcc : %.c;@noop $(assign all_local_slo+:=$<)
not_existing/o_%.dpcc : %.c;@noop $(assign all_local_obj+:=$<)

# Objective-C++ files
$(OBJ)/%.obj : %.mm
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
.IF "$(TEST)"!=""
	$(COMMAND_ECHO)$(objcpp) $(CFLAGS) $(INCLUDE) $(CFLAGSCC) $(OBJCXXFLAGS) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) -E $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $*.mm
.ELSE
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objcpp) $(CFLAGS) $(INCLUDE) $(CFLAGSCC) $(OBJCXXFLAGS) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $*.mm
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.ENDIF
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C++ files is available for this platform"
.ENDIF			# "$(OS)"=="MACOSX"

# Objective-C++ files
$(OBJ)/%.obj : $(MISC)/%.mm
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objcpp) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(OBJCXXFLAGS) $(CFLAGSOBJ) $(CDEFS) $(CDEFSOBJ) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(OBJ)/$*.o $(MISC)/$*.mm
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< ) > $(MISC)/o_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=o_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C++ files is available for this platform"
.ENDIF			# "$(OS)"=="MACOSX"

# Objective-C++ files
$(SLO)/%.obj : $(MISC)/%.mm
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(INPATH)/misc/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objcpp) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(OBJCXXFLAGS) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $(MISC)/$*.mm
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C++ files is available for this platform"
.ENDIF			# "$(OS)"=="MACOSX"

# Objective-C++ files
$(SLO)/%.obj : %.mm
	@echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(OS)"=="MACOSX"
	@$(RM) $@ $(@:s/.obj/.o/)
	$(COMMAND_ECHO)$(objcpp) $(CFLAGS) $(INCLUDE) $(CFLAGSCXX) $(OBJCXXFLAGS) $(CFLAGSSLO) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $(CFLAGSAPPEND) $(CFLAGSOUTOBJ) $(SLO)/$*.o $*.mm
	@$(IFEXIST) $(@:s/.obj/.o/) $(THEN) $(TOUCH) $@ $(FI)
.IF "$(nodep)"==""
	$(COMMAND_ECHO)$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< ) > $(MISC)/s_$(@:f:s/.obj/.dpcc/)
	@noop $(assign DEPS_MADE+:=s_$(@:f:s/.obj/.dpcc/))
.ENDIF			# "$(nodep)"==""
.ELSE			# "$(OS)"=="MACOSX"
	@echo "No recipe for compiling Objective-C++ files is available for this platform"
.ENDIF			# "$(OS)"=="MACOSX"

$(MISC)/%.dpslo :
	@echo "Making:   " $(@:f)
	@@$(RM) $@
	@@-$(CDD) $(MISC) && $(!null,$(all_local_slo) $(TYPE:s/+//) echo #) $(foreach,i,$(all_local_slo:b:+".dpcc") s_$(i)) > $(@:f)
	@@-$(CDD) $(MISC) && $(!null,$(all_misc_slo) $(TYPE:s/+//) echo #) $(foreach,i,$(all_misc_slo:b:+".dpcc") s_$(i)) >> $(@:f)
	@$(TYPE) $(mktmp $(foreach,i,$(all_local_slo:b:+".dpcc") $(@:s#\#/#:^"\n") : $(MISC:s#\#/#)/s_$i) $(foreach,i,$(all_misc_slo:b:+".dpcc") $(@:s#\#/#:^"\n") : $(MISC:s#\#/#)/s_$i)) >> $@
	@$(TYPE) $(mktmp $(TARGET)_known_dpcc+=$(all_local_slo:b:+".dpcc":^"s_") $(all_misc_slo:b:+".dpcc":^"s_")) >> $@

$(MISC)/%.dpobj :
	@echo "Making:   " $(@:f)
	@@$(RM) $@
	@@-$(CDD) $(MISC) && $(!null,$(all_local_obj) $(TYPE:s/+//) echo #) $(foreach,i,$(all_local_obj:b:+".dpcc") o_$(i)) > $(@:f)
	@@-$(CDD) $(MISC) && $(!null,$(all_misc_obj) $(TYPE:s/+//) echo #) $(foreach,i,$(all_misc_obj:b:+".dpcc") o_$(i)) >> $(@:f)
	@$(TYPE) $(mktmp $(foreach,i,$(all_local_obj:b:+".dpcc") $(@:s#\#/#:^"\n") : $(MISC:s#\#/#)/o_$i) $(foreach,i,$(all_misc_obj:b:+".dpcc") $(@:s#\#/#:^"\n") : $(MISC:s#\#/#)/o_$i)) >> $@
	@$(TYPE) $(mktmp $(TARGET)_known_dpcc+=$(all_local_obj:b:+".dpcc":^"s_") $(all_misc_obj:b:+".dpcc":^"s_")) >> $@

# see also %.dpslo 
not_existing/s_%.dpcc : %.cxx;@noop $(assign all_local_slo+:=$<)
not_existing/o_%.dpcc : %.cxx;@noop $(assign all_local_obj+:=$<)

not_existing/s_%.dpcc : %.cpp;@noop $(assign all_local_slo+:=$<)
not_existing/o_%.dpcc : %.cpp;@noop $(assign all_local_obj+:=$<)

not_existing/s_%.dpcc : $(MISC)/%.c;@noop $(assign all_misc_slo+:=$<)
not_existing/o_%.dpcc : $(MISC)/%.c;@noop $(assign all_misc_obj+:=$<)

not_existing/s_%.dpcc : $(MISC)/%.cxx;@noop $(assign all_misc_slo+:=$<)
not_existing/o_%.dpcc : $(MISC)/%.cxx;@noop $(assign all_misc_obj+:=$<)

not_existing/s_%.dpcc : %.mm;@noop $(assign all_local_slo+:=$<)
not_existing/o_%.dpcc : %.mm;@noop $(assign all_local_obj+:=$<)

not_existing/s_%.dpcc : $(MISC)/%.mm;@noop $(assign all_misc_slo+:=$<)
not_existing/o_%.dpcc : $(MISC)/%.mm;@noop $(assign all_misc_obj+:=$<)

# dependencies objective-c

$(MISC)/s_%.dpcc : %.m
	@@-$(RM) $@
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< > $@
	@echo $@ : $(SLO)/$(<:b).obj >> $@

$(MISC)/o_%.dpcc : %.m
	@@-$(RM) $@
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< > $@
	@echo $@ : $(OBJ)/$(<:b).obj >> $@

$(MISC)/s_%.dpcc : $(MISC)/%.m
	@@-$(RM) $@
.IF "$(GUI)"=="UNX"	
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< | sed s\#$(MISC)/\#\# > $@
.ELSE			# "$(GUI)"=="UNX"	
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< | $(SED) s/$(MISC:s/\/\\/)\\// > $@
.ENDIF			# "$(GUI)"=="UNX"	
	@echo $@ : $(SLO)/$(<:b).obj >> $@

$(MISC)/o_%.dpcc : $(MISC)/%.m
	@@-$(RM) $@
.IF "$(GUI)"=="UNX"	
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< | sed s\#$(MISC)/\#\# > $@
.ELSE			# "$(GUI)"=="UNX"	
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $< | $(SED) s/$(MISC:s/\/\\/)\\// > $@
.ENDIF			# "$(GUI)"=="UNX"	
	@echo $@ : $(OBJ)/$(<:b).obj >> $@

# dependency dummy for *.s files

$(MISC)/s_%.dpcc : %.s
	@@-$(RM) $@
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $< > $@
	@echo $@ : $(SLO)/$(<:b).obj >> $@

$(MISC)/s_%.dpcc : $(SLO)/%.obj
	@noop $(assign DEPS_MISSING+:=$(@:f))

$(MISC)/o_%.dpcc : $(OBJ)/%.obj
	@noop $(assign DEPS_MISSING+:=$(@:f))

# dependencies script files

$(MISC)/%.dpsc :
	@@-$(RM) $@
	@@-$(MKDIR) $(MISC)/{$(subst,$(@:d:d:d), $(@:d:d))}
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(PAR)/{$(subst,$(@:d:d:d), $(@:d:d))} -o.par -D{$(subst,$(@:d:d:d:u), $(@:d:d:u))}_PRODUCT $(CDEFS) -DDLLPOSTFIX=$(DLLPOSTFIX) -I. -I$(INC) -I$(INCLOCAL) -I$(INCGUI) -I$(INCCOM) $(*:b).scp > $@
	@echo $@ : $(PAR)/{$(subst,$(@:d:d:d), $(@:d:d))}/$(*:b).par  >> $@
	@echo $(PAR)/{$(subst,$(@:d:d:d), $(@:d:d))}/$(*:b).par : $(*:b).scp >> $@

# dependencies rc files (native resources for windows)

$(MISC)/%.dprc : 
	@@-$(RM) $@
	$(COMMAND_ECHO)$(MAKEDEPEND) -f - -p$(RES) -o.res $(RCLANGFLAGS_{$(subst,$(@:d:d:d:u), $(@:d:d:u))}:u:s/ //) $(CDEFS) -DDLLPOSTFIX=$(DLLPOSTFIX) -I. -I$(INC) -I$(INCLOCAL) -I$(INCGUI) -I$(INCCOM) -I$(SOLARENV)/inc $(*:b).rc >> $@
#	@echo $@ : $(RES)/{$(subst,$(@:d:d:d), $(@:d:d))}/$(*:b).res  >> $@
	@echo $@ : $(RES)/$(*:b).res  >> $@

$(MISC)/%.dpr :
	@@noop
.IF "$(nodep)"==""
	@echo "Making:   " $(@:f)
    @@-$(RM) $@
	$(COMMAND_ECHO)dmake $(MFLAGS) $(MAKEFILE) $(CALLMACROS) make_srs_deps=true $(DEPSRSFILES)
	$(COMMAND_ECHO)-$(TYPE) $(MISC)/$(TARGET).*.dprr >> $@
.ENDIF			# "$(nodep)"==""

$(MISC)/%.dpz :
	@@noop
.IF "$(nodep)"==""
	@echo "Making:   " $(@:f)
    @@-$(RM) $@
    $(COMMAND_ECHO)dmake $(MFLAGS) $(MAKEFILE) $(CALLMACROS) make_zip_deps=true $(ZIPDEPFILES)
	$(COMMAND_ECHO)$(TYPE) $(ZIPDEPFILES) $(mktmp $(NULL)) | grep -v "CVS" | grep -v "\.svn" >> $@
	@echo zipdep_langs=$(alllangiso) >> $@
	@@-$(RM) $(ZIPDEPFILES)
.ENDIF			# "$(nodep)"==""

# Dependencies fuer java - Files

$(MISC)/%.dpj :
	@echo "Making:   " $(@:f)
.IF "$(nodep)"!=""
.IF "$(GUI)"=="UNX"
	@echo > $@
.ELSE
	@$(ECHONL) > $@
.ENDIF
.ELSE 			# "$(ndep)"==""
.IF "$(GUI)"=="UNX"
	@echo $(shell @$(STARDEP) @$(mktmp -o $@ -i $(CLASSDIR) $(foreach,i,$(JAVADEPINCLUDES:s/:/ /) -i $i) $(JAVACLASSFILES)))
.ELSE
	@echo javadeps
	@echo $(shell @$(STARDEP) @$(mktmp -o $@ -i $(CLASSDIR) $(foreach,i,$(JAVADEPINCLUDES:s/;/ /) -i $i) $(JAVACLASSFILES)))
.ENDIF
.ENDIF			# "$(nodep)"==""

$(SLO)/%.obj : %.asm
	   @echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(COM)"=="GCC"
.IF "$(ASM)"=="ml"
	   $(COMMAND_ECHO)$(ASM) $(AFLAGS) -D$(COM) /Fo$(SLO)/$*.obj $*.asm
	   @@-$(RM) $*.err
.ELSE			# "$(ASM)"=="ml"
		@@-$(RM) $@
		$(COMMAND_ECHO)$(ASM) $(AFLAGS) $*.asm,$(SLO)/$*.obj;
.ENDIF			# "$(ASM)"=="ml"
.ELSE
.IF "$(COM)"=="WTC"
		$(COMMAND_ECHO)$(ASM) $(AFLAGS) -D$(COM) $*.asm -fo=$(SLO)/$*.obj
		@-$(IFEXIST) $*.err $(THEN) $(RM:s/+//) $*.err $(FI)
.ELSE
.IF "$(COM)"=="MSC"
.IF "$(ASM)"=="ml"
	$(COMMAND_ECHO)$(ASM) $(AFLAGS) -D$(COM) /Fo$(SLO)/$*.obj $*.asm
	@-$(IFEXIST) $*.err $(THEN) $(RM:s/+//) $*.err $(FI)
.ELSE			# "$(ASM)"=="ml"
	@@-$(RM) $@
	$(COMMAND_ECHO)$(ASM) $(AFLAGS) $*.asm,$(SLO)/$*.obj;
.ENDIF			# "$(ASM)"=="ml"
.ENDIF			 "$(COM)"=="MSC"
.ENDIF
.ENDIF

$(OBJ)/%.obj : %.asm
	   @echo $(COMPILE_ECHO_SWITCH) Compiling: $(PRJNAME)/$(PATH_IN_MODULE)/$(COMPILE_ECHO_FILE)
.IF "$(COM)"=="GCC"
.IF "$(ASM)"=="ml"
	   $(COMMAND_ECHO)$(ASM) $(AFLAGS) -D$(COM) /Fo$(OBJ)/$*.obj $*.asm
	   @@-$(RM) $*.err
.ELSE			# "$(ASM)"=="ml"
		@@-$(RM) $@
		$(COMMAND_ECHO)$(ASM) $(AFLAGS) $*.asm,$(OBJ)/$*.obj;
.ENDIF			# "$(ASM)"=="ml"
.ELSE
.IF "$(COM)"=="MSC"
.IF "$(ASM)"=="ml"
	$(COMMAND_ECHO)$(ASM) $(AFLAGS) -D$(COM) /Fo$(SLO)/$*.obj $*.asm
	@-$(IFEXIST) $*.err $(THEN) $(RM:s/+//) $*.err $(FI)
.ELSE			# "$(ASM)"=="ml"
	@$(IFEXIST) $@ $(THEN) $(RM:s/+//) $@ >& $(NULLDEV) $(FI)
	$(COMMAND_ECHO)$(ASM) $(AFLAGS) $*.asm,$(OBJ)/$*.obj;
.ENDIF			# "$(ASM)"=="ml"
.ENDIF			 "$(COM)"=="MSC"
.ENDIF

$(OUT)/ucr/$(IDLPACKAGE)/%.urd : %.idl
		@noop $(assign all_outdated_idl+:=$<)

$(OUT)/ucrdoc/$(IDLPACKAGE)/%.urd : %.idl
		@noop $(assign all_outdated_doc_idl+:=$<)

# make *.xml descriptions available in $(MISC)
$(MISC)/%$($(WINVERSIONNAMES)_MAJOR).xml : %.xml
    $(COMMAND_ECHO)$(COPY) $< $@

# dummy rule to make sure xml file is in place when used in settings.mk
$(MISC)/%.mk : $(MISC)/%$($(WINVERSIONNAMES)_MAJOR).xml
	@$(TOUCH) $@
	@echo XML2MK_FILES += $(@:b) >> $@

#generate descriptions from xml
$(MISC)/%$($(WINVERSIONNAMES)_MAJOR)_description.cxx : $(MISC)/%$($(WINVERSIONNAMES)_MAJOR).xml 
	xml2cmp -func $(MISC)/$*$($(WINVERSIONNAMES)_MAJOR)_description.cxx $<

#generate private rdb
$(BIN)/%.rdb: $(MISC)/%$($(WINVERSIONNAMES)_MAJOR).xml
	$(COMMAND_ECHO)$(RDBMAKER) -BUCR -O$(BIN)/$*.rdb @$(mktmp $(foreach,i,$($(@:b)_XML2CMPTYPES) -T$i ) $(COMPRDB))

#strip dos lineends
$(MISC)/%.sh : %.sh
	@@-$(RM) -f $@
	@tr -d "\015" < $< > $@

# merge targets
.IF "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.ulf : %.ulf
	@echo "Making:   " $(@:f)
    $(COMMAND_ECHO)-$(MKDIR) $(@:d)
    $(COMMAND_ECHO)-$(RM) $@
	$(COMMAND_ECHO)$(ULFEX) -p $(PRJNAME) -i $(@:f) -o $(@).$(INPATH) -m $(LOCALIZESDF) -l all
    $(COMMAND_ECHO)$(RENAME) $@.$(INPATH) $@
    $(COMMAND_ECHO)-$(RM) $@.$(INPATH)

$(COMMONMISC)/$(TARGET)/%.xrb : %.xrb
    $(COMMAND_ECHO)-$(MKDIR) $(@:d)
    $(COMMAND_ECHO)-$(RM) $@
	$(COMMAND_ECHO)$(XMLEX) -t xrb -p $(PRJNAME) -i $(@:f) -o $(@).$(INPATH) -m $(LOCALIZESDF) -l all
    $(COMMAND_ECHO)$(RENAME) $@.$(INPATH) $@
    $(COMMAND_ECHO)-$(RM) $@.$(INPATH)

$(COMMONMISC)/$(MYPATH)/%.xrm : %.xrm
    $(COMMAND_ECHO)-$(MKDIRHIER) $(@:d)
    $(COMMAND_ECHO)-$(RM) $@
	@echo trysdf = $(TRYSDF)
	$(COMMAND_ECHO)$(XRMEX) -p $(PRJNAME) -i $(@:f) -o $(@).$(INPATH) -m $(LOCALIZESDF) -l all
    $(COMMAND_ECHO)$(RENAME) $@.$(INPATH) $@
    $(COMMAND_ECHO)-$(RM) $@.$(INPATH)

#$(COMMONMISC)/$(TARGET)/%.xrm : %.xrm
#    -$(MKDIRHIER) $(@:d)
#    -$(RM) $@
#	$(XRMEX) -p $(PRJNAME) -i $(@:f) -o $(@).$(INPATH) -m $(LOCALIZESDF) -l all
#    $(RENAME) $@.$(INPATH) $@
#    -$(RM) $@.$(INPATH)
#
#$(COMMONMISC)/%.xrm : %.xrm
#    -$(MKDIR) $(@:d)
#    -$(RM) $@
#	$(XRMEX) -p $(PRJNAME) -i $(@:f) -o $(@).$(INPATH) -m $(LOCALIZESDF) -l all
#    $(RENAME) $@.$(INPATH) $@
#    -$(RM) $@.$(INPATH)
.ENDIF			# "$(WITH_LANG)"!=""

.IF "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.jlf : $$(@:db).ulf
.ELSE			# "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.jlf : $$(@:b).ulf
.ENDIF			# "$(WITH_LANG)"!=""
	@-$(MKDIRHIER) $(@:d)
    $(COMMAND_ECHO)-$(RM) $@
	$(COMMAND_ECHO)$(ULFCONV) -o $@.$(INPATH) $<
    $(COMMAND_ECHO)$(RENAME) $@.$(INPATH) $@
    $(COMMAND_ECHO)-$(RM) $@.$(INPATH)

.IF "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.mlf : $$(@:db).ulf
.ELSE			# "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.mlf : $$(@:b).ulf
.ENDIF			# "$(WITH_LANG)"!=""
	@-$(MKDIRHIER) $(@:d)
	@-$(RM) $@
	@$(ULFCONV) -o $@.$(INPATH) -t $(SOLARBINDIR)/msi-encodinglist.txt $<
	@$(RENAME) $@.$(INPATH) $@
	@-$(RM) $@.$(INPATH)

.IF "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.uulf : $$(@:db).ulf
.ELSE			# "$(WITH_LANG)"!=""
$(COMMONMISC)/$(TARGET)/%.uulf : $$(@:b).ulf
.ENDIF			# "$(WITH_LANG)"!=""
	@-$(MKDIRHIER) $(@:d)
	$(COMMAND_ECHO)-$(RM) $@
	@$(COPY) $< $@.$(INPATH)
	@$(RENAME) $@.$(INPATH) $@
	@-$(RM) $@.$(INPATH)

# This is still needed?????
$(COMMONMISC)/$(TARGET)/%.xrm : %.xrm
    $(COMMAND_ECHO)-$(MKDIR) $(@:d)
    $(COMMAND_ECHO)-$(RM) $@
	$(COMMAND_ECHO)$(XRMEX) -p $(PRJNAME) -i $(@:f) -o $(@).$(INPATH) -m $(LOCALIZESDF) -l all
    $(COMMAND_ECHO)$(RENAME) $@.$(INPATH) $@
    $(COMMAND_ECHO)-$(RM) $@.$(INPATH)

# dirty hack
# if local *.sdf file is missing
#%.sdf:
#    echo > $@

