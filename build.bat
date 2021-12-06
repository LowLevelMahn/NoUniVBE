set link_exe=f:\projects\fun\dos_games_rev\tools\mslink\link.exe
set ulink_exe=f:\projects\fun\dos_games_rev\tools\ulink\ulink.exe
set uasm_exe=f:\projects\fun\dos_games_rev\tools\uasm_x64\uasm64.exe

%uasm_exe% NoUniVBE.asm

rem call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
rem ml.exe /c /omf NoUniVBE.asm

rem %link_exe% /MAP NoUniVBE.obj,NoUniVBE.com,NoUniVBE.map,,,
%ulink_exe% NoUniVBE.obj, NoUniVBE.exe, NoUniVBE.map

pause

rem dosbox

pause

