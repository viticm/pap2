文件扩展名说明：
===========================================================================

.inf文件
	文本文件
	一个关卡通常情况下只由一个地形组成，但某些特殊地形(如高山之颠)，可能
	由若干个彼此分离的较小地形组成，inf文件列出了所有这些地形的文件名及
	其位置、朝向等信息

.lev文件
	文本文件
	室外关卡的描述文件，定义了精灵、物品、障碍物、树木、房屋、事件触发
	区域、地形修饰物体等对象的信息(包括模型文件名、位置、朝向等)，定义了
	StartPoint。格式未定

.ttd文件
	文本文件
	是地形树的描述文件，它所包含的信息用于创建地形树，格式参见Demo.ttd

.hmv文件
	二进制文件
	是高度图文件，地形的高度数据即存于此文件中，格式如下：

	Type		Size (bytes)			Identifier		Desc
	unsigned int	4				TypeOfHP		Specify the type of height-point, 0 means char, 1 short, 2 int, 3 float.
	unsigned int	4				NumOfHPs		The number of height-points
	unsigned int	4				NumOfHPsPerRow		The number of height-points per row (ie., width of the heightmap)
	unsigned int	4				NumOfHPsPerColumn	The number of height-points per column (ie., height of the heightmap)
	TypeOfHP	NumOfHPs * sizeof(TypeOfHP)	HPBuffer		The buffer of height-points