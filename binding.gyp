{
  'targets': [
    {
      # Needed declarations for the target
      'target_name': 'zlib',
      'product_name':'zlib',
	  'type': 'static_library',
      'sources': [ #Specify your source files here
        # Zlib
            'deps/zlib/contrib/minizip/ioapi.c',
            'deps/zlib/contrib/minizip/iowin32.c',
            'deps/zlib/contrib/minizip/unzip.c',
            'deps/zlib/contrib/minizip/zip.c',
            'deps/zlib/adler32.c',
            'deps/zlib/compress.c',
            'deps/zlib/crc32.c',
            'deps/zlib/deflate.c',
            'deps/zlib/gzio.c',
            'deps/zlib/infback.c',
            'deps/zlib/inffast.c',
            'deps/zlib/inflate.c',
            'deps/zlib/inftrees.c',
            'deps/zlib/trees.c',
            'deps/zlib/uncompr.c',
            'deps/zlib/zutil.c'
		],
		'include_dirs': [
			'deps/zlib/',
			'deps/zlib/contrib/minizip/',
		],
	},
	{
	  # Needed declarations for the target
      'target_name': 'libzip',
      'product_name':'libzip',
	  'type': 'shared_library',
	  'dependencies': [
          'zlib',
       ],
      'sources': [ #Specify your source files here
        # libzip files
			'deps/libzip-0.11.1/src/getopt.c',
			'deps/libzip-0.11.1/src/zipcmp.c',
			'deps/libzip-0.11.1-modified/zip_set_file_compression.c',
			'deps/libzip-0.11.1/src/zipmerge.c',
			'deps/libzip-0.11.1/src/ziptorrent.c',
			'deps/libzip-0.11.1/lib/mkstemp.c',
			'deps/libzip-0.11.1/lib/zip_add.c',
			'deps/libzip-0.11.1/lib/zip_add_dir.c',
			'deps/libzip-0.11.1/lib/zip_add_entry.c',
			'deps/libzip-0.11.1/lib/zip_close.c',
			'deps/libzip-0.11.1/lib/zip_delete.c',
			'deps/libzip-0.11.1/lib/zip_dirent.c',
			'deps/libzip-0.11.1/lib/zip_dir_add.c',
			'deps/libzip-0.11.1/lib/zip_discard.c',
			'deps/libzip-0.11.1/lib/zip_entry.c',
			'deps/libzip-0.11.1/lib/zip_error.c',
			'deps/libzip-0.11.1/lib/zip_error_clear.c',
			'deps/libzip-0.11.1/lib/zip_error_get.c',
			'deps/libzip-0.11.1/lib/zip_error_get_sys_type.c',
			'deps/libzip-0.11.1/lib/zip_error_strerror.c',
			'deps/libzip-0.11.1/lib/zip_error_to_str.c',
			'deps/libzip-0.11.1/lib/zip_err_str.c',
			'deps/libzip-0.11.1/lib/zip_extra_field.c',
			'deps/libzip-0.11.1/lib/zip_extra_field_api.c',
			'deps/libzip-0.11.1/lib/zip_fclose.c',
			'deps/libzip-0.11.1/lib/zip_fdopen.c',
			'deps/libzip-0.11.1/lib/zip_filerange_crc.c',
			'deps/libzip-0.11.1/lib/zip_file_add.c',
			'deps/libzip-0.11.1/lib/zip_file_error_clear.c',
			'deps/libzip-0.11.1/lib/zip_file_error_get.c',
			'deps/libzip-0.11.1/lib/zip_file_get_comment.c',
			'deps/libzip-0.11.1/lib/zip_file_get_offset.c',
			'deps/libzip-0.11.1/lib/zip_file_rename.c',
			'deps/libzip-0.11.1/lib/zip_file_replace.c',
			'deps/libzip-0.11.1/lib/zip_file_set_comment.c',
			'deps/libzip-0.11.1/lib/zip_file_strerror.c',
			'deps/libzip-0.11.1/lib/zip_fopen.c',
			'deps/libzip-0.11.1/lib/zip_fopen_encrypted.c',
			'deps/libzip-0.11.1/lib/zip_fopen_index.c',
			'deps/libzip-0.11.1/lib/zip_fopen_index_encrypted.c',
			'deps/libzip-0.11.1/lib/zip_fread.c',
			'deps/libzip-0.11.1/lib/zip_get_archive_comment.c',
			'deps/libzip-0.11.1/lib/zip_get_archive_flag.c',
			'deps/libzip-0.11.1/lib/zip_get_compression_implementation.c',
			'deps/libzip-0.11.1/lib/zip_get_encryption_implementation.c',
			'deps/libzip-0.11.1/lib/zip_get_file_comment.c',
			'deps/libzip-0.11.1/lib/zip_get_name.c',
			'deps/libzip-0.11.1/lib/zip_get_num_entries.c',
			'deps/libzip-0.11.1/lib/zip_get_num_files.c',
			'deps/libzip-0.11.1/lib/zip_memdup.c',
			'deps/libzip-0.11.1/lib/zip_name_locate.c',
			'deps/libzip-0.11.1/lib/zip_new.c',
			'deps/libzip-0.11.1/lib/zip_open.c',
			'deps/libzip-0.11.1/lib/zip_rename.c',
			'deps/libzip-0.11.1/lib/zip_replace.c',
			'deps/libzip-0.11.1/lib/zip_set_archive_comment.c',
			'deps/libzip-0.11.1/lib/zip_set_archive_flag.c',
			'deps/libzip-0.11.1/lib/zip_set_default_password.c',
			'deps/libzip-0.11.1/lib/zip_set_file_comment.c',
			'deps/libzip-0.11.1/lib/zip_set_name.c',
			'deps/libzip-0.11.1/lib/zip_source_buffer.c',
			'deps/libzip-0.11.1/lib/zip_source_close.c',
			'deps/libzip-0.11.1/lib/zip_source_crc.c',
			'deps/libzip-0.11.1/lib/zip_source_deflate.c',
			'deps/libzip-0.11.1/lib/zip_source_error.c',
			'deps/libzip-0.11.1/lib/zip_source_file.c',
			'deps/libzip-0.11.1/lib/zip_source_filep.c',
			'deps/libzip-0.11.1/lib/zip_source_free.c',
			'deps/libzip-0.11.1/lib/zip_source_function.c',
			'deps/libzip-0.11.1/lib/zip_source_layered.c',
			'deps/libzip-0.11.1/lib/zip_source_open.c',
			'deps/libzip-0.11.1/lib/zip_source_pkware.c',
			'deps/libzip-0.11.1/lib/zip_source_pop.c',
			'deps/libzip-0.11.1/lib/zip_source_read.c',
			'deps/libzip-0.11.1/lib/zip_source_stat.c',
			'deps/libzip-0.11.1/lib/zip_source_window.c',
			'deps/libzip-0.11.1/lib/zip_source_zip.c',
			'deps/libzip-0.11.1/lib/zip_source_zip_new.c',
			'deps/libzip-0.11.1/lib/zip_stat.c',
			'deps/libzip-0.11.1/lib/zip_stat_index.c',
			'deps/libzip-0.11.1/lib/zip_stat_init.c',
			'deps/libzip-0.11.1/lib/zip_strerror.c',
			'deps/libzip-0.11.1/lib/zip_string.c',
			'deps/libzip-0.11.1/lib/zip_unchange.c',
			'deps/libzip-0.11.1/lib/zip_unchange_all.c',
			'deps/libzip-0.11.1/lib/zip_unchange_archive.c',
			'deps/libzip-0.11.1/lib/zip_unchange_data.c',
			'deps/libzip-0.11.1/lib/zip_utf-8.c',
      ],
	  'include_dirs': [
	    'deps/libzip-0.11.1-modified',
		'deps/libzip-0.11.1/lib',
		'deps/zlib/',
		'deps/zlib/contrib/minizip/',
	  ],
	  'libraries': [ 
		  '<(PRODUCT_DIR)/zlib.lib',
       ],
	  'msvs_settings': {
            'VCLinkerTool': {
              'AdditionalOptions': [
                '/FORCE:MULTIPLE'
              ],
			}
	  }
	   
    },
	{
	  # Needed declarations for the target
      'target_name': '_zipfile',
      'product_name':'_zipfile',
      'sources': [ #Specify your source files here
		"src/_zipfile.cc",
        "src/node_zipfile.cpp",
	  ],
	  'dependencies': [
          'libzip',
       ],
	  'defines': [
        'EXCLUDE_WIN_FIXES',
      ],
	  'include_dirs': [
	    'include/',
	    'deps/libzip-0.11.1-modified/',
		'deps/libzip-0.11.1/lib',
		'deps/zlib/',
		'deps/zlib/contrib/minizip/',
	  ],
	  'msvs_settings': {
            'VCLinkerTool': {
              'AdditionalOptions': [
                '/FORCE:MULTIPLE'
              ],
			}
	  }
    },
	{
        'target_name': 'copyToLib',
        'type': 'none',
        'toolsets': ['host'],
        'copies': [
          {
            'files': [
              '<(PRODUCT_DIR)/libzip.dll',
              '<(PRODUCT_DIR)/_zipfile.node',
            ],
            'destination': 'lib/',
          }],
        }, 
  ] # end targets
}

