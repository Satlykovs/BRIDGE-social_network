FROM ghcr.io/userver-framework/ubuntu-22.04-userver:latest

<<<<<<< HEAD
RUN sudo apt update && sudo apt install -y clang-format
=======
#RUN sudo apt update && sudo apt install -y clang-format
>>>>>>> dev/Satlykov



WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && cd build &&\
    cmake --build . -- -j$(nproc) && cd ..


CMD ["./build/profile_service", "--config", "./configs/static_config.yaml"]