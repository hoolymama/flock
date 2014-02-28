
@ECHO OFF

SET CONFIG_DIR=%~dp0

CALL :RESOLVE "%CONFIG_DIR%\.." MODULE_LOCATION

CALL :EXTRACT "%MODULE_LOCATION%" VERSION_STRING

CALL :EXTRACT "%MODULE_LOCATION%\.." PROJECT_NAME

echo ---------------------------------------------
echo Setting up %PROJECT_NAME%
echo ---------------------------------------------


echo --------------------MAKING MODULE FILE---------
set maya_module_dir=%USERPROFILE%\Documents\maya\modules
mkdir %maya_module_dir%
echo + %PROJECT_NAME% %VERSION_STRING% %MODULE_LOCATION%\maya > "%maya_module_dir%\%PROJECT_NAME%"
echo Created  %maya_module_dir%\%PROJECT_NAME%
echo ---------------------------------------------

GOTO :EOF

:RESOLVE
SET %2=%~f1
GOTO :EOF


:EXTRACT
SET %2=%~nx1
GOTO :EOF

