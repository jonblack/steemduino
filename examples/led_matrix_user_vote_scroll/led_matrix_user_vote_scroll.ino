// led_matrix_user_vote_count
//
// Outputs a count of incoming upvotes and flags for a given user

#include <Ethernet.h>
#include <SPI.h>

#include <JsonStreamingParser.h>
#include <OpListener.h>
#include <SteemRpc.h>
#include <ht1632c.h>

// Display constants

#define PIN_LED_DATA  0
#define PIN_LED_WR    1
#define PIN_LED_CLOCK 3
#define PIN_LED_CS    2

#define LED_HEIGHT    16
#define LED_WIDTH     32
#define FONT_HEIGHT    7
#define FONT_WIDTH     6

static const char USER[] = "bitcalm";

// Renderer

ht1632c ledMatrix = ht1632c(&PORTA, PIN_LED_DATA, PIN_LED_WR, PIN_LED_CLOCK,
                            PIN_LED_CS, GEOM_32x16, 2);
EthernetClient client = EthernetClient();
SteemRpc steem(client, "node.steem.ws", "/", 80);

unsigned long current_block = 0;

// Steem handler

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

    int weight_beg_pos = op_data.indexOf("weight") + 10;
    int weight_end_pos = op_data.indexOf(',', weight_beg_pos) - 1;
    int weight = atoi(op_data.substring(weight_beg_pos, weight_end_pos).c_str());

    if (author == USER) {
        uint8_t color;
        String msg;
        if (weight > 0) {
            msg = "Upvoted by " + voter;
            color = GREEN;
        } else if (weight < 0) {
            msg = "Flagged by " + voter;
            color = RED;
        } else {
            return;
        }

        Serial.println(msg);
        ledMatrix.clear();
        ledMatrix.hscrolltext(4, msg.c_str(), color, 10);
        ledMatrix.sendframe();

    }
}

void setup() {
    Serial.begin(9600);

    ledMatrix.setfont(FONT_5x7W);
    ledMatrix.clear();
    ledMatrix.pwm(1);
    ledMatrix.sendframe();

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
