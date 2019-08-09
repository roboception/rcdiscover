FROM alpine:latest as builder
WORKDIR /workspace
RUN apk --no-cache add g++ gcc git cmake make musl-dev linux-headers
COPY . .
RUN mkdir build && cd build && cmake -DBUILD_RCDISCOVER_GUI=OFF -DBUILD_RCDISCOVER_SHARED_LIB=OFF -DCMAKE_EXE_LINKER_FLAGS=\"-static\" .. && make

FROM alpine:latest
COPY --from=builder /workspace/build/tools/rcdiscover /usr/bin/
ENTRYPOINT ["rcdiscover"]
