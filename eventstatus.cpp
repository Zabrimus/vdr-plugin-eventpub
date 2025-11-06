#include "eventstatus.h"
#include "eventconfig.h"
#include "natsconfig.h"

void cEventStatus::TimerChange(const cTimer *Timer, eTimerChange Change) {
    if (!Timer) {
        return;
    }

    if (Change == tcAdd && vdr_timer_add_enabled) {
        publisher->sendMessageJs("vdr.timer.add." + natsConfig.name, std::string(Timer->ToText(true)), vdr_timer_add_ttl);
    } else if (Change == tcDel && vdr_timer_del_enabled) {
        publisher->sendMessageJs("vdr.timer.del." + natsConfig.name, std::string(Timer->ToText(true)), vdr_timer_del_ttl);
    }
}

void cEventStatus::ChannelSwitch(const cDevice *Device, int ChannelNumber, bool LiveView)
{
    if (vdr_channelswitch_enabled && ChannelNumber > 0 && LiveView) {
        LOCK_CHANNELS_READ;
        publisher->sendMessageJs("vdr.channelswitch." + natsConfig.name, std::string(Channels->GetByNumber(ChannelNumber)->ToText()), vdr_channelswitch_ttl);
    }
}

void cEventStatus::Recording(const cDevice *Device, const char *Name, const char *FileName, bool On)
{
    if (On && vdr_recording_start_enabled) {
        publisher->sendMessageJs("vdr.recording.start." + natsConfig.name, std::string(FileName), vdr_recording_start_ttl);
    } else if (!On && vdr_recording_stop_enabled) {
        publisher->sendMessageJs("vdr.recording.stop." + natsConfig.name, std::string(FileName), vdr_recording_stop_ttl);
    }
}

void cEventStatus::Replaying(const cControl *Control, const char *Name, const char *FileName, bool On)
{
    if (On && vdr_replay_start_enabled) {
        publisher->sendMessageJs("vdr.replay.start." + natsConfig.name, std::string(FileName), vdr_replay_start_ttl);
    } else if (!On && vdr_replay_stop_enabled) {
        publisher->sendMessageJs("vdr.replay.stop." + natsConfig.name, std::string(FileName), vdr_replay_stop_ttl);
    }
}

void cEventStatus::SetVolume(int Volume, bool Absolute)
{
    if (vdr_volume_enabled) {
        if (Absolute) {
            publisher->sendMessageJs("vdr.volume.absolute." + natsConfig.name, std::to_string(Volume), vdr_volume_ttl);
        } else {
            publisher->sendMessageJs("vdr.volume.relative." + natsConfig.name, std::to_string(Volume), vdr_volume_ttl);
        }
    }
}

void cEventStatus::SetAudioTrack(int Index, const char * const *Tracks)
{
    if (vdr_audiotrack_enabled) {
        publisher->sendMessageJs("vdr.audiotrack." + natsConfig.name, std::to_string(Index) + ":" + std::string(Tracks[Index]), vdr_audiotrack_ttl);
    }
}

void cEventStatus::SetAudioChannel(int AudioChannel)
{
    if (vdr_audiochennel_enabled) {
        publisher->sendMessageJs("vdr.audiochennel." + natsConfig.name, std::to_string(AudioChannel), vdr_audiochennel_ttl);
    }
}

void cEventStatus::SetSubtitleTrack(int Index, const char * const *Tracks)
{
    if (vdr_subtitletrack_enabled) {
        publisher->sendMessageJs("vdr.subtitletrack." + natsConfig.name, std::to_string(Index) + ":" + std::string(Tracks[Index]), vdr_subtitletrack_ttl);
    }
}

void cEventStatus::OsdStatusMessage(const char *Message)
{
    if (vdr_osdstatusmessage_enabled && Message) {
        publisher->sendMessageJs("vdr.osdstatusmessage." + natsConfig.name, std::string(Message), vdr_osdstatusmessage_ttl);
    }
}

void cEventStatus::OsdChannel(const char *Text)
{
    if (vdr_osdchannel_enabled && Text) {
        publisher->sendMessageJs("vdr.osdchannel." + natsConfig.name, std::string(Text), vdr_osdchannel_ttl);
    }
}
