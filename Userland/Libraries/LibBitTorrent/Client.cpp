/*
 * Copyright (c) 2023, Romain Chardiny <romain.chardiny@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Client.h"
#include "Peer.h"

namespace BitTorrent {

ErrorOr<NonnullRefPtr<Client>> Client::try_create()
{
    auto request_client = TRY(Protocol::RequestClient::try_create());
    auto tcp_server = TRY(Core::TCPServer::try_create());

    // Bind TCP server to a random available port
    TRY(tcp_server->listen(IPv4Address(0, 0, 0, 0), 0));

    return adopt_nonnull_ref_or_enomem(new (nothrow) Client(tcp_server, request_client, tcp_server->local_port().value(), generate_peer_id()));
}

Client::Client(RefPtr<Core::TCPServer> tcp_server, RefPtr<Protocol::RequestClient> request_client, u16 port, DeprecatedString peer_id)
    : m_tcp_server(move(tcp_server))
    , m_request_client(move(request_client))
    , m_peer_id(move(peer_id))
    , m_port(port)
{
}

void Client::add_torrent(NonnullRefPtr<Torrent> torrent)
{
    m_torrents.append(move(torrent));
}

Optional<NonnullRefPtr<Torrent>> Client::get_torrent_by_hash(Torrent::Hash const& hash) const
{
    auto it = m_torrents.find_if([hash](auto const torrent) { return torrent->info_hash() == hash; });
    if (it.is_end())
        return {};
    return *it;
}

}
