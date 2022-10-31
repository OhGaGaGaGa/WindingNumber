## Cell Type Representation

![TestLinearCellDemo](https://ohg-typora-image.oss-cn-hangzhou.aliyuncs.com/imgs/202210291855978.png)

## Some Manual C++ Output Code - Vertex

```cpp
out << "# vtk DataFile Version 2.0" << endl;
out << "vtk output" << endl;
out << "ASCII" << endl;
out << "DATASET UNSTRUCTURED_GRID" << endl;

// POINTS 点的数量 坐标表示的数据类型
out << "POINTS " << V.rows() << " double" << endl;
for (int i = 0; i < V.rows(); i++) {
    for (int j = 0; j < V.cols(); j++)
        out << V(i, j) << " ";
    out << endl;
}
out << endl;

// CELLS CELL的数量 下面数字的总数目（包括每行的第一个数，代指这一行有多少个数）
out << "CELLS " << V.rows() << " " << V.rows() * 2 << endl;
for (int i = 0; i < V.rows(); i++) {
    out << "1 " << i << endl;
}
out << endl;

// CELL_TYPES CELL的数量
// 按照文首的图片对应出CELL类型
out << "CELL_TYPES " << V.rows() << endl;
for (int i = 0; i < V.rows(); i++)
    out << 1 << endl;
out << endl;

// POINT_DATA 点的数量
// SCALARS 名称 类型 1
// LOOKUP_TABLE default
// 代号
out << "POINT_DATA " << V.rows() << endl;
out << "SCALARS color double 1" << endl;
out << "LOOKUP_TABLE default" << endl;

for (int i = 0; i < V.rows(); i++) {
    out << sort_arr[i] - min_tag << endl;
}
```

## Add VTK using CPM

```cmake
CPMAddPackage(
    NAME "VTK"
    GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/VTK.git"
    GIT_TAG "v9.1.0"
    GIT_SUBMODULES ""
    OPTIONS
    "VTK_ENABLE_LOGGING OFF"
    "VTK_ENABLE_REMOTE_MODULES OFF"
    "VTK_FORBID_DOWNLOADS ON"
    "VTK_GROUP_ENABLE_Imaging DONT_WANT"
    "VTK_GROUP_ENABLE_MPI DONT_WANT"
    "VTK_GROUP_ENABLE_Qt DONT_WANT"
    "VTK_GROUP_ENABLE_Rendering DONT_WANT"
    "VTK_GROUP_ENABLE_StandAlone DONT_WANT"
    "VTK_GROUP_ENABLE_Views DONT_WANT"
    "VTK_GROUP_ENABLE_Web DONT_WANT"
    "VTK_INSTALL_SDK OFF"
    "VTK_ENABLE_WRAPPING OFF"
    "VTK_MODULE_ENABLE_VTK_IOLegacy YES"
)
```

？然后呢