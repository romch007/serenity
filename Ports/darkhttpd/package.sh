#!/usr/bin/env -S bash ../.port_include.sh
port='darkhttpd'
version='1.14'
files=(
    "https://github.com/emikulic/darkhttpd/archive/refs/tags/v${version}.tar.gz#e063de9efa5635260c8def00a4d41ec6145226a492d53fa1dac436967670d195"
)

build() {
    run make CC="${CC}"
}

install() {
    run cp -r darkhttpd "${SERENITY_INSTALL_ROOT}/usr/local/bin"
}
