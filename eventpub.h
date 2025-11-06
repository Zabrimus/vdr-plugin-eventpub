/*
 * eventpub.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#pragma once

#include <vdr/plugin.h>
#include "eventstatus.h"

class cPluginEventpub : public cPlugin {
private:
    // Add any member variables or functions you may need here.
    cEventStatus *eventStatus;

public:
    cPluginEventpub();
    ~cPluginEventpub() override;

    const char *Version() override;
    const char *Description() override;
    const char *CommandLineHelp() override;
    bool ProcessArgs(int argc, char *argv[]) override;
    bool Initialize() override;
    bool Start() override;
    void Stop() override;
    void Housekeeping() override;
    cString Active() override;
    time_t WakeupTime() override;
    const char *MainMenuEntry() override;
    cOsdObject *MainMenuAction() override;
    cMenuSetupPage *SetupMenu() override;
    bool SetupParse(const char *Name, const char *Value) override;
    bool Service(const char *Id, void *Data) override;
    const char **SVDRPHelpPages() override;
    cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) override;
};

