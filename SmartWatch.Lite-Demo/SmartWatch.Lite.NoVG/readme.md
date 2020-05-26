# 项目说明

### 1.编译说明

​	由于该项目是针对小资源的板子的，这些板子一般都是小 Flash 和小 RAM，所以 awtk-port/awtk_config.h 的宏是**取消定义 WITH_STB_IMAGE 和 WITH_STB_FONT 宏的**。

​	在 tool 文件夹中是 clock_pinter_gen 工具的源码，这一部分的代码只能在 PC 上面编译使用，**不能放到嵌入式平台中做交叉编译**。

### 2.裁剪板手表项目的资源替换问题

​	由于该版本是使用在没有 vg 的情况下，所以表盘的指针旋转提供了另外一个方法来实现，该方法是通过两个控件来实现的，这两个控件的名字分别为 watch_clock 和 watch_clock_pointer 来配合实现。

​	watch_clock_pointer 控件的指针图片资源在 .\res\assets\default\raw\images\clock_pointer 文件夹中，该文件夹下分别提供了 watch_main_hour ，watch_main_minute 和 watch_main_second 三个文件夹，这三个文件夹中分别存放这对应的指针贴图。

​	例如在 watch_main_hour 文件夹中，存放了时针的 8 张图片，编号分别是 0 到 7 的序号，分别代表表盘上面的 0 到 7 分钟的指向表针贴图，如果用户需要替换图片资源，就生成对应的时针资源。

​	clock_pinter_gen 工具是会生成一个指针的资源，把资源文件夹下的指针图片压缩保存到一个文件中，给程序使用。

​	clock_pinter_gen 的用法： clock_pinter_gen.exe [空格] image_dir [空格] save_file_path [空格] image_name_format

|                   | 说明                   | 备注                                                         |
| :---------------- | ---------------------- | ------------------------------------------------------------ |
| image_dir         | 指针图片资源文件夹路径 | 例如上面说的 .\res\assets\default\raw\images\clock_pointer\watch_main_hour 文件夹 |
| save_file_path    | 保存资源路径           | 例如 .\res\assets\default\raw\data\watch_main_hour           |
| image_name_format | 图片文件名格式         | 例如 watch_main_hour_%4d                                     |

> 备注：
>
> 1. watch_clock_pointer 控件中的 image 属性是设置使用 clock_pinter_gen 工具生成的资源，该资源一定需要放在 data 文件夹中。
> 2. 指针贴图资源一定是需要8张图片，不能缺少否则界面显示不正常。

### 3.手表项目 rom 和 ram 问题

​	本项目是基于小资源平台做的，所以本项目的 ROM 大概为 550 KB左右，峰值的 RAM 大概为 70 KB 左右，考虑到内存碎片化的问题，所以建议 RAM 大于等于 100 KB 会比较合理。

### 4.手表项目过度动画问题

​	在项目中的过度动画，是通过定义 WATCH_ANIMATOR 实现的，如果不定义该宏，所有的过度动画都改为直接切换画面，主要是为了针对低端平台性能比较低的情况，过度效果比较差的问题。

> 备注：
>
> ​	由于过度动画是通过 awtk 的 widget 动画来实现的，所以需要取消定义 WITHOUT_WIDGET_ANIMATORS 宏来让 awtk 支持 widget 动画，同时需要添加 awtk 的相关代码进行编译。