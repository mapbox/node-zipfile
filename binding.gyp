{
  'includes': [ 'deps/common-libzip.gypi' ],
  'variables': {
      'shared_libzip%':'false',
      'shared_libzip_includes%':'/usr/lib',
      'shared_libzip_libpath%':'/usr/include'
  },
  'targets': [
    {
      'target_name': '<(module_name)',
      "include_dirs" : [
          "<!(node -e \"require('nan')\")"
      ],
      'defines': [
        'MODULE_NAME=<(module_name)'
      ],
      'sources': [
        'src/node_zipfile.cpp'
      ],
      "win_delay_load_hook": "false",
      'ldflags': [
        '-Wl,-z,now',
      ],
      'xcode_settings': {
        'OTHER_LDFLAGS':[
          '-Wl,-bind_at_load'
        ],
        'GCC_ENABLE_CPP_RTTI': 'YES',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'MACOSX_DEPLOYMENT_TARGET':'10.8',
        'CLANG_CXX_LIBRARY': 'libc++',
        'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
        'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0'
      },
      'cflags_cc!': ['-fno-exceptions'],
      'conditions': [
        ['shared_libzip == "false"', {
            'dependencies': [
              'deps/libzip.gyp:libzip'
            ]
        },
        {
            'libraries': [
               '-L<@(shared_libzip_libpath)',
               '-lzip'
            ],
            'include_dirs': [
               '<@(shared_libzip_includes)',
               '<@(shared_libzip_libpath)/libzip/include',
            ]
        }
        ]
      ]
    },
    {
      'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ '<(module_name)' ],
      'copies': [
          {
            'files': [ '<(PRODUCT_DIR)/<(module_name).node' ],
            'destination': '<(module_path)'
          }
      ],
      'conditions': [
          ['OS=="win"', {
              'copies': [
                {
                  'files': [ '<(PRODUCT_DIR)/libzip.dll' ],
                  'destination': '<(module_path)'
                }
              ]
          }]
       ]
    }
  ]
}
