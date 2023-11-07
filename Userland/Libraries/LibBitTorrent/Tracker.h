/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/DeprecatedString.h>
#include <AK/Vector.h>
#include <LibCore/SocketAddress.h>

namespace BitTorrent {

enum class TrackerRequestEvent {
    Started,
    Stopped,
    Completed,
};

struct TrackerResponse {
    u32 interval;
    u32 complete;
    u32 incomplete;
    Vector<Core::SocketAddress> peers;

    static ErrorOr<TrackerResponse> parse(StringView input);
};

}
