一种可能的应用是自动生成缩略图, 便于迅速浏览3D视图, 人工快速识别

之前的视角选择没有一个统一的评判标准

可以用人工选择的视角来估计物品的最佳视角, 但是这篇中只有28个人标记的68个视角, 会不会太少了点?

已提出的算法: 可视面积 / 视角熵 / 识别表现 / 曲率熵? / 轮廓熵 / 网格显著性(mash saliency)?

提高看到的显著点的个数?

可选的视角的个数越多越好

平衡面: 让物体可以平稳站立的平面

也可以把视角选择问题当成一个分类问题

本文提出了一些评判的标准
 - 可视面积
 - 可视面积的比例
 - 表面熵
 - 轮廓图的长度
 - 曲率熵(curvature entropy)
 - 网格显著性?

从正八面体开始分割三角形, 近似得到ground truth

算法的误差可以通过算法的视角与最佳视角(是一个对称集:Fourier-Mellin image matching)的最近球面距离来衡量, 再对物体求平均

一致性: 人类视角离平均值的差的和的平均

如果物体可以站住的话, 人们不会选择地面的视角

表面熵表现最好, 其次是网格显著性