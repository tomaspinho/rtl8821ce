FROM ubuntu:latest AS build

ARG KVER=5.8
ARG URL_LINUX_HEADERS=https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.8-rc2/amd64/linux-headers-5.8.0-050800_5.8.0-050800.202006212330_all.deb
ARG URL_LINUX_HEADERS_GENERIC=https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.8-rc2/amd64/linux-headers-5.8.0-050800-generic_5.8.0-050800.202006212330_amd64.deb
ARG URL_LINUX_MODULES=https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.8-rc2/amd64/linux-modules-5.8.0-050800-generic_5.8.0-050800.202006212330_amd64.deb
ARG URL_LINUX_IMAGE=https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.8-rc2/amd64/linux-image-unsigned-5.8.0-050800-generic_5.8.0-050800.202006212330_amd64.deb

ENV WORKDIR /build/

RUN mkdir -p $WORKDIR

WORKDIR /debs

RUN apt-get update

RUN apt-get install -y bc build-essential dkms linux-headers-$(uname -r) wget linux-base

RUN wget $URL_LINUX_HEADERS $URL_LINUX_HEADERS_GENERIC $URL_LINUX_MODULES $URL_LINUX_IMAGE

RUN dpkg -i ./*.deb
RUN apt-get install -f

WORKDIR $WORKDIR

FROM build

ADD . .

CMD /bin/bash