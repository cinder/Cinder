set( CLANG_FOUND FALSE )

# Find clang first
find_program( CLANG_CLANG NAMES clang-3.8 clang-3.7 clang-3.6 clang-3.5 clang )

# If clang is found - try to find the rest of the tools in the same dir. 
if( CLANG_CLANG )
	get_filename_component( _tmp_dir "${CLANG_CLANG}" DIRECTORY )
	message( "_tmp_dir: ${_tmp_dir}" )

	string( REGEX REPLACE "^.*-([0-9]+\\.[0-9]+)" "\\1" _tmp_ver "${CLANG_CLANG}" )
	message( "_tmp_ver: ${_tmp_ver}" )

	if( _tmp_ver )
		set( _tmp_ver "-${_tmp_ver}" )
	endif()

	find_program( CLANG_CLANG		NAMES clang${_tmp_ver}			clang		PATHS ${_tmp_dir} NO_DEFAULT_PATH )
	find_program( CLANG_CLANGXX 	NAMES clang++${_tmp_ver}		clang++		PATHS ${_tmp_dir} NO_DEFAULT_PATH )
	find_program( CLANG_LLVM_AR		NAMES llvm-ar${_tmp_ver}		llvm-ar		PATHS ${_tmp_dir} NO_DEFAULT_PATH )
	find_program( CLANG_LLVM_LINK	NAMES llvm-link${_tmp_ver}		llvm-link	PATHS ${_tmp_dir} NO_DEFAULT_PATH )
	find_program( CLANG_LLVM_NM		NAMES llvm-nm${_tmp_ver}		llvm-nm		PATHS ${_tmp_dir} NO_DEFAULT_PATH )
	find_program( CLANG_LLVM_RANLIB	NAMES llvm-ranlib${_tmp_ver}	llvm-ranlib	PATHS ${_tmp_dir} NO_DEFAULT_PATH )

	unset( _tmp_dir )
	unset( _tmp_ver )
endif()

# Search in the paths for things that aren't found yet.
if( NOT CLANG_CLANG )
	find_program( CLANG_CLANG		NAMES clang clang-3.8 clang-3.7 clang-3.6 clang-3.5 )
endif()
if( NOT CLANG_CLANGXX )
	find_program( CLANG_CLANGXX 	NAMES clang++ clang++-3.8 clang++-3.7 clang++-3.6 clang++-3.5 )
endif()
if( NOT CLANG_LLVM_AR )
	find_program( CLANG_LLVM_AR		NAMES llvm-ar llvm-ar-3.8 llvm-ar-3.7 llvm-ar-3.6 llvm-ar-3.5 )
endif()
if( NOT CLANG_LLVM_LINK )
	find_program( CLANG_LLVM_LINK	NAMES llvm-link llvm-link-3.8 llvm-link-3.7 llvm-link-3.6 llvm-link-3.5 )
endif()
if( NOT CLANG_LLVM_NM )
	find_program( CLANG_LLVM_NM		NAMES llvm-nm llvm-nm-3.8 llvm-nm-3.7 llvm-nm-3.6 llvm-nm-3.5 )
endif()
if( NOT CLANG_LLVM_RANLIB )
	find_program( CLANG_LLVM_RANLIB	NAMES llvm-ranlib llvm-ranlib-3.8 llvm-ranlib-3.7 llvm-ranlib-3.6 llvm-ranlib-3.5 )
endif()

#find_program( CLANG_CLANG		NAMES clang			clang-3.8		clang-3.7		clang-3.6		clang-3.5		)
#find_program( CLANG_CLANGXX 	NAMES clang++		clang++-3.8		clang++-3.7		clang++-3.6		clang++-3.5		)
#find_program( CLANG_LLVM_AR		NAMES llvm-ar		llvm-ar-3.8		llvm-ar-3.7		llvm-ar-3.6		llvm-ar-3.5		)
#find_program( CLANG_LLVM_LINK	NAMES llvm-link		llvm-link-3.8	llvm-link-3.7	llvm-link-3.6	llvm-link-3.5	)
#find_program( CLANG_LLVM_NM		NAMES llvm-nm		llvm-nm-3.8		llvm-nm-3.7		llvm-nm-3.6		llvm-nm-3.5		)
#find_program( CLANG_LLVM_RANLIB	NAMES llvm-ranlib	llvm-ranlib-3.8	llvm-ranlib-3.7	llvm-ranlib-3.6	llvm-ranlib-3.5	)

if( CLANG_CLANG AND CLANG_CLANGXX AND CLANG_LLVM_AR AND CLANG_LLVM_LINK AND CLANG_LLVM_NM AND CLANG_LLVM_RANLIB )	
	set( CLANG_FOUND TRUE )

	mark_as_advanced(
		CLANG_CLANG
		CLANG_CLANGXX
		CLANG_LLVM_AR
		CLANG_LLVM_LINK
		CLANG_LLVM_NM
		CLANG_LLVM_RANLIB
	)
	
	include( FindPackageHandleStandardArgs )
	find_package_handle_standard_args( clang		REQUIRED_VARS CLANG_CLANG 		)
	find_package_handle_standard_args( clang++		REQUIRED_VARS CLANG_CLANGXX		)
	find_package_handle_standard_args( llvm-ar		REQUIRED_VARS CLANG_LLVM_AR		)
	find_package_handle_standard_args( llvm-link	REQUIRED_VARS CLANG_LLVM_LINK	)
	find_package_handle_standard_args( llvm-nm		REQUIRED_VARS CLANG_LLVM_NM		)
	find_package_handle_standard_args( llvm-ranlib	REQUIRED_VARS CLANG_LLVM_RANLIB	)
endif()
