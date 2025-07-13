FROM alpine:latest

RUN apk add clang clang-extra-tools
WORKDIR /mnt

CMD clang-format -i --style=Microsoft /mnt/src/*/*.cpp /mnt/inc/*.hpp