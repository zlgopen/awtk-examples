# 工具脚本

## 一、update\_res.py 资源生成工具

```
Usage: ./scripts/update_res.py action[clean|all|font|image|ui|style|string]
Example:
./scripts/update_res.py all
./scripts/update_res.py clean
./scripts/update_res.py style
```

参数action是必须的，其取值如下：

* clean 清除之前生成的资源文件。
* all 重新生成全部资源。
* font 仅生成有变化的字体资源。
* image 仅生成有变化的图像资源。
* ui 仅生成有变化的UI资源。
* style 仅生成有变化的主题资源。
* string 仅生成有变化的字符串资源。

### 1.1 设置

如果需要修改资源生成工具的设置，修改 project.json 即可，设置参数的详细信息如下：

```json
{  
  ......
  "assets": {
    "activedTheme": "default",    // APP运行时的默认主题
    "outputDir": "res",           // 资源生成的输出目录
    "loadFrom": "any",
    "const": "resource_data",     // 资源的常量格式，有2个选项：
                                  //   “resource_data”表示原始数据，“bitmap_data”表示位图数据
    "screenDPR": "x1",            // LCD的设备像素比（DPR），有3个选项：x1，x2，x3
    "defaultLanguage": "zh",
    "defaultCountry": "CN",
    "themes": {                   // 项目的所有主题
      "default": {                // default主题
        "activedSystemBar": "system_bar",
        "activedBottomSystemBar": "system_bar_b",
        "packaged": true,         // 是否生成资源
        "fonts": {                // 主题包含的字体
          "default": {            // default字体
            "text": " !\"#$%&'()*+,-./0123456789:",     // “text”为TTF字体的保留字符，
            "18": " !\"#$%&'()*+,-./0123456789确定取消"  // “18”为Bitmap字体18字号的保留字符
          }
        },
        "lcd": {
          "width": "800",
          "height": "480",
          "colorDepth": "16bit",  // LCD的颜色深度
          "colorFormat": "BGR(A)" // LCD的颜色尺寸
        }
      }
    }
  },
  ......
}
```

> 关于 project.json 的更多说明，请参考 [AWTK项目描述文件](https://github.com/zlgopen/awtk/blob/master/docs/awtk_project_description_file.md)
> 也可以通过打开 Designer，进入 “项目设置” 页面进行设置。

### 1.2 添加资源生成事件回调，实现额外处理逻辑

update_res.py 在生成资源的前后会分别触发一个before事件和一个after事件。如果需要在资源生成前后做一些额外的处理，则在相应的事件回调中添加代码即可。

具体步骤如下：

- 在 update_res.py 所在目录新建一个Python文件，并命名为 update_res_generate_res_handler.py。默认文件内容如下：

```python
#!/usr/bin/python

# ctx参数是一个dict类型，有如下属性：
# ctx['type']: 当前正在生成的资源类型，有all、string、font、script、image、ui、style、data、xml
# ctx['theme']: 当前正在生成的主题
# ctx['imagegen_options']: 当前的图片生成选项，有mono、bgr565、bgra、rgb565、rgba
# ctx['input']: 当前主题原始资源的存放路径，比如'e:/AWTK Application/design/default'
# ctx['output']: 当前主题生成资源的存放路径，比如'e:/AWTK Application/res/assets/default'
#
# 资源生成事件的流程如下：
# |----->-----  开始生成某个主题的资源
# |                       |
# |         on_generate_res_before('all')
# |                       |
# |         on_generate_res_before('string')
# |                       |
# |         on_generate_res_after('string')
# |                       |
# ^         on_generate_res_before('font')
# |                       |
# |         on_generate_res_after('font')
# |                     ......
# |         on_generate_res_before('xml')
# |                       |
# |         on_generate_res_after('xml')
# |                       |
# |         on_generate_res_after('all')
# |                       |
# |-----<-----    继续生成下一个主题
#

def on_generate_res_before(ctx):
    print('======================= '+ ctx['type'] + ' =======================')


def on_generate_res_after(ctx):
    print('======================= '+ ctx['type'] + ' =======================')

```

- 在 on_generate_res_before 或 on_generate_res_after 添加处理逻辑。

### 1.3 注意事项

##### 1.3.1 awtk 的路径

update_res.py 须依赖 awtk 提供的公共模块，默认使用 AWTK Designer 安装目录中的 awtk，如果找不到则会尝试在当前工作目录的上3级目录中查找。

如果查找不到 awtk，则会出现如下错误：

```cmd
ImportError: No module named update_res_app
```

此时只需修改 update_res.py 中的 AWTK_ROOT 为正确的 awtk 路径即可。

##### 1.3.2 用于单色屏的位图字体生成工具 fontgen_ft

默认情况下，awtk 编译后生成的 fontgen 工具只能生成用于非单色屏的位图字体。因此，如果需要生成用于单色屏的位图字体，重新生成一个专门的 fontgen。

步骤如下：

- 拷贝awtk 目录，并重命名为 awtk-mono。
- 修改 awtk-mono/awtk_config.py 文件，将：

```python
......
#LCD='SDL_FB_MONO'
......
```

​       改为

```python
......
LCD='SDL_FB_MONO'
......
```

- 在 awtk-mono 目录下，执行 scons 命令，重新编译 awtk。
- 将 awtk-mono/bin目录下fontgen工具重命名为 fontgen_ft，并拷贝到 awtk/bin目录下。