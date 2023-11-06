set(SOURCES
        BencodeParser.h
        BencodeParser.cpp
        Torrent.h
        Torrent.cpp
        )

serenity_lib(LibBitTorrent bittorrent)
target_link_libraries(LibBitTorrent PRIVATE LibCore LibCrypto)
