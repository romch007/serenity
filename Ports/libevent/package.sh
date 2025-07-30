#!/usr/bin/env -S bash ../.port_include.sh
port='libevent'
version='2.1.12'
useconfigure='true'
use_fresh_config_sub='true'
config_sub_paths=(
    'build-aux/config.sub'
)
depends=(
    'openssl'
)
files=(
    "https://github.com/libevent/libevent/releases/download/release-${version}-stable/libevent-${version}-stable.tar.gz#92e6de1be9ec176428fd2367677e61ceffc2ee1cb119035037a27d346b0403bb"
)
configopts=(
    "-DCMAKE_TOOLCHAIN_FILE=${SERENITY_BUILD_DIR}/CMakeToolchain.txt"
    "-DEVENT__LIBRARY_TYPE=SHARED"
    "-DEVENT__DISABLE_REGRESS=ON"
    "-DCMAKE_SKIP_INSTALL_RPATH=ON"
    "-DCMAKE_INSTALL_PREFIX=${DESTDIR}/usr/local/"
    "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
)
workdir="libevent-${version}-stable"

configure() {
    run cmake "${configopts[@]}"
}

install() {
    run make install
}
