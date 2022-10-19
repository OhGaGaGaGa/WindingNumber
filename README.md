# WindingNumber

**Some Math Concept**: 

[Solid Angle](https://mathworld.wolfram.com/SolidAngle.html) $\Omega(\bold{p})$

定义：
$$
\Omega(\bold{p}) = \iint_\mathcal{S}\sin(\phi)\mathrm{d}\theta\mathrm{d}\phi
$$
可直观理解为将 $\mathcal{S}$ 投影到以 $\bold{p}$ 为圆心的单位圆上的圆上的投影面积，$\Omega(\bold{p})\in[-4\pi, 4\pi]$。

对于三角形面的数值计算：

Let a = vi − p, b = vj − p, c = vk − p and a = ‖a‖, b = ‖b‖, c = ‖c‖
$$
\tan\left(\dfrac{\Omega(\bold{p})}{2}\right) = \frac{\det([\bold{a}\, \bold{b}\, \bold{c}])}{abc + (\bold{a}\cdot \bold{b})c + (\bold{b}\cdot \bold{c})a+(\bold{c}\cdot \bold{a})b}
$$
**Some Related Work**: 

Jacobson, Alec, Ladislav Kavan, and Olga Sorkine-Hornung. “Robust Inside-Outside Segmentation Using Generalized Winding Numbers.” ACM Transactions on Graphics 32, no. 4 (July 21, 2013): 1–12. https://doi.org/10.1145/2461912.2461916.


Jacobson, Gavin Barill, Neil Dickson, Ryan Schmidt, David I. W. Levin, Alec. “Fast Winding Numbers for Soups and Clouds.” Accessed September 27, 2022. https://www.dgp.toronto.edu/projects/fast-winding-numbers/.

**Some Learning Notes**:

[Basic Eigen](Eigen%20Notes%20-%20Matrix%20Vector.md)

实际处理时的特殊情况：

当 $[\bold{a}, \bold{b}, \bold{c}]$ 共线时，$\tan\left(\dfrac{\Omega(\bold{p})}{2}\right) = 0$

由于 C 语言中 `atan2` 函数的返回定义域为 $[-\pi, \pi]$，具体如下

- If `y` is `±0` and `x` is negative or `-0`, `±π` is returned
- If `y` is `±0` and `x` is positive or `+0`, `±0` is returned



