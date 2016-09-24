# steemduino

An arduino library for interacting with the [steem](http://steemit.com) web
socket api.

This library is in **alpha** which means the following: you cannot rely on it
to fetch and parse blocks.

The libraries it depends on, and its own implemenation, aren't well-suited to
fetching and parsing large amounts of data returned from the steem web socket
API. This is especially true for the web socket library.

**The biggest issue is memory use, which is why you need an Arduino Mega.**
Even with the Mega you may still encounter issues. This is because the web
socket library loads the entire response into memory, which for some blocks is
too large. This will be fixed in future releases if there is enough demand.

## Features

The current features are limited. The only calls available are:

* `get_block`
* `get_last_block_num`

With these two call you can already do a lot:

* Notify when a user posts an article
* Notify when a post is upvoted
* Notify when a user is followed
* And more!

In time more features will be added. If you want a particular feature, please
create an issue.

## Hardware requirements

This is the hardware the library was written on. You won't have much luck with
other boards, but a WiFi shield may work. In time I'll add examples for this.

* Arduino Mega (due to memory requirements)
* Ethernet Shield (must be compatible with the Arduino Mega)

## Software requirements

This library requires the following libraries, which must be installed into the
Arduino IDE's libraries folder.

* [json-streaming-parser](https://github.com/jonblack/json-streaming-parser)
* [Arduino-Websocket](https://github.com/jonblack/Arduino-Websocket)

These are forked versions of existing projects with changes made to fix minor
issues. They are not available in the IDE library manager, so you will need to
download/clone them yourself and put them in the correct place.

## Examples

Check out the `examples` folder for various examples. All examples are provided
with a Makefile for building with
[Arduino-Makefile](https://github.com/sudar/Arduino-Makefile).

## Contribution

If you'd like to contribute to `steemduino`, please submit a pull-request.
