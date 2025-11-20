#!/bin/bash

# This is only a sample file which is called by nats-object to do the following in VDR*ELEC:
#   - save new channels.conf
#   - save VDR*ELEC upgrade in /storage/.update

# channels.conf
# The channels.conf will be copied after a system reboot. Change /.profile/autostart.sh and add the following just before
# the call of /usr/local/bin/autostart.sh:
# if [ -f /tmp/nats/channels.conf ]; then
#    mv /storage/.config/vdropt/channels.conf /storage/.config/vdropt/channels.conf.Backup.$(date +"%d.%m.%y-%T")
#    mv /tmp/nats/channels.conf /storage/.config/vdropt/channels.conf
# fi
#
# VDR*ELEC upgrade
# copy the upgrade tar to /storage/.update
# After the next reboot the update will be installed
#


# get all arguments
if [[ "$#" -ne 2 ]]; then
   echo "Exactly 2 arguments are needed (time, filename)"
   exit 1
fi

TIME=$1
FILENAME=$2

## check channels.conf
if [[ "${FILENAME}" =~ channels\.conf$ ]]; then
  # check if the channels.conf is really different (needs Kodi Addon system-tools for cmp)
 cmp --silent ${FILENAME} /storage/.config/vdropt/channels.conf && exit 0

  if [ $(systemctl is-active vdropt) = 'active' ]; then
      # VDR is running. Copy the files to the desired directory and restart VDR
      systemctl stop vdropt
      mv /storage/.config/vdropt/channels.conf /storage/.config/vdropt/channels.conf.Backup.$(date +"%d.%m.%y-%T")
      mv ${FILENAME} /storage/.config/vdropt/channels.conf
      systemctl start vdropt
  else
      # VDR is not running. Copy the files to the desired directory
      mv /storage/.config/vdropt/channels.conf /storage/.config/vdropt/channels.conf.Backup.$(date +"%d.%m.%y-%T")
      mv ${FILENAME} /storage/.config/vdropt/channels.conf
  fi
fi;

## check release upgrade
if [[ ${FILENAME} =~ ^CE-.*\.tar$ ]] || [[ ${FILENAME} =~ ^LE-.*\.tar$ ]] || [[ ${FILENAME} =~ ^LibreELEC-.*\.tar$ ]] || [[ ${FILENAME} =~ ^CoreELEC-.*\.tar$ ]]; then
    if [ -f /storage/.update/last_version ]; then
        if [[ ${FILENAME} = $(cat /storage/.update/last_version) ]]; then
            # upgrade is already installed
            exit 0
        fi
    fi

    # delete old files. Can happen if multiple upgrades are received without an reboot
    rm -f /storage/.update/CE*.tar
    rm -f /storage/.update/LE*.tar
    rm -f /storage/.update/LibreELEC*.tar
    rm -f /storage/.update/CoreELEC*.tar

    # copy upgrade file
    echo "${FILENAME}" > /storage/.update/last_version
    mv ${$FILENAME} /storage/.update

    # After next reboot, the upgrade will be installed
    # reboot
fi
