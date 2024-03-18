.PHONY: all clean

CMAKE_BUILD_TYPE ?= Debug

all: build

configure: CMakeLists.txt
	cmake -S . -B build -G Ninja \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

build: configure
	cmake --build ./build -j

clean:
	rm -rfv ./build
