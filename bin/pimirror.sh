rsync  \
    --progress \
    --stats \
    --archive --verbose --delete --delete-delay --delay-updates \
    archive.raspbian.org::archive raspbian
#    -normal \
