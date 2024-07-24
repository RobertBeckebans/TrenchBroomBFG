del /s /q build
mkdir build
cd build
cmake -G "Visual Studio 17" -A x64 -DCMAKE_MODULE_PATH="cmake/packages" -DCMAKE_PREFIX_PATH="C:\Qt\5.15.2\msvc2019_64" ..
pause