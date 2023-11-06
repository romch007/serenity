/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "BencodeParser.h"
#include <AK/JsonArray.h>
#include <AK/JsonObject.h>
#include <AK/QuickSort.h>
#include <AK/Tuple.h>

namespace BitTorrent {

DeprecatedString to_bencode(JsonValue const& value)
{
    StringBuilder builder;
    serialize_bencode(builder, value);
    return builder.to_deprecated_string();
}

void serialize_bencode(StringBuilder& builder, JsonValue const& value)
{
    switch (value.type()) {
    case JsonValue::Type::String: {
        auto str = value.as_string();
        builder.appendff("{}", str.length());
        builder.append(':');
        builder.append(str);
        break;
    }
    case JsonValue::Type::Int64:
        builder.append('i');
        builder.appendff("{}", value.as_i64());
        builder.append('e');
        break;
    case JsonValue::Type::UnsignedInt64:
        builder.append('i');
        builder.appendff("{}", value.as_u64());
        builder.append('e');
        break;
    case JsonValue::Type::Array:
        builder.append('l');
        value.as_array().for_each([&](auto inner_value) {
            serialize_bencode(builder, inner_value);
        });
        builder.append('e');
        break;
    case JsonValue::Type::Object: {
        Vector<Tuple<DeprecatedString, JsonValue>> elements;
        value.as_object().for_each_member([&](auto key, auto value) {
            elements.append(Tuple(key, value));
        });

        // All keys must appear in lexicographical order
        quick_sort(elements, [](Tuple<DeprecatedString, JsonValue>& a, Tuple<DeprecatedString, JsonValue>& b) {
            return a.get<0>() < b.get<0>();
        });

        builder.append('d');

        for (auto const& element : elements) {
            serialize_bencode(builder, JsonValue(element.get<0>()));
            serialize_bencode(builder, element.get<1>());
        }

        builder.append('e');
        break;
    }
    default:
        VERIFY_NOT_REACHED();
    }
}

BencodeParser::BencodeParser(StringView input)
    : AK::GenericLexer(input)
{
}

ErrorOr<JsonValue> BencodeParser::run() { return parse_value(); }

ErrorOr<JsonValue> BencodeParser::parse_value()
{
    switch (peek()) {
    case 'i':
        return parse_integer();
    case 'l':
        return parse_list();
    case 'd':
        return parse_dict();
    default:
        return parse_str();
    }
}

ErrorOr<JsonValue> BencodeParser::parse_integer()
{
    consume();
    auto parse_result = consume_until('e').to_int<i64>();
    consume();

    if (!parse_result.has_value())
        return Error::from_string_literal("BencodeParser: invalid integer");

    return JsonValue(parse_result.release_value());
}

ErrorOr<JsonValue> BencodeParser::parse_str()
{
    auto parse_len_result = consume_until(':').to_uint<u64>();
    consume();

    if (!parse_len_result.has_value())
        return Error::from_string_literal("BencodeParser: invalid size for string");

    u64 len = parse_len_result.release_value();

    auto str = consume(len);

    return JsonValue(str);
}

ErrorOr<JsonValue> BencodeParser::parse_list()
{
    consume();

    JsonArray list;
    while (!next_is('e')) {
        auto inner_value = TRY(parse_value());
        TRY(list.append(inner_value));
    }
    consume();

    return list;
}

ErrorOr<JsonValue> BencodeParser::parse_dict()
{
    consume();

    JsonObject dict;
    while (!next_is('e')) {
        auto key = TRY(parse_str());
        auto value = TRY(parse_value());

        dict.set(key.as_string(), value);
    }

    consume();

    return dict;
}

}
