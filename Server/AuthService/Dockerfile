FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest

RUN sudo apt update && sudo apt install -y clang-format

RUN git clone https://github.com/trusch/libbcrypt.git /tmp/libbcrypt && \
    cd /tmp/libbcrypt && \
    mkdir build && cd build && \
    cmake .. && \
    make && \
    sudo make install && \
    rm -rf /tmp/libbcrypt

WORKDIR /app
COPY . .

RUN cmake -B build && cd build &&\
    cmake --build . -- -j$(nproc) && cd ..


CMD ["./build/auth_service", "--config", "./configs/static_config.yaml"]