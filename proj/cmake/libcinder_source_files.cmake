cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

# ----------------------------------------------------------------------------------------------------------------------
# Cinder
# ----------------------------------------------------------------------------------------------------------------------
list( APPEND SRC_SET_CINDER_APP
	${CINDER_SRC_DIR}/cinder/app/AppBase.cpp
	${CINDER_SRC_DIR}/cinder/app/KeyEvent.cpp
	${CINDER_SRC_DIR}/cinder/app/Platform.cpp
	${CINDER_SRC_DIR}/cinder/app/Renderer.cpp
	${CINDER_SRC_DIR}/cinder/app/RendererGl.cpp
	${CINDER_SRC_DIR}/cinder/app/Window.cpp
)

if( CINDER_TARGET MATCHES "linux" )
    list( APPEND SRC_SET_CINDER_APP_LINUX
        ${CINDER_SRC_DIR}/cinder/app/linux/AppImplLinuxGlfw.cpp
        ${CINDER_SRC_DIR}/cinder/app/linux/AppLinux.cpp
        ${CINDER_SRC_DIR}/cinder/app/linux/PlatformLinux.cpp
        ${CINDER_SRC_DIR}/cinder/app/linux/RendererGlLinuxGlfw.cpp
        ${CINDER_SRC_DIR}/cinder/app/linux/WindowImplLinuxGlfw.cpp
    )
endif()

list( APPEND SRC_SET_CINDER
	${CINDER_SRC_DIR}/cinder/Area.cpp
	${CINDER_SRC_DIR}/cinder/Area.cpp
	${CINDER_SRC_DIR}/cinder/BandedMatrix.cpp
	${CINDER_SRC_DIR}/cinder/Base64.cpp
	${CINDER_SRC_DIR}/cinder/BSpline.cpp
	${CINDER_SRC_DIR}/cinder/BSplineFit.cpp
	${CINDER_SRC_DIR}/cinder/Buffer.cpp
	${CINDER_SRC_DIR}/cinder/Camera.cpp
	${CINDER_SRC_DIR}/cinder/CameraUi.cpp
	${CINDER_SRC_DIR}/cinder/Capture.cpp
	${CINDER_SRC_DIR}/cinder/Channel.cpp
	${CINDER_SRC_DIR}/cinder/CinderAssert.cpp
	${CINDER_SRC_DIR}/cinder/CinderMath.cpp
	${CINDER_SRC_DIR}/cinder/Clipboard.cpp
	${CINDER_SRC_DIR}/cinder/Color.cpp
	${CINDER_SRC_DIR}/cinder/ConvexHull.cpp
	${CINDER_SRC_DIR}/cinder/DataSource.cpp
	${CINDER_SRC_DIR}/cinder/DataTarget.cpp
	${CINDER_SRC_DIR}/cinder/Display.cpp
	${CINDER_SRC_DIR}/cinder/Exception.cpp
	${CINDER_SRC_DIR}/cinder/Font.cpp
	${CINDER_SRC_DIR}/cinder/Frustum.cpp
	${CINDER_SRC_DIR}/cinder/GeomIo.cpp
	${CINDER_SRC_DIR}/cinder/ImageFileTinyExr.cpp
	${CINDER_SRC_DIR}/cinder/ImageIo.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileRadiance.cpp
	${CINDER_SRC_DIR}/cinder/ImageSourceFileStbImage.cpp
	${CINDER_SRC_DIR}/cinder/ImageTargetFileStbImage.cpp
	${CINDER_SRC_DIR}/cinder/Json.cpp
	${CINDER_SRC_DIR}/cinder/Log.cpp
	${CINDER_SRC_DIR}/cinder/Matrix.cpp
	${CINDER_SRC_DIR}/cinder/ObjLoader.cpp
	${CINDER_SRC_DIR}/cinder/Path2d.cpp
	${CINDER_SRC_DIR}/cinder/Perlin.cpp
	${CINDER_SRC_DIR}/cinder/Plane.cpp
	${CINDER_SRC_DIR}/cinder/PolyLine.cpp
	${CINDER_SRC_DIR}/cinder/Rand.cpp
	${CINDER_SRC_DIR}/cinder/Ray.cpp
	${CINDER_SRC_DIR}/cinder/Rect.cpp
	${CINDER_SRC_DIR}/cinder/Serial.cpp
	${CINDER_SRC_DIR}/cinder/Shape2d.cpp
	${CINDER_SRC_DIR}/cinder/Signals.cpp
	${CINDER_SRC_DIR}/cinder/Sphere.cpp
	${CINDER_SRC_DIR}/cinder/Stream.cpp
	${CINDER_SRC_DIR}/cinder/Surface.cpp
	${CINDER_SRC_DIR}/cinder/System.cpp
	${CINDER_SRC_DIR}/cinder/Text.cpp
	${CINDER_SRC_DIR}/cinder/Timeline.cpp
	${CINDER_SRC_DIR}/cinder/TimelineItem.cpp
	${CINDER_SRC_DIR}/cinder/Timer.cpp
	${CINDER_SRC_DIR}/cinder/Triangulate.cpp
	${CINDER_SRC_DIR}/cinder/TriMesh.cpp
	${CINDER_SRC_DIR}/cinder/Tween.cpp
	${CINDER_SRC_DIR}/cinder/Unicode.cpp
	${CINDER_SRC_DIR}/cinder/Url.cpp
	${CINDER_SRC_DIR}/cinder/UrlImplCurl.cpp
	${CINDER_SRC_DIR}/cinder/Utilities.cpp
	${CINDER_SRC_DIR}/cinder/Xml.cpp
)

# ----------------------------------------------------------------------------------------------------------------------
# libpng
# ----------------------------------------------------------------------------------------------------------------------

if( PNG_FOUND )
	list( APPEND SRC_SET_CINDER
		${CINDER_SRC_DIR}/cinder/ImageSourcePng.cpp
	)
endif()

# ----------------------------------------------------------------------------------------------------------------------
# Source groups
# ----------------------------------------------------------------------------------------------------------------------
source_group( "cinder\\app"             FILES ${SRC_SET_CINDER_APP} )
source_group( "cinder\\app\\linux"      FILES ${SRC_SET_CINDER_APP_LINUX} )
source_group( "cinder" 		            FILES ${SRC_SET_CINDER} )

# ----------------------------------------------------------------------------------------------------------------------
# Combine the sources
# ----------------------------------------------------------------------------------------------------------------------
list( APPEND CINDER_SRC_FILES
	${SRC_SET_CINDER_APP}
	${SRC_SET_CINDER_APP_ANDROID}
	${SRC_SET_CINDER_APP_LINUX}
	${SRC_SET_CINDER}
)

# ----------------------------------------------------------------------------------------------------------------------
# FreeType
# ----------------------------------------------------------------------------------------------------------------------

if( NOT CINDER_FREETYPE_USE_SYSTEM )
	list( APPEND SRC_SET_FREETYPE
		${CINDER_SRC_DIR}/freetype/bdf/bdf.c
		${CINDER_SRC_DIR}/freetype/cff/cff.c
		${CINDER_SRC_DIR}/freetype/pcf/pcf.c
		${CINDER_SRC_DIR}/freetype/pfr/pfr.c
		${CINDER_SRC_DIR}/freetype/sfnt/sfnt.c
		${CINDER_SRC_DIR}/freetype/truetype/truetype.c
		${CINDER_SRC_DIR}/freetype/type1/type1.c
		${CINDER_SRC_DIR}/freetype/type42/type42.c
		${CINDER_SRC_DIR}/freetype/winfonts/winfnt.c
		${CINDER_SRC_DIR}/freetype/base/ftbase.c
		${CINDER_SRC_DIR}/freetype/base/ftbbox.c
		${CINDER_SRC_DIR}/freetype/base/ftbdf.c
		${CINDER_SRC_DIR}/freetype/base/ftbitmap.c
		${CINDER_SRC_DIR}/freetype/base/ftcid.c
		${CINDER_SRC_DIR}/freetype/base/ftdebug.c
		${CINDER_SRC_DIR}/freetype/base/ftfstype.c
		${CINDER_SRC_DIR}/freetype/base/ftgasp.c
		${CINDER_SRC_DIR}/freetype/base/ftglyph.c
		${CINDER_SRC_DIR}/freetype/base/ftgxval.c
		${CINDER_SRC_DIR}/freetype/base/ftinit.c
		${CINDER_SRC_DIR}/freetype/base/ftlcdfil.c
		${CINDER_SRC_DIR}/freetype/base/ftmm.c
		${CINDER_SRC_DIR}/freetype/base/ftotval.c
		${CINDER_SRC_DIR}/freetype/base/ftpatent.c
		${CINDER_SRC_DIR}/freetype/base/ftpfr.c
		${CINDER_SRC_DIR}/freetype/base/ftstroke.c
		${CINDER_SRC_DIR}/freetype/base/ftsynth.c
		${CINDER_SRC_DIR}/freetype/base/ftsystem.c
		${CINDER_SRC_DIR}/freetype/base/fttype1.c
		${CINDER_SRC_DIR}/freetype/base/ftwinfnt.c
		${CINDER_SRC_DIR}/freetype/raster/raster.c
		${CINDER_SRC_DIR}/freetype/smooth/smooth.c
		${CINDER_SRC_DIR}/freetype/autofit/autofit.c
		${CINDER_SRC_DIR}/freetype/bzip2/ftbzip2.c
		${CINDER_SRC_DIR}/freetype/cache/ftcache.c
		${CINDER_SRC_DIR}/freetype/gzip/ftgzip.c
		${CINDER_SRC_DIR}/freetype/lzw/ftlzw.c
		${CINDER_SRC_DIR}/freetype/gxvalid/gxvalid.c
		${CINDER_SRC_DIR}/freetype/otvalid/otvalid.c
		${CINDER_SRC_DIR}/freetype/psaux/psaux.c
		${CINDER_SRC_DIR}/freetype/pshinter/pshinter.c
		${CINDER_SRC_DIR}/freetype/psnames/psnames.c
		${CINDER_SRC_DIR}/freetype/cid/type1cid.c
		${CINDER_SRC_DIR}/freetype/bdf/bdflib.c
	)

	list( APPEND CINDER_SRC_FILES
		${SRC_SET_FREETYPE}
	)

	source_group( "freetype" 		        FILES ${SRC_SET_FREETYPE} )

endif() # ! CINDER_FREETYPE_USE_SYSTEM
