FROM satlykovs/bridges-base:1.0

#RUN sudo apt update && sudo apt install -y clang-format clang-tidy

RUN git clone https://github.com/trusch/libbcrypt.git /tmp/libbcrypt && \
    cd /tmp/libbcrypt && \
    cmake . -DCMAKE_POLICY_VERSION_MINIMUM=3.5 && \
    cmake --build . -- -j$(nproc) && \
    sudo cmake --install . && \
    rm -rf /tmp/libbcrypt

# RUN git clone https://github.com/Thalhammer/jwt-cpp.git /tmp/jwt-cpp && \
#     cd /tmp/jwt-cpp && \
#     cmake . -DCMAKE_POLICY_VERSION_MINIMUM=3.5 &&  cmake --build . -- -j$(nproc)  && \
#     cmake --install . && \
#     rm -rf /tmp/jwt-cpp


WORKDIR /app
COPY . ./code

RUN cd code && cmake -B build -DCMAKE_BUILD_TYPE=Release && cd build &&\
    cmake --build . -- -j$(nproc) && cd ..


WORKDIR /app/code
CMD ["./build/auth_service", "--config", "./configs/static_config.yaml"]