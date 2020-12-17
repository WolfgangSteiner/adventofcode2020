import os
import ycm_core

flags = [
  '-Wall',
  '-Wextra',
  '-Werror',
  '-Wno-long-long',
  '-Wno-variadic-macros',
  '-DNDEBUG',
  '-std=c99',
  '-IUnity/src',
  ]

SOURCE_EXTENSIONS = [ '.c', ]

def FlagsForFile( filename, **kwargs ):
  return {
  'flags': flags,
  'do_cache': True
}
