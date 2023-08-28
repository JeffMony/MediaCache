
CURRENT_DIR=$(pwd)

SOURCE_STREAM=git@github.com:openssl/openssl.git

TAG_NAME=OpenSSL_1_1_1v

DIST_DIR=${CURRENT_DIR}/openssl

git clone ${SOURCE_STREAM} ${DIST_DIR}

cd ${DIST_DIR}

git checkout -b ${TAG_NAME} ${TAG_NAME}

cd -