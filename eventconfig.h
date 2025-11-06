#pragma once

// enable subject (event) for stream vdrevents
extern bool vdr_timer_add_enabled;
extern bool vdr_timer_del_enabled;
extern bool vdr_channelswitch_enabled;
extern bool vdr_recording_start_enabled;
extern bool vdr_recording_stop_enabled;
extern bool vdr_replay_start_enabled;
extern bool vdr_replay_stop_enabled;
extern bool vdr_volume_enabled;
extern bool vdr_audiotrack_enabled;
extern bool vdr_audiochennel_enabled;
extern bool vdr_subtitletrack_enabled;
extern bool vdr_osdtitle_enabled;
extern bool vdr_osdstatusmessage_enabled;
extern bool vdr_osdchannel_enabled;

// per message time-to-live in hours, 0 means server default
extern int vdr_timer_add_ttl;
extern int vdr_timer_del_ttl;
extern int vdr_channelswitch_ttl;
extern int vdr_recording_start_ttl;
extern int vdr_recording_stop_ttl;
extern int vdr_replay_start_ttl;
extern int vdr_replay_stop_ttl;
extern int vdr_volume_ttl;
extern int vdr_audiotrack_ttl;
extern int vdr_audiochennel_ttl;
extern int vdr_subtitletrack_ttl;
extern int vdr_osdtitle_ttl;
extern int vdr_osdstatusmessage_ttl;
extern int vdr_osdchannel_ttl;
