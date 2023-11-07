/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "Torrent.h"
#include <AK/RefPtr.h>
#include <LibCore/TCPServer.h>
#include <LibProtocol/RequestClient.h>

namespace BitTorrent {

class Client : public RefCounted<Client> {
public:
    static ErrorOr<NonnullRefPtr<Client>> try_create();

    void add_torrent(NonnullRefPtr<Torrent> torrent);
    Optional<NonnullRefPtr<Torrent>> get_torrent_by_hash(Torrent::Hash const& hash) const;

private:
    Client(RefPtr<Core::TCPServer> tcp_server, RefPtr<Protocol::RequestClient> request_client, u16 port, DeprecatedString peer_id);

    static constexpr u16 max_port = 6000;
    static constexpr u16 min_port = 7000;

    RefPtr<Core::TCPServer> m_tcp_server;
    RefPtr<Protocol::RequestClient> m_request_client;

    DeprecatedString m_peer_id;
    u16 m_port;
    Vector<NonnullRefPtr<Torrent>> m_torrents;
};

}
