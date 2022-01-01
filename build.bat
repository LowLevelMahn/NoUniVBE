rem source folder
set src_dir=%~dp0
rem out of source build folder
set build_dir=%~dp0\..\_build
mkdir %build_dir%

set tools_dir=f:\projects\fun\dos_games_rev\tools

set link_exe=%tools_dir%\mslink\link.exe
set ulink_exe=%tools_dir%\ulink\ulink.exe
set uasm_exe=%tools_dir%\uasm_x64\uasm64.exe

pushd %build_dir%

%uasm_exe% %src_dir%\NoUniVBE.asm

pause

rem building with VS2017/2019 MASM
rem call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
rem ml.exe /c /omf NoUniVBE.asm -o %build_dir%\NoUniVBE.obj

rem %link_exe% /MAP NoUniVBE.obj,NoUniVBE.com,NoUniVBE.map,,,
%ulink_exe% NoUniVBE.obj, NoUniVBE.exe, NoUniVBE.map

pause

set WATCOM=%tools_dir%\open-watcom-2_0-c-win-x64
set WATCOM_BIN=%watcom%\binnt64
set INCLUDE=%watcom%\h
set PATH=%WATCOM_BIN%;%PATH%
set wcc_exe=%watcom_bin%\wcc.exe
set wcl_exe=%watcom_bin%\wcl.exe

%wcl_exe% -bt=dos -mm %src_dir%\UVBCHECK.C

pause

popd
