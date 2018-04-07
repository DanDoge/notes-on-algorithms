受人类低层次视觉的启发

视觉上感兴趣的地方, 感知启发的方法

重复出现的地方感官上并不是有趣的

基于曲线的度量并不一定能切合人们的注意

提出mesh saliency, 每一个点和周围的点做比较, 同时注意尺度的影响

之前有用2D的算法做的, 本文借鉴了它的中心-包围算法

首先计算surface curvature, 这方法很多

之后用高斯均值计算平均点, 边界取到两个σ

之后把不同尺度的估计值做差

需要suppression Itti, et al., 是suppression让它消除了纹理造成的点, 把每一个图归一化, 再乘上显著度的差值的平方, 求和

salient simplification

mash simplification有用error metric和simplification算子.

修改了Qslim算法

这里提到好多方法...

化简的方法不断寻找最小的两个点合并(huffman-like)

用分段线性函数去放大saliency map, 保留最突出的点

选择视角的方法类似, 也是选一个让看到的东西熵最大的角度

居然用的是梯度下降!!!
