#pragma once

#include <JsonListener.h>

class OpListener: public JsonListener {
    public:
        using OpHandlerFnPtr = void (*)(const String& op_type,
                                        const String& op_data);

    public:
        OpListener(OpHandlerFnPtr op_handler);

        virtual void startObject();
        virtual void endObject();
        virtual void startArray();
        virtual void endArray();
        virtual void key(const String& key);
        virtual void value(const String& value);

        // Override but do nothing
        virtual void startDocument() {}
        virtual void endDocument() {}
        virtual void whitespace(char c) {}

    private:
        OpHandlerFnPtr _op_handler;
        String _key;
        String _op_type;
        String _op_data;
        int _op_level;
        bool _in_op;
};
