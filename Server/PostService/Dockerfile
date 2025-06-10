FROM satlykovs/bridges-base:1.0

#RUN sudo apt update && sudo apt install -y clang-format



WORKDIR /app
COPY . ./code


RUN cd code && cmake -B build -DCMAKE_BUILD_TYPE=Release && cd build &&\
    cmake --build . -- -j$(nproc) && cd ..

WORKDIR /app/code
CMD ["./build/post_service", "--config", "./configs/static_config.yaml"]