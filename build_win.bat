
:: Build script for rcdiscover under Windows

@echo off
setlocal enabledelayedexpansion

:: Get branding parameter

:: Check for existance of some tools

where nmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo This must be run in Visual Studio command prompt for x64
  exit /b 1
)

where git >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo You must download and install git from: git-scm.com/download/win
  exit /b 1
)

where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
  echo You must download and install cmake from: https://cmake.org/download/
  exit /b 1
)

:: Create directories building and installing

if not exist "build\" mkdir build
cd build

if not exist "install\" mkdir install
cd install
set INSTALL_PATH=%CD%
set LIB=%LIB%;%INSTALL_PATH%\lib

cd ..\..\..

:: Clone FLTK

if not exist "fltk\" (
  git clone https://github.com/fltk/fltk.git
  git checkout branch-1.3
)

echo ----- Building fltk -----

cd fltk

if not exist "build\" mkdir build
cd build

set FLTK_OPT=-DFLTK_BUILD_TEST=OFF -DOPTION_USE_GL=OFF -DCMAKE_BUILD_TYPE=Release -DOPTION_PRINT_SUPPORT=OFF -DOPTION_USE_GDIPLUS=OFF -DOPTION_USE_KDIALOG=OFF -DOPTION_USE_SVG=OFF

if exist "build_rcdiscover\" (
  cd build_rcdiscover\
) else (
  mkdir build_rcdiscover\
  cd build_rcdiscover\
  cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG" -DFLTK_MSVC_RUNTIME_DLL=OFF -DCMAKE_CXX_STANDARD_LIBRARIES="kernel32.lib user32.lib gdi32.lib shell32.lib ole32.lib comdlg32.lib" %FLTK_OPT% -DCMAKE_INSTALL_PREFIX="%INSTALL_PATH%" ..\..
)

nmake install
if %ERRORLEVEL% NEQ 0 exit /b 1

cd ..\..\..

echo ----- Building rcdiscover -----

cd rcdiscover\build

if exist "build_rcdiscover\" (
  cd build_rcdiscover\
) else (
  mkdir build_rcdiscover\
  cd build_rcdiscover\
  cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_FLAGS_RELEASE="/MT /O2 /Ob2 /DNDEBUG /EHsc" -DCMAKE_INSTALL_PREFIX="%INSTALL_PATH%" -DBUILD_RCDISCOVER_CLI=ON -DBUILD_RCDISCOVER_GUI=ON BUILD_RCDISCOVER_SHARED_LIB=OFF ..\..
)

nmake install
if %ERRORLEVEL% NEQ 0 exit /b 1

cd ..

echo ----- Copy resulting binary -----

copy %INSTALL_PATH%\bin\rcdiscover.exe %INSTALL_PATH%\..
copy %INSTALL_PATH%\bin\rcdiscover-gui.exe %INSTALL_PATH%\..

echo Executables are here: %INSTALL_PATH%
