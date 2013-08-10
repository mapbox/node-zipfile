{
  'includes': [ 'deps/common-libzip.gypi' ],
  'variables': {
      'libzip%':'internal',
  },
  'conditions': [
      ['OS=="win"', {
        'variables': {
          'copy_command%': 'copy',
        },
      },{
        'variables': {
          'copy_command%': 'cp',
        },
      }]
  ],
  'targets': [
    {
      'target_name': 'node_zipfile',
      'conditions': [
        ['libzip != "internal"', {
            'libraries': [
               '-L<@(libzip)/lib',
               '-lzip'
            ],
            'include_dirs': [ '<@(libzip)/include' ]
        },
        {
            'dependencies': [
              'deps/libzip.gyp:libzip'
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
      'actions': [
        {
          'action_name': 'move_node_module',
          'inputs': [
            '<@(PRODUCT_DIR)/node_zipfile.node'
          ],
          'outputs': [
            'lib/node_zipfile.node'
          ],
          'action': ['<(copy_command)', '<@(PRODUCT_DIR)/node_zipfile.node', 'lib/node_zipfile.node']
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
