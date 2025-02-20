FROM ghcr.io/userver-framework/ubuntu-22.04-userver-pg:latest

RUN sudo apt update && sudo apt install -y clang-format

RUN git clone https://github.com/trusch/libbcrypt.git /tmp/libbcrypt && \
    cd /tmp/libbcrypt && \
    cmake . && \
    cmake --build . && \
    sudo cmake --install . && \
    rm -rf /tmp/libbcrypt

RUN git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
    cd /tmp/jwt-cpp && \
    cmake . &&  cmake --build .  && \
    sudo cmake --install . && \
    rm -rf /tmp/jwt-cpp


WORKDIR /app
COPY . .

RUN cmake -B build && cd build &&\
    cmake --build . -- -j$(nproc) && cd ..


CMD ["./build/auth_service", "--config", "./configs/static_config.yaml"]