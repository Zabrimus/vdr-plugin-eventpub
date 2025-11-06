#pragma once

#include <vdr/plugin.h>
#include "eventconfig.h"

class cMenuSetupEvent : public cMenuSetupPage {
    private:
        int new_vdr_timer_add_enabled;
        int new_vdr_timer_del_enabled;
        int new_vdr_channelswitch_enabled;
        int new_vdr_recording_start_enabled;
        int new_vdr_recording_stop_enabled;
        int new_vdr_replay_start_enabled;
        int new_vdr_replay_stop_enabled;
        int new_vdr_volume_enabled;
        int new_vdr_audiotrack_enabled;
        int new_vdr_audiochennel_enabled;
        int new_vdr_subtitletrack_enabled;
        int new_vdr_osdtitle_enabled;
        int new_vdr_osdstatusmessage_enabled;
        int new_vdr_osdchannel_enabled;

        int new_vdr_timer_add_ttl;
        int new_vdr_timer_del_ttl;
        int new_vdr_channelswitch_ttl;
        int new_vdr_recording_start_ttl;
        int new_vdr_recording_stop_ttl;
        int new_vdr_replay_start_ttl;
        int new_vdr_replay_stop_ttl;
        int new_vdr_volume_ttl;
        int new_vdr_audiotrack_ttl;
        int new_vdr_audiochennel_ttl;
        int new_vdr_subtitletrack_ttl;
        int new_vdr_osdtitle_ttl;
        int new_vdr_osdstatusmessage_ttl;
        int new_vdr_osdchannel_ttl;

    protected:
        void Store() override;

    public:
        cMenuSetupEvent();
};