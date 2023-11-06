/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Torrent.h"
#include "BencodeParser.h"
#include <AK/JsonObject.h>
#include <LibCrypto/Hash/SHA1.h>

#define REQUIRE(expr)                                                  \
    ({                                                                 \
        auto&& maybe_value = expr;                                     \
        if (!maybe_value.has_value())                                  \
            return Error::from_string_literal("Torrent: missing key"); \
        maybe_value.release_value();                                   \
    })

namespace BitTorrent {

ErrorOr<NonnullRefPtr<Torrent>> Torrent::create(StringView input)
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
