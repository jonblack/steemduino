#pragma once

#include <WebSocketClient.h>

class SteemRpc {
    public:
        SteemRpc(Client& client, const String& host, const String& path,
                 uint16_t port);

        bool connect();
        bool is_connected();

        bool get_block(unsigned long block_num, String& block);
        unsigned long get_last_block_num();

    private:
        bool _rpc_call(const String& call, String& res);
        byte _count_digits(long num);

    private:
        Client& _client;
        WebSocketClient _ws;
        long _call_id;
        String _host;
        String _path;
        uint16_t _port;
};
