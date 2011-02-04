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
    opt.tool_options('misc')
    opt.add_option( '--libzip'
                  , action='store'
                  , default=None
                  , help='Directory prefix containing libzip "lib" and "include" files'
                  , dest='shared_libzip_dir'
                  )

    opt.add_option( '--shared-libzip'
                  , action='store_true'
                  , default=False
                  , help='Build dynamically against external install of libzip (default False - build uses internal copy)'
                  , dest='shared_libzip'
                  )

def configure(conf):
    conf.check_tool("compiler_cxx")
    conf.check_tool("node_addon")

    libzip_includes = []
    libzip_libpath  = []
    auto_configured = False
    o = Options.options
    libzip_dir = o.shared_libzip_dir
    shared_libzip = o.shared_libzip
    
    if libzip_dir:
        norm_path = os.path.realpath(libzip_dir)
        if norm_path.endswith('lib') or norm_path.endswith('include'):
            norm_path = os.path.dirname(norm_path)
        
        libzip_includes = [os.path.join('%s' % norm_path,'include'),
                           os.path.join('%s' % norm_path,'lib/libzip/include')
                          ]
        libzip_libpath  = [os.path.join('%s' % norm_path,'lib')]
    
    elif shared_libzip:
        pkg_config = conf.find_program('pkg-config', var='PKG_CONFIG', mandatory=False)
        if pkg_config:
            cmd = '%s libzip' %  pkg_config
            if int(call(cmd.split(' '))) == 0:
                Utils.pprint('GREEN','Sweet, found libzip via pkg-config')
                libzip_includes.extend(popen("pkg-config --cflags libzip").readline().strip().split(' '))
                libzip_libpath.extend(popen("pkg-config --libs libzip").readline().strip().split(' '))
                auto_configured = True

        if not auto_configured:
            # reasonable defaults for searching
            libzip_includes = ['/usr/local/include',
                               '/usr/local/lib/libzip/include',
                               '/usr/include',
                               '/usr/lib/libzip/include',
                               ]
            libzip_libpath  = ['/usr/local/lib','/usr/lib']
    
    elif not shared_libzip:
        auto_configured = True
        libzip_includes = ['-I../deps/libzip-0.9.3/lib']
        libzip_libpath  = ['-L../deps/libzip-0.9.3/lib/.libs']
        
    if not auto_configured:
        if not conf.check_cxx(lib='zip', header_name='zip.h',
                              uselib_store='ZIP',
                              includes=libzip_includes,
                              libpath=libzip_libpath):
            conf.fatal("\n\n  Cannot find libzip, required for node-zipfile,\n  please install from:\n  'http://nih.at/libzip/libzip-0.9.3.tar.bz2'\n  (see README.md for more info)\n")
        else:
            Utils.pprint('GREEN', 'Sweet, found viable libzip depedency')
    
        # strip paths that don't exist, turn into proper flags
        new_inc = []
        for i in libzip_includes:
           if os.path.exists(i):
               new_inc.append('-I%s' % i)
        libzip_includes = new_inc
    
        new_inc = []
        for i in libzip_libpath:
           if os.path.exists(i):
               new_inc.append('-L%s' % i)
        libzip_libpath = new_inc

    linkflags = libzip_libpath
    #linkflags.append('-L/usr/local/lib')
    if '-lzip' not in linkflags:
        linkflags.append('-lzip')
    if '-lz' not in linkflags:
        linkflags.append('-lz')

    conf.env.append_value("LINKFLAGS", linkflags)
    
    cxxflags = libzip_includes
    #cxxflags.append('-I/usr/local/include')
    #cxxflags.append('-I/usr/local/lib/libzip/include/')

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
