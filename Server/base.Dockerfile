FROM ghcr.io/userver-framework/ubuntu-22.04-userver:v2.8


RUN sudo apt update && sudo apt install -y clang-format


RUN git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
    cd /tmp/jwt-cpp && \
    cmake . -DCMAKE_POLICY_VERSION_MINIMUM=3.5 &&  cmake --build . -- -j$(nproc)  && \
    cmake --install . && \
    rm -rf /tmp/jwt-cpp


WORKDIR /app

COPY ./Common ./Common

CMD ["sleep", "infinity"]