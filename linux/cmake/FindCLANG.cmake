set( CLANG_FOUND FALSE )

find_program( CLANG_CLANG		NAMES clang			clang-3.8		clang-3.7		clang-3.6		)
find_program( CLANG_CLANGXX 	NAMES clang++		clang++-3.8		clang++-3.7		clang++-3.6		)
find_program( CLANG_LLVM_AR		NAMES llvm-ar		llvm-ar-3.8		llvm-ar-3.7		llvm-ar-3.6		)
find_program( CLANG_LLVM_LINK	NAMES llvm-link		llvm-link-3.8	llvm-link-3.7	llvm-link-3.6	)
find_program( CLANG_LLVM_NM		NAMES llvm-nm		llvm-nm-3.8		llvm-nm-3.7		llvm-nm-3.6		)
find_program( CLANG_LLVM_RANLIB	NAMES llvm-ranlib	llvm-ranlib-3.8	llvm-ranlib-3.7	llvm-ranlib-3.6	)

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
