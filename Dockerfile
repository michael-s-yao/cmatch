FROM debian:bookworm-slim

RUN apt-get update && \
    apt-get install -y --no-install-recommends gcc make python3 build-essential gdb && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN make

ENTRYPOINT ["bash"]
