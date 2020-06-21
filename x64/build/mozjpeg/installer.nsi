!include x64.nsh
Name "mozjpeg SDK for Visual C++ 64-bit"
OutFile "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}mozjpeg-4.0.0-vc64.exe"
InstallDir "C:\Users\momo8\Documents\Git\MozjpegGUI\x64"

SetCompressor bzip2

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "mozjpeg SDK for Visual C++ 64-bit (required)"
!ifdef WIN64
	${If} ${RunningX64}
	${DisableX64FSRedirection}
	${Endif}
!endif
	SectionIn RO
!ifdef GCC
	IfFileExists $SYSDIR/libturbojpeg.dll exists 0
!else
	IfFileExists $SYSDIR/turbojpeg.dll exists 0
!endif
	goto notexists
	exists:
!ifdef GCC
	MessageBox MB_OK "An existing version of the mozjpeg SDK for Visual C++ 64-bit is already installed.  Please uninstall it first."
!else
	MessageBox MB_OK "An existing version of the mozjpeg SDK for Visual C++ 64-bit or the TurboJPEG SDK is already installed.  Please uninstall it first."
!endif
	quit

	notexists:
	SetOutPath $SYSDIR
!ifdef GCC
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libturbojpeg.dll"
!else
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}turbojpeg.dll"
!endif
	SetOutPath $INSTDIR\bin
!ifdef GCC
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libturbojpeg.dll"
!else
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}turbojpeg.dll"
!endif
!ifdef GCC
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libjpeg-62.dll"
!else
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}jpeg62.dll"
!endif
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}cjpeg.exe"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}djpeg.exe"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}jpegtran.exe"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}tjbench.exe"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}rdjpgcom.exe"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}wrjpgcom.exe"
	SetOutPath $INSTDIR\lib
!ifdef GCC
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libturbojpeg.dll.a"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libturbojpeg.a"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libjpeg.dll.a"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\libjpeg.a"
	SetOutPath $INSTDIR\lib\pkgconfig
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\pkgscripts\libjpeg.pc"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\pkgscripts\libturbojpeg.pc"
!else
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}turbojpeg.lib"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}turbojpeg-static.lib"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}jpeg.lib"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\${BUILDDIR}jpeg-static.lib"
!endif
!ifdef JAVA
	SetOutPath $INSTDIR\classes
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\java\turbojpeg.jar"
!endif
	SetOutPath $INSTDIR\include
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/x64/build/mozjpeg\jconfig.h"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\jerror.h"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\jmorecfg.h"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\jpeglib.h"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\turbojpeg.h"
	SetOutPath $INSTDIR\doc
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\README.ijg"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\README.md"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\LICENSE.md"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\example.txt"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\libjpeg.txt"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\structure.txt"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\usage.txt"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\wizard.txt"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\tjexample.c"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\java\TJExample.java"
!ifdef GCC
	SetOutPath $INSTDIR\man\man1
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\cjpeg.1"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\djpeg.1"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\jpegtran.1"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\rdjpgcom.1"
	File "C:/Users/momo8/Documents/Git/MozjpegGUI/mozjpeg\wrjpgcom.1"
!endif

	WriteRegStr HKLM "SOFTWARE\64 4.0.0" "Install_Dir" "$INSTDIR"

	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\64 4.0.0" "DisplayName" "mozjpeg SDK v4.0.0 for Visual C++ 64-bit"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\64 4.0.0" "UninstallString" '"$INSTDIR\uninstall_4.0.0.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\64 4.0.0" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\64 4.0.0" "NoRepair" 1
	WriteUninstaller "uninstall_4.0.0.exe"
SectionEnd

Section "Uninstall"
!ifdef WIN64
	${If} ${RunningX64}
	${DisableX64FSRedirection}
	${Endif}
!endif

	SetShellVarContext all

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\64 4.0.0"
	DeleteRegKey HKLM "SOFTWARE\64 4.0.0"

!ifdef GCC
	Delete $INSTDIR\bin\libjpeg-62.dll
	Delete $INSTDIR\bin\libturbojpeg.dll
	Delete $SYSDIR\libturbojpeg.dll
	Delete $INSTDIR\lib\libturbojpeg.dll.a
	Delete $INSTDIR\lib\libturbojpeg.a
	Delete $INSTDIR\lib\libjpeg.dll.a
	Delete $INSTDIR\lib\libjpeg.a
	Delete $INSTDIR\lib\pkgconfig\libjpeg.pc
	Delete $INSTDIR\lib\pkgconfig\libturbojpeg.pc
!else
	Delete $INSTDIR\bin\jpeg62.dll
	Delete $INSTDIR\bin\turbojpeg.dll
	Delete $SYSDIR\turbojpeg.dll
	Delete $INSTDIR\lib\jpeg.lib
	Delete $INSTDIR\lib\jpeg-static.lib
	Delete $INSTDIR\lib\turbojpeg.lib
	Delete $INSTDIR\lib\turbojpeg-static.lib
!endif
!ifdef JAVA
	Delete $INSTDIR\classes\turbojpeg.jar
!endif
	Delete $INSTDIR\bin\cjpeg.exe
	Delete $INSTDIR\bin\djpeg.exe
	Delete $INSTDIR\bin\jpegtran.exe
	Delete $INSTDIR\bin\tjbench.exe
	Delete $INSTDIR\bin\rdjpgcom.exe
	Delete $INSTDIR\bin\wrjpgcom.exe
	Delete $INSTDIR\include\jconfig.h
	Delete $INSTDIR\include\jerror.h
	Delete $INSTDIR\include\jmorecfg.h
	Delete $INSTDIR\include\jpeglib.h
	Delete $INSTDIR\include\turbojpeg.h
	Delete $INSTDIR\uninstall_4.0.0.exe
	Delete $INSTDIR\doc\README.ijg
	Delete $INSTDIR\doc\README.md
	Delete $INSTDIR\doc\LICENSE.md
	Delete $INSTDIR\doc\example.txt
	Delete $INSTDIR\doc\libjpeg.txt
	Delete $INSTDIR\doc\structure.txt
	Delete $INSTDIR\doc\usage.txt
	Delete $INSTDIR\doc\wizard.txt
	Delete $INSTDIR\doc\tjexample.c
	Delete $INSTDIR\doc\TJExample.java
!ifdef GCC
	Delete $INSTDIR\man\man1\cjpeg.1
	Delete $INSTDIR\man\man1\djpeg.1
	Delete $INSTDIR\man\man1\jpegtran.1
	Delete $INSTDIR\man\man1\rdjpgcom.1
	Delete $INSTDIR\man\man1\wrjpgcom.1
!endif

	RMDir "$INSTDIR\include"
!ifdef GCC
	RMDir "$INSTDIR\lib\pkgconfig"
!endif
	RMDir "$INSTDIR\lib"
	RMDir "$INSTDIR\doc"
!ifdef GCC
	RMDir "$INSTDIR\man\man1"
	RMDir "$INSTDIR\man"
!endif
!ifdef JAVA
	RMDir "$INSTDIR\classes"
!endif
	RMDir "$INSTDIR\bin"
	RMDir "$INSTDIR"

SectionEnd
