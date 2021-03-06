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



#
# check - a perl script to check some files and directories if they exists
# A first simple check if the SDK was built completely.
#
#use IO::File;

$return = 0;

$StartDir = "$ARGV[0]";
$OperatingSystem = "$ARGV[1]";
$ExePrefix = "$ARGV[2]";

print "Check for $OperatingSystem\n";

if (-d "$StartDir") {
    # check binaries
    print "check binaries: ";
    if (-d "$StartDir/bin") {
	my @binarylist = ( "idlc","ucpp","cppumaker","javamaker",
			   "regcompare","autodoc",
			   "unoapploader", "uno-skeletonmaker" );

	foreach $i (@binarylist)
	{
	    if (! -e "$StartDir/bin/$i$ExePrefix") {
		$return++;
		print "\nERROR: \"$StartDir/bin/$i$ExePrefix\" is missing\n";
	    } else {
		print "+";
	    }
	}

	if ($OperatingSystem eq "windows" || $OperatingSystem eq "mingw") {
	    if ($OperatingSystem eq "windows" && ! -e "$StartDir/bin/climaker.exe") {
		$return++;
		print "\nERROR: \"$StartDir/bin/climaker.exe\" is missing\n";
	    } else {
		print "+";
	    }
	}
	if ($OperatingSystem eq "macosx") {
	    if (! -e "$StartDir/bin/addsym-macosx.sh") {
		$return++;
		print "\nERROR: \"$StartDir/bin/addsym-macosx.sh\" is missing\n";
	    } else {
		print "+";
	    }
	}
    } else {
	$return++;
    }
    print "\n";

    # packaging files
    print "check packaging files: ";
    if (-d "$StartDir/docs") {
	my @filelist = ( "install.html",
			 "notsupported.html","sdk_styles.css","tools.html",
			 "images/arrow-1.gif", "images/arrow-3.gif",
			 "images/odk-footer-logo.gif", 
			 "images/bg_table.png","images/bg_table2.png",
			 "images/bg_table3.png", "images/nav_down.png",
			 "images/nav_home.png","images/nav_left.png",
			 "images/nav_right.png","images/nav_up.png",
			 "images/sdk_head-1.png", "images/sdk_head-2.png",
			 "images/sdk_line-1.gif", "images/sdk_line-2.gif", 
			 "common/ref/idl.css", "images/nada.gif",
			 "images/arrow-2.gif", "images/bluball.gif",
			 "images/ooo-main-app_32.png");

	foreach $i (@filelist)
	{
	    if (! -e "$StartDir/docs/$i") {
		$return++;
		print "\nERROR: \"$StartDir/docs/$i\" is missing\n";
	    } else {
		print "+";
	    }
	}
    } else {
	$return++;
    }
    print "\n";

    #check configure files
    print "check config files: ";
    if ($OperatingSystem eq "windows" || $OperatingSystem eq "mingw") {
	if (! -e "$StartDir/setsdkenv_windows.bat") {
	    print "\nERROR: \"$StartDir/setsdkenv_windows.bat\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/setsdkenv_windows.template") {
	    print "\nERROR: \"$StartDir/setsdkenv_windows.template\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/cfgWin.js") {
	    print "\nERROR: \"$StartDir/cfgWin.js\" is missing\n";
	    $return++;
	}
    } else {
	if (! -e "$StartDir/configure.pl") {
	    print "\nERROR: \"$StartDir/configure.pl\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/config.guess") {
	    print "\nERROR: \"$StartDir/config.guess\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/config.sub") {
	    print "\nERROR: \"$StartDir/config.sub\" is missing\n";
	    $return++;
       }
	if (! -e "$StartDir/setsdkenv_unix") {
	    print "\nERROR: \"$StartDir/setsdkenv_unix\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/setsdkenv_unix.sh.in") {
	    print "\nERROR: \"$StartDir/setsdkenv_unix.sh.in\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/setsdkenv_unix.csh.in") {
	    print "\nERROR: \"$StartDir/setsdkenv_unix.csh.in\" is missing\n";
	    $return++;
	}
    }
    print "\n";

    #check setting files
    print "check setting files: ";
    if (-d "$StartDir/settings") {
	if (! -e "$StartDir/settings/settings.mk") {
	    print "\nERROR: \"$StartDir/settings/settings.mk\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/settings/std.mk") {
	    print "\nERROR: \"$StartDir/settings/std.mk\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/settings/stdtarget.mk") {
	    print "\nERROR: \"$StartDir/settings/stdtarget.mk\" is missing\n";
	    $return++;
	}
    } else {
	$return++;
    }
    print "\n";

    #check cpp docu, it is only a first and simple check
    # improvement required
    print "check cpp docu: ";
    if (-d "$StartDir/docs/cpp/ref") {
	if (! -e "$StartDir/docs/cpp/ref/index.html") {
	    print "\nERROR: \"$StartDir/docs/cpp/ref/index.html\" is missing\n";
	    $return++;
	}
	if (! -d "$StartDir/docs/cpp/ref/index-files") {
	    print "\nERROR: \"$StartDir/docs/cpp/ref/index-files\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/docs/cpp/ref/index-files/index-10.html") {
	    print "\nERROR: \"$StartDir/docs/cpp/ref/index-files/index-10.html\" is missing\n";
	    $return++;
	}

	my @dir_list = ( "com","com/sun","com/sun/star","com/sun/star/uno",
			 "com/sun/star/uno/Any","com/sun/star/uno/Type",
			 "com/sun/star/uno/Array","com/sun/star/uno/WeakReferenceHelper",
			 "com/sun/star/uno/Reference","com/sun/star/uno/WeakReference",
			 "com/sun/star/uno/Environment","com/sun/star/uno/Sequence",
			 "com/sun/star/uno/BaseReference","com/sun/star/uno/Mapping",
			 "com/sun/star/uno/ContextLayer","com/sun/star/uno/TypeDescription",
			 "osl","osl/File","osl/Pipe","osl/FileStatus","osl/FileBase",
			 "osl/Guard","osl/Mutex","osl/VolumeInfo","osl/GetGlobalMutex",
			 "osl/Security","osl/Profile","osl/DatagramSocket","osl/SocketAddr",
			 "osl/StreamPipe","osl/ResettableGuard","osl/AcceptorSocket",
			 "osl/ClearableGuard","osl/VolumeDevice","rtl","rtl/Uri","rtl/math",
			 "rtl/OUStringHash","rtl/MalformedUriException","rtl/OUStringBuffer",
			 "rtl/OUString","rtl/Reference","rtl/ByteSequence","rtl/OLocale",
			 "rtl/Logfile","rtl/OString","rtl/IReference","rtl/OStringBuffer",
			 "rtl/OStringHash","_typelib_CompoundTypeDescription","cppu",
			 "cppu/ContextEntry_Init","cppu/ImplInheritanceHelper10",
			 "cppu/ImplInheritanceHelper11","cppu/ImplInheritanceHelper12",
			 "cppu/WeakAggImplHelper1","cppu/WeakAggImplHelper2",
			 "cppu/WeakAggImplHelper3","cppu/WeakAggImplHelper4",
			 "cppu/WeakAggImplHelper5","cppu/WeakAggImplHelper6",
			 "cppu/WeakAggImplHelper7","cppu/WeakAggImplHelper8",
			 "cppu/WeakAggImplHelper9",
			 "cppu/OMultiTypeInterfaceContainerHelperInt32","cppu/AccessControl",
			 "cppu/OPropertyArrayHelper","cppu/ImplHelper1","cppu/ImplHelper2",
			 "cppu/ImplHelper3","cppu/ImplHelper4","cppu/ImplHelper5",
			 "cppu/ImplHelper6","cppu/ImplHelper7","cppu/ImplHelper8",
			 "cppu/ImplHelper9","cppu/WeakComponentImplHelper10",
			 "cppu/WeakComponentImplHelper11","cppu/WeakComponentImplHelper12",
			 "cppu/UnoUrl","cppu/WeakComponentImplHelper1",
			 "cppu/WeakComponentImplHelper2","cppu/WeakComponentImplHelper3",
			 "cppu/WeakComponentImplHelper4","cppu/WeakComponentImplHelper5",
			 "cppu/WeakComponentImplHelper6","cppu/WeakComponentImplHelper7",
			 "cppu/WeakComponentImplHelper8","cppu/WeakComponentImplHelper9",
			 "cppu/OInterfaceIteratorHelper",
			 "cppu/OMultiTypeInterfaceContainerHelper","cppu/UnoUrlDescriptor",
			 "cppu/IPropertyArrayHelper","cppu/OBroadcastHelperVar",
			 "cppu/OComponentHelper","cppu/OWeakAggObject",
			 "cppu/ImplementationEntry","cppu/WeakImplHelper10",
			 "cppu/WeakImplHelper11","cppu/WeakImplHelper12",
			 "cppu/OPropertySetHelper","cppu/ImplHelper10","cppu/ImplHelper11",
			 "cppu/ImplHelper12","cppu/WeakAggImplHelper10",
			 "cppu/WeakAggImplHelper11","cppu/WeakAggImplHelper12",
			 "cppu/ImplInheritanceHelper1","cppu/ImplInheritanceHelper2",
			 "cppu/ImplInheritanceHelper3","cppu/ImplInheritanceHelper4",
			 "cppu/ImplInheritanceHelper5","cppu/ImplInheritanceHelper6",
			 "cppu/ImplInheritanceHelper7","cppu/ImplInheritanceHelper8",
			 "cppu/ImplInheritanceHelper9","cppu/OTypeCollection",
			 "cppu/WeakAggComponentImplHelper10",
			 "cppu/WeakAggComponentImplHelper11",
			 "cppu/WeakAggComponentImplHelper12",
			 "cppu/WeakAggComponentImplHelper1",
			 "cppu/WeakAggComponentImplHelper2",
			 "cppu/WeakAggComponentImplHelper3",
			 "cppu/WeakAggComponentImplHelper4",
			 "cppu/WeakAggComponentImplHelper5",
			 "cppu/WeakAggComponentImplHelper6",
			 "cppu/WeakAggComponentImplHelper7",
			 "cppu/WeakAggComponentImplHelper8",
			 "cppu/WeakAggComponentImplHelper9",
			 "cppu/OMultiTypeInterfaceContainerHelperVar",
			 "cppu/OInterfaceContainerHelper","cppu/OImplementationId",
			 "cppu/AggImplInheritanceHelper1","cppu/AggImplInheritanceHelper2",
			 "cppu/AggImplInheritanceHelper3","cppu/AggImplInheritanceHelper4",
			 "cppu/AggImplInheritanceHelper5","cppu/AggImplInheritanceHelper6",
			 "cppu/AggImplInheritanceHelper7","cppu/AggImplInheritanceHelper8",
			 "cppu/AggImplInheritanceHelper9","cppu/AggImplInheritanceHelper10",
			 "cppu/AggImplInheritanceHelper11","cppu/AggImplInheritanceHelper12",
			 "cppu/WeakImplHelper1","cppu/WeakImplHelper2","cppu/WeakImplHelper3",
			 "cppu/WeakImplHelper4","cppu/WeakImplHelper5","cppu/WeakImplHelper6",
			 "cppu/WeakImplHelper7","cppu/WeakImplHelper8","cppu/WeakImplHelper9",
			 "cppu/OWeakObject","__store_FindData","_rtl_StandardModuleCount",
			 "RTUik","RTConstValue","_typelib_TypeDescriptionReference",
			 "_typelib_InterfaceMethodTypeDescription","store","RegistryKey",
			 "_typelib_Union_Init","_sal_Sequence","_typelib_Parameter_Init",
			 "_typelib_TypeDescription","_uno_Environment",
			 "_typelib_InterfaceAttributeTypeDescription",
			 "_rtl_ModuleCount","_uno_ExtEnvironment",
			 "_typelib_IndirectTypeDescription",
			 "Registry_Api","_oslFileStatus",
			 "_typelib_InterfaceMemberTypeDescription","RegistryValueList",
			 "RegistryTypeWriter_Api","_rtl_TextEncodingInfo",
			 "namespace_anonymous_1",
			 "_oslVolumeInfo","_uno_Interface",
			 "_typelib_InterfaceTypeDescription","_uno_Mapping","Registry",
			 "RegistryTypeReader_Api","_typelib_Uik",
			 "_typelib_ArrayTypeDescription",
			 "RegistryKeyArray","RegistryTypeReader","RegistryKeyNames",
			 "RTConstValueUnion","_typelib_UnionTypeDescription","_uno_Any",
			 "RegistryTypeWriter","_rtl_Locale","_typelib_CompoundMember_Init",
			 "_typelib_EnumTypeDescription","_typelib_MethodParameter");

	foreach $i (@dir_list)
	{
	    if (! -d "$StartDir/docs/cpp/ref/names/$i") {
		$return++;
		print "\nERROR: \"$StartDir/docs/cpp/ref/names/$i\" is missing\n";
	    } else {
		print "+";
	    }
	}
    } else {
	$return++;
    }
    print "\n";

    #check java docu, it is only a first and simple check
    # improvement required
    my $solar_java = $ENV{"SOLAR_JAVA"};
    my $JDK = $ENV{"JDK"};
    if (defined($solar_java) && $solar_java ne "" && (!defined($JDK) || $JDK ne "gcj")) {
	print "check java docu: ";
	if (-d "$StartDir/docs/java/ref") {
	    if (! -e "$StartDir/docs/java/ref/index.html") {
		print "\nERROR: \"$StartDir/docs/java/ref/index.html\" is missing\n";
		$return++;
	    }

	    my @dir_list = ( "lib","lib/uno","lib/uno/helper","lib/uno/helper/class-use",
			     "uno","uno/class-use","comp","comp/helper",
			     "comp/helper/class-use");
	    
	    foreach $i (@dir_list) 
	    {
		if (! -d "$StartDir/docs/java/ref/com/sun/star/$i") {
		    $return++;
		    print "\nERROR: \"$StartDir/docs/java/ref/com/sun/star/$i\" is missing\n";
		} else {
		    print "+";
		}
	    }
	} else {
	    $return++;
	}
	print "\n";
    }

    #check idl docu, it is only a first and simple check
    # improvement required
    print "check idl docu: ";
    if (-d "$StartDir/docs/common/ref") {
	if (! -e "$StartDir/docs/common/ref/module-ix.html") {
	    print "\nERROR: \"$StartDir/docs/common/ref/module-ix.html\" is missing\n";
	    $return++;
	}
	if (! -d "$StartDir/docs/common/ref/index-files") {
	    print "\nERROR: \"$StartDir/docs/common/ref/index-files\" is missing\n";
	    $return++;
	}
	if (! -e "$StartDir/docs/common/ref/index-files/index-10.html") {
	    print "\nERROR: \"$StartDir/docs/common/ref/index-files/index-10.html\" is missing\n";
	    $return++;
	}

	my @idl_dirlist = ( "accessibility", 
			    "animations",
			    "auth",
			    "awt",
			    "awt/tab",
			    "awt/tree",
			    "awt/grid",
			    "beans",
			    "bridge",
			    "bridge/oleautomation",
			    "chart",
			    "chart2",
			    "chart2/data",
			    "configuration",
			    "configuration/backend",
			    "configuration/backend/xml",
			    "configuration/bootstrap",
			    "connection",
			    "container",
			    "datatransfer",
			    "datatransfer/clipboard",
			    "datatransfer/dnd",
			    "deployment",
			    "deployment/ui",
			    "document",
			    "drawing",
			    "drawing/framework",
			    "embed",
			    "form",
			    "form/binding",
			    "form/component",
			    "form/control",
			    "form/inspection",
			    "form/runtime",
			    "form/submission",
			    "form/validation",
			    "formula",
			    "frame",
			    "frame/status",
			    "gallery",
			    "geometry",
			    "graphic",
			    "i18n",
			    "image",
			    "inspection",
			    "installation",
			    "io",
			    "java",
			    "lang",
			    "ldap",
			    "linguistic2",
			    "loader",
			    "logging",
			    "mail",
			    "media",
			    "mozilla",
			    "packages",
			    "packages/manifest",
			    "packages/zip",
			    "plugin",
			    "presentation",
			    "reflection",
			    "registry",
			    "rendering",
			    "report",
			    "report/inspection",
			    "resource",
			    "scanner",
			    "script",
			    "script/browse",
			    "script/provider",
			    "sdb",
			    "sdb/application",
			    "sdb/tools",
			    "sdbc",
			    "sdbcx",
			    "security",
			    "setup",
			    "sheet",
			    "smarttags",
			    "style",
			    "svg",
			    "sync",
			    "sync2",
			    "system",
			    "table",
			    "task",
			    "test",
			    "test/bridge",
			    "test/performance",
			    "text",
			    "text/fieldmaster",
			    "text/textfield",
			    "text/textfield/docinfo",
			    "ucb",
			    "ui",
			    "ui/dialogs",
			    "uno",
			    "uri",
			    "util",
			    "util/logging",
			    "view",
			    "xforms",
			    "xml",
			    "xml/crypto",
			    "xml/crypto/sax",
			    "xml/csax",
			    "xml/dom",
			    "xml/dom/events",
			    "xml/dom/views",
			    "xml/input",
			    "xml/sax",
			    "xml/wrapper",
			    "xml/xpath",
			    "xsd" );

	foreach $i (@idl_dirlist)
	{
	    if (! -d "$StartDir/docs/common/ref/com/sun/star/$i") {
		$return++;
		print "\nERROR: \"$StartDir/docs/common/ref/com/sun/star/$i\" is missing\n";
	    } else {
		print "+";
	    }
	}
    } else {
	$return++;
    }
    print "\n";

} else {
    $return++;
}

if( $return != 0 )
{
    print "ERROR\n";
    unlink "$ARGV[3]";
} else {
    print "OK\n";
}
exit $return;
