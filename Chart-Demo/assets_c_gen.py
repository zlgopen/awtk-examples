#!/usr/bin/python

import os
import sys
import glob
import shutil
import copy

def joinPath(root, subdir):
  return os.path.normpath(os.path.join(root, subdir))

def writeToFile(file_name, str):
  fd = os.open(file_name, os.O_RDWR|os.O_CREAT|os.O_TRUNC)
  if check_python_version() :
    os.write(fd, bytes(str, 'utf-8'))
  else:
    os.write(fd, str)
  os.close(fd)

def genIncludes(files, dir_name):
  str1 = ""
  for f in files:
    incf = copy.copy(f);
    incf=incf.replace(dir_name, "assets/default/inc");
    incf=incf.replace('\\', '/');
    str1 += '#include "'+incf+'"\n'
  return str1

def GetFileBaseName(file_name, root_dir_name, subdir_name, suffix):
  name = file_name.replace(root_dir_name, '');
  name = name.replace('\\', '/');
  name = name.replace('/' + subdir_name + '/', '');
  name = name.replace(suffix, '');
  return name;

def genAssetsManagerAdd(assets_inc_dir, filter, dir_name, name, suffix):
  files=glob.glob(joinPath(assets_inc_dir, filter))
  result = ''
  for f in files:
    basename = GetFileBaseName(copy.copy(f), assets_inc_dir, dir_name,  suffix)
    if dir_name == "data":
      result += '  assets_manager_add(rm, ' + name + basename + '_bin' +');\n'
    else:
      result += '  assets_manager_add(rm, ' + name + basename + ');\n'
  return result

def gen_assets_c(assets_dir, assets_c_path):
  assets_inc_dir = joinPath(assets_dir, 'default/inc')
  if not os.path.exists(assets_inc_dir) :
    print('assets inc dir not exist')
    exit()

  result = '#include "awtk.h"\n'
  result += '#include "base/assets_manager.h"\n'

  result += '#ifndef WITH_FS_RES\n'
  
  files=glob.glob(joinPath(assets_inc_dir, 'strings/*.data')) \
    + glob.glob(joinPath(assets_inc_dir, 'styles/*.data')) \
    + glob.glob(joinPath(assets_inc_dir, 'ui/*.data')) \
    + glob.glob(joinPath(assets_inc_dir, 'data/*.res')) 
  result += genIncludes(files, assets_inc_dir);

  result += "#ifdef WITH_STB_IMAGE\n"
  files=glob.glob(joinPath(assets_inc_dir, 'images/*.res')) 
  result += genIncludes(files, assets_inc_dir)
  result += "#else\n"
  files=glob.glob(joinPath(assets_inc_dir, 'images/*.data')) 
  result += genIncludes(files, assets_inc_dir)
  result += '#endif/*WITH_STB_IMAGE*/\n'
  
  result += "#if defined(WITH_STB_FONT) || defined(WITH_FT_FONT)\n"
  files=glob.glob(joinPath(assets_inc_dir, 'fonts/*.res'))
  result += genIncludes(files, assets_inc_dir)
  result += "#else/*WITH_STB_FONT or WITH_FT_FONT*/\n"
  files=glob.glob(joinPath(assets_inc_dir, 'fonts/*.data'))
  result += genIncludes(files, assets_inc_dir)
  result += '#endif/*WITH_STB_FONT or WITH_FT_FONT*/\n'

  result += '#endif/*WITH_FS_RES*/\n'

  result += '\n';
  result += 'ret_t assets_init(void) {\n'
  result += '  assets_manager_t* rm = assets_manager();\n\n'
  result += ''

  result += '#ifdef WITH_FS_RES\n'
  result += '  assets_manager_preload(rm, ASSET_TYPE_FONT, "default");\n'
  result += '  assets_manager_preload(rm, ASSET_TYPE_STYLE, "default");\n'
  result += '#else\n'
  
  result += genAssetsManagerAdd(assets_inc_dir, 'fonts/*.res', 'fonts', 'font_', '.res')
  result += genAssetsManagerAdd(assets_inc_dir, 'images/*.res', 'images', 'image_', '.res')
  result += genAssetsManagerAdd(assets_inc_dir, 'styles/*.data', 'styles', 'style_', '.data')
  result += genAssetsManagerAdd(assets_inc_dir, 'ui/*.data', 'ui', 'ui_', '.data')
  result += genAssetsManagerAdd(assets_inc_dir, 'strings/*.data', 'strings', 'strings_', '.data')
  result += genAssetsManagerAdd(assets_inc_dir, 'data/*.res', 'data', 'data_', '.res')
  result += '#endif\n'

  result += '\n'
  result += '  tk_init_assets();\n'
  result += '  return RET_OK;\n'
  result += '}\n'
  writeToFile(assets_c_path, result);

def check_python_version():
	major_version = sys.version_info[0]
	if major_version > 2:
		# print("The python version is %d.%d. But python2.x is required.(Version 2.7 is well tested!)" %(major_version, sys.version_info[1]))
		return True
	return False

def run():
  if len(sys.argv) <= 1 :
    print('Usage: assets_c_gen.py assets_dir')
    exit()
  assets_dir = os.path.abspath(sys.argv[1])
  assets_c_path = joinPath(assets_dir, '../assets.inc')
  gen_assets_c(assets_dir, assets_c_path)

run()

