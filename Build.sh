#!/bin/bash
path=$(cd $(dirname "${BASH_SOURCE:-$0}") && pwd)

cd "${path}/CloudCoinFlipper"
rm -fr bin
cmake -S . -B bin

cd "bin"
cmake --build .

if [ -d "x64" ]
then
	cp "${path}/CloudCoinFlipper/SFML-2.5.1/extlibs/bin/x64/openal32.dll" "Debug/"
else
	cp "${path}/CloudCoinFlipper/SFML-2.5.1/extlibs/bin/x32/openal32.dll" "Debug/"
fi

rm -fr "${path}/exec"
mkdir "${path}/exec"

if [ -d "Debug" ]
then
	cp SFML-2.5.1/lib/Debug/*.dll Debug/
	cp -r Assets Debug/
	mv "Debug" "${path}/exec"
else
	cp SFML-2.5.1/lib/Release/*.dll Release/
	cp -r Assets Release/
	mv "Release" "${path}/exec"
fi