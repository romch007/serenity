set(SOURCES
        BencodeParser.h
        BencodeParser.cpp
        Torrent.h
        Torrent.cpp
        Tracker.h
        Tracker.cpp
        Peer.h
        Peer.cpp
        Client.h
        Client.cpp
        )

serenity_lib(LibBitTorrent bittorrent)
target_link_libraries(LibBitTorrent PRIVATE LibCore LibCrypto LibProtocol)
