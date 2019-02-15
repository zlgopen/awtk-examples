@rem 这里的AWTK_DIR的值在SConstruct脚本的getAwtkRoot()函数会用到，修改时请注意

@set AWTK_DIR=../../awtk

cd res_480_272
call assets_gen.bat ../%AWTK_DIR%/bin
cd ..

cd res_800_480
call assets_gen.bat ../%AWTK_DIR%/bin
cd ..