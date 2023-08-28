
CURRENT_DIR=$(pwd)

SOURCE_STREAM=git@github.com:openssl/openssl.git

DIST_DIR=${CURRENT_DIR}/openssl

git clone ${SOURCE_STREAM} ${DIST_DIR}

cd ${DIST_DIR}

git checkout -b ${TAG_NAME} ${TAG_NAME}

cd -