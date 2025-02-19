#!/usr/bin/bash

pushd $(dirname "$0")

BUILD_TYPE="Release"
CONAN_PRESET="conan-release"
STD_MODULE=""

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -c|--config)
            BUILD_TYPE="$2";
            CONAN_PRESET="conan-$(echo $BUILD_TYPE | tr '[:upper:]' '[:lower:]')"
            shift ;;
        -m|--std-module)
            STD_MODULE="-DCMAKE_CXX_MODULE_STD=ON"
            ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

# sudo apt install python3 pipx
# pipx install conan
DI_BUILD_TYPE=$BUILD_TYPE conan install . --output-folder=build --build=missing --profile conanprofile.txt
pushd build
cmake .. --preset $CONAN_PRESET -DDI_BUILD_TESTS=ON $STD_MODULE
cmake --build .
ctest
popd
popd
