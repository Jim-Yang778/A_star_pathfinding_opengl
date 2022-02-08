# A*寻路算法可视化 OpenGL实现

## 基本介绍

### 算法部分

实现了网格图中的A*寻路算法的可视化

寻路算法中的代价函数 f(n) = g(n) + h(n) 中的g(n)和h(n)都是欧几里得距离



### OpenGL部分 

根据 https://learnopengl-cn.github.io/ 中前三章的内容学习并实现

完成了：

1. 模型加载
2. 冯氏光照模型
3. 多光源：Y轴方向直线光 + 两个点光源

<img src="/res/file_ext/aStar.gif"/>

## TODO部分

* 代码中实现各节点(node)的层级结构

* 阴影
* 帧缓冲

