mkdir build32
mkdir build64
cd build32
cmake -G "Visual Studio 14" ..
msbuild WinImmImeSample.sln

cd ..
cd build64
cmake -G "Visual Studio 14 Win64" ..
msbuild WinImmImeSample.sln
cd ..
