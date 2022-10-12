FROM d.ryon.ren:2443/cmake-gcc:12 AS builder
WORKDIR /src
COPY cmake cmake
COPY extern extern
COPY include include
COPY src src
COPY tests tests
COPY CMakeLists.txt .
COPY dependencies.cmake .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./build/padding -DCMAKE_INSTALL_RPATH=./build/padding/lib
RUN cmake --build build -j $(nproc)
RUN cmake --install build


FROM ubuntu:22.04
WORKDIR /app
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai
COPY --from=builder /src/build/padding ./padding
RUN sed -i 's/archive.ubuntu.com/mirror.zju.edu.cn/g' /etc/apt/sources.list \
    && sed -i 's/security.ubuntu.com/mirror.zju.edu.cn/g' /etc/apt/sources.list  \
    && apt-get update \
    && apt-get install -y python3 pip libgl1-mesa-glx ffmpeg libsm6 libxext6 &&\
    pip install vtk
COPY ./scripts ./scripts
ENTRYPOINT ["./scripts/entry.sh"]
CMD [ "--input", "./data/" ]