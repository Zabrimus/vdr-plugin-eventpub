#pragma once

#include <nats/nats.h>
#include <string>

class PublishNats {
private:
    natsConnection *conn = nullptr;
    natsConnection *connJs = nullptr;
    natsOptions *opts = nullptr;

    jsCtx *js = nullptr;
    jsOptions jsOpts;

    natsStatus createNatsOptions();

public:
    PublishNats();
    ~PublishNats();

    natsStatus connect();

    natsStatus sendMessage(const std::string &subject, const std::string &payload, int ttl);
    natsStatus sendMessageJs(const std::string &subject, const std::string &payload, int ttl);
};

extern PublishNats *publisher;