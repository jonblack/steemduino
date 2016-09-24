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
unsigned upvote_count = 0;
unsigned flaggd_count = 0;

// Led Matrix functions

void render_text(const char* text, uint8_t x_idnt, uint8_t y_idnt, uint8_t color) {
    for (size_t i = 0; i < strlen(text); ++i) {
        ledMatrix.putchar((i * FONT_WIDTH) + x_idnt, y_idnt, text[i], color);
    }
}

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

    int permlink_beg_pos = op_data.indexOf("permlink") + 12;
    int permlink_end_pos = op_data.indexOf(',', permlink_beg_pos) - 1;
    String permlink = op_data.substring(permlink_beg_pos, permlink_end_pos);

    int weight_beg_pos = op_data.indexOf("weight") + 10;
    int weight_end_pos = op_data.indexOf(',', weight_beg_pos) - 1;
    int weight = atoi(op_data.substring(weight_beg_pos, weight_end_pos).c_str());

    if (author == USER) {
        Serial.print(voter);
        if (weight > 0) {
            Serial.print(" upvoted ");
            upvote_count += 1;
        } else if (weight < 0) {
            Serial.print(" flagged ");
            flaggd_count += 1;
        }
        Serial.println(permlink);

        char upvote_buffer[4];
        sprintf(upvote_buffer, "%05d", upvote_count);

        char flaggd_buffer[4];
        sprintf(flaggd_buffer, "%05d", flaggd_count);

        ledMatrix.clear();
        render_text(upvote_buffer, 1, 1, GREEN);
        render_text(flaggd_buffer, 1, 9, RED);
        ledMatrix.sendframe();
    }
}

void setup() {
    Serial.begin(9600);

    ledMatrix.setfont(FONT_5x7W);
    ledMatrix.clear();
    ledMatrix.pwm(1);
    render_text("00000", 1, 1, GREEN);
    render_text("00000", 1, 9, RED);
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
