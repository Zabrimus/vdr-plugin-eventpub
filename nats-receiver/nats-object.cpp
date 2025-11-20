#include <cstdio>
#include <getopt.h>
#include <string>
#include <nats/nats.h>
#include <cstring>
#include <csignal>
#include <thread>
#include <vector>
#include <filesystem>
#include "tiny-process/process.hpp"

bool runloop = true;

std::string servers;
std::string store;
std::string dir;
std::string script;

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
    objStoreConfig      cfg;
    objStore            *obs = nullptr;
    objStoreWatcher     *watch = nullptr;
    natsStatus          status;

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    // read and save all parameters
    static const struct option long_options[] = {
            {"server",      required_argument, nullptr, 's'},
            {"store",       required_argument, nullptr, 'o'},
            {"directory",   required_argument, nullptr, 'd'},
            {"script",      required_argument, nullptr, 'x'},
            {nullptr,       no_argument, nullptr, 0}
    };

    int c;
    while ((c = getopt_long(argc, argv, "s:o:", long_options, nullptr)) != -1) {
        switch (c) {
            case 's':
                servers = std::string(optarg);
                break;

            case 'o':
                store = std::string(optarg);
                break;

            case 'd':
                dir = std::string(optarg);
                break;

            case 'x':
                script = std::string(optarg);
                break;

            default:
                break;
        }
    }

    if (servers.empty() || store.empty() || dir.empty()) {
        printf("Usage:\n");
        printf("%s --server=<server> --store=<object_store> --directory=<directory> --script=<script>\n", argv[0]);
        printf("   server, store, directory and script are mandatory\n");
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

    if ((status = natsConnection_JetStream(&js, conn, &jsOpts)) != NATS_OK) {
        fprintf(stderr, "Error init js: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    objStoreConfig_Init(&cfg);
    cfg.Bucket = store.c_str();
    cfg.Storage = js_FileStorage;
    cfg.TTL = 100000; // in millisekunden. Wahrscheinlich muss man das ändern

    // create or connect to object store
    if (js_UpdateObjectStore(&obs, js, &cfg) == NATS_NOT_FOUND) {
        if (js_CreateObjectStore(&obs, js, &cfg) != NATS_OK) {
            fprintf(stderr, "Error creating object store: %u - %s\n", status, natsStatus_GetText(status));
            exit(1);
        }
    }

    if ((status = objStore_Watch(&watch, obs, nullptr)) != NATS_OK) {
        fprintf(stderr, "Error creating object watcher: %u - %s\n", status, natsStatus_GetText(status));
        exit(1);
    }

    objStoreInfo *info = nullptr;

    while (runloop) {
        status = objStoreWatcher_Next(&info, watch, 100);

        if (status == NATS_TIMEOUT) {
            // no new object currently available, wait 5 seconds
            if (runloop) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            continue;
        } else  if (status != NATS_OK) {
            fprintf(stderr, "Object Store next: %u - %s\n", status, natsStatus_GetText(status));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (info != nullptr) {
            int64_t time = info->ModTime;

            // create directory if necessary
            std::string filename;
            std::string path;
            try {
                filename = dir + "/" + std::string(info->Meta.Name);
                auto const pos = filename.find_last_of('/');
                path = filename.substr(0, pos);
                std::filesystem::create_directories(path);
            } catch (std::filesystem::filesystem_error const& ex) {
                fprintf(stderr, "Error creating directory '%s'\n", path.c_str());
                fprintf(stderr, "   %s\n", ex.what());
                continue;
            }

            // download file
            if ((status = objStore_GetFile(obs, info->Meta.Name, (dir + "/" + std::string(info->Meta.Name)).c_str(), nullptr)) != NATS_OK) {
                fprintf(stderr, "Error downloading file: %s, %u - %s\n", filename.c_str(), status, natsStatus_GetText(status));
            }

            // execute script
            // call script and wait
            std::vector<std::string> callStr {
                    script, std::to_string(time), filename
            };

            TinyProcessLib::Process process(callStr, "");

            int exit = process.get_exit_status();

            if (exit != 0) {
                fprintf(stderr, "Error executing script: %ld %s %s\n", time, script.c_str(), filename.c_str());
            }
        }

        objStoreInfo_Destroy(info);
        info = nullptr;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Destroy all our objects to avoid report of memory leak
    jsCtx_Destroy(js);
    natsSubscription_Destroy(sub);
    natsConnection_Destroy(conn);
    natsOptions_Destroy(opts);
    nats_Close();

    return 0;
}