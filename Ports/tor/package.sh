#!/usr/bin/env -S bash ../.port_include.sh
port='tor'
version='0.4.8.17'
useconfigure='true'
use_fresh_config_sub='true'
depends=(
    'openssl'
    'libevent'
)
files=(
    "https://dist.torproject.org/tor-${version}.tar.gz#79b4725e1d4b887b9e68fd09b0d2243777d5ce3cd471e538583bcf6f9d8cdb56"
)
configopts=(
    "--disable-tool-name-check"
    "--disable-unittests"
    "--with-ssl-dir=${SERENITY_INSTALL_ROOT}/usr/local/lib"
)
