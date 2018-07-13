{
  'includes': [ 'common-libzip.gypi' ],
  'target_defaults': {
    'default_configuration': 'Release',
    'configurations': {
      'Debug': {
        'defines': [ 'DEBUG', '_DEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 1, # static debug
          },
          'VCLinkerTool': {
            'GenerateDebugInformation': 'true',
          },
        },
      },
      'Release': {
        'defines': [ 'NDEBUG' ],
        'msvs_settings': {
          'VCCLCompilerTool': {
            'RuntimeLibrary': 0, # static release
          },
        },
      }
    }
  },
  'conditions': [
      ['OS=="win"', {
        'variables': {
          # due to linking errors when built statically
          'library_type%': 'shared_library',
        },
      },{
        'variables': {
          'library_type%': 'static_library',
        },
      }]
  ],
  'targets': [
    {
      'target_name': 'action_before_build',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'unpack_libzip_dep',
          'inputs': [
            './libzip-<@(libzip_version)-mod2.tar.gz'
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_add.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/mkstemp.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_add_dir.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_add_entry.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_close.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_delete.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_dir_add.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_dirent.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_discard.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_entry.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_err_str.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_clear.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_get.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_get_sys_type.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_strerror.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_to_str.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_extra_field.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_extra_field_api.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fclose.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fdopen.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_add.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_error_clear.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_error_get.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_get_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_get_external_attributes.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_get_offset.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_rename.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_replace.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_set_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_set_external_attributes.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_strerror.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_filerange_crc.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen_encrypted.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen_index.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen_index_encrypted.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fread.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_archive_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_archive_flag.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_compression_implementation.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_encryption_implementation.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_file_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_name.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_num_entries.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_num_files.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_memdup.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_name_locate.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_new.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_open.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_rename.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_replace.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_archive_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_archive_flag.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_default_password.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_file_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_file_compression.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_name.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_buffer.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_close.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_crc.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_deflate.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_error.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_file.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_filep.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_free.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_function.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_layered.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_open.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_pkware.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_pop.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_read.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_stat.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_window.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_zip.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_zip_new.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_stat.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_stat_index.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_stat_init.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_strerror.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_string.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange_all.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange_archive.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange_data.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_utf-8.c'
          ],
          'action': ['python','./extract.py','./libzip-<@(libzip_version)-mod2.tar.gz','<(SHARED_INTERMEDIATE_DIR)']
        }
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '.',
          '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib',
          '<(node_root_dir)/deps/zlib'
        ],
        'conditions': [
          ['OS == "win"', {
            'defines': [
              'WIN32',
              '_WIN32',
              'WINDOWS',
              '_WINDOWS'
            ],
            'include_dirs': [
               './win-include'
            ]
          }],
          ['OS=="solaris"', {
            'include_dirs': [
               './solaris-include'
            ]
          } , {
            'include_dirs': [
               './unix-include'
            ]
            }
          ]
        ]
      }
    },
    {
      'target_name': 'libzip',
      'type': '<(library_type)',
      'include_dirs': [
          '.',
          '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib',
          '<(node_root_dir)/deps/zlib'
       ],
       'conditions': [
          ['OS == "win"', {
            'defines': [
              'WIN32',
              '_WIN32',
              'WINDOWS',
              '_WINDOWS'
            ],
            'include_dirs': [
               './win-include'
            ]
          }],
          ['OS=="solaris"', {
            'include_dirs': [
               './solaris-include'
            ]
          } , {
            'include_dirs': [
               './unix-include'
            ]
            }
          ]
      ],
      'dependencies': [
          'action_before_build',
          'zlib/zlib.gyp:zlib'
      ],
      "win_delay_load_hook": "false",
      'sources': [
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_add.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_add_dir.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_add_entry.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_close.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_delete.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_dir_add.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_dirent.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_discard.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_entry.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_err_str.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_clear.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_get.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_get_sys_type.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_strerror.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_error_to_str.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_extra_field.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_extra_field_api.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fclose.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fdopen.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_add.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_error_clear.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_error_get.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_get_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_get_external_attributes.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_get_offset.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_rename.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_replace.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_set_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_set_external_attributes.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_file_strerror.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_filerange_crc.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen_encrypted.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen_index.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fopen_index_encrypted.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_fread.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_archive_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_archive_flag.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_compression_implementation.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_encryption_implementation.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_file_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_name.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_num_entries.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_get_num_files.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_memdup.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_name_locate.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_new.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_open.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_rename.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_replace.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_archive_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_archive_flag.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_default_password.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_file_comment.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_file_compression.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_set_name.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_buffer.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_close.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_crc.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_deflate.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_error.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_file.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_filep.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_free.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_function.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_layered.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_open.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_pkware.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_pop.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_read.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_stat.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_window.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_zip.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_source_zip_new.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_stat.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_stat_index.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_stat_init.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_strerror.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_string.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange_all.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange_archive.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_unchange_data.c',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/zip_utf-8.c'
      ],
      'conditions': [
        ['OS == "win"', {
          'sources': [
              '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib/mkstemp.c',
          ],
        }]
      ],
      'direct_dependent_settings': {
        'include_dirs': [
            '.',
            '<(SHARED_INTERMEDIATE_DIR)/libzip-<@(libzip_version)/lib',
            '<(node_root_dir)/deps/zlib'
         ],
        'conditions': [
          ['OS == "win"', {
            'defines': [
              'WIN32',
              '_WIN32',
              'WINDOWS',
              '_WINDOWS'
            ],
            'include_dirs': [
               './win-include'
            ]
          }],
          ['OS=="solaris"', {
            'include_dirs': [
               './solaris-include'
            ]
          } , {
            'include_dirs': [
               './unix-include'
            ]
            }
          ]
        ]
      },
      'export_dependent_settings': [
        'action_before_build',
      ]
    }
  ]
}
