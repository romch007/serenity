/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Torrent.h"
#include <AK/Array.h>
#include <AK/DeprecatedString.h>

namespace BitTorrent {

DeprecatedString generate_peer_id();

struct Handshake {
    u8 length;
    DeprecatedString bittorrent;
    Array<u8, 8> reserved;
    Torrent::Hash info_hash;
    DeprecatedString peer_id;
};

}
