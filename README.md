# Compiling
This project has not been tested on windows, however it may still work as all libraries used
should be cross-platform.

This project uses Conan for managing a large portion of its dependencies.

```bash
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install .. --build missing
cmake ..
cmake --build .
./bin/out
```