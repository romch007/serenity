/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/GenericLexer.h>
#include <AK/JsonValue.h>
#include <AK/StringBuilder.h>

#define REQUIRE(expr)                                                  \
    ({                                                                 \
        auto&& maybe_value = expr;                                     \
        if (!maybe_value.has_value())                                  \
            return Error::from_string_literal("Torrent: missing key"); \
        maybe_value.release_value();                                   \
    })

namespace BitTorrent {

DeprecatedString to_bencode(JsonValue const& value);

void serialize_bencode(StringBuilder& builder, JsonValue const& value);

class BencodeParser : private GenericLexer {
public:
    BencodeParser(StringView input);

    ErrorOr<JsonValue> run();

private:
    ErrorOr<JsonValue> parse_value();
    ErrorOr<JsonValue> parse_integer();
    ErrorOr<JsonValue> parse_str();
    ErrorOr<JsonValue> parse_list();
    ErrorOr<JsonValue> parse_dict();
};

}
