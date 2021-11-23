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



PRJ=.

PRJNAME=openssl
TARGET=openssl

# --- Settings -----------------------------------------------------

.INCLUDE :	settings.mk

# --- Files --------------------------------------------------------

.IF "$(SYSTEM_OPENSSL)" == "YES"
@all:
	@echo "Using system openssl...."
.ENDIF

.IF "$(DISABLE_OPENSSL)" == "TRUE"
@all:
	@echo "openssl disabled...."
.ENDIF

OPENSSL_NAME=openssl-1.1.1l

TARFILE_NAME=$(OPENSSL_NAME)
TARFILE_MD5=ac0d4387f3ba0ad741b0580dd45f6ff3

CONFIGURE_DIR=.
CONFIGURE_ACTION=config
CONFIGURE_FLAGS=-I$(SYSBASE)$/usr$/include -L$(SYSBASE)$/usr$/lib shared

BUILD_DIR=.
BUILD_ACTION=make CC='$(CC)' build_libs

OUT2LIB = libssl.*
OUT2LIB += libcrypto.*
OUT2INC += include/openssl/*

UNAME=$(shell uname)

.IF "$(COM)"=="GCC" && "$(CCNUMVER)">="000400060000" || "$(OS)"=="WNT" && "$(NASM_PATH)"=="NO_NASM_HOME"
  NO_ASM="no-asm"
.ELSE
  NO_ASM=
.ENDIF

.IF "$(OS)" == "LINUX" || "$(OS)" == "FREEBSD"
	PATCH_FILES=openssllnx.patch
	.IF "$(CPU)" == "I"
		.IF "$(UNAME)" == "GNU/kFreeBSD"
			CONFIGURE_ACTION=Configure debian-kfreebsd-i386 no-dso no-shared $(NO_ASM)
		.ELIF "$(UNAME)" == "FreeBSD"
			CONFIGURE_ACTION=Configure BSD-x86-elf no-dso no-shared $(NO_ASM)
		.ELSE
			CONFIGURE_ACTION=Configure linux-generic32 no-dso no-shared $(NO_ASM)
		.ENDIF
	.ELIF "$(BUILD64)" == "1"
		.IF "$(UNAME)" == "GNU/kFreeBSD"
			CONFIGURE_ACTION=Configure debian-kfreebsd-amd64 no-dso no-shared $(NO_ASM)
		.ELIF "$(UNAME)" == "FreeBSD"
			CONFIGURE_ACTION=Configure BSD-x86_64 no-dso no-shared $(NO_ASM)
		.ELSE
			CONFIGURE_ACTION=Configure linux-x86_64 no-dso no-shared $(NO_ASM)
		.ENDIF
	.ELSE
		CONFIGURE_ACTION=Configure linux-generic32 no-dso no-shared $(NO_ASM)
	.ENDIF
	# if you build openssl as shared library you have to patch the Makefile.Shared "LD_LIBRARY_PATH=$$LD_LIBRARY_PATH \"
	#BUILD_ACTION=make 'SHARED_LDFLAGS=-Wl,--version-script=./lib$$(SHLIBDIRS)_OOo_0_9_8e.map'
.ENDIF

.IF "$(OS)" == "SOLARIS"
	PATCH_FILES=opensslsol.patch
	#BUILD_ACTION=make 'SHARED_LDFLAGS=-G -dy -z text -M./lib$$$$$$$$(SHLIBDIRS)_OOo_0_9_8e.map'

	# Use BUILD64 when 1 to select new specific 64bit Configurations if necessary

	.IF "$(CPUNAME)" == "INTEL" # Solaris INTEL
		.IF "$(CPU)" == "X"
		   CONFIGURE_ACTION=Configure solaris64-x86_64-cc
		.ELSE
		   CONFIGURE_ACTION=Configure solaris-x86-cc
		.ENDIF
	.ELIF "$(CPU)" == "U" # Solaris SPARC
	   CONFIGURE_ACTION=Configure solaris64-sparcv9-cc
	.ELSE
	   CONFIGURE_ACTION=Configure solaris-sparcv9-cc
	.ENDIF
.ENDIF

.IF "$(OS)" == "MACOSX"
	PATCH_FILES=opensslmacos.patch
	CONFIGURE_ACTION=Configure darwin64-x86_64-cc no-dso no-shared $(NO_ASM)
.ENDIF

.IF "$(OS)" == "WNT"
PATCH_FILES+=opensslwnt.patch

.IF "$(COM)"=="GCC"
PATCH_FILES=opensslmingw.patch
.IF "$(USE_MINGW)" == "cygwin"
CONFIGURE_ACTION=$(PERL) configure
CONFIGURE_FLAGS=mingw shared
INSTALL_ACTION=mv libcrypto.a libcrypto_static.a && mv libcrypto.dll.a libcrypto.a && mv libssl.a libssl_static.a && mv libssl.dll.a libssl.a
OUT2LIB = libcrypto_static.*
OUT2LIB += libssl_static.*
OUT2LIB += libcrypto.*
OUT2LIB += libssl.*
.ELSE
CONFIGURE_ACTION=
BUILD_ACTION=cmd /c "ms\mingw32"
OUT2LIB = out/libcrypto_static.*
OUT2LIB += out/libssl_static.*
OUT2LIB += out/libcrypto.*
OUT2LIB += out/libssl.*
.ENDIF
.ELSE

		.IF "$(MAKETARGETS)" == ""
			# The env. vars CC and PERL are used by nmake, and nmake insists on '\'s
			# If WRAPCMD is set it is prepended before the compiler, don't touch that.
			.IF "$(WRAPCMD)"==""
				CC!:=$(subst,/,\ $(normpath,1 $(CC)))
				.EXPORT : CC
			.ENDIF
			PERL_bak:=$(PERL)
			PERL!:=$(subst,/,\ $(normpath,1 $(PERL)))
			.EXPORT : PERL
			PERL!:=$(PERL_bak)
		.ENDIF

		#CONFIGURE_ACTION=cmd /c $(PERL:s!\!/!) configure
		# We must set the env variable CONFIGURE_INSIST
		# so that Configurations/windows-checker.pm does not
		# complain about our Perl using unix-like paths
		.IF "$(CPUNAME)"=="INTEL"
			CONFIGURE_ACTION=CONFIGURE_INSIST=1 $(PERL) configure $(NO_ASM)
			CONFIGURE_FLAGS=VC-WIN32
		.ELIF "$(CPUNAME)"=="X86_64"
			CONFIGURE_ACTION=CONFIGURE_INSIST=1 $(PERL) configure $(NO_ASM)
			CONFIGURE_FLAGS=VC-WIN64A
		.ENDIF
		BUILD_ACTION=nmake
	.ENDIF
.ENDIF

#set INCLUDE=D:\sol_temp\n\msvc7net3\PlatformSDK\include;D:\sol_temp\n\msvc7net3\include\ && set path=%path%;D:\sol_temp\r\btw\SRC680\perl\bin &&

# --- Targets ------------------------------------------------------

.INCLUDE : set_ext.mk
.INCLUDE : target.mk
.INCLUDE : tg_ext.mk

