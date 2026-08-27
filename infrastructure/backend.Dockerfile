FROM debian:bookworm
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y     build-essential cmake git pkg-config libboost-system-dev libssl-dev     libdrogon-dev libsodium-dev libjsoncpp-dev ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY backend /src/backend
RUN cmake -S /src/backend -B /src/build -DCMAKE_BUILD_TYPE=Release && cmake --build /src/build -j2
CMD ["/src/build/gateway/monetix_gateway"]
