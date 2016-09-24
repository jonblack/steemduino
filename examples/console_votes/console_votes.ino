// console_votes
//
// Output incoming upvotes and flags to the console.

#include <Ethernet.h>
#include <SPI.h>

#include <JsonStreamingParser.h>
#include <OpListener.h>

#include <SteemRpc.h>

EthernetClient client = EthernetClient();
SteemRpc steem(client, "node.steem.ws", "/", 80);

unsigned long current_block = 0;

void op_handler(const String& op_type, const String& op_data) {
    if (op_type != "vote")
        return;

    // Get the voter and author names. This is not even close to rhobust.
    int voter_beg_pos = op_data.indexOf("voter") + 9;
    int voter_end_pos = op_data.indexOf(',') - 1;
    String voter = op_data.substring(voter_beg_pos, voter_end_pos);

    int author_beg_pos = op_data.indexOf("author") + 10;
    int author_end_pos = op_data.indexOf(',', author_beg_pos) - 1;
    String author = op_data.substring(author_beg_pos, author_end_pos);

    int permlink_beg_pos = op_data.indexOf("permlink") + 12;
    int permlink_end_pos = op_data.indexOf(',', permlink_beg_pos) - 1;
    String permlink = op_data.substring(permlink_beg_pos, permlink_end_pos);

    int weight_beg_pos = op_data.indexOf("weight") + 10;
    int weight_end_pos = op_data.indexOf(',', weight_beg_pos) - 1;
    int weight = atoi(op_data.substring(weight_beg_pos, weight_end_pos).c_str());

    Serial.print(voter);
    if (weight > 0)
        Serial.print(" upvoted ");
    else if (weight < 0 )
        Serial.print(" flagged ");
    Serial.print(permlink);
    Serial.print(" by ");
    Serial.println(author);
}

void setup() {
    Serial.begin(9600);

    Serial.println("Connecting to network");
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    Ethernet.begin(mac);

    Serial.println("Connecting to Steem web socket");
    if (steem.connect()) {
        Serial.println("Connected to Steem web socket");
    } else {
        Serial.println("Failed to connect to Steem websocket");
        while (1) {};
    }
}

void loop() {
    // Hang if the client disconnects
    if (!steem.is_connected()) {
        Serial.println("Client disconnected");
        while (1) {}
    }

    // Get the last block number
    unsigned long last_block = steem.get_last_block_num();
    if (current_block == 0) current_block = last_block;

    while (current_block <= last_block) {
        // Get the block
        String block = "";
        if (!steem.get_block(current_block, block)) {
            Serial.print("Failed to get block ");
            Serial.println(current_block);
            continue;
        }

        // Parse the block
        JsonStreamingParser parser;
        OpListener listener(&op_handler);
        parser.setListener(&listener);

        const char* block_c = block.c_str();
        for (size_t i = 0; i < strlen(block_c); ++i) {
            parser.parse(block_c[i]);
        }

        current_block += 1;
    }

    delay(5000);
}
