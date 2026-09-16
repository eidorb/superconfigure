#!/bin/sh
set -e

CURDIR="$(pwd)"
sha256sum --check check.signature || exit 1
SRC_FILE=$(cat check.signature | sed 's/^[0-9a-f]*\s*//g')

echo "check successful. extracting from $SRC_FILE"
case "$SRC_FILE" in
    sources.tar.gz )
        tar xf sources.tar.gz --no-same-owner
        ;;
    sources.tar.xz )
        tar xf "$SRC_FILE" --no-same-owner
        ;;
    sources.tar.lz )
        tar xf "$SRC_FILE" --no-same-owner
        ;;
    sources.tar.bz2 )
        tar xf "$SRC_FILE" --no-same-owner
        ;;
    sources.zip )
        unzip -qo sources.zip
        ;;
    * )
        echo "unable to extract files"
        exit 1
        ;;
esac
