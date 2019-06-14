#!/bin/bash

# usually need sudo for this
docker run \
    --user root \
    --rm \
    --detach \
    --publish 8080:8080 \
    --publish 50000:50000 \
    --volume jenkins-data:/var/jenkins_home \
    --volume /var/run/docker.sock:/var/run/docker.sock \
    jenkinsci/blueocean
