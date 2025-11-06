#pragma once

#include <string>

struct NatsConfig {
    std::string server;
    std::string name;
    bool tls = false;
    std::string tlscacert;
    std::string tlscert;
    std::string tlskey;
    std::string tlsciphers;
    std::string tlshost;
    bool tlsskip = false;
    std::string creds;
};

extern struct NatsConfig natsConfig;