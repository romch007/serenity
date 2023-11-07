/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Peer.h"
#include <AK/Random.h>

namespace BitTorrent {

DeprecatedString generate_peer_id()
{
    StringBuilder builder;
    builder.append("-SE1000-"sv);
    for (int i = 0; i < 12; i++) {
        builder.append(get_random<u8>());
    }
    return builder.to_deprecated_string();
}

}
