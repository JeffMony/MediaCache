
CURRENT_DIR=$(pwd)

SOURCE_STREAM=git@github.com:libevent/libevent.git

TAG_NAME=release-2.1.12-stable

DIST_DIR=${CURRENT_DIR}/libevent

git clone ${SOURCE_STREAM} ${DIST_DIR}

cd ${DIST_DIR}

git checkout -b ${TAG_NAME} ${TAG_NAME}

cd -