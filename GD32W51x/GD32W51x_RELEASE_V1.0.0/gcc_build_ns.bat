@echo on

:: Remove cmake_build/ for reconfigure
if exist cmake_build rmdir /s/q cmake_build
mkdir cmake_build
cd cmake_build

:: Configure
:: cmake -G "Unix Makefiles" -DCONFIG_WLANLIB_BUILDED=OFF -DCMAKE_TOOLCHAIN_FILE:PATH=./scripts/cmake/toolchain.cmake ..
cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:PATH=./scripts/cmake/toolchain.cmake ..

:: Make
make -j

if ERRORLEVEL 2 pause
cd ..
