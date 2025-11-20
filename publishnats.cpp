#include <cstring>
#include <vdr/plugin.h>

#include "publishnats.h"
#include "natsconfig.h"

const int MAX_SERVERS = 10;

 PublishNats *publisher;

// prefined subjects
const char *predefinedSubjects[] = {
    "vdr.>"
};

// helper methods

// Found in https://github.com/nats-io/nats.c/blob/main/examples/examples.h
static natsStatus parseUrls(const char *urls, natsOptions *gopts) {
    char        *serverUrls[MAX_SERVERS];
    int         num       = 0;
    natsStatus  s         = NATS_OK;
    char        *urlsCopy = nullptr;
    char        *commaPos = nullptr;
    char        *ptr      = nullptr;

    urlsCopy = strdup(urls);
    if (urlsCopy == nullptr)
        return NATS_NO_MEMORY;

    memset(serverUrls, 0, sizeof(serverUrls));

    ptr = urlsCopy;

    do {
        if (num == MAX_SERVERS) {
            s = NATS_INSUFFICIENT_BUFFER;
            break;
        }

        serverUrls[num++] = ptr;
        commaPos = strchr(ptr, ',');
        if (commaPos != nullptr) {
            ptr = (char*)(commaPos + 1);
            *(commaPos) = '\0';
        } else {
            ptr = nullptr;
        }
    } while (ptr != nullptr);

    if (s == NATS_OK)
        s = natsOptions_SetServers(gopts, (const char**) serverUrls, num);

    free(urlsCopy);

    return s;
}

PublishNats::PublishNats() {
    publisher = this;
}

PublishNats::~PublishNats() {
    // Destroy all our objects to avoid report of memory leak
    natsConnection_Destroy(conn);
    natsOptions_Destroy(opts);

    // To silence reports of memory still in used with valgrind
    nats_Close();
}

natsStatus PublishNats::createNatsOptions() {
    natsStatus status = NATS_OK;

    if (natsOptions_Create(&opts) != NATS_OK) {
        return NATS_NO_MEMORY;
    }

    // set simple options

    if (natsConfig.tls) {
        status = natsOptions_SetSecure(opts, true);
    }

    if (status == NATS_OK && ! natsConfig.tlscacert.empty()) {
        status = natsOptions_LoadCATrustedCertificates(opts, natsConfig.tlscacert.c_str());
    }

    if (status == NATS_OK && ! natsConfig.tlsciphers.empty()) {
        status = natsOptions_SetCiphers(opts, natsConfig.tlsciphers.c_str());
    }

    if (status == NATS_OK && ! natsConfig.tlshost.empty()) {
        status = natsOptions_SetExpectedHostname(opts, natsConfig.tlshost.c_str());
    }

    if (status == NATS_OK && natsConfig.tlsskip) {
        status = natsOptions_SkipServerVerification(opts, true);
    }

    if (status == NATS_OK && ! natsConfig.creds.empty()) {
        status = natsOptions_SetUserCredentialsFromFiles(opts, natsConfig.creds.c_str(), nullptr);
    }

    if (status == NATS_OK && ! natsConfig.server.empty()) {
        status = parseUrls(natsConfig.server.c_str(), opts);
    }

    if (status == NATS_OK && (! natsConfig.tlscert.empty() || ! natsConfig.tlskey.empty())) {
        status = natsOptions_LoadCertificatesChain(opts, natsConfig.tlscert.c_str(), natsConfig.tlskey.c_str());
    }

    return status;
}

natsStatus PublishNats::connect() {
    natsStatus status = createNatsOptions();

    if (status != NATS_OK) {
        esyslog("[eventpub]] Error parsing arguments: %u - %s\n", status, natsStatus_GetText(status));

        nats_PrintLastErrorStack(stderr);
        natsOptions_Destroy(opts);
        nats_Close();

        return status;
    }


    status = natsConnection_Connect(&conn, opts);

    if (status != NATS_OK) {
        esyslog("[eventpub] Error natsConnection_Connect: %u - %s\n", status, natsStatus_GetText(status));
        return status;
    }

    // initialize Jetstream
    status = jsOptions_Init(&jsOpts);

    if (status != NATS_OK) {
        esyslog("[eventpub] Error jsOptions_Init: %u - %s\n", status, natsStatus_GetText(status));
        return status;
    }

    status = natsConnection_JetStream(&js, conn, &jsOpts);

    if (status != NATS_OK) {
        esyslog("[eventpub] Error natsConnection_JetStream: %u - %s\n", status, natsStatus_GetText(status));
        return status;
    }

    // First check if the stream already exists.
    jsStreamInfo *si = nullptr;
    jsErrCode jerr = static_cast<jsErrCode>(0);

    status = js_GetStreamInfo(&si, js, "vdrevents", nullptr, &jerr);

    jsStreamConfig  cfg;

    // Initialize the configuration structure.
    jsStreamConfig_Init(&cfg);
    cfg.Name = "vdrevents";

    // Set the subject
    cfg.Subjects = &predefinedSubjects[0];

    // Set the subject count
    cfg.SubjectsLen = sizeof(predefinedSubjects) / sizeof(predefinedSubjects[0]);

    // get ack from server
    cfg.NoAck = false;

    // allow TTL per message
    cfg.AllowMsgTTL = true;

    if (status == NATS_NOT_FOUND) {
        // Add the stream,
        status = js_AddStream(&si, js, &cfg, nullptr, &jerr);
    } else if (status == NATS_OK) {
        // Update the stream
        status = js_UpdateStream(&si, js, &cfg, nullptr, &jerr);
    }

    if (status != NATS_OK) {
        esyslog("[eventpub] js_AddStream/js_UpdateStream: %u - %s - jerr %d\n", status, natsStatus_GetText(status), (int)jerr);
    }

    jsStreamInfo_Destroy(si);

    return status;
}

natsStatus PublishNats::sendMessage(const std::string& subject, const std::string& payload, int ttl) {
    return natsConnection_Publish(conn, subject.c_str(), (const void*) payload.c_str(), (int)payload.length());
}

natsStatus PublishNats::sendMessageJs(const std::string& subject, const std::string& payload, int ttl) {
    natsStatus status;
    jsErrCode jerr = static_cast<jsErrCode>(0);
    jsPubAck *pa = nullptr;

    natsMsg *msg = nullptr;
    status = natsMsg_Create(&msg, subject.c_str(), nullptr, payload.c_str(), (int)payload.length());

    if (status != NATS_OK) {
        esyslog("[eventpub] Error natsMsg_Create: %u - %s\n", status, natsStatus_GetText(status));
        return status;
    }

    if (ttl > 0) {
        // set ttl nats header (c-api has problems with higher ttl values)
        status = natsMsgHeader_Set(msg, "Nats-TTL", (std::to_string(ttl) + "h0m0s").c_str());
    }
    status = js_PublishMsg(&pa, js, msg, nullptr, &jerr);

    if (status == NATS_OK) {
        jsPubAck_Destroy(pa);
    } else {
        esyslog("[eventpub] js_PublishMsg: %u - %s - jerr %d\n", status, natsStatus_GetText(status), (int)jerr);
    }

    natsMsg_Destroy(msg);

    return status;
}