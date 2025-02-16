#!/usr/bin/bash

# sudo apt install python3 pipx
# pipx install conan
# conan profile detect --force
pushd $(dirname "$0")
conan install . --output-folder=build --build=missing --profile conanprofile.txt
pushd build
cmake .. --preset conan-release -DDI_BUILD_TESTS=ON #-DCMAKE_CXX_MODULE_STD=ON
cmake --build .
ctest
popd
popd
