# VDR plugin eventpub
As the name says, the plugin sends (publish) events to a nats server. Any stream client can read 
the events and can be used as monitor or to start some actions.
Which events shall be send by the plugin and the TTL (time to live) can be configured in the VDR plugin setup.
Not all possible events are really useful, but only implemented, because they exists.

In directory `nats-receiver` a stream client can be found which reads configurable subjects and call a configurable 
script, if a message has been received. The client reads the messages sequentially.

A dummy call which reads all messages (`--subject="vdr.>"`) and call a script is e.g.
> ./nats-receiver --server="nats://127.0.0.1:4222" --stream="vdrevents" --subject="vdr.>" --script="./sample-script.sh" --name="mytest"

In directory `nats-receiver/markad` a sample script and systemd service script can be found, which triggers markad,
if a recording has been finished.

With `nats` it's also possible to monitor all messages in realtime

> /usr/local/bin/nats sub --stream vdrevents

The above call also prints all messages which are not yet deleted, because of TTL timeout.

Currently the following subjects can be created:
```
vdr.timer.add.VDRNAME
vdr.timer.del.VDRNAME
vdr.channelswitch.VDRNAME
vdr.recording.start.VDRNAME
vdr.recording.stop.VDRNAME
vdr.replay.start.VDRNAME
vdr.replay.stop.VDRNAME
vdr.volume.absolute.VDRNAME
vdr.volume.relative.VDRNAME
vdr.audiotrack.VDRNAME
vdr.audiochennel.VDRNAME
vdr.subtitletrack.VDRNAME
vdr.osdstatusmessage.VDRNAME
vdr.osdchannel.VDRNAME
```
The `VDRNAME` is part of the configuration of the plugin.

# Messages per subject
- ***vdr.timer.add.testvdr***<br >
  1:C-1-1079-11110:2025-11-04:1608:1710:50:99:Die Rosenheim-Cops:
- ***vdr.timer.del.testvdr***<br >
  1:C-1-1079-11110:2025-11-05:1608:1710:50:99:Die Rosenheim-Cops:
- ***vdr.channelswitch.testvdr***<br >
  ZDF HD;ZDF:450000:C0M256:C:6900:6110=27:0;6120=deu@106,6121=mis@106,6123=mul@106:6130;6131=deu:0:11110:1:1079:0
- ***vdr.recording.start.testvdr***<br >
  /home/vdr/videodir/@ZDF-Mittagsmagazin_Magazin_(DE_2025)/2025-11-04.12.58.2-0.rec
- ***vdr.recording.stop.testvdr***<br >
  /home/vdr/videodir/@ZDF-Mittagsmagazin_Magazin_(DE_2025)/2025-11-04.12.58.2-0.rec
- ***vdr.replay.start.testvdr***<br >
  /home/vdr/videodir/@ZDF-Mittagsmagazin_Magazin_(DE_2025)/2025-11-04.12.58.2-0.rec
- ***vdr.replay.stop.testvdr***<br >
  /home/vdr/videodir/@ZDF-Mittagsmagazin_Magazin_(DE_2025)/2025-11-04.12.58.2-0.rec
- ***vdr.volume.absolute.testvdr***<br >
  5
- ***vdr.volume.relative.testvdr***<br >
  5
- ***vdr.audiotrack.testvdr***<br >
  1:mis
- ***vdr.audiochennel.testvdr***<br >
  -1
- ***vdr.subtitletrack.testvdr***<br >
  currently unknown :(
- ***vdr.osdstatusmessage.testvdr***<br >
  Aufzeichnung beenden?
- ***vdr.osdchannel.testvdr***<br >
  1  Das Erste HD

# Possible use cases
- Monitor the creation and deletion of timers (subjects vdr.timer.add.testvdr and vdr.timer.del.testvdr)
- Trigger markad (subject vdr.recording.stop.testvdr)
- ....

The executable `nats-receiver` and the configurable script available in nats-receiver is a good starting point to implement something useful.

# Build
> make -j && make install

# Simple plugin configuration
```
[eventpub]
--name=develop
--server=nats://10.183.229.141:4222
```
The server parameter contains the address of the nats server or (if configured) of servers part of the nats cluster.
```
--server=nats://10.183.229.141:4222,nats://10.183.229.142:4222,nats://10.183.229.143:4222
```
The name parameter needs to be unique for every VDR to be able to determine, which VDR has been sent the message.
The above described VDRNAME of the subject is exactly the name parameter.


# nats
Good sources for nats can be found at<br />
https://nats.io/<br />
https://docs.nats.io/<br />
https://natsbyexample.com/<br />

A huge list of supported languages exist to implement nats clients.<br />
https://nats.io/download/


# Install nats server
I describe only the minimum installation of the nats server. Further configuration can be found in the above links.

At first we need the nats user and group.
```
adduser nats
adduser nats nats
```
Download the latest nats server and nats client. As of time beeing the latest packages can be be found at
https://github.com/nats-io/nats-server/releases/tag/v2.12.1 and https://github.com/nats-io/natscli/releases/tag/v0.3.0.
Both server and client are standalone executables without any dependencies.

I personally used the archive files
https://github.com/nats-io/natscli/releases/download/v0.3.0/nats-0.3.0-linux-amd64.zip and
https://github.com/nats-io/nats-server/releases/download/v2.12.1/nats-server-v2.12.1-linux-amd64.tar.gz. 

```
> tar -tf nats-server-v2.12.1-linux-amd64.tar.gz
nats-server-v2.12.1-linux-amd64/LICENSE
nats-server-v2.12.1-linux-amd64/README.md
nats-server-v2.12.1-linux-amd64/nats-server

> unzip -l nats-0.3.0-linux-amd64.zip 
Archive:  nats-0.3.0-linux-amd64.zip
  Length      Date    Time    Name
---------  ---------- -----   ----
    11357  2025-09-22 16:01   nats-0.3.0-linux-amd64/LICENSE
    29457  2025-09-22 16:01   nats-0.3.0-linux-amd64/README.md
 31846584  2025-09-22 16:05   nats-0.3.0-linux-amd64/nats
---------                     -------
 31887398                     3 files
```
and copied `nats` and `nats-server` to `/usr/local/bin`.

## create storage directory for jetstreams
The VDR plugin uses jetstreams to persists all messages (with a configurable TTL (time to live)).
```
> mkdir -p /var/nats/jetstream
> chown -R nats:nats /var/nats
```
## create a nats system user 
The following configuration is necessary to be able to use `nats` to get server information and do some configuation - if wished.
```
> mkdir -p /root/.config/nats/context
> chmod -R og-rwx /root/.config/nats

> cat <<EOF >/root/.config/nats/context/sys.json
{
  "description": "",
  "url": "nats://127.0.0.1:4222",
  "socks_proxy": "",
  "token": "",
  "user": "sys",
  "password": "pass",
  "creds": "",
  "nkey": "",
  "cert": "",
  "key": "",
  "ca": "",
  "nsc": "",
  "jetstream_domain": "",
  "jetstream_api_prefix": "",
  "jetstream_event_prefix": "",
  "inbox_prefix": "",
  "user_jwt": "",
  "color_scheme": "",
  "tls_first": false,
  "windows_cert_store": "",
  "windows_cert_match_by": "",
  "windows_cert_match": "",
  "windows_ca_certs_match": null
}
EOF
```

## create a nats server configuration
A very simple configuration can be created as follows
```
> cat << EOF > /etc/nats-server.conf
# server name
server_name: server
jetstream: enabled

# Client port of 4222 on all interfaces
port: 4222

# HTTP monitoring port 
# monitor_port: 8222

jetstream {
   store_dir=/var/nats
}

accounts: {
  \$SYS: {
    users: [{user: sys, password: pass}]
  }
}
EOF

> chmod og-rwx /etc/nats-server.conf
```

## First start to test the server
The server can now be testwise started by
> /usr/local/bin/nats-server -c /etc/nats

In another console you can use `nats` to get some server information
```
> /usr/local/bin/nats --context=sys server info

Server information for server (NDRWR5NCHNM675GT6XCZUZQ25XWATIQR2OQ4KMRRZRLUFM2D4FEGGC4A)

Process Details:

                          Version: 2.12.1
                       Git Commit: fab5f99
                       Go Version: go1.25.3
                       Start Time: 2025-11-04 12:11:57
          Configuration Load Time: 2025-11-04 12:11:57
             Configuration Digest: sha256:31ca85891a72916fe51f96b5409b31e659dcc7d2a2e4fff87f9f4d678e922139
                           Uptime: 15s

Connection Details:

                    Auth Required: true
                     TLS Required: false
                             Host: 0.0.0.0:4222

JetStream:

                           Domain: 
                API Support Level: 2
                Storage Directory: /var/nats/jetstream
                  Active Accounts: 1
                    Memory In Use: 0 B
                      File In Use: 0 B
                     API Requests: 0
                       API Errors: 0
       Always sync writes to disk: false
             Write sync Frequency: 2m0s
           Maximum Memory Storage: 47 GiB
             Maximum File Storage: 615 GiB
      Cluster Message Compression: false
                Maximum HA Assets: unlimited
              Maximum Ack Pending: unlimited
            Maximum Request Batch: unlimited
         Maximum Duplicate Window: unlimited
       Maximum Messages Per Batch: 0
            Maximum Batch Timeout: 0s
  Maximum Open Batches Per Stream: 0
  Maximum Open Batches Per Server: 0
               Strict API Parsing: true

Limits:

              Maximum Connections: 65,536
            Maximum Subscriptions: 0
                  Maximum Payload: 1.0 MiB
                      TLS Timeout: 2.00s
                   Write Deadline: 10.00s

Statistics:

                        CPU Cores: 12 0.00%
                       GOMAXPROCS: 12
                           Memory: 12 MiB
                      Connections: 1
                    Subscriptions: 64
                         Messages: 1 in 0 out
                            Bytes: 20 B in 0 B out
                   Slow Consumers: 0
```
or
```
> /usr/local/bin/nats --context=sys server list

╭───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────╮
│                                                    Server Overview                                                    │
├────────┬─────────┬──────┬─────────┬─────┬───────┬──────┬────────┬─────┬────────┬───────┬───────┬──────┬────────┬──────┤
│ Name   │ Cluster │ Host │ Version │ JS  │ Conns │ Subs │ Routes │ GWs │ Mem    │ CPU % │ Cores │ Slow │ Uptime │ RTT  │
├────────┼─────────┼──────┼─────────┼─────┼───────┼──────┼────────┼─────┼────────┼───────┼───────┼──────┼────────┼──────┤
│ server │         │ 0    │ 2.12.1  │ yes │ 1     │ 64   │      0 │   0 │ 12 MiB │ 0     │    12 │ 0    │ 1m41s  │ 16ms │
├────────┼─────────┼──────┼─────────┼─────┼───────┼──────┼────────┼─────┼────────┼───────┼───────┼──────┼────────┼──────┤
│        │ 0       │ 1    │         │ 1   │ 1     │ 64   │        │     │ 12 MiB │       │       │ 0    │        │      │
╰────────┴─────────┴──────┴─────────┴─────┴───────┴──────┴────────┴─────┴────────┴───────┴───────┴──────┴────────┴──────╯
```

## Create a systemd service script
If everything looks okay, it's time create a systemd service file
```
> cat << EOF > /etc/systemd/system/nats-server.service
[Unit]
Description=NATS Server
After=network-online.target ntp.service

[Service]
PrivateTmp=true
Type=simple
ExecStart=/usr/local/bin/nats-server -c /etc/nats-server.conf
ExecReload=/bin/kill -s HUP \$MAINPID

# The nats-server uses SIGUSR2 to trigger Lame Duck Mode (LDM) shutdown
# https://docs.nats.io/running-a-nats-service/nats_admin/lame_duck_mode
ExecStop=/bin/kill -s SIGUSR2  \$MAINPID

# This should be \`lame_duck_duration\` + some buffer to finish the shutdown.
# By default, \`lame_duck_duration\` is 2 mins.
TimeoutStopSec=150

Restart=on-failure

User=nats
Group=nats

[Install]
WantedBy=multi-user.target
EOF
```
and reload and start the service.
```
> systemd daemon-reload
> systemd enable nats-server
> systemd start nats-server
```

That's all. The server has been installed and be used now. It's still a good idea to read the links in the first 
chapters above to get an idea of nuts and how powerful the server and also the nats cli really is. 