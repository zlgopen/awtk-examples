@rem ---------------------------------------------------------------------------------------------------
@rem 一、双击生成主题、字符串、字体、图片、UI等二进制文件(其中调用assets_gen.py)                                   
@rem 二、修改 AWTK_BIN_DIR=E:/zlgopen/awtk/bin 为本地awtk对应的bin目录
@rem 三、assets_gen.bat说明
@rem	  1 为空(使用800*480屏幕)：
@rem		即assets_gen.bat
@rem	  2 -480_272(使用480*272屏幕):
@rem		即assets_gen.bat -480_272	
@rem 四、%PY_VERSION% assets_gen.py %AWTK_BIN_DIR%  ./src/assets/raw ./src/assets/inc说明：
@rem    1 为空：
@rem      即assets_gen.py AWTK_BIN_DIR ./src/assets/raw ./src/assets/inc，生成主题、字符串、字体、图片、UI等二进制文件
@rem    2 -style 
@rem      即assets_gen.py AWTK_BIN_DIR ./src/assets/raw ./src/assets/inc -style，  生成主题
@rem    3 -string
@rem      即assets_gen.py AWTK_BIN_DIR ./src/assets/raw ./src/assets/inc -string， 生成字符串
@rem    4 -font
@rem      即assets_gen.py AWTK_BIN_DIR ./src/assets/raw ./src/assets/inc -font，   生成字体
@rem    5 -image
@rem      即assets_gen.py AWTK_BIN_DIR ./src/assets/raw ./src/assets/inc -image，  生成图片
@rem    6 -ui
@rem      即assets_gen.py AWTK_BIN_DIR ./src/assets/raw ./src/assets/inc -ui，     生成ui文件
	
@rem ----------------------------------------------------------------------------------------------------

@set AWTK_BIN_DIR=%1%
@set PY_VERSION=python
@if exist %SystemRoot%\py.exe (
	@set PY_VERSION=py -2
)

%PY_VERSION% ../assets_gen.py %AWTK_BIN_DIR% ./assets/raw ./assets/inc
%PY_VERSION% ../assets_c_gen.py ./assets