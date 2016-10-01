/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/gl/TextureFormatParsers.h"

#if defined( CINDER_GL_ANGLE )
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
#endif

namespace cinder { namespace gl {

void parseKtx( const DataSourceRef &dataSource, TextureData *resultData )
{
	typedef struct {
		uint8_t		identifier[12];
		uint32_t	endianness;
		uint32_t	glType;
		uint32_t	glTypeSize;
		uint32_t	glFormat;
		uint32_t	glInternalFormat;
		uint32_t	glBaseInternalFormat;
		uint32_t	pixelWidth;
		uint32_t	pixelHeight;
		uint32_t	pixelDepth;
		uint32_t	numberOfArrayElements;
		uint32_t	numberOfFaces;
		uint32_t	numberOfMipmapLevels;
		uint32_t	bytesOfKeyValueData;
	} KtxHeader;

	static const uint8_t FileIdentifier[12] = {
		0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
	};
	
	KtxHeader header;
	auto ktxStream = dataSource->createStream();
	ktxStream->readData( &header, sizeof(header) );
	
	if( memcmp( header.identifier, FileIdentifier, sizeof(FileIdentifier) ) )
		throw KtxParseExc( "File identifier mismatch" );			
	
	if( header.endianness != 0x04030201 )
		throw KtxParseExc( "Only little endian currently supported" );
	
	if( header.numberOfArrayElements != 0 )
		throw KtxParseExc( "Array textures not currently supported" );

	if( ( header.numberOfFaces != 1 ) && ( header.numberOfFaces != 6 ) )
		throw KtxParseExc( "Unsupported number of faces" );

	if( header.pixelDepth != 0 )
		throw KtxParseExc( "3D textures not currently supported" );
	
	resultData->setWidth( header.pixelWidth );
	resultData->setHeight( header.pixelHeight );
	resultData->setDepth( header.pixelDepth );
	resultData->setNumFaces( header.numberOfFaces );
	resultData->setInternalFormat( header.glInternalFormat );
	resultData->setDataFormat( header.glFormat );
	resultData->setDataType( header.glType );
	resultData->setUnpackAlignment( 4 );
	
	ktxStream->seekRelative( header.bytesOfKeyValueData );

	// clear output containers
	resultData->clear();
	size_t byteOffset = 0;
	for( int levelIdx = 0; levelIdx < std::max<int>( 1, header.numberOfMipmapLevels ); ++levelIdx ) {
		
		resultData->push_back( TextureData::Level() );
		TextureData::Level &level = resultData->back();
		level.width = std::max<int>( 1, header.pixelWidth >> levelIdx );
		level.height = std::max<int>( 1, header.pixelHeight >> levelIdx );
		level.depth = 0;
		
		uint32_t imageSize;
		ktxStream->readData( &imageSize, sizeof(imageSize) ); // the size of a single face

		if( levelIdx == 0 ) { // if this is our first level, we need to allocate storage. If mipmapping is on we need double the memory required for the first level
			if( header.numberOfMipmapLevels > 1 )
				resultData->allocateDataStore( imageSize * 2 * header.numberOfFaces );
			else
				resultData->allocateDataStore( imageSize * header.numberOfFaces );
			resultData->mapDataStore();
		}
		
		for( int arrayElement = 0; arrayElement < std::max<int>( 1, header.numberOfArrayElements ); ++arrayElement ) { // currently always 0->1
			for( uint32_t faceIdx = 0; faceIdx < header.numberOfFaces; ++faceIdx ) { // currently always 0->1 or 0->6
				level.push_back( TextureData::Face() );
				TextureData::Face &face = level.back();
				for( uint32_t zSlice = 0; zSlice < header.pixelDepth + 1; ++zSlice ) { // curently always 0->1
					face.dataSize = imageSize;
					face.offset = byteOffset;
					if( byteOffset + imageSize > resultData->getDataStoreSize() )
						throw TextureDataStoreTooSmallExc();
					ktxStream->readData( resultData->getDataStorePtr( byteOffset ), imageSize );
					
//					newFace.back().depth = zSlice;
					byteOffset += imageSize;
				}
				ktxStream->seekRelative( 3 - (imageSize + 3) % 4 ); // cube padding
			}
		}
		ktxStream->seekRelative( 3 - (ktxStream->tell() + 3) % 4 ); // mip padding
	}

	resultData->unmapDataStore();
}

#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
void parseDds( const DataSourceRef &dataSource, TextureData *resultData )
{
	typedef struct { // DDCOLORKEY
		uint32_t dw1;
		uint32_t dw2;
	} ddColorKey;

	typedef struct  { // DDSCAPS2
		uint32_t dwCaps1;
		uint32_t dwCaps2;
		uint32_t Reserved[2];
	} ddCaps2;

	typedef struct { // DDPIXELFORMAT
		uint32_t  dwSize;
		uint32_t  dwFlags;
		uint32_t  dwFourCC;
		union {
			uint32_t  dwRGBBitCount;
			uint32_t  dwYUVBitCount;
			uint32_t  dwZBufferBitDepth;
			uint32_t  dwAlphaBitDepth;
			uint32_t  dwLuminanceBitCount;
			uint32_t  dwBumpBitCount;
			uint32_t  dwPrivateFormatBitCount;
		} ;
		union {
			uint32_t  dwRBitMask;
			uint32_t  dwYBitMask;
			uint32_t  dwStencilBitDepth;
			uint32_t  dwLuminanceBitMask;
			uint32_t  dwBumpDuBitMask;
			uint32_t  dwOperations;
		} ;
		union {
			uint32_t  dwGBitMask;
			uint32_t  dwUBitMask;
			uint32_t  dwZBitMask;
			uint32_t  dwBumpDvBitMask;
			struct {
				uint16_t wFlipMSTypes;
				uint16_t wBltMSTypes;
			} MultiSampleCaps;
		};
		union {
			uint32_t  dwBBitMask;
			uint32_t  dwVBitMask;
			uint32_t  dwStencilBitMask;
			uint32_t  dwBumpLuminanceBitMask;
		};
		union {
			uint32_t  dwRGBAlphaBitMask;
			uint32_t  dwYUVAlphaBitMask;
			uint32_t  dwLuminanceAlphaBitMask;
			uint32_t  dwRGBZBitMask;
			uint32_t  dwYUVZBitMask;
		} ;
	} DdPixelFormat;

	typedef struct DdSurface // this is lifted and adapted from DDSURFACEDESC2
	{
		uint32_t               dwSize;                 // size of the DDSURFACEDESC structure
		uint32_t               dwFlags;                // determines what fields are valid
		uint32_t               dwHeight;               // height of surface to be created
		uint32_t               dwWidth;                // width of input surface
		union
		{
			int32_t            lPitch;                 // distance to start of next line (return value only)
			uint32_t           dwLinearSize;           // Formless late-allocated optimized surface size
		};
		union
		{
			uint32_t           dwBackBufferCount;      // number of back buffers requested
			uint32_t           dwDepth;                // the depth if this is a volume texture 
		};
		union
		{
			uint32_t			dwMipMapCount;          // number of mip-map levels requestde
													// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
			uint32_t			dwRefreshRate;          // refresh rate (used when display mode is described)
			uint32_t			dwSrcVBHandle;          // The source used in VB::Optimize
		};
		uint32_t				dwAlphaBitDepth;        // depth of alpha buffer requested
		uint32_t				dwReserved;             // reserved
		uint32_t				lpSurface;              // pointer to the associated surface memory
		union
		{
			ddColorKey			ddckCKDestOverlay;      // color key for destination overlay use
			uint32_t			dwEmptyFaceColor;       // Physical color for empty cubemap faces
		};
		ddColorKey          ddckCKDestBlt;          // color key for destination blt use
		ddColorKey          ddckCKSrcOverlay;       // color key for source overlay use
		ddColorKey          ddckCKSrcBlt;           // color key for source blt use
		union
		{
			DdPixelFormat		ddpfPixelFormat;        // pixel format description of the surface
			uint32_t			dwFVF;                  // vertex format description of vertex buffers
		};
		ddCaps2			ddsCaps;                // direct draw surface capabilities
		uint32_t		dwTextureStage;         // stage in multitexture cascade
	} DdSurface;

	typedef struct {
		uint32_t/*DXGI_FORMAT*/					dxgiFormat;
		uint32_t/*D3D10_RESOURCE_DIMENSION*/	resourceDimension;
		uint32_t								miscFlag;
		uint32_t								arraySize;
		uint32_t								reserved;
	} DdsHeader10;

	enum { FOURCC_DXT1 = 0x31545844, FOURCC_DXT3 = 0x33545844, FOURCC_DXT5 = 0x35545844, FOURCC_DX10 = 0x30315844,
		FOURCC_ATI1 = 0x31495441, FOURCC_ATI2= 0x32495441 };

	enum { DDPF_ALPHAPIXELS = 0x1, DDPF_ALPHA = 0x2, DDPF_FOURCC = 0x4, DDPF_RGB = 0x40, DDPF_YUV = 0x200, DDPF_LUMINANCE = 0x20000 };

	auto ddsStream = dataSource->createStream();
	DdSurface ddsd;
	DdsHeader10 ddsHeader10;
	char filecode[4];
	int numFaces = 1;
	ddsStream->readData( filecode, 4 );
	if( strncmp( filecode, "DDS ", 4 ) != 0 ) { 
		throw DdsParseExc( "File identifier mismatch" );
	}

	ddsStream->readData( &ddsd, 124/*sizeof(ddsd)*/ );

	// has header 10
	if( ( ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC ) && ( ddsd.ddpfPixelFormat.dwFourCC == FOURCC_DX10 ) ) {
		ddsStream->readData( &ddsHeader10, sizeof(DdsHeader10) );
		if( ddsHeader10.miscFlag == 0x4/*DDS_RESOURCE_MISC_TEXTURECUBE*/ )
			numFaces = 6;
	}
	else { // classic header
		if( ( ddsd.ddsCaps.dwCaps1 & 0x8 /*DDSCAPS_COMPLEX*/) && ( ddsd.ddsCaps.dwCaps2 & 0x200 /*DDSCAPS2_CUBEMAP*/ ) )
			numFaces = 6;
	}

	resultData->setWidth( ddsd.dwWidth );
	resultData->setHeight( ddsd.dwHeight );
	resultData->setDepth( 1 );
	resultData->setNumFaces( numFaces );

	int numMipMaps = ddsd.dwMipMapCount > 0 ? ddsd.dwMipMapCount : 1;
	int internalFormat, dataFormat = 0, dataType = 0;
	int32_t blockSizeBytes = 16;

	if( ! ( ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC ) ) { // does not have valid FOURCC value
#if ! defined( CINDER_GL_ANGLE )
		if( ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB ) { // uncompressed RGB data
			if( ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS ) { // contains alpha data
				internalFormat = GL_RGBA8;
#if defined( CINDER_GL_ES )
				dataFormat = GL_BGRA_EXT;
#else
				dataFormat = GL_BGRA;
#endif
				blockSizeBytes = sizeof(uint8_t) * 4;
				dataType = GL_UNSIGNED_BYTE;
			}
			else {
				internalFormat = GL_RGB8;
				dataFormat = GL_BGR;
				blockSizeBytes = sizeof(uint8_t) * 3;
				dataType = GL_UNSIGNED_BYTE;
			}
		}
		else {
			throw DdsParseExc( "Unsupported image format" );
		}
#else // ANGLE doesn't support GL_BGR or GL_BGRA
		throw DdsParseExc( "Unsupported image format" );
#endif
	}
	else {
		switch( ddsd.ddpfPixelFormat.dwFourCC ) { 
#if ! defined( CINDER_GL_ANGLE )
			case 20 /*D3DFMT_R8G8B8*/:
				internalFormat = GL_RGB8;
				dataFormat = GL_BGR;
				dataType = GL_UNSIGNED_BYTE;
				blockSizeBytes = sizeof(uint8_t) * 3;
			break;
			case 32 /*D3DFMT_A8B8G8R8*/:
				internalFormat = GL_RGBA8;
				dataFormat = GL_BGRA;
				dataType = GL_UNSIGNED_BYTE;
				blockSizeBytes = sizeof(uint8_t) * 4;
			break;
#endif
			case FOURCC_DXT1: 
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				blockSizeBytes = 8;
			break; 
			case FOURCC_DXT3: 
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break; 
			case FOURCC_DXT5: 
				internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
			break;
#if ! defined( CINDER_GL_ANGLE )
			case FOURCC_ATI1: // aka DX10 BC4
				internalFormat = GL_COMPRESSED_RED_RGTC1;
				blockSizeBytes = 8;
			break;
			case FOURCC_ATI2: // aka DX10 BC5
				internalFormat = GL_COMPRESSED_RG_RGTC2;
			break;
#endif
			case FOURCC_DX10:
				switch( ddsHeader10.dxgiFormat ) {
#if ! defined( CINDER_GL_ES_2 )
					case 10/*DXGI_FORMAT_R16G16B16A16_FLOAT*/:
						internalFormat = GL_RGBA16F;
						dataType = GL_HALF_FLOAT;
						dataFormat = GL_RGBA;
						blockSizeBytes = sizeof(uint16_t) * 4;
					break;
					case 2/*DXGI_FORMAT_R32G32B32A32_FLOAT*/:
						internalFormat = GL_RGBA32F;
						dataType = GL_FLOAT;
						dataFormat = GL_RGBA;
						blockSizeBytes = sizeof(float) * 4;
					break;
#endif
#if ! defined( CINDER_GL_ANGLE )
					case 12/*DXGI_FORMAT_R16G16B16A16_UINT*/:
						internalFormat = GL_RGBA16;
						dataType = GL_UNSIGNED_SHORT;
						dataFormat = GL_RGBA;
						blockSizeBytes = sizeof(uint16_t) * 4;
					break;
#endif
					case 70/*DXGI_FORMAT_BC1_TYPELESS*/:
					case 71/*DXGI_FORMAT_BC1_UNORM*/:
					case 72/*DXGI_FORMAT_BC1_UNORM_SRGB*/:
						internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
						blockSizeBytes = 8;
					break;
					case 73/*DXGI_FORMAT_BC2_TYPELESS*/:
					case 74/*DXGI_FORMAT_BC2_UNORM*/:
					case 75/*DXGI_FORMAT_BC2_UNORM_SRGB*/:
						internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					break;
					case 76/*DXGI_FORMAT_BC3_TYPELESS*/:
					case 77/*DXGI_FORMAT_BC3_UNORM*/:
					case 78/*DXGI_FORMAT_BC3_UNORM_SRGB*/:
						internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					break;
#if ! defined( CINDER_GL_ANGLE )
					case 97/*DXGI_FORMAT_BC7_TYPELESS*/:
					case 98/*DXGI_FORMAT_BC7_UNORM*/:
					case 99/*DXGI_FORMAT_BC7_UNORM_SRGB*/:
						internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
					break;
#endif
					default:
						throw DdsParseExc( "Unsupported image format" );
				}
			break;
			default:
				throw DdsParseExc( "Unsupported image format" );
			break;
		} 
	}

	if( (dataType == 0) && ( ! (ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC) ) )
		blockSizeBytes = ( ddsd.ddpfPixelFormat.dwRGBBitCount + 7 ) / 8;

	// Create the texture
	resultData->setUnpackAlignment( 4 );
	resultData->setDataFormat( dataFormat );
	resultData->setInternalFormat( internalFormat );
	resultData->setDataType( dataType ); // 0 implies compressed

	// fn which calculates memory requirements for a given MIP level 'level'
	auto calcImageLevelSize = [ddsd,blockSizeBytes,dataType]( int level ) {
		int levelWidth = std::max<int>( 1, (ddsd.dwWidth>>level) );
		int levelHeight = std::max<int>( 1, (ddsd.dwHeight>>level) );
		if( dataType != 0 ) { // uncompressed
			int pitch = ( levelWidth * blockSizeBytes * 8 + 7 ) / 8;
			return levelHeight * pitch;
		}
		else {
			int blockWidth = std::max<int>( 1, (levelWidth+3) / 4 );
			int blockHeight = std::max<int>( 1, (levelHeight+3) / 4 );
			int rowBytes = blockWidth * blockSizeBytes;
			return blockHeight * rowBytes;
		}
	};

	// calculate the space we need
	uint32_t spaceRequired = 0;
	for( int level = 0; level < numMipMaps && (ddsd.dwWidth || ddsd.dwHeight); ++level )
		spaceRequired += calcImageLevelSize( level );
	spaceRequired *= numFaces;
	resultData->allocateDataStore( spaceRequired );

	// allocate all levels and faces
	for( int levelIdx = 0; levelIdx < numMipMaps && (ddsd.dwWidth || ddsd.dwHeight); ++levelIdx ) { 
		resultData->push_back( TextureData::Level() );
		TextureData::Level &level = resultData->back();		
		level.width = std::max<int>( 1, (ddsd.dwWidth>>levelIdx) );
		level.height = std::max<int>( 1, (ddsd.dwHeight>>levelIdx) );
		level.depth = 0;
		for( int faceIdx = 0; faceIdx < numFaces; ++faceIdx )
			level.push_back( TextureData::Face() );
	}

	resultData->mapDataStore();
	size_t byteOffset = 0;
	for( int faceIdx = 0; faceIdx < numFaces; ++faceIdx ) {
		for( int levelIdx = 0; levelIdx < numMipMaps && (ddsd.dwWidth || ddsd.dwHeight); ++levelIdx ) { 
			const uint32_t imageSize = calcImageLevelSize( levelIdx );

			TextureData::Level &level = resultData->getLevels()[levelIdx];
			TextureData::Face &face = level.getFaces()[faceIdx];
			face.dataSize = imageSize;
			face.offset = byteOffset;
			if( byteOffset + imageSize > resultData->getDataStoreSize() )
				throw TextureDataStoreTooSmallExc();

			ddsStream->readDataAvailable( resultData->getDataStorePtr( byteOffset ), imageSize );
			byteOffset += imageSize;
		}
	}

	resultData->unmapDataStore();
}
#endif // ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )

} } // namespace cinder::gl