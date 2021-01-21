#!/usr/bin/python

import os

def joinPath(root, subdir):
  return os.path.normpath(os.path.join(root, subdir))

def genClockPinterGen(src_dir):
  data_dir = joinPath(src_dir, 'data')
  if os.path.exists(data_dir):
    image_dir = joinPath(src_dir, 'images/clock_pointer')
    if os.path.exists(image_dir):
      tool_path = os.path.abspath('bin/clock_pinter_gen.exe')
      for clock_pointer_dir in os.listdir(image_dir) :
        # print(tool_path + ' ' +  os.path.abspath(joinPath(image_dir, clock_pointer_dir)) + ' ' + joinPath(data_dir, clock_pointer_dir) + ' ' + clock_pointer_dir + '_%4d ');
        os.system(tool_path + ' ' +  os.path.abspath(joinPath(image_dir, clock_pointer_dir)) + ' ' + joinPath(data_dir, clock_pointer_dir) + ' ' + clock_pointer_dir + '_%4d ')


def on_generate_res_before(ctx):
    print('======================= '+ ctx['type'] + ' =======================')
    # ctx = {'type': 'ui', 'theme': 'default', 'imagegen_options': 'mono', 'input': './', 'output': './'}
    if ctx['type'] == "data" :
        genClockPinterGen("design/default")


def on_generate_res_after(ctx):
    print('======================= '+ ctx['type'] + ' =======================')
