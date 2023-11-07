/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Torrent.h"
#include "BencodeParser.h"
#include <AK/JsonObject.h>
#include <LibCrypto/Hash/SHA1.h>

namespace BitTorrent {

ErrorOr<NonnullRefPtr<Torrent>> Torrent::parse(StringView input)
{
    BencodeParser bencode_parser(input);
    auto content = TRY(bencode_parser.run());

    if (!content.is_object())
        return Error::from_string_literal("Torrent: not a bencode dict");

    auto root_object = content.as_object();

    auto announce_url = URL(REQUIRE(root_object.get_deprecated_string("announce"sv)));
    auto info_object = REQUIRE(root_object.get_object("info"sv));

    // Compute the SHA1 hash of the info section
    auto info_raw = to_bencode(info_object);
    auto digest = Crypto::Hash::SHA1::hash(info_raw);
    VERIFY(digest.data_length() == 20);
    auto info_hash = Torrent::Hash::from_span(digest.bytes());

    auto file_length = REQUIRE(info_object.get_integer<u64>("length"sv));
    auto piece_length = REQUIRE(info_object.get_integer<u64>("piece length"sv));
    auto suggested_name = REQUIRE(info_object.get_deprecated_string("name"sv));
    auto comment = info_object.get_deprecated_string("comment"sv);
    auto created_by = info_object.get_deprecated_string("created by"sv);
    auto creation_date = info_object.get_i64("creation date"sv).map([](i64 seconds) {
        return UnixDateTime::from_seconds_since_epoch(seconds);
    });

    // Break the pieces string into a vector of 20-byte hashes
    Vector<Torrent::Hash> pieces;
    pieces.ensure_capacity((file_length / piece_length) + 1);
    auto raw_piece_hashes = REQUIRE(info_object.get_deprecated_string("pieces"sv)).bytes();
    size_t pos = 0;

    while (pos < raw_piece_hashes.size() - 20) {
        auto hash = Torrent::Hash::from_span(raw_piece_hashes.slice(pos, 20));
        pieces.append(hash);
        pos += 20;
    }

    auto hash = Torrent::Hash::from_span(raw_piece_hashes.slice(pos));
    pieces.append(hash);

    return adopt_nonnull_ref_or_enomem(new (nothrow) Torrent(announce_url, suggested_name, file_length, piece_length, pieces, info_hash, comment, created_by, creation_date));
}

template<typename Element>
static DeprecatedString urlencode(Element const& element)
{
    StringBuilder builder;
    for (u8 byte : element) {
        auto is_intact = (byte >= '0' && byte <= '9') || (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || byte == '.' || byte == '-' || byte == '_' || byte == '~';

        if (is_intact)
            builder.append(static_cast<char>(byte));
        else
            builder.appendff("%{:02X}", byte);
    }

    return builder.to_deprecated_string();
}

ErrorOr<RefPtr<Protocol::Request>> Torrent::make_tracker_request(RefPtr<Protocol::RequestClient> request_client, DeprecatedString peer_id, u16 port, TrackerRequestEvent event) const
{
    DeprecatedString event_str;
    switch (event) {
    case TrackerRequestEvent::Started:
        event_str = "started";
        break;
    case TrackerRequestEvent::Stopped:
        event_str = "stopped";
        break;
    case TrackerRequestEvent::Completed:
        event_str = "completed";
        break;
    }
    HashMap<DeprecatedString, DeprecatedString> headers;
    headers.set("User-Agent", "LibBitTorrent/1.0");

    HashMap<DeprecatedString, DeprecatedString> params;
    params.set("downloaded", DeprecatedString::number(m_bytes_downloaded));
    params.set("uploaded", DeprecatedString::number(m_bytes_uploaded));
    params.set("port", DeprecatedString::number(port));
    params.set("left", DeprecatedString::number(bytes_left()));
    params.set("compact", "1");
    params.set("info_hash", urlencode(m_info_hash));
    params.set("peer_id", urlencode(peer_id));
    params.set("event", event_str);

    StringBuilder query_builder;

    for (auto const& entry : params) {
        query_builder.append(entry.key);
        query_builder.append("="sv);
        query_builder.append(entry.value);
        query_builder.append('&');
    }
    query_builder.trim(1);

    URL target = m_announce_url;
    target.set_query(TRY(query_builder.to_string()));
    return request_client->start_request("GET", target, headers);
}

Torrent::Torrent(URL announce_url, DeprecatedString suggested_name, u64 file_length, u64 piece_length, Vector<Torrent::Hash> pieces, Torrent::Hash info_hash, Optional<DeprecatedString> comment, Optional<DeprecatedString> created_by, Optional<UnixDateTime> created_at)
    : m_announce_url(move(announce_url))
    , m_suggested_name(move(suggested_name))
    , m_file_length(file_length)
    , m_piece_length(piece_length)
    , m_pieces(move(pieces))
    , m_info_hash(info_hash)
    , m_comment(move(comment))
    , m_created_by(move(created_by))
    , m_creation_date(move(created_at))
{
}
}
