#include "eventconfig.h"

// enable subject (event) for stream vdrevents
bool vdr_timer_add_enabled;
bool vdr_timer_del_enabled;
bool vdr_channelswitch_enabled;
bool vdr_recording_start_enabled;
bool vdr_recording_stop_enabled;
bool vdr_replay_start_enabled;
bool vdr_replay_stop_enabled;
bool vdr_volume_enabled;
bool vdr_audiotrack_enabled;
bool vdr_audiochennel_enabled;
bool vdr_subtitletrack_enabled;
bool vdr_osdtitle_enabled;
bool vdr_osdstatusmessage_enabled;
bool vdr_osdchannel_enabled;

// per message time-to-live in hours, 0 means server default
int vdr_timer_add_ttl;
int vdr_timer_del_ttl;
int vdr_channelswitch_ttl;
int vdr_recording_start_ttl;
int vdr_recording_stop_ttl;
int vdr_replay_start_ttl;
int vdr_replay_stop_ttl;
int vdr_volume_ttl;
int vdr_audiotrack_ttl;
int vdr_audiochennel_ttl;
int vdr_subtitletrack_ttl;
int vdr_osdtitle_ttl;
int vdr_osdstatusmessage_ttl;
int vdr_osdchannel_ttl;
