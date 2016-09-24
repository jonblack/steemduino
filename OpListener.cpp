#include <OpListener.h>

OpListener::OpListener(OpHandlerFnPtr op_handler)
: _op_handler(op_handler),
  _key(),
  _op_type(),
  _op_data(),
  _op_level(0),
  _in_op(false) {
}

void OpListener::startArray() {
    if (_in_op) {
        _op_data += "[";
    }
}

void OpListener::endArray() {
    if (_in_op) {
        _op_data += "]";
    }
}

void OpListener::startObject() {
    if (_in_op) {
        _op_data += "{";
        _op_level += 1;
    }
}

void OpListener::endObject() {
    if (_in_op) {
        _op_data += "}";
        _op_level -= 1;

        if (_op_level == 0) {
            _op_handler(_op_type, _op_data);
            _op_data = "";
            _in_op = false;
        }
    }
}

void OpListener::key(const String& key) {
    _key = key;

    if (_in_op) {
        if (_op_data.length() > 1) {
            _op_data += ", ";
        }
        _op_data += "\"" + key + "\": ";
    }
}

void OpListener::value(const String& value) {
    if (_key == "operations") {
        _op_type = value;
        _in_op = true;
    } else {
        if (_in_op) {
            // Don't quote values that contain json
            if (_key == "json_metadata" || _key == "json") {
                _op_data += value;
            } else {
                _op_data += "\"" + value + "\"";
            }
        }
    }
}
