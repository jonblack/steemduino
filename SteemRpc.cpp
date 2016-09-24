#include <SteemRpc.h>

SteemRpc::SteemRpc(Client& client, const String& host, const String& path,
        uint16_t port)
    : _client(client),
    _ws(WebSocketClient()),
    _call_id(1),
    _host(host),
    _path(path),
    _port(port) {
}

bool SteemRpc::connect() {
    _ws.host = const_cast<char*>(_host.c_str());
    _ws.path = const_cast<char*>(_path.c_str());

    if (!_client.connect(_host.c_str(), _port))
        return false;

    if (!_ws.handshake(_client))
        return false;

    return true;
}

bool SteemRpc::is_connected() {
    return _client.connected();
}

bool SteemRpc::get_block(unsigned long block_num, String& block) {
    char call[45 + _count_digits(block_num) + _count_digits(_call_id) + 1];
    sprintf(call,
        "{\"method\": \"get_block\", \"params\": [%lu], \"id\": %lu}",
        block_num, _call_id);

    if (!_rpc_call(call, block)) {
        Serial.println("rpc_call failed");
        return false;
    }

    return true;
}

unsigned long SteemRpc::get_last_block_num() {
    if (!_client.connected()) {
        return 0;
    }

    char call[65 + _count_digits(_call_id) + 1];
    sprintf(call,
        "{\"method\": \"get_dynamic_global_properties\", \"params\": [], \"id\": %lu}",
        _call_id);

    String res;
    if (!_rpc_call(call, res)) {
        return 0;
    }

    int libn_beg_pos = res.indexOf("last_irreversible_block_num") + 29;
    int libn_end_pos = res.indexOf(',', libn_beg_pos);
    unsigned long last_block = atol(res.substring(libn_beg_pos, libn_end_pos).c_str());

    return last_block;
}

bool SteemRpc::_rpc_call(const String& call, String& res) {
    if (!_client.connected()) {
        Serial.println("Client isn't connected");
        return false;
    }

    // Send the request and wait until we get a response that contains data.
    _ws.sendData(call);
    bool success = false;
    while (res.length() == 0) {
        success = _ws.getData(res);
        delay(250);
    }

    _call_id += 1;
    return success;
}

byte SteemRpc::_count_digits(long num) {
    byte count = 0;
    while(num) {
        num = num / 10;
        count++;
    }
    return count;
}
