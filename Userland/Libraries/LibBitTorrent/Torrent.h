/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Tracker.h"
#include <AK/DeprecatedString.h>
#include <AK/Error.h>
#include <AK/RefPtr.h>
#include <AK/Span.h>
#include <AK/Time.h>
#include <AK/URL.h>
#include <AK/Vector.h>
#include <LibProtocol/Request.h>
#include <LibProtocol/RequestClient.h>

namespace BitTorrent {

class Torrent : public RefCounted<Torrent> {
public:
    using Hash = Array<u8, 20>;

    static ErrorOr<NonnullRefPtr<Torrent>> parse(StringView input);

    ErrorOr<RefPtr<Protocol::Request>> make_tracker_request(RefPtr<Protocol::RequestClient> request_client, DeprecatedString peer_id, u16 port, TrackerRequestEvent event) const;

    [[nodiscard]] URL const& announce_url() const { return m_announce_url; }
    [[nodiscard]] u64 file_length() const { return m_file_length; }
    [[nodiscard]] u64 piece_length() const { return m_piece_length; }
    [[nodiscard]] DeprecatedString const& suggested_name() const { return m_suggested_name; }
    [[nodiscard]] Vector<Hash> pieces() const { return m_pieces; }
    [[nodiscard]] Hash info_hash() const { return m_info_hash; }
    [[nodiscard]] u64 bytes_downloaded() const { return m_bytes_downloaded; }
    [[nodiscard]] u64 bytes_uploaded() const { return m_bytes_uploaded; }
    [[nodiscard]] u64 bytes_left() const { return m_file_length - m_bytes_downloaded; }

private:
    Torrent(URL announce_url, DeprecatedString suggested_name, u64 file_length, u64 piece_length, Vector<Hash> pieces, Hash info_hash, Optional<DeprecatedString> comment, Optional<DeprecatedString> created_by, Optional<UnixDateTime> created_at);

    // File info
    URL m_announce_url;
    DeprecatedString m_suggested_name;
    u64 m_file_length { 0 };
    u64 m_piece_length { 0 };
    Vector<Hash> m_pieces;
    Hash m_info_hash;
    Optional<DeprecatedString> m_comment;
    Optional<DeprecatedString> m_created_by;
    Optional<UnixDateTime> m_creation_date;

    // Download info
    u64 m_bytes_downloaded { 0 };
    u64 m_bytes_uploaded { 0 };
};

}
