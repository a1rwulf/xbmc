#!/bin/bash

git archive --format=tar.gz --prefix=kodi-17.1-$(git rev-parse --short HEAD)/ $(git rev-parse --short HEAD) > kodi-17.1-$(git rev-parse --short HEAD).tar.gz
