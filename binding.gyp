{
  'includes': [ 'deps/common-libzip.gypi' ],
  'variables': {
      'libzip%':'false',
  },
  'targets': [
    {
      'target_name': 'node_zipfile',
      'conditions': [
        ['libzip == "false"', {
            'dependencies': [
              'deps/libzip.gyp:libzip'
            ]
        },
        {
            'libraries': [
               '-L<@(libzip)/lib',
               '-lzip'
            ],
            'include_dirs': [
               '<@(libzip)/include',
               '<@(libzip)/lib/libzip/include',
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
            'destination': './lib/'
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
