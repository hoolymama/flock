# This file should be sourced in bash.
# No csh option currently exists.

# append FLOCK_PROJECT_PATH bin paths to PATH
export PATH=$PATH:${FLOCK_PROJECT_PATH}/bin

# append FLOCK_PROJECT_PATH maya module path
if [[ -n $MAYA_MODULE_PATH ]] ;
   	then export MAYA_MODULE_PATH=${FLOCK_PROJECT_PATH}/maya:${MAYA_MODULE_PATH} ;
   	else export MAYA_MODULE_PATH=${FLOCK_PROJECT_PATH}/maya ;
fi

# append FLOCK_PROJECT_PATH nuke path
#if [[ -n $NUKE_PATH ]] ;
#	then export NUKE_PATH=${FLOCK_PROJECT_PATH}/nuke/scripts:${NUKE_PATH} ;
#	else export NUKE_PATH=${FLOCK_PROJECT_PATH}/nuke/scripts ;
#fi
