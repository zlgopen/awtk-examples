@rem 这里的AWTK_DIR的值在SConstruct脚本的getAwtkRoot()函数会用到，修改时请注意

@set AWTK_DIR=../../awtk

cd res
call assets_gen.bat ../%AWTK_DIR%/bin
cd ..
