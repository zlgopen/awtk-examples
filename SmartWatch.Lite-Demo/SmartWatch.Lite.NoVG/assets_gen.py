#!/usr/bin/python

import os
import sys
import glob
import shutil
import platform
import copy
import time

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
  
def genData(tools_dir, src_dir, dst_dir): 
  if os.path.exists(joinPath(src_dir, 'data')):
    forceMakeDirs(joinPath(dst_dir, 'data'))
    time.sleep(0.1)
    tool_path = toExePath(tools_dir, 'resgen')
    src_dir = joinPath(src_dir, "data")
    dst_dir = joinPath(dst_dir, "data")
    if not os.path.exists(tool_path) :
      print(tool_path + ' not exist')
      return
    if not os.path.exists(src_dir) :
      print(src_dir + ' not exist')
      return
    for f in os.listdir(src_dir):
      inc = f + '.res'
      # print(tool_path + ' ' + joinPath(src_dir, f) + ' ' + joinPath(dst_dir, inc))
      os.system(tool_path + ' ' + joinPath(src_dir, f) + ' ' + joinPath(dst_dir, inc))

def genClockPinterGen(src_dir):
  data_dir = joinPath(src_dir, 'data')
  if os.path.exists(data_dir):
    image_dir = joinPath(src_dir, 'images/clock_pointer')
    if os.path.exists(image_dir):
      tool_path = os.path.abspath('bin/clock_pinter_gen.exe')
      for clock_pointer_dir in os.listdir(image_dir) :
        # print(tool_path + ' ' +  os.path.abspath(joinPath(image_dir, clock_pointer_dir)) + ' ' + joinPath(data_dir, clock_pointer_dir) + ' ' + clock_pointer_dir + '_%4d ');
        os.system(tool_path + ' ' +  os.path.abspath(joinPath(image_dir, clock_pointer_dir)) + ' ' + joinPath(data_dir, clock_pointer_dir) + ' ' + clock_pointer_dir + '_%4d ')

def run():
  
  tools_dir = "../../../awtk/bin"
  src_dir = "design/default"
  dst_dir = "res/assets/default/inc"

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

  genClockPinterGen(src_dir)
  genData(tools_dir, src_dir, dst_dir)

run()