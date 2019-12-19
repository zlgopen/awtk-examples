#!/usr/bin/python

import os
import sys
import glob
import shutil
import platform
import copy

def joinPath(root, subdir):
  return os.path.normpath(os.path.join(root, subdir))

def removeDir(path):
  if os.path.isdir(path):
   print('rmdir:' + path);
   shutil.rmtree(path)

def toExePath(root, name):
  if platform.system() == 'Windows':
    return joinPath(root, name + '.exe')
  else:
    return joinPath(root, name)

def forceMakeDirs(dst_dir):
  removeDir(dst_dir)
  os.makedirs(dst_dir)

# tool.exe src_file dst_file option
def generate(tools_dir, tool_name, src_dir, src_sub_dir, src_suffix, dst_dir, dst_sub_dir, dst_suffix, option, is_override):
  tool_path = toExePath(tools_dir, tool_name)
  src_dir = joinPath(src_dir, src_sub_dir)
  dst_dir = joinPath(dst_dir, dst_sub_dir)

  if not os.path.exists(tool_path) :
    print(tool_path + ' not exist')
    return
  if not os.path.exists(src_dir) :
    print(src_dir + ' not exist')
    return
  # Generate
  if isinstance(src_suffix, list) :
    for f in glob.glob(joinPath(src_dir, '*.*')):
      raw=copy.copy(f);
      if dst_suffix == '':
        inc=''
      else :
        inc=copy.copy(f);
        inc=inc.replace(src_dir, dst_dir)
        for suffix in src_suffix : 
          inc=inc.replace(suffix, dst_suffix)
      if is_override:
        if os.path.exists(inc):
          os.remove(inc)
      print(tool_path + ' ' + joinPath(src_dir, raw) + ' ' + joinPath(dst_dir, inc) + ' ' + option)
      os.system(tool_path + ' ' + joinPath(src_dir, raw) + ' ' + joinPath(dst_dir, inc) + ' ' + option)
  else :
    for f in glob.glob(joinPath(src_dir, '*' + src_suffix)):
      raw=copy.copy(f);
      if dst_suffix == '':
        inc=''
      else :
        inc=copy.copy(f);
        inc=inc.replace(src_dir, dst_dir)
        inc=inc.replace(src_suffix, dst_suffix)
      if is_override:
        if os.path.exists(inc):
          os.remove(inc)
      print(tool_path + ' ' + joinPath(src_dir, raw) + ' ' + joinPath(dst_dir, inc) + ' ' + option)
      os.system(tool_path + ' ' + joinPath(src_dir, raw) + ' ' + joinPath(dst_dir, inc) + ' ' + option)

# tool.exe src_file text_file dst_file font_size
def genBmpFont(tools_dir, tool_name, src_dir, src_suffix, dst_dir, dst_suffix, sub_dir, font_size, is_remake_dir):
  tool_path = toExePath(tools_dir, tool_name)
  src_dir = joinPath(src_dir, sub_dir)
  dst_dir = joinPath(dst_dir, sub_dir)

  if not os.path.exists(tool_path) :
    print(tool_path + ' not exist')
    return
  if not os.path.exists(src_dir) :
    print(src_dir + ' not exist')
    return
  # Delete History
  if is_remake_dir :
    removeDir(dst_dir)
    os.makedirs(dst_dir);
  # Generate
  text_file = joinPath(src_dir, 'text.txt')
  for f in glob.glob(joinPath(src_dir, '*' + src_suffix)):
    raw=copy.copy(f);
    inc=copy.copy(f);
    inc=inc.replace(src_dir, dst_dir)
    if font_size == 18 :
      inc=inc.replace(src_suffix, dst_suffix)
    else :
      inc=inc.replace(src_suffix, '_' + str(font_size) + dst_suffix)
    if raw.find('.mini' + src_suffix) == -1 :
      print(tool_path + ' ' + joinPath(src_dir, raw) + ' ' + text_file + ' ' + joinPath(dst_dir, inc) + ' ' + str(font_size))
      os.system(tool_path + ' ' + joinPath(src_dir, raw) + ' ' + text_file + ' ' + joinPath(dst_dir, inc) + ' ' + str(font_size))

def genTheme(tools_dir, src_dir, dst_dir):
  forceMakeDirs(joinPath(dst_dir, 'styles'))
  generate(tools_dir, 'themegen', src_dir, 'styles', '.xml', dst_dir, 'styles', '.data', '', 0)
  generate(tools_dir, 'themegen', src_dir, 'styles', '.xml', src_dir, 'styles', '.bin', 'bin', 0)

def genString(tools_dir, src_dir, dst_dir):
  forceMakeDirs(joinPath(dst_dir, 'strings'))
  generate(tools_dir, 'strgen', src_dir, 'strings', '.xml', dst_dir, 'strings', '', '', 0)
  generate(tools_dir, 'strgen', src_dir, 'strings', '.xml', src_dir, 'strings', '', 'bin', 0)

def genFont(tools_dir, src_dir, dst_dir):
  forceMakeDirs(joinPath(dst_dir, 'fonts'))
  generate(tools_dir, 'resgen', src_dir, 'fonts', '.ttf', dst_dir, 'fonts', '.res', '', 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 10, 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 18, 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 28, 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 32, 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 48, 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 64, 0)
  # genBmpFont(tools_dir, 'fontgen', src_dir, '.ttf', dst_dir, '.data', 'fonts', 128, 0)

def genImage(tools_dir, src_dir, dst_dir, dpi): 
  IMAGEGEN_OPTIONS = '\"bgra|bgr565\"'
  suffix = ['.png', '.jpg', '.bmp']

  forceMakeDirs(joinPath(dst_dir, 'images'))
  generate(tools_dir, 'resgen', src_dir, 'images/xx', suffix, dst_dir, 'images', '.res', '', 0)
  generate(tools_dir, 'resgen', src_dir, 'images/' + dpi, suffix, dst_dir, 'images', '.res', '', 1)
  #generate(tools_dir, 'imagegen', src_dir, 'images/' + dpi, suffix, dst_dir, 'images', '.data', IMAGEGEN_OPTIONS, 0)
  
def genData(tools_dir, src_dir, dst_dir): 
  if os.path.exists(joinPath(src_dir, 'data')):
    forceMakeDirs(joinPath(dst_dir, 'data'))
    generate(tools_dir, 'resgen', src_dir, 'data', '.bin', dst_dir, 'data', '.res', '', 0)
  
def genUI(tools_dir, src_dir, dst_dir): 
  forceMakeDirs(joinPath(dst_dir, 'ui'))
  generate(tools_dir, 'xml_to_ui', src_dir, 'ui', '.xml', dst_dir, 'ui', '.data', '', 0)
  generate(tools_dir, 'xml_to_ui', src_dir, 'ui', '.xml', src_dir, 'ui', '.bin', 'bin', 0)

def genBSVG(tools_dir, src_dir, dst_dir):
  generate(tools_dir, 'bsvggen', src_dir, 'images/svg', '.svg', dst_dir, 'images', '.bsvg', '', 0)
  generate(tools_dir, 'bsvggen', src_dir, 'images/svg', '.svg', src_dir, 'images/svg', '.bsvg', 'bin', 0)
def check_python_version():
  major_version = sys.version_info[0]
  if major_version > 2:
    # print("The python version is %d.%d. But python2.x is required.(Version 2.7 is well tested!)" %(major_version, sys.version_info[1]))
    return True
  return False

def run():
  if len(sys.argv) <= 3 :
    print('Usage: assets_gen.py tools_dir src_dir dst_dir')
    exit()

  tools_dir = os.path.abspath(sys.argv[1])
  src_dir = os.path.abspath(sys.argv[2])
  dst_dir = os.path.abspath(sys.argv[3])

  if len(sys.argv) > 4 :
    opt = sys.argv[4]
  else:
    opt = ''
  if not os.path.exists(tools_dir) :
    print('tools dir not exist')
    exit()
  if not os.path.exists(src_dir) :
    print('src dir not exist')
    exit()

  # Start Generate
  if opt == '' or opt == '-style':
    genTheme(tools_dir, src_dir, dst_dir)
  if opt == '' or opt == '-string':
    genString(tools_dir, src_dir, dst_dir)
  if opt == '' or opt == '-font':
    genFont(tools_dir, src_dir, dst_dir)
  if opt == '' or opt == '-image':
    if (len(sys.argv) > 5):
      dpi = sys.argv[5]
    else:
      dpi = 'x1'
    genImage(tools_dir, src_dir, dst_dir, dpi)
  if opt == '' or opt == '-ui':
    genUI(tools_dir, src_dir, dst_dir)
  if opt == '' or opt == '-data':
    genData(tools_dir, src_dir, dst_dir)
  if opt == '' or opt == '-svg':
    genBSVG(tools_dir, src_dir, dst_dir)

run()