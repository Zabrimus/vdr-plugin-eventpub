#include <cstdio>
#include <getopt.h>
#include <string>
#include <nats/nats.h>
#include <cstring>
#include <csignal>
#include <thread>
#include <vector>
#include "tiny-process/process.hpp"

bool runloop = true;

std::string servers;
std::string stream;
std::string subject;
std::string script;
std::string durableName;

natsConnection      *conn  = nullptr;
natsOptions         *opts  = nullptr;
natsSubscription    *sub   = nullptr;
jsCtx               *js    = nullptr;

// helper methods

// Found in https://github.com/nats-io/nats.c/blob/main/examples/examples.h
static natsStatus parseUrls(const char *urls, natsOptions *gopts) {
    char        *serverUrls[10];
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
        if (num == 10) {
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

    if (s == NATS_OK) {
        s = natsOptions_SetServers(gopts, (const char **) serverUrls, num);
    }

    free(urlsCopy);

    return s;
}

void sighandler(int signum) {
    runloop = false;
}

int main(int argc, char *argv[]) {
    jsErrCode           jerr   = static_cast<jsErrCode>(0);
    jsOptions           jsOpts;
    jsSubOptions        so;
    natsStatus          status;

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    // read and save all parameters
    static const struct option long_options[] = {
            {"server",      required_argument, nullptr, 's'},
            {"stream",      required_argument, nullptr, 't'},
            {"subject",     required_argument, nullptr, 'u'},
            {"script",      required_argument, nullptr, 'x'},
            {"durable",     optional_argument, nullptr, 'd'},
            {nullptr,       no_argument, nullptr, 0}
    };

    int c;
    while ((c = getopt_long(argc, argv, "s:t:u:x:d:", long_options, nullptr)) != -1) {
        switch (c) {
            case 's':
                servers = std::string(optarg);
                break;

            case 't':
                stream = std::string(optarg);
                break;

            case 'u':
                subject = std::string(optarg);
                break;

            case 'x':
                script = std::string(optarg);
                break;

            case 'd':
                durableName = std::string(optarg);
                break;

            default:
                break;
        }
    }

    if (servers.empty() || stream.empty() || subject.empty() || script.empty()) {
        printf("Usage:\n");
        printf("%s --server=<server> --stream=<stream> --subject=<subject> --script=<script> --name=<name>\n", argv[0]);
        printf("   server, stream, script are mandatory\n");
        printf("   if durable is not set, then all existing messages will be read, otherwise only new ones.\n");
        printf("   durable should be unique across all nats-receiver.\n");
        exit(1);
    };

    if (natsOptions_Create(&opts) != NATS_OK) {
        return NATS_NO_MEMORY;
    }

    status = parseUrls(servers.c_str(), opts);
    if (status != NATS_OK) {
        fprintf(stderr, "Error parsing servers: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    if ((status = natsConnection_Connect(&conn, opts)) != NATS_OK) {
        fprintf(stderr, "Error create connection: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    if ((status = jsOptions_Init(&jsOpts)) != NATS_OK) {
        fprintf(stderr, "Error init js options: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    if ((status = jsSubOptions_Init(&so)) != NATS_OK) {
        fprintf(stderr, "Error init js suboptions: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    so.Stream = stream.c_str();

    if ((status = natsConnection_JetStream(&js, conn, &jsOpts)) != NATS_OK) {
        fprintf(stderr, "Error init js: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    // Check if the stream already exists.
    jsStreamInfo *si = nullptr;
    status = js_GetStreamInfo(&si, js, stream.c_str(), nullptr, &jerr);

    if (status == NATS_NOT_FOUND) {
        jsStreamConfig cfg;

        // Initialize the configuration structure.
        jsStreamConfig_Init(&cfg);
        cfg.Name = stream.c_str();

        // Set the subject
        const char *subjects[1];
        subjects[0] = &subject.c_str()[0];

        cfg.Subjects = &subjects[0];

        // Set the subject count
        cfg.SubjectsLen = 1;

        // get ack from server
        cfg.NoAck = false;

        // allow TTL per message
        cfg.AllowMsgTTL = true;

        // Add the stream,
        status = js_AddStream(&si, js, &cfg, nullptr, &jerr);
        fprintf(stderr, "Add Stream: %u - %s - jerr %d\n", status, natsStatus_GetText(status), (int)jerr);

        jsStreamInfo_Destroy(si);
    }

    if ((status = js_PullSubscribe(&sub, js, subject.c_str(), durableName.empty() ? nullptr : durableName.c_str(), &jsOpts, &so, &jerr)) != NATS_OK) {
        fprintf(stderr, "Error create pull subscriber: %u - %s - jerr %d\n", status, natsStatus_GetText(status), (int)jerr);
        exit(1);
    }

    if ((status = natsSubscription_SetPendingLimits(sub, -1, -1)) != NATS_OK) {
        fprintf(stderr, "Error set pending limits: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    status = jsSubOptions_Init(&so);
    if (status != NATS_OK) {
        fprintf(stderr, "Error init jsSubOptions: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    natsMsgList list;
    int         i;

    while (runloop) {
        // endless loop, wait for SIGINT, SIGKILL
        status = natsSubscription_Fetch(&list, sub, 1, 1000, &jerr);
        if (status == NATS_TIMEOUT) {
            // no new message currently available, wait 5 seconds
            if (runloop) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            }
            continue;
        } else if (status == NATS_NO_RESPONDERS) {
            // no durableName is set and no message are available
            fprintf(stderr, "No more messages are available. Exit.\n");
            exit(0);
        } else if (status != NATS_OK) {
            fprintf(stderr, "Error fetching message: %u - %s\n", status, natsStatus_GetText(status));

            // TODO: Exists there more possible errors which needs to be handled?
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            continue;
        }


        for (i = 0; (status == NATS_OK) && (i < list.Count); i++) {
            const char* msgSubject = natsMsg_GetSubject(list.Msgs[i]);
            int msgLen = natsMsg_GetDataLength(list.Msgs[i]);
            const char* msgData = natsMsg_GetData(list.Msgs[i]);

            struct jsMsgMetaData *md = nullptr;
            natsMsg_GetMetaData(&md, list.Msgs[i]);
            int64_t time = md->Timestamp;
            jsMsgMetaData_Destroy(md);

            /* Print nats headers - currently unused
            const char* *keys = nullptr;
            int         nkeys = 0;
            int         j;

            status = natsMsgHeader_Keys(list.Msgs[i], &keys, &nkeys);
            for (j = 0; (status == NATS_OK) && (j < nkeys); j++) {
                // To get all values that are associated with a key
                const char* *values = nullptr;
                int         nvalues = 0;
                int         k;

                status = natsMsgHeader_Values(list.Msgs[i], keys[j], &values, &nvalues);

                for (k = 0; (status == NATS_OK) && (k < nvalues); k++) {
                    printf("Key: '%s' Value: '%s'\n", keys[j], values[k]);
                }

                // We need to free the returned array of pointers.
                free((void*) values);
            }

            // We need to free the returned array of pointers.
            free((void*) keys);
            */

            status = natsMsg_Ack(list.Msgs[i], &jsOpts);

            // call script and wait
            std::vector<std::string> callStr {
                script, std::to_string(time), std::string(msgSubject), std::to_string(msgLen), std::string(msgData, msgLen)
            };

            TinyProcessLib::Process process(callStr, "");

            int exit = process.get_exit_status();

            if (exit != 0) {
                fprintf(stderr, "Error executing script: %ld %s %s %d %s\n", time, script.c_str(), msgSubject, msgLen, msgData);
            }
        }

        natsMsgList_Destroy(&list);
    }

    // Destroy all our objects to avoid report of memory leak
    jsCtx_Destroy(js);
    natsSubscription_Destroy(sub);
    natsConnection_Destroy(conn);
    natsOptions_Destroy(opts);
    nats_Close();

    return 0;
}