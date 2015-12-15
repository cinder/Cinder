# HodginParticlesRedux
cmake_minimum_required( VERSION 3.0 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE on )

get_filename_component( CINDER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE )
include( ${CINDER_DIR}/linux/cmake/Cinder.cmake )

project( HodginParticlesRedux )

get_filename_component( SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src" ABSOLUTE )
get_filename_component( INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE )

if( NOT TARGET cinder${CINDER_LIB_SUFFIX} )
    find_package( cinder REQUIRED
        PATHS ${PROJECT_SOURCE_DIR}/../../../linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
        $ENV{Cinder_DIR}/linux/${CMAKE_BUILD_TYPE}/${CINDER_OUT_DIR_PREFIX}
    )
endif()

# Use PROJECT_NAME since CMAKE_PROJET_NAME returns the top-level project name.
set( EXE_NAME ${PROJECT_NAME} )

set( SRC_FILES
	${SRC_DIR}/Emitter.cpp
	${SRC_DIR}/Force.cpp
	${SRC_DIR}/ForceRepulsion.cpp
	${SRC_DIR}/HodginParticlesReduxApp.cpp
	${SRC_DIR}/ParticleController.cpp
	${SRC_DIR}/Constraint.cpp
	${SRC_DIR}/ForcePerlin.cpp
	${SRC_DIR}/ForceGravity.cpp
	${SRC_DIR}/Particle.cpp
	${SRC_DIR}/InfoPanel.cpp
)

add_executable( "${EXE_NAME}" ${SRC_FILES} )

target_include_directories(
	"${EXE_NAME}"
	PUBLIC ${INC_DIR}
)

target_link_libraries( "${EXE_NAME}" cinder${CINDER_LIB_SUFFIX} )
