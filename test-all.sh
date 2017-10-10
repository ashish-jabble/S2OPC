#!/bin/bash


#  Builds INGOPCS OPC UA stack and run tests
#
#  Binary files are generated in out/
#  Use "LOCAL" as first argument to run tests locally
#

set -e

BIN_DIR=./bin

DOCKER_IMAGE=abc5dd2cdb44
ISLOCAL=$1

mid() {
if [[ -z $ISLOCAL || $ISLOCAL != "LOCAL" ]]; then
    sudo /etc/scripts/make-in-docker $DOCKER_IMAGE CC=gcc "$@"
else
    make "$@"
fi
}

# Build and run tests
if [[ -z $ISLOCAL || $ISLOCAL != "LOCAL" ]]; then
    mid cleanall all
else
    ./cleanall.sh
    ./pre-build.sh
    ./build.sh
fi


# run helpers tests
export CK_TAP_LOG_FILE_NAME=$BIN_DIR/helpers.tap && $BIN_DIR/check_helpers
# run sockets test
export CK_TAP_LOG_FILE_NAME=$BIN_DIR/sockets.tap && $BIN_DIR/check_sockets
# run secure channels client / server test
./run_client_server_test_SC_level.sh

# run services tests
## unitary service tests
./bin/toolkit_test_read
if [[ $? -eq 0 ]]; then
    echo "ok 1 - test: read service test: Passed" > $BIN_DIR/service_read.tap
else
    echo "not ok 1 - test: read service test: $?" > $BIN_DIR/service_read.tap
fi
echo "1..1" >> $BIN_DIR/service_read.tap

./bin/toolkit_test_write
if [[ $? -eq 0 ]]; then
    echo "ok 1 - test: write service test: Passed" > $BIN_DIR/service_write.tap
else
    echo "not ok 1 - test: write service test: $?" > $BIN_DIR/service_write.tap
fi
echo "1..1" >> $BIN_DIR/service_write.tap

## run toolkit cilent / server test
./run_client_server_test.sh
