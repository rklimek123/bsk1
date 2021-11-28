#!/bin/bash

docker build --no-cache=true -t bsk1 .
docker run -d --name test1 bsk1
