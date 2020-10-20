import os
import sys
import platform

def getAwtkRoot():
  if platform.system() == "Windows":
    fo = open('assets_gen.bat', 'r')
  else:
    fo = open('assets_gen.sh', 'r')
  for line in fo.readlines():
    line = line.replace('\r', '')
    line = line.replace('\n', '')
    if line.find('AWTK_DIR=') != -1:
      awtk_root = line.split('=')[1]
      return os.path.abspath(awtk_root)

sys.path.insert(0, getAwtkRoot())
import awtk_config as awtk

APP_CPPPATH = []
APP_CCFLAGS = ' '

LINUX_FB = ARGUMENTS.get('LINUX_FB', '') != ''
if LINUX_FB:
  print('!!! ERROR: This APP not support LINUX_FB option, scons script will stop!')
  sys.exit(1)

LCD_FLAG = ARGUMENTS.get('LCD', 0)
if LCD_FLAG == '480_272':
  APP_CCFLAGS = APP_CCFLAGS + ' -DLCD_W=480 -DLCD_H=272 '

APP_ROOT    = os.path.normpath(os.getcwd())
APP_BIN_DIR = os.path.join(APP_ROOT, 'bin')
APP_LIB_DIR = os.path.join(APP_ROOT, 'lib')

os.environ['APP_ROOT'] = APP_ROOT
os.environ['BIN_DIR'] = APP_BIN_DIR
os.environ['LIB_DIR'] = APP_LIB_DIR

APP_LIBS = []
APP_LIBPATH = [APP_LIB_DIR]

APP_TOOLS = None
if hasattr(awtk, 'TOOLS_NAME') and awtk.TOOLS_NAME != '' :
  APP_TOOLS = [awtk.TOOLS_NAME]

if hasattr(awtk, 'AWTK_CCFLAGS'):
  AWTK_CCFLAGS = awtk.AWTK_CCFLAGS
else:
  AWTK_CCFLAGS = awtk.CCFLAGS
  
DefaultEnvironment(TOOLS     = APP_TOOLS,
  CPPPATH   = APP_CPPPATH + awtk.CPPPATH,
  LINKFLAGS = awtk.LINKFLAGS,
  LIBS      = APP_LIBS + awtk.LIBS,
  LIBPATH   = APP_LIBPATH + awtk.LIBPATH,
  CCFLAGS   = APP_CCFLAGS + AWTK_CCFLAGS, 
  OS_SUBSYSTEM_CONSOLE=awtk.OS_SUBSYSTEM_CONSOLE,
  OS_SUBSYSTEM_WINDOWS=awtk.OS_SUBSYSTEM_WINDOWS)

SConscriptFiles=[
  'src/SConscript'
  ]
  
SConscript(SConscriptFiles)
