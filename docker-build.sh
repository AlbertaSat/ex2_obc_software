#!/bin/bash
docker volume create DockerVolume1
docker build --tag satellite_sim:latest .
