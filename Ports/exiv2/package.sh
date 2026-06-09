#!/usr/bin/env -S bash ../.port_include.sh
port='exiv2'
version='0.28.8'
useconfigure='true'
files=(
    "https://github.com/Exiv2/exiv2/archive/refs/tags/v${version}.tar.gz#ea51b0609f58a9afa063b60daa1539948b62247721e154f4fff0ad3aec9f9756"
)
depends=(
    'brotli'
    'zlib'
    'expat'
)

configopts=(
    "-DCMAKE_TOOLCHAIN_FILE=${SERENITY_BUILD_DIR}/CMakeToolchain.txt"
    "-DEXIV2_ENABLE_INIH=OFF"
    "-GNinja"
)

configure() {
    run cmake "${configopts[@]}" .
}

build() {
    run ninja -j${MAKEJOBS}
}

install() {
    run ninja install
}
