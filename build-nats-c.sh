#!/bin/bash

VERSION=3.11.0

# Build latest nats.c locally.
# The nats.c library can - of course - be installed via a packet manager like apt.
# But sometimes it useful to have newer client libraries.
# The client libraries in local_nats/lib/*.so* needs to be installed in e.g. /usr/local/lib

CURDIR=$(pwd)

# Download the latest release
mkdir -p thirdparty
cd thirdparty
wget  https://github.com/nats-io/nats.c/archive/refs/tags/v${VERSION}.tar.gz -O - | tar -xzf -

# build
cd nats.c-${VERSION}
mkdir -p build
cd build
cmake .. -DNATS_BUILD_WITH_TLS=ON -DNATS_BUILD_USE_SODIUM=ON -DCMAKE_INSTALL_PREFIX=${CURDIR}/local_nats -DPOSITION_INDEPENDENT_CODE=ON

make -j && make install
