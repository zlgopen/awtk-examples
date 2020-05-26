# 项目说明

### 1.编译说明

​	该项目主要分为两个主题，一个是 400 * 400 分辨率的屏幕，另外一个是 320 * 240 分辨率的屏幕。

​	320 * 240 分辨率的主题是针对小资源的板子的，这些板子一般都是小 Flash 和小 RAM，所以 awtk-port/awtk_config.h 的宏是**取消定义 WITH_STB_IMAGE 和 WITH_STB_FONT 宏的**。

#### 2. 项目资源类型选择说明

​	320 * 240 分辨率主题主要是针对 256KB 的 RAM 和 1MB 的 Flash 资源的板子，所以一般资源选用为位图字体资源格式和位图资源格式的。

​	而 400 * 400 分辨率主题主要是针对资源大一点的板子来设计的，例如 3MB 的 RAM 和 3 MB 的 Flash 的板子，所以资源一般选用字库资源格式和图片文件资源格式。

​	当然也可以根据板子的实际情况来选择设置资源类型。

#### 3. Designer 的资源选项问题

​	在 Designer 的资源选择中可选择配置的资源分别是字体和图片资源，字体资源可以分为 tff 格式（字库资源格式）或者 data 格式（位图字体资源格式），图片资源可以分为图片文件格式（res 格式）和位图资源格式（data 格式）。

​	下图是分别配置字体资源和图片资源的 Designer 截图：

![.\readme_image\font.png](.\readme_image\font.png)

​	![.\readme_image\image.png](.\readme_image\image.png)

​	该项目默认状态下，字体资源为位图字体资源格式，图片资源为位图资源格式，所以 awtk-port/awtk_config.h 的宏是**取消定义 WITH_STB_IMAGE 和 WITH_STB_FONT 宏的**。

​	如果实际的板子的 Flash 比较小，同时 RAM 比较大的话，可以把图片的资源改为原始数据，把字体资源改为 tff 格式，同时 awtk-port/awtk_config.h 的宏是**定义 WITH_STB_IMAGE 和 WITH_STB_FONT 宏的**。

> 备注：资源类型对应 awtk-port/awtk_config.h 的宏的表格：
>
> | Designer 的资源类型                           | awtk_config.h 的宏           |
> | --------------------------------------------- | ---------------------------- |
> | 图片资源的常量格式为“位图数据”                | 取消定义 WITH_STB_IMAGE 宏的 |
> | 图片资源的常量格式为“原始数据”                | 定义 WITH_STB_IMAGE 宏的     |
> | 字体资源的格式为“ tff 格式（字库格式）”       | 定义 WITH_STB_FONT 宏的      |
> | 字体资源的格式为“bitmap 格式（位图字体格式）” | 取消定义 WITH_STB_FONT 宏的  |



​	