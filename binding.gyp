{
  'includes': [ 'deps/common-libzip.gypi' ],
  'variables': {
      'shared_libzip%':'false',
      'shared_libzip_includes%':'/usr/lib',
      'shared_libzip_libpath%':'/usr/include'
  },
  'targets': [
    {
      'target_name': 'node_zipfile',
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
      ],
      'sources': [
        'src/node_zipfile.cpp'
      ],
    },
    {
      'target_name': 'action_after_build',
      'type': 'none',
      'dependencies': [ 'node_zipfile' ],
      'copies': [
          {
            'files': [ '<(PRODUCT_DIR)/node_zipfile.node' ],
            'destination': './lib/binding/'
          }
      ],
      'conditions': [
          ['OS=="win"', {
              'copies': [
                {
                  'files': [ '<(PRODUCT_DIR)/libzip.dll' ],
                  'destination': 'lib/'
                }
              ]
          }]
       ]
    }
  ]
}
