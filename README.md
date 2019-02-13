# AWTK 示例使用说明

#### 项目介绍：

- awtk-demos：AWTK 内置的演示 Demo，包含各种控件的使用方法
- HelloWorld-Demo：最简单的入门 Demo，包含基本的文本和按钮控件使用方法
- CleanAir-Demo：新风系统演示 Demo
- Meter-Demo：普通工业仪表演示 Demo
- Motormeter-Demo：汽车仪表演示 Demo
- Chart-Demo：图表演示 Demo，包含仪表、饼图、曲线图、柱状图
- MusicPlayer-Demo：音乐播放器演示 Demo



#### 入门文档：

- docs/AWTK开发实践.pdf



#### 使用方法：

1. 打开已经配置好的 AWTK（Eclipse） 工程

2. 将上面某个 Demo 文件夹直接拷贝或拖放到工程目录下

3. 将其他 Demo 文件夹从工程中屏蔽（Exclude from Build）

   ```
   1. 在工程数中选中要屏蔽的 Demo 文件夹
   2. 右键菜单选中 Resource Configurations -> Exclude from Build...
   3. 弹出对话框点击 Select All -> OK
   ```

4. 编译工程

5. 下载到板子调试运行