编译：

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./build/padding -DCMAKE_INSTALL_RPATH=./build/padding/lib
cmake --build build -j $(nproc)
cmake --install build
```

安装python库：

```
pip install vtk
```



运行：

```shell
LD_LIBRARY_PATH=./build/padding/lib bash ./scripts/run.sh --input <输入路径，路径下包括vtk以及配置文件> --output <输出，可为空，默认在输入路径的out目录下> --bin ./build/padding/bin --script ./scripts
```

build docker 镜像：

```shell
docker build . -t padding-by-level-set
data=...(absolute path to the directory of input data)
docker run --rm -v $data:/app/data padding-by-level-set
```
