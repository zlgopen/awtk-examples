@rem ---------------------------------------------------------------------------------------------------
@rem 一、这里的AWTK_DIR的值在SConstruct脚本的getAwtkRoot()函数会用到，修改时请注意
@rem 二、双击生成主题、字符串、字体、图片、UI等二进制文件(其中调用assets_gen.py)                                   
@rem 三、修改 AWTK_DIR =../../awtk 为本地awtk对应的目录
@rem 四、修改该脚本中的for循环为应用中对应的资源目录，例如有多个资源目录（res_800_480和res_480_272）则修改为：for %%i in (res_800_480, res_480_272) do (
@rem 五、python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc说明：
@rem    1 为空：
@rem      即python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc，生成主题、字符串、字体、图片、UI等二进制文件
@rem    2 -style 
@rem      即python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc -style，  生成主题
@rem    3 -string
@rem      即python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc -string， 生成字符串
@rem    4 -font
@rem      即python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc -font，   生成字体
@rem    5 -image
@rem      即python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc -image，  生成图片
@rem    6 -ui
@rem      即python assets_gen.py %AWTK_DIR%/bin %%i/assets/raw %%i/assets/inc -ui，     生成ui文件	
@rem ---------------------------------------------------------------------------------------------------

@set AWTK_DIR=../../awtk
for %%i in (res_480_272, res_800_480) do (
  python assets_gen.py %AWTK_DIR%/bin %%i/assets/default/raw %%i/assets/default/inc 
  python assets_c_gen.py %%i/assets
)

