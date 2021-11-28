#!/bin/bash

docker build -t bsk1 .
docker run -d --name test1 bsk1
