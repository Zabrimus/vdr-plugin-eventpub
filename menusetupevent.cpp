#include "menusetupevent.h"

cMenuSetupEvent::cMenuSetupEvent() {
    new_vdr_timer_add_enabled = vdr_timer_add_enabled;
    new_vdr_timer_del_enabled = vdr_timer_del_enabled;
    new_vdr_channelswitch_enabled = vdr_channelswitch_enabled;
    new_vdr_recording_start_enabled = vdr_recording_start_enabled;
    new_vdr_recording_stop_enabled = vdr_recording_stop_enabled;
    new_vdr_replay_start_enabled = vdr_replay_start_enabled;
    new_vdr_replay_stop_enabled = vdr_replay_stop_enabled;
    new_vdr_volume_enabled = vdr_volume_enabled;
    new_vdr_audiotrack_enabled = vdr_audiotrack_enabled;
    new_vdr_audiochennel_enabled = vdr_audiochennel_enabled;
    new_vdr_subtitletrack_enabled = vdr_subtitletrack_enabled;
    new_vdr_osdtitle_enabled = vdr_osdtitle_enabled;
    new_vdr_osdstatusmessage_enabled = vdr_osdstatusmessage_enabled;
    new_vdr_osdchannel_enabled = vdr_osdchannel_enabled;

    new_vdr_timer_add_ttl = vdr_timer_add_ttl;
    new_vdr_timer_del_ttl = vdr_timer_del_ttl;
    new_vdr_channelswitch_ttl = vdr_channelswitch_ttl;
    new_vdr_recording_start_ttl = vdr_recording_start_ttl;
    new_vdr_recording_stop_ttl = vdr_recording_stop_ttl;
    new_vdr_replay_start_ttl = vdr_replay_start_ttl;
    new_vdr_replay_stop_ttl = vdr_replay_stop_ttl;
    new_vdr_volume_ttl = vdr_volume_ttl;
    new_vdr_audiotrack_ttl = vdr_audiotrack_ttl;
    new_vdr_audiochennel_ttl = vdr_audiochennel_ttl;
    new_vdr_subtitletrack_ttl = vdr_subtitletrack_ttl;
    new_vdr_osdtitle_ttl = vdr_osdtitle_ttl;
    new_vdr_osdstatusmessage_ttl = vdr_osdstatusmessage_ttl;
    new_vdr_osdchannel_ttl = vdr_osdchannel_ttl;

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Timer"))));
    Add(new cMenuEditBoolItem(tr("Add timer event"), &new_vdr_timer_add_enabled));
    Add(new cMenuEditIntItem( tr("TTL for add timer event (h)"), &new_vdr_timer_add_ttl));

    Add(new cMenuEditBoolItem(tr("Del timer event"), &new_vdr_timer_del_enabled));
    Add(new cMenuEditIntItem( tr("TTL for Del timer event (h)"), &new_vdr_timer_del_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Channel switch"))));
    Add(new cMenuEditBoolItem(tr("Channel switch event"), &new_vdr_channelswitch_enabled));
    Add(new cMenuEditIntItem( tr("TTL for Channel switch event (h)"), &new_vdr_channelswitch_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Recording"))));
    Add(new cMenuEditBoolItem(tr("Recording start event"), &new_vdr_recording_start_enabled));
    Add(new cMenuEditIntItem( tr("TTL for recording start event (h)"), &new_vdr_recording_start_ttl));

    Add(new cMenuEditBoolItem(tr("Recording end event"), &new_vdr_recording_stop_enabled));
    Add(new cMenuEditIntItem( tr("TTL for recording end event (h)"), &new_vdr_recording_stop_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Replay"))));
    Add(new cMenuEditBoolItem(tr("Replay start event"), &new_vdr_replay_start_enabled));
    Add(new cMenuEditIntItem( tr("TTL for replay start event (h)"), &new_vdr_replay_start_ttl));

    Add(new cMenuEditBoolItem(tr("Replay stop event"), &new_vdr_replay_stop_enabled));
    Add(new cMenuEditIntItem( tr("TTL for replay stop event (h)"), &new_vdr_replay_stop_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Volume"))));
    Add(new cMenuEditBoolItem(tr("Volume event"), &new_vdr_volume_enabled));
    Add(new cMenuEditIntItem( tr("TTL for volume event (h)"), &new_vdr_volume_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Audio track"))));
    Add(new cMenuEditBoolItem(tr("Audio track event"), &new_vdr_audiotrack_enabled));
    Add(new cMenuEditIntItem( tr("TTL for audio track event (h)"), &new_vdr_audiotrack_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Audio Channel"))));
    Add(new cMenuEditBoolItem(tr("Audio channel event"), &new_vdr_audiochennel_enabled));
    Add(new cMenuEditIntItem( tr("TTL for audio channel event (h)"), &new_vdr_audiochennel_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("Subtitle track"))));
    Add(new cMenuEditBoolItem(tr("Subtitle track event"), &new_vdr_subtitletrack_enabled));
    Add(new cMenuEditIntItem( tr("TTL for subtitle track event (h)"), &new_vdr_subtitletrack_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("OSD title"))));
    Add(new cMenuEditBoolItem(tr("OSD title event"), &new_vdr_osdtitle_enabled));
    Add(new cMenuEditIntItem( tr("TTL for OSD title event (h)"), &new_vdr_osdtitle_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("OSD status"))));
    Add(new cMenuEditBoolItem(tr("OSD status message event"), &new_vdr_osdstatusmessage_enabled));
    Add(new cMenuEditIntItem( tr("TTL for OSD status message event (h)"), &new_vdr_osdstatusmessage_ttl));

    Add(new cOsdItem(cString::sprintf("-------------------- %s ----------------------------------", tr("OSD channel"))));
    Add(new cMenuEditBoolItem(tr("OSD channal event"), &new_vdr_osdchannel_enabled));
    Add(new cMenuEditIntItem( tr("TTL for OSD channal event (h)"), &new_vdr_osdchannel_ttl));
}

void cMenuSetupEvent::Store() {
    SetupStore("vdr_timer_add_enabled", vdr_timer_add_enabled = new_vdr_timer_add_enabled);
    SetupStore("vdr_timer_del_enabled", vdr_timer_del_enabled = new_vdr_timer_del_enabled);
    SetupStore("vdr_channelswitch_enabled", vdr_channelswitch_enabled = new_vdr_channelswitch_enabled);
    SetupStore("vdr_recording_start_enabled", vdr_recording_start_enabled = new_vdr_recording_start_enabled);
    SetupStore("vdr_recording_stop_enabled", vdr_recording_stop_enabled = new_vdr_recording_stop_enabled);
    SetupStore("vdr_replay_start_enabled", vdr_replay_start_enabled = new_vdr_replay_start_enabled);
    SetupStore("vdr_replay_stop_enabled", vdr_replay_stop_enabled = new_vdr_replay_stop_enabled);
    SetupStore("vdr_volume_enabled", vdr_volume_enabled = new_vdr_volume_enabled);
    SetupStore("vdr_audiotrack_enabled", vdr_audiotrack_enabled = new_vdr_audiotrack_enabled);
    SetupStore("vdr_audiochennel_enabled", vdr_audiochennel_enabled = new_vdr_audiochennel_enabled);
    SetupStore("vdr_subtitletrack_enabled", vdr_subtitletrack_enabled = new_vdr_subtitletrack_enabled);
    SetupStore("vdr_osdtitle_enabled", vdr_osdtitle_enabled = new_vdr_osdtitle_enabled);
    SetupStore("vdr_osdstatusmessage_enabled", vdr_osdstatusmessage_enabled = new_vdr_osdstatusmessage_enabled);
    SetupStore("vdr_osdchannel_enabled", vdr_osdchannel_enabled = new_vdr_osdchannel_enabled);

    SetupStore("vdr_timer_add_ttl", vdr_timer_add_ttl = new_vdr_timer_add_ttl);
    SetupStore("vdr_timer_del_ttl", vdr_timer_del_ttl = new_vdr_timer_del_ttl);
    SetupStore("vdr_channelswitch_ttl", vdr_channelswitch_ttl = new_vdr_channelswitch_ttl);
    SetupStore("vdr_recording_start_ttl", vdr_recording_start_ttl = new_vdr_recording_start_ttl);
    SetupStore("vdr_recording_stop_ttl", vdr_recording_stop_ttl = new_vdr_recording_stop_ttl);
    SetupStore("vdr_replay_start_ttl", vdr_replay_start_ttl = new_vdr_replay_start_ttl);
    SetupStore("vdr_replay_stop_ttl", vdr_replay_stop_ttl = new_vdr_replay_stop_ttl);
    SetupStore("vdr_volume_ttl", vdr_volume_ttl = new_vdr_volume_ttl);
    SetupStore("vdr_audiotrack_ttl", vdr_audiotrack_ttl = new_vdr_audiotrack_ttl);
    SetupStore("vdr_audiochennel_ttl", vdr_audiochennel_ttl = new_vdr_audiochennel_ttl);
    SetupStore("vdr_subtitletrack_ttl", vdr_subtitletrack_ttl = new_vdr_subtitletrack_ttl);
    SetupStore("vdr_osdtitle_ttl", vdr_osdtitle_ttl = new_vdr_osdtitle_ttl);
    SetupStore("vdr_osdstatusmessage_ttl", vdr_osdstatusmessage_ttl = new_vdr_osdstatusmessage_ttl);
    SetupStore("vdr_osdchannel_ttl", vdr_osdchannel_ttl = new_vdr_osdchannel_ttl);
}

