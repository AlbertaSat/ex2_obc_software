FROM ubuntu:20.04
LABEL maintainer="AlbertaSat"
ENV DEBIAN_FRONTEND=noninteractive
ARG COMPONENTS=PF_HERCULES

RUN apt-get update -qq && \
    apt-get install -qq -y \
    curl \
    ca-certificates \
    git \
    # CCS Requirements \
    libc6-i386 \
    libusb-0.1-4  \
    libgconf-2-4 \
    libncurses5  \
    libpython2.7 \
    libtinfo5
# Requirements for ubuntu 19
#    libgtk2.0-0  \
#    build-essential

SHELL ["/bin/bash", "-c"]
RUN if [[ ${DOCKER_TAG} == *"all"* ]]; then COMPONENTS=all;fi;
RUN if [ "${COMPONENTS}" = "all" ]; then COMPONENTS=PF_HERCULES; fi;

# Download and install CCS
RUN mkdir -p /ti/ ~/ccs-workspace/ /download_ccs \
    && curl -L https://software-dl.ti.com/ccs/esd/CCSv10/CCS_10_4_0/exports/CCS10.4.0.00006_web_linux-x64.tar.gz | tar -xvz -C /download_ccs
RUN /download_ccs/ccs_setup_10.4.0.00006.run --prefix /ti/ --mode unattended --enable-components ${COMPONENTS}\
    && rm -rf /download_ccs

# RUN /ti/ccs/install_scripts/install_drivers.sh

ENV PATH="/ti/ccs/eclipse:${PATH}"
