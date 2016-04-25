Name "chess"
OutFile "chess-installer.exe"
InstallDir $PROGRAMFILES\com.lucidfusionlabs\chess

Page directory
Page instfiles

Section "";

RmDir /r "$INSTDIR"

SetOutPath "$INSTDIR"
File "..\Win32\chess\Debug\chess.exe"

SetOutPath "$INSTDIR\assets"
File "assets\*"
File "..\core\app\assets\Nobile.ttf,32,*"
File "..\core\app\shaders\default.*"

SetOutPath "$INSTDIR"
CreateDirectory "$SMPROGRAMS\com.lucidfusionlabs"
createShortCut "$SMPROGRAMS\com.lucidfusionlabs\chess.lnk" "$INSTDIR\chess.exe"

SectionEnd
 
