#!/bin/tcsh

#TODO - rewrite in ruby

# INSTALL script for bash
# Does the following.
# A new file ".flak" is created in home dir if it doesn't exist.
# adds line at bottom of .bashrc to source .flak
# Commands to source this tool's env.csh are added to .flak.

set rootdir = `dirname $0`
set rootdir = `cd $rootdir && pwd`    # ensure absolute path
set basedir = `basename $0`

# set full_path_to_this_script = `echo $0 | sed "s@^./@$PWD/@"`
set full_path_to_this_script = ${rootdir}/${basedir}
echo "full path to script : ${full_path_to_this_script}"

set tools = $full_path_to_this_script:h:h:h:h
set product_version = $full_path_to_this_script:h:h:t
set product =  $full_path_to_this_script:h:h:h:t
set product_install_dir = ${tools}/${product}
set product_up = `echo $product | tr '[a-z]' '[A-Z]'`

echo "**************************************************************" 
echo ""

echo  "working from : ${tools}"
pushd ${tools}

echo ${product}" install directory is: ${product_install_dir}"

set the_product_dir = ${product_install_dir}/${product_version}

# add source .flak to .bashrc
set bashrc = ${HOME}/.bashrc
if ( ! -e ${bashrc} )  then
	touch ${bashrc}
endif

set flaksrc = '# added by flak'
set res = `grep  "${flaksrc}" ${HOME}/.bashrc`

if ( "${res}" ==  ""  ) then	
	echo "" >>   ${HOME}/.bashrc
	echo '[[ -s "$HOME/.flak" ]] && . "$HOME/.flak" # added by flak' >>  ${HOME}/.bashrc
endif

# add source product env to .flak
set flakenv = ${HOME}/.flak

if (-f ${flakenv}) then
	cp  ${flakenv}  ${flakenv}.${product}.bck
	cat ${flakenv}.${product}.bck | grep -v "#.*added by ${product}" > ${flakenv}
else
	touch ${flakenv}
endif


echo "appending ${product} source commands to ${flakenv}"
echo ""  >> ${flakenv}
echo "# The following lines have been added by ${product}"  >> ${flakenv}
echo 'export '${product_up}'_PROJECT_PATH="'${the_product_dir}'" # added by '${product} >> ${flakenv}
echo '[[ -e "${'${product_up}'_PROJECT_PATH}/config/env.sh" ]] && . "${'${product_up}'_PROJECT_PATH}/config/env.sh" # added by '${product} >>  ${flakenv}

echo ""
echo "*********NOW RUN THESE COMMANDS OR (preferebly) OPEN A NEW SHELL***********" 
echo ""
echo "export ${product_up}_PROJECT_PATH=${the_product_dir}"
echo '. ${'${product_up}'_PROJECT_PATH}/bin/env.sh'
echo ""
echo "**************************************************************" 


popd


