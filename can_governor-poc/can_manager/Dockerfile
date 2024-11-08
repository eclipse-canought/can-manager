FROM arm64v8/ubuntu:focal AS tests
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
     && apt-get upgrade -y \
     && apt-get install cmake \
                              make \
                              g++ \
                              gcc \
                              clang \
                              git \
                              unzip \
                              build-essential \
                              wget  -y

ARG SETUP_DIR="/root"
WORKDIR ${SETUP_DIR}
COPY . .
CMD ["gcc","-o","/governor/executable","main.c","-lpthread"]