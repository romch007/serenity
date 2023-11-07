/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Tracker.h"
#include "BencodeParser.h"
#include <AK/JsonObject.h>

namespace BitTorrent {

ErrorOr<TrackerResponse> TrackerResponse::parse(StringView input)
{
    BencodeParser bencode_parser(input);
    auto content = TRY(bencode_parser.run());

    if (!content.is_object())
        return Error::from_string_literal("Tracker did not respond an object");

    TrackerResponse response;

    auto root_object = content.as_object();
    response.interval = REQUIRE(root_object.get_u32("interval"sv));
    response.complete = REQUIRE(root_object.get_u32("complete"sv));
    response.incomplete = REQUIRE(root_object.get_u32("incomplete"sv));

    auto raw_peers = REQUIRE(root_object.get_deprecated_string("peers"sv)).bytes();
    auto peers_count = raw_peers.size() / 6;
    response.peers.ensure_capacity(peers_count);

    for (size_t i = 0; i < peers_count; i++) {
        u8 const* raw_peer = raw_peers.offset(i % 6);
        auto ip_address = IPv4Address(raw_peer[0], raw_peer[1], raw_peer[2], raw_peer[3]);
        // FIXME: is endianness correct?
        u16 port = (raw_peer[4] << 8) | raw_peer[5];

        response.peers.append(Core::SocketAddress(ip_address, port));
    }

    return response;
}
}
