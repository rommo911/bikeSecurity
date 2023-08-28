@ECHO OFF
setlocal enabledelayedexpansion
TITLE OTA uploader
If "%~1"=="" ( set port=443)  ELSE  ( set port=%1  )

If "%~2"=="" ( set file= build/blind_socket.bin )  ELSE  ( set file=%2  )
If "%~2"=="" ( ECHO using default file in %file% )  ELSE  ( ECHO using file %2  )

call :TRIM port
call :TRIM file

set ESP073D64=192.168.1.14
set ESP06E4D0=192.168.1.18
set ESPF70148=192.168.1.15
set ESP3D22D4=192.168.1.22

ECHO curl %ESP073D64%:%port%  %file%
curl %ESP073D64%:%port% --http0.9  --verbos -H "token: MYPASSWORD"  --data-binary @- < %file%

ECHO curl %ESP06E4D0%:%port%  %file%
curl %ESP06E4D0%:%port% --http0.9  --verbos -H "token: MYPASSWORD" --data-binary @- < %file%

ECHO curl %ESPF70148%:%port%  %file%
curl %ESPF70148%:%port% --http0.9  --verbos -H "token: MYPASSWORD"  --data-binary @- < %file%

ECHO curl %ESP3D22D4%:%port% %file%
curl %ESP3D22D4%:%port% --http0.9  --verbos -H "token: MYPASSWORD" --data-binary @- < %file%

ECHO exiting 
PAUSE 


:TRIM
SetLocal EnableDelayedExpansion
Call :TRIMSUB %%%1%%
EndLocal & set %1=%tempvar%
GOTO :EOF

:TRIMSUB
set tempvar=%*
GOTO :EOF
