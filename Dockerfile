FROM ubuntu:latest AS build

ENV WORKDIR /build/

RUN mkdir -p $WORKDIR

WORKDIR $WORKDIR

ADD . .

RUN apt-get update

RUN apt-get install -y bc build-essential dkms

CMD /bin/bash

# make KVER=4.15.0-46-generic