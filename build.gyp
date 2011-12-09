
{
  'variables': {
      'node_module_sources': [
          "src/_zipfile.cc",
          "src/node_zipfile.cpp",
      ],
      'node_root': '/opt/node-v6.1',
      'node_root_win': 'c:\\node',
      'deps_root_win': 'c:\\dev2'
  },
  'targets': [
    {
      'target_name': '_zipfile',
      'product_name': '_zipfile',
      'type': 'loadable_module',
      'product_prefix': '',
      'product_extension':'node',
      'sources': [
        '<@(node_module_sources)',
      ],
      'defines': [
        'PLATFORM="<(OS)"',
        '_LARGEFILE_SOURCE',
        '_FILE_OFFSET_BITS=64',
      ],
      'conditions': [
        [ 'OS=="mac"', {
          'libraries': [
            '-lz',
            '-L../../deps/libzip-0.10/lib/.libs',
            '-lzip',
            '-undefined dynamic_lookup'
          ],
          'include_dirs': [
             'include/',
             'deps/libzip-0.10/',
             'deps/libzip-0.10/lib/',
             '<@(node_root)/include/node',
             '<@(node_root)/include',
          ],
        }],
        [ 'OS=="win"', {
          'defines': [
            'PLATFORM="win32"',
            '_WINDOWS',
            '__WINDOWS__', # ltdl
            'BUILDING_NODE_EXTENSION'
          ],
          'libraries': [ 
              'node.lib',
              'zip.lib',
			  'zlib.lib'
          ],
          'include_dirs': [
             'include',
             '<@(deps_root_win)\\node-zipfile\\deps\\libzip-0.10\\lib',
             '<@(deps_root_win)\\zlib',
             '<@(deps_root_win)\\node-v0.6.2\\deps\\v8\\include',
             '<@(deps_root_win)\\node-v0.6.2\\src',
             '<@(deps_root_win)\\node-v0.6.2\\deps\\uv\\include',
          ],
          'msvs_settings': {
            'VCLinkerTool': {
              'AdditionalOptions': [
                # https://github.com/mapnik/node-mapnik/issues/74
                '/FORCE:MULTIPLE'
              ],
              'AdditionalLibraryDirectories': [
                '<@(deps_root_win)\\zlib',
                '<@(deps_root_win)\\node-zipfile\\deps\\libzip-0.10\\build_vc100\\lib',
                '<@(node_root_win)\\Release\\lib',
                '<@(node_root_win)\\Release',
              ],
            },
          },
        },
      ], # windows
      ] # condition
    }, # targets
  ],
}