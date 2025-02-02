## Def

```cpp
typedef Matrix<float, 4, 4> Matrix4f;
typedef Matrix<float, 3, 1> Vector3f;
typedef Matrix<int, 1, 2> RowVector2i;

typedef Matrix<double, Dynamic, Dynamic> MatrixXd;
typedef Matrix<int, Dynamic, 1> VectorXi;
Matrix<float, 3, Dynamic>
```

## 访问元素

Init size: 

```cpp
MatrixXd m(2, 2); // init size
m(1,1) = m(1,0) + m(0,1); // m(row, col)

VectorXd v(2);
```

`m(index)` 也可以用于获取矩阵元素，但取决于 matrix 的存储顺序，默认是按列存储的

`[]` 操作符可以用于向量元素的获取，但是不能用于 matrix

Init nums:

```cpp
Matrix3f m;
m << 1, 2, 3,
     4, 5, 6,
     7, 8, 9;

// For argument: 
Eigen::MatrixXd(3, 3) <<
    0.0,0.0,0.0,
    0.0,0.0,1.0,
    0.0,1.0,0.0;

// For some new versions
Eigen::Vector3d tmpVec{ {testV(i, 0), testV(i, 1), testV(i, 2)} };
```

## Size

`rows()`, `cols()`, `size()`, `size() = rows() * cols()`

```cpp
VectorXd v(2);
v.resize(5);
```

如果对指定大小的 Matrix 进行与定义不同的 `resize()`, 会报错

？？ 如果matrix的实际大小不改变, resize函数不做任何操作。resize操作会执行析构函数: 元素的值会被改变, 如果不想改变, 执行 conservativeResize()。

## Assignment

```cpp
MatrixXf a(2,2);
std::cout << "a is of size " << a.rows() << "x" << a.cols() << std::endl;
MatrixXf b(3,3);
// ------ Here ------
a = b;
// ------ Here ------
std::cout << "a is now of size " << a.rows() << "x" << a.cols() << std::endl;

a is of size 2x2
a is now of size 3x3
```

