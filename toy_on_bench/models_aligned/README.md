- 3D model alignment based on minimum projection area

先找投影面积最小的轴, 再找和它垂直的面上, 投影面积最小的轴, 叉乘得到第三个轴

用下面的论文看哪一个轴最稳, 当成y轴, 剩下的方差大的是x轴

x和z的取向用viewpoint entropy

- Upright orientation of man-made objects

看一个物品能不能站住, 也可以用来align模型
