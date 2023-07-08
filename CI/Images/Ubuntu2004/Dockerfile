FROM ubuntu:20.04

ARG REPO_ROOT_DIR
ARG DEBIAN_FRONTEND=noninteractive

ENV TZ=Etc/UTC

RUN apt update
RUN apt -y install sudo tzdata
RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo
RUN bash ${REPO_ROOT_DIR}/Tools/Setup.sh
