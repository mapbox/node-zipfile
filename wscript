import os
from glob import glob
from os import unlink, symlink, popen, uname, environ
from os.path import exists
from shutil import copy2 as copy
from subprocess import call

# http://www.freehackers.org/~tnagy/wafbook/index.html

# node-wafadmin
import Options
import Utils

TARGET = '_zipfile'
TARGET_FILE = '%s.node' % TARGET
built = 'build/default/%s' % TARGET_FILE
dest = 'lib/%s' % TARGET_FILE
settings = 'lib/settings.js'

def set_options(opt):
    opt.tool_options("compiler_cxx")
    
def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.check_tool("node_addon")

    linkflags = []
    linkflags.append('-L/usr/local/lib')
    linkflags.append('-lzip')

    conf.env.append_value("LINKFLAGS", linkflags)
    
    cxxflags = []
    cxxflags.append('-I/usr/local/include')
    cxxflags.append('-I/usr/local/lib/libzip/include/')

    conf.env.append_value("CXXFLAGS", cxxflags)
    
    #ldflags = []
    #conf.env.append_value("LDFLAGS", ldflags)

def build(bld):
    obj = bld.new_task_gen("cxx", "shlib", "node_addon", install_path=None)
    obj.cxxflags = ["-DNDEBUG", "-O3", "-g", "-Wall", "-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]
    obj.target = TARGET
    obj.source = "src/_zipfile.cc"
    obj.source += " src/node_zipfile.cpp"
    obj.uselib = "ZIPFILE"
    start_dir = bld.path.find_dir('lib')
    # http://www.freehackers.org/~tnagy/wafbook/index.html#_installing_files
    bld.install_files('${PREFIX}/lib/node/zipfile', start_dir.ant_glob('*'), cwd=start_dir, relative_trick=True)
    # install command line programs
    bin_dir = bld.path.find_dir('./bin')
    bld.install_files('${PREFIX}/bin', bin_dir.ant_glob('*'), cwd=bin_dir, relative_trick=True, chmod=0755)

def shutdown():
    if Options.commands['clean']:
        if exists(TARGET): unlink(TARGET)
    if Options.commands['clean']:
        if exists(dest):
            unlink(dest)
    else:
        if exists(built):
            copy(built,dest)
