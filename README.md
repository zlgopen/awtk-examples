# AWTK 示例 v2 使用说明

#### 项目介绍：

- HelloDesigner-Demo
- HelloWorld.Xml.d-Demo
- HelloWorld.Max.d-Demo
- Chart.d-Demo
- CleanAir.d-Demo
- MusicPlayer.d-Demo
- SmartWatch.d-Demo



#### Windows 使用方法：

1. 确保已经安装好 VS2015（Visual C++）、Python2.7（x64）、SCons3.0.1 等工具

2. 将 awtk 和 awtk-examples.v2 下载到同一个文件夹下

3. 修改 awtk 文件夹的 awtk_config.py 脚本使用 AGGE 渲染方式

   > AGGE 渲染方式兼容性最好，可以兼容 Windows、Linux、macOS 以及各种嵌入式平台

   ```
   NANOVG_BACKEND='AGGE'
   ```

4. 进入 awtk 文件夹，在命令行中输入 scons 编译 awtk

   ```
   C:\zlgopen\awtk> scons
   ```

5. 进入 awtk-examples.v2 中某个项目文件夹，在命令行中输入 scons 编译项目

   ```
   C:\zlgopen\awtk-examples.v2\HelloDesigner-Demo> scons
   ```

6. 进入项目的 bin 文件夹启动 demo

   ```
   C:\zlgopen\awtk-examples.v2\HelloDesigner-Demo\bin> demo.exe
   ```



#### 运行效果截图：

![Chart-Demo](docs/images/chart_main.png)

![Meter-Demo](docs/images/meter_main.png)

![CleanAir-Demo](docs/images/cleanair_main.png)

![MusicPlayer-Demo](docs/images/musicplayer_main.png)

![SmartWatch-Demo](docs/images/smartwatch_main.png)