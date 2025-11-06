/*
 * eventpub.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <getopt.h>
#include <vdr/plugin.h>
#include "eventpub.h"
#include "natsconfig.h"
#include "publishnats.h"
#include "eventconfig.h"
#include "menusetupevent.h"

static const char *VERSION = "0.0.1";
static const char *DESCRIPTION = "Publish different VDR event via nats";
static const char *MAINMENUENTRY = "Eventpub";

cPluginEventpub::cPluginEventpub() {
    // Initialize any member variables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
    eventStatus = nullptr;
}

cPluginEventpub::~cPluginEventpub() {
    // Clean up after yourself!
    delete eventStatus;
}

const char *cPluginEventpub::Version() { return VERSION; }
const char *cPluginEventpub::Description() { return DESCRIPTION; }
const char *cPluginEventpub::MainMenuEntry() { return MAINMENUENTRY; }

const char *cPluginEventpub::CommandLineHelp() {
    // Return a string that describes all known command line options.
    return "-s <urls>,     --server=<urls>         server url(s) (list of comma separated nats urls)\n"
           "-n <name>,     --name=<name>           name of VDR. Will be added to nats subject\n"
           "-t,            --tls                   use secure (SSL/TLS) connection\n"
           "-c <file>,     --tlscacert=<file>      trusted certificates file\n"
           "-e <file>,     --tlscert=<file>        client certificate (PEM format only)\n"
           "-k <file>,     --tlskey=<file>         client private key file (PEM format only)\n"
           "-i <ciphers>,  --tlsciphers=<ciphers>  use only the specified TLS cipher suites\n"
           "-o <hostname>, --tlshost=<hostname>    server certificate's expected hostname\n"
           "-l,            --tlsskip               skip server certificate verification\n"
           "-d <file>,     --creds=<file>          user credentials chained file\n";
}

bool cPluginEventpub::ProcessArgs(int argc, char *argv[]) {
    static const struct option long_options[] = {
            {"server",      required_argument, nullptr, 's'},
            {"name",        required_argument, nullptr, 'n'},
            {"tls",         optional_argument, nullptr, 't'},
            {"tlscacert",   optional_argument, nullptr, 'c'},
            {"tlscert",     optional_argument, nullptr, 'e'},
            {"tlskey",      optional_argument, nullptr, 'k'},
            {"tlsciphers",  optional_argument, nullptr, 'i'},
            {"tlshost",     optional_argument, nullptr, 'o'},
            {"tlsskip",     optional_argument, nullptr, 'l'},
            {"creds",       optional_argument, nullptr, 'd'},
            {nullptr,       no_argument, nullptr, 0}
    };

    int c;
    while ((c = getopt_long(argc, argv, "s:tc:e:k:i:o:ld:n:", long_options, nullptr)) != -1) {
        switch (c) {
            case 's':
                natsConfig.server = std::string(optarg);
                break;

            case 't':
                natsConfig.tls = true;
                break;

            case 'c':
                natsConfig.tlscacert = std::string(optarg);
                break;

            case 'e':
                natsConfig.tlscert = std::string(optarg);
                break;

            case 'k':
                natsConfig.tlskey = std::string(optarg);
                break;

            case 'i':
                natsConfig.tlsciphers = std::string(optarg);
                break;

            case 'o':
                natsConfig.tlshost = std::string(optarg);
                break;

            case 'l':
                natsConfig.tlsskip = true;
                break;

            case 'd':
                natsConfig.creds = std::string(optarg);
                break;

            case 'n':
                natsConfig.name = std::string(optarg);
                break;

            default:
                esyslog("[eventpub] Ignore unknown configuration parameter '%c'", c);
                break;
        }
    }

    return true;
}

bool cPluginEventpub::Initialize() {
    // Initialize any background activities the plugin shall perform.
    return true;
}

bool cPluginEventpub::Start() {
    // Start any background activities the plugin shall perform.
    eventStatus = new cEventStatus;

    // connect to nats
    new PublishNats();
    publisher->connect();

    return true;
}

void cPluginEventpub::Stop() {
    // Stop any background activities the plugin is performing.
    if (publisher != nullptr) {
        delete publisher;
        publisher = nullptr;
    }
}

void cPluginEventpub::Housekeeping() {
    // Perform any cleanup or other regular tasks.
}

cString cPluginEventpub::Active() {
    // Return a message string if shutdown should be postponed
    return nullptr;
}

time_t cPluginEventpub::WakeupTime() {
    // Return custom wakeup time for shutdown script
    return 0;
}

cOsdObject *cPluginEventpub::MainMenuAction() {
    // Perform the action when selected from the main VDR menu.
    return nullptr;
}

cMenuSetupPage *cPluginEventpub::SetupMenu() {
    // Return a setup menu in case the plugin supports one.
    return new cMenuSetupEvent();
}

bool cPluginEventpub::SetupParse(const char *Name, const char *Value) {
    if      (!strcasecmp(Name, "vdr_timer_add_enabled")) vdr_timer_add_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_timer_del_enabled")) vdr_timer_del_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_channelswitch_enabled")) vdr_channelswitch_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_recording_start_enabled")) vdr_recording_start_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_recording_stop_enabled")) vdr_recording_stop_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_replay_start_enabled")) vdr_replay_start_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_replay_stop_enabled")) vdr_replay_stop_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_volume_enabled")) vdr_volume_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_audiotrack_enabled")) vdr_audiotrack_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_audiochennel_enabled")) vdr_audiochennel_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_subtitletrack_enabled")) vdr_subtitletrack_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_osdtitle_enabled")) vdr_osdtitle_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_osdstatusmessage_enabled")) vdr_osdstatusmessage_enabled = atoi(Value) > 0;
    else if (!strcasecmp(Name, "vdr_osdchannel_enabled")) vdr_osdchannel_enabled = atoi(Value) > 0;

    else if (!strcasecmp(Name, "vdr_timer_add_ttl")) vdr_timer_add_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_timer_del_ttl")) vdr_timer_del_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_channelswitch_ttl")) vdr_channelswitch_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_recording_start_ttl")) vdr_recording_start_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_recording_stop_ttl")) vdr_recording_stop_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_replay_start_ttl")) vdr_replay_start_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_replay_stop_ttl")) vdr_replay_stop_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_volume_ttl")) vdr_volume_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_audiotrack_ttl")) vdr_audiotrack_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_audiochennel_ttl")) vdr_audiochennel_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_subtitletrack_ttl")) vdr_subtitletrack_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_osdtitle_ttl")) vdr_osdtitle_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_osdstatusmessage_ttl")) vdr_osdstatusmessage_ttl = atoi(Value);
    else if (!strcasecmp(Name, "vdr_osdchannel_ttl")) vdr_osdchannel_ttl = atoi(Value);
    else  return false;

    return true;
}

bool cPluginEventpub::Service(const char *Id, void *Data = nullptr) {
    // Handle custom service requests from other plugins
    return false;
}

const char **cPluginEventpub::SVDRPHelpPages() {
    // Return help text for SVDRP commands this plugin implements
    return nullptr;
}

cString cPluginEventpub::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    // Process SVDRP commands this plugin implements
    return nullptr;
}

VDRPLUGINCREATOR(cPluginEventpub); // Don't touch this!
