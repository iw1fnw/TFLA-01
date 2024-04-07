#!/bin/sh
set -e

read VERSION < VERSION
ARCHIVE=tfla-01-$VERSION.tar.bz2
git archive --format=tar --prefix=tfla-01-$VERSION/ master | bzip2 > $ARCHIVE
echo "Archive '$ARCHIVE' ready!"
