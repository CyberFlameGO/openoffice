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



package installer::languagepack;

use installer::converter;
use installer::existence;
use installer::files;
use installer::globals;
use installer::logger;
use installer::pathanalyzer;
use installer::scpzipfiles;
use installer::scriptitems;
use installer::systemactions;
use installer::worker;

####################################################
# Selecting all files with the correct language
####################################################

sub select_language_items
{
	my ( $itemsref, $languagesarrayref, $itemname ) = @_;

	installer::logger::include_header_into_logfile("Selecting languages for language pack. Item: $itemname");

	my @itemsarray = ();

	for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
	{
		my $oneitem = ${$itemsref}[$i];

		my $ismultilingual = $oneitem->{'ismultilingual'};
		
		if (!($ismultilingual))
		{
			# Files with style "LANGUAGEPACK" and "FORCELANGUAGEPACK" also have to be included into the language pack.
			# Files with style "LANGUAGEPACK" are only included into language packs.
			# Files with style "FORCELANGUAGEPACK" are included into language packs and non language packs. They are
			# forced, because otherwise they not not be included into languagepacks.
			
			my $styles = "";
			if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'}; }
			
			if (( $styles =~ /\bLANGUAGEPACK\b/ ) || ( $styles =~ /\bFORCELANGUAGEPACK\b/ )) { push(@itemsarray, $oneitem); }
					
			next; 	# single language files are not included into language pack
		}

		my $specificlanguage = "";
		if ( $oneitem->{'specificlanguage'} ) { $specificlanguage = $oneitem->{'specificlanguage'}; }
		
		for ( my $j = 0; $j <= $#{$languagesarrayref}; $j++ )	# iterating over all languages
		{
			my $onelanguage = ${$languagesarrayref}[$j];
			my $locallang = $onelanguage;
			$locallang =~ s/-/_/;
			
			if ( $specificlanguage eq $onelanguage )
			{
				# $oneitem->{'modules'} = $installer::globals::rootmodulegid; 	 # all files in a language pack are root files
				# Using $installer::globals::languagemodulesbase (?)

#				# no more automatic change of module assignments
#				$oneitem->{'modules'} = $installer::globals::rootmodulegid . "_$locallang"; 	 # all files in a language pack are root files
#
#				if (( $installer::globals::islinuxbuild ) || ( $installer::globals::issolarispkgbuild ))
#				{
#					if ( $oneitem->{'Dir'} )
#					{
#						if ( $oneitem->{'Dir'} eq "gid_Dir_Fonts_Truetype" ) { $oneitem->{'modules'} = "gid_Module_Langpack_Fonts_$locallang"; }
#						if ( $oneitem->{'Dir'} eq "gid_Dir_Resource" ) { $oneitem->{'modules'} = "gid_Module_Langpack_Resource_$locallang"; }
#						if ( $oneitem->{'Dir'} eq "gid_Dir_Help_Isolanguage" ) { $oneitem->{'modules'} = "gid_Module_Langpack_Help_$locallang"; }
#					}
#				}

				# preparing different modules for Windows Installer language packs
				# my $underlinelanguage = $specificlanguage;
				# $underlinelanguage =~ s/-/_/;
				# if ( $installer::globals::iswindowsbuild ) { $oneitem->{'modules'} = $installer::globals::languagemodulesbase . $underlinelanguage; }

#               # no more collecting of language pack feature				
#				if (! installer::existence::exists_in_array($oneitem->{'modules'}, \@installer::globals::languagepackfeature))
#				{
#					push(@installer::globals::languagepackfeature, $oneitem->{'modules'});	# Collecting all language pack feature 
#				}

				push(@itemsarray, $oneitem); 	
			}
		}
	}

	return \@itemsarray;
}

sub replace_languagestring_variable
{
	my ($onepackageref, $languagestringref) = @_;

	my $key;

	foreach $key (keys %{$onepackageref})
	{
		my $value = $onepackageref->{$key};
		$value =~ s/\%LANGUAGESTRING/$$languagestringref/g;
		$onepackageref->{$key} = $value;			
	}
}

#########################################################
# Including the license text into the script template 
#########################################################

sub put_license_file_into_script
{
	my ($scriptfile, $licensefile) = @_;

	my $infoline = "Adding licensefile into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	my $includestring = "";

	for ( my $i = 0; $i <= $#{$licensefile}; $i++ )
	{
		$includestring = $includestring . ${$licensefile}[$i];
	}

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		${$scriptfile}[$i] =~ s/LICENSEFILEPLACEHOLDER/$includestring/;
	}
}

#########################################################
# Creating a tar.gz file from a Solaris package
#########################################################

sub create_tar_gz_file
{
	my ($installdir, $packagename, $packagestring) = @_;
	
	$packagename =~ s/\.rpm\s*$//;
	my $targzname = $packagename . ".tar.gz";
	$systemcall = "cd $installdir; tar -cf - $packagestring | gzip > $targzname";
	installer::logger::print_message( "... $systemcall ...\n" );

	my $returnvalue = system($systemcall);

	my $infoline = "Systemcall: $systemcall\n";
	push( @installer::globals::logfileinfo, $infoline);
		
	if ($returnvalue)
	{
		$infoline = "ERROR: Could not execute \"$systemcall\"!\n";
		push( @installer::globals::logfileinfo, $infoline);
	}
	else
	{
		$infoline = "Success: Executed \"$systemcall\" successfully!\n";
		push( @installer::globals::logfileinfo, $infoline);
	}
	
	return $targzname;
}

#########################################################
# Determining the name of the package file 
#########################################################

sub get_packagename_from_packagelist
{
	my ( $alldirs, $allvariables, $languagestringref ) = @_;
	
	# my $packagename = "";

	# for ( my $i = 0; $i <= $#{$alldirs}; $i++ )
	# {
	#	if ( ${$alldirs}[$i] =~ /-fonts/ ) { next; } 
	#	if ( ${$alldirs}[$i] =~ /-help/ ) { next; } 
	#	if ( ${$alldirs}[$i] =~ /-res/ ) { next; } 
	#
	#	$packagename = ${$alldirs}[$i];
	#	last;
	# }

	# if ( $packagename eq "" ) { installer::exiter::exit_program("ERROR: Could not find base package in directory $installdir!", "get_packagename_from_packagelist"); } 

	my $localproductname = $allvariables->{'PRODUCTNAME'};
	$localproductname = lc($localproductname);
	$localproductname =~ s/ //g;
	$localproductname =~ s/-/_/g;

	my $packagename = $localproductname . "_" . $$languagestringref;

	return $packagename;
}

#########################################################
# Determining the name of the package file or the rpm
# in the installation directory. For language packs
# there is only one file in this directory 
#########################################################

sub determine_packagename
{
	my ( $installdir, $allvariables, $languagestringref ) = @_;
	
	my $packagename = "";
	my $allnames = "";
	
	if ( $installer::globals::islinuxrpmbuild )
	{
		# determining the rpm file in directory $installdir
			
		my $fileextension = "rpm";
		my $rpmfiles = installer::systemactions::find_file_with_file_extension($fileextension, $installdir);
		if ( ! ( $#{$rpmfiles} > -1 )) { installer::exiter::exit_program("ERROR: Could not find package in directory $installdir!", "determine_packagename"); } 
		my $rpmsav = installer::converter::copy_array_from_references($rpmfiles);
		for ( my $i = 0; $i <= $#{$rpmfiles}; $i++ ) { installer::pathanalyzer::make_absolute_filename_to_relative_filename(\${$rpmfiles}[$i]); }

		$packagename = get_packagename_from_packagelist($rpmfiles, $allvariables, $languagestringref);

		my $packagestring = installer::converter::convert_array_to_space_separated_string($rpmfiles);
		$packagename = create_tar_gz_file($installdir, $packagename, $packagestring);	# only one file
		for ( my $i = 0; $i <= $#{$rpmsav}; $i++ )
		{
			my $onefile = $installdir . $installer::globals::separator . ${$rpmsav}[$i];
			unlink($onefile);
		}
		
		$allnames = $rpmfiles;
	}

	if ( $installer::globals::issolarisbuild )
	{
		# determining the Solaris package file in directory $installdir
		my $alldirs = installer::systemactions::get_all_directories($installdir);

		if ( ! ( $#{$alldirs} > -1 )) { installer::exiter::exit_program("ERROR: Could not find package in directory $installdir!", "determine_packagename"); } 
		my $alldirssav = installer::converter::copy_array_from_references($alldirs);
		for ( my $i = 0; $i <= $#{$alldirs}; $i++ ) { installer::pathanalyzer::make_absolute_filename_to_relative_filename(\${$alldirs}[$i]); }

		$packagename = get_packagename_from_packagelist($alldirs, $allvariables, $languagestringref);
		my $packagestring = installer::converter::convert_array_to_space_separated_string($alldirs);
		$packagename = create_tar_gz_file($installdir, $packagename, $packagestring);	# only a file (not a directory) can be included into the shell script
		for ( my $i = 0; $i <= $#{$alldirssav}; $i++ ) { installer::systemactions::remove_complete_directory(${$alldirssav}[$i], 1); }
		$allnames = $alldirs;
	}
	
	my $infoline = "Found package in installation directory $installdir : $packagename\n";
	push( @installer::globals::logfileinfo, $infoline);

	return ( $packagename, $allnames);
}

#########################################################
# Including the name of the package file or the rpm
# into the script template 
#########################################################

sub put_packagename_into_script
{
	my ($scriptfile, $packagename, $allnames) = @_;

	my $localpackagename = $packagename;
	$localpackagename =~ s/\.tar\.gz//;	# making "OOOopenoffice-it-ea.tar.gz" to "OOOopenoffice-it-ea"
	my $infoline = "Adding packagename $localpackagename into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	my $installline = "";

	if ( $installer::globals::issolarisbuild ) { $installline = "  /usr/sbin/pkgadd -d \$outdir -a \$adminfile"; }

	if ( $installer::globals::islinuxrpmbuild ) { $installline = "  rpm --prefix \$PRODUCTINSTALLLOCATION --replacepkgs -i"; }		

	for ( my $i = 0; $i <= $#{$allnames}; $i++ )
	{
		if ( $installer::globals::issolarisbuild ) { $installline = $installline . " ${$allnames}[$i]"; }

		if ( $installer::globals::islinuxrpmbuild ) { $installline = $installline . " \$outdir/${$allnames}[$i]"; }
	}	

	for ( my $j = 0; $j <= $#{$scriptfile}; $j++ )
	{
		${$scriptfile}[$j] =~ s/INSTALLLINES/$installline/;
	}
}

##################################################################
# Including the lowercase product name into the script template 
##################################################################

sub put_productname_into_script
{
	my ($scriptfile, $variableshashref) = @_;
	
	my $productname = $variableshashref->{'PRODUCTNAME'};
	$productname = lc($productname);
	$productname =~ s/\.//g;	# openoffice.org -> openofficeorg
	
	my $infoline = "Adding productname $productname into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		${$scriptfile}[$i] =~ s/PRODUCTNAMEPLACEHOLDER/$productname/;
	}
}

##################################################################
# Including the full product name into the script template
# (name and version) 
##################################################################

sub put_fullproductname_into_script
{
	my ($scriptfile, $variableshashref) = @_;

	my $productname = $variableshashref->{'PRODUCTNAME'};
	my $productversion = "";
	if ( $variableshashref->{'PRODUCTVERSION'} ) { $productversion = $variableshashref->{'PRODUCTVERSION'}; };
	my $fullproductname = $productname . " " . $productversion;

	my $infoline = "Adding full productname \"$fullproductname\" into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		${$scriptfile}[$i] =~ s/FULLPRODUCTNAMELONGPLACEHOLDER/$fullproductname/;
	}
}

##################################################################
# Including the name of the search package (-core01)
# into the script template 
##################################################################

sub put_searchpackage_into_script
{
	my ($scriptfile, $variableshashref) = @_;
	
	my $basispackageprefix = $variableshashref->{'BASISPACKAGEPREFIX'};
	my $basispackageversion = $variableshashref->{'OOOBASEVERSION'};

	if ( $installer::globals::issolarisbuild ) { $basispackageversion =~ s/\.//g; }	# "3.0" -> "30"

	my $infoline = "Adding basis package prefix $basispackageprefix into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	$infoline = "Adding basis package version $basispackageversion into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		${$scriptfile}[$i] =~ s/BASISPACKAGEPREFIXPLACEHOLDER/$basispackageprefix/;
		${$scriptfile}[$i] =~ s/OOOBASEVERSIONPLACEHOLDER/$basispackageversion/;
	}
	
}

#########################################################
# Including the linenumber into the script template 
#########################################################

sub put_linenumber_into_script
{
	my ( $scriptfile, $licensefile, $allnames ) = @_;
	
	my $linenumber =  $#{$scriptfile} + $#{$licensefile} + 3;	# also adding the content of the license file!

	my $infoline = "Adding linenumber $linenumber into language pack script\n";
	push( @installer::globals::logfileinfo, $infoline);

	for ( my $i = 0; $i <= $#{$scriptfile}; $i++ )
	{
		${$scriptfile}[$i] =~ s/LINENUMBERPLACEHOLDER/$linenumber/;
	}	
}

#########################################################
# Determining the name of the new scriptfile 
#########################################################

sub determine_scriptfile_name
{
	my ( $packagename ) = @_;
	
	my $scriptfilename = $packagename;
	
#	if ( $installer::globals::islinuxrpmbuild ) { $scriptfilename =~ s/\.rpm\s*$/\.sh/; }
#	if ( $installer::globals::issolarisbuild ) { $scriptfilename =~ s/\.tar\.gz\s*$/\.sh/; }

	$scriptfilename =~ s/\.tar\.gz\s*$/\.sh/;

	my $infoline = "Setting language pack script file name to $scriptfilename\n";
	push( @installer::globals::logfileinfo, $infoline);

	return $scriptfilename;
}

#########################################################
# Saving the script file in the installation directory 
#########################################################

sub save_script_file
{
	my ($installdir, $newscriptfilename, $scriptfile) = @_;
	
	$newscriptfilename = $installdir . $installer::globals::separator . $newscriptfilename;
	installer::files::save_file($newscriptfilename, $scriptfile);
	
	my $infoline = "Saving script file $newscriptfilename\n";
	push( @installer::globals::logfileinfo, $infoline);
	
	return $newscriptfilename;
}

#########################################################
# Including the binary package into the script 
#########################################################

sub include_package_into_script
{
	my ( $scriptfilename, $installdir, $packagename ) = @_;
	
	my $longpackagename = $installdir . $installer::globals::separator . $packagename;
	my $systemcall = "cat $longpackagename >>$scriptfilename";

	my $returnvalue = system($systemcall);

	my $infoline = "Systemcall: $systemcall\n";
	push( @installer::globals::logfileinfo, $infoline);
		
	if ($returnvalue)
	{
		$infoline = "ERROR: Could not execute \"$systemcall\"!\n";
		push( @installer::globals::logfileinfo, $infoline);
	}
	else
	{
		$infoline = "Success: Executed \"$systemcall\" successfully!\n";
		push( @installer::globals::logfileinfo, $infoline);
	}
	
	my $localcall = "chmod 775 $scriptfilename \>\/dev\/null 2\>\&1";
	system($localcall);

}

#########################################################
# Removing the binary package 
#########################################################

sub remove_package
{
	my ( $installdir, $packagename ) = @_;

	my $remove_package = 1;

	if ( $ENV{'DONT_REMOVE_PACKAGE'} ) { $remove_package = 0; }

	if ( $remove_package )
	{
		my $longpackagename = $installdir . $installer::globals::separator . $packagename;
		unlink $longpackagename;

		my $infoline = "Removing package: $longpackagename \n";
		push( @installer::globals::logfileinfo, $infoline);
	}
}

####################################################
# Unix language packs, that are not part of 
# multilingual installation sets, need a
# shell script installer
####################################################

sub build_installer_for_languagepack
{
	my ($installdir, $allvariableshashref, $includepatharrayref, $languagesarrayref, $languagestringref) = @_;

	installer::logger::print_message( "... creating shell script installer ...\n" );

	installer::logger::include_header_into_logfile("Creating shell script installer:");

	# find and read setup script template
	
	my $scriptfilename = "langpackscript.sh";
	my $scriptref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$scriptfilename, $includepatharrayref, 0);
	if ($$scriptref eq "") { installer::exiter::exit_program("ERROR: Could not find script file $scriptfilename!", "build_installer_for_languagepack"); }
	my $scriptfile = installer::files::read_file($$scriptref);

	my $infoline = "Found  script file $scriptfilename: $$scriptref \n";
	push( @installer::globals::logfileinfo, $infoline);

	# find and read english license file
	my $licenselanguage = "en-US";					# always english !
	my $licensefilename = "LICENSE";
	my $licenseincludepatharrayref = installer::worker::get_language_specific_include_pathes($includepatharrayref, $licenselanguage);

	my $licenseref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$licensefilename, $licenseincludepatharrayref, 0);
	if ($$licenseref eq "") { installer::exiter::exit_program("ERROR: Could not find License file $licensefilename!", "build_installer_for_languagepack"); }
	my $licensefile = installer::files::read_file($$licenseref);

	$infoline = "Found licensefile $licensefilename: $$licenseref \n";
	push( @installer::globals::logfileinfo, $infoline);

	# including variables into license file
	installer::scpzipfiles::replace_all_ziplistvariables_in_file($licensefile, $allvariableshashref);

	# add license text into script template
	put_license_file_into_script($scriptfile, $licensefile);

	# add rpm or package file name into script template
	my ( $packagename, $allnames) = determine_packagename($installdir, $allvariableshashref, $languagestringref);
	put_packagename_into_script($scriptfile, $packagename, $allnames);

	# add product name into script template
	put_productname_into_script($scriptfile, $allvariableshashref);

	# add product name into script template
	put_fullproductname_into_script($scriptfile, $allvariableshashref);

	# add product name into script template
	put_searchpackage_into_script($scriptfile, $allvariableshashref);

	# replace linenumber in script template
	put_linenumber_into_script($scriptfile, $licensefile, $allnames);
	
	# saving the script file
	my $newscriptfilename = determine_scriptfile_name($packagename);
	$newscriptfilename = save_script_file($installdir, $newscriptfilename, $scriptfile);

	# include rpm or package into script
	include_package_into_script($newscriptfilename, $installdir, $packagename);
	
	# remove rpm or package
	remove_package($installdir, $packagename);
}

1;
