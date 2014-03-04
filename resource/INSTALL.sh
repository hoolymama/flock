#!/bin/bash

#TODO - rewrite in ruby

# INSTALL script for bash
# Write lines at the bottom of bashrc to source the env.sh file

 # ensure absolute path to this file and other useful places
rootdir=`dirname $0`
rootdir=`cd $rootdir && pwd`   
basefn=`basename $0`

full_path_to_this_script=${rootdir}/${basefn}
echo "full path to script : ${full_path_to_this_script}"

tools=$(dirname $(dirname $(dirname $(dirname $full_path_to_this_script))))
product_version=$(basename $(dirname $(dirname $full_path_to_this_script)))
product=$(basename $(dirname $(dirname $(dirname $full_path_to_this_script))))
product_install_dir=${tools}/${product}
product_up=`echo $product | tr '[a-z]' '[A-Z]'`

echo "**************************************************************" 
echo ""

echo  "working from : ${tools}"
pushd ${tools}

echo ${product}" install directory is: ${product_install_dir}"

the_product_dir=${product_install_dir}/${product_version}

# add source .flak cmds to .bashrc
bashrc=${HOME}/.bashrc
if [ ! -f ${bashrc} ]; then
	touch ${bashrc}
fi

flaksrc='# added by flak'
res=`grep "${flaksrc}" ${HOME}/.bashrc`
if [ -z "${res}" ]; then
	echo "" >>   ${bashrc}
	echo '[[ -s "$HOME/.flak" ]] && . "$HOME/.flak" # added by flak' >>  ${bashrc}
fi

# remove old entries for this tool.
# remove blank lines at top of .flak file.
flakenv=${HOME}/.flak
if [ -f ${flakenv} ]; then
	cp  ${flakenv}  ${flakenv}.${product}.bck
	cat ${flakenv}.${product}.bck | grep -v "#.*added by ${product}" | sed '/./,$!d' > ${flakenv}
else
	touch ${flakenv}
fi

# add source product env lines to .flak
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


