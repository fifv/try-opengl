## Setup glbinding
```sh
mkdir external
cd ./external
git clone --filter=blob:none --recursive https://github.com/glfw/glfw
git clone --filter=blob:none --recursive https://github.com/cginternals/glbinding

cd glbinding
cmake -S. -Bbuild -G"Ninja Multi-Config" -DBUILD_SHARED_LIBS=OFF
cmake --build build --config Debug
cmake --build build --config Release
cmake --install build --config Debug --prefix build/dist  
cmake --install build --config Release --prefix build/dist
cd ..

```