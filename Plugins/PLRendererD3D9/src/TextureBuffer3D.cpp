/*********************************************************\
 *  File: TextureBuffer3D.cpp                            *
 *
 *  Copyright (C) 2002-2013 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 *  and associated documentation files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or
 *  substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/Log/Log.h>
#include <PLGraphics/Image/ImagePart.h>
#include <PLGraphics/Image/ImageBuffer.h>
#include <PLGraphics/Image/Image.h>
#include "PLRendererD3D9/Renderer.h"
#include "PLRendererD3D9/TextureBuffer3D.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLMath;
using namespace PLGraphics;
namespace PLRendererD3D9 {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
TextureBuffer3D::~TextureBuffer3D()
{
	// Release the Direct3D 9 texture
	if (m_pD3D9Texture)
		m_pD3D9Texture->Release();

	// Update renderer statistics
	static_cast<PLRenderer::RendererBackend&>(GetRenderer()).GetWritableStatistics().nTextureBuffersNum--;
	static_cast<PLRenderer::RendererBackend&>(GetRenderer()).GetWritableStatistics().nTextureBuffersMem -= GetTotalNumOfBytes();
}

/**
*  @brief
*    Returns the Direct3D 9 texture
*/
LPDIRECT3DVOLUMETEXTURE9 TextureBuffer3D::GetD3D9Texture() const
{
	return m_pD3D9Texture;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
TextureBuffer3D::TextureBuffer3D(PLRenderer::Renderer &cRenderer, Image &cImage, EPixelFormat nInternalFormat, uint32 nFlags) :
	PLRenderer::TextureBuffer3D(cRenderer, nFlags),
	m_pD3D9Texture(nullptr)
{
	// Get the concrete renderer implementation
	Renderer &cRendererD3D9 = static_cast<Renderer&>(cRenderer);

	// Update renderer statistics
	cRendererD3D9.GetWritableStatistics().nTextureBuffersNum++;

	// Choose the texture buffer pixel formats which should be used
	EPixelFormat nImageFormat;
	bool bUsePreCompressedData;
	m_nFormat = cRendererD3D9.ChooseFormats(cImage, nInternalFormat, nFlags, nImageFormat, bUsePreCompressedData);

	// Get the first image part
	const ImagePart *pImagePart = cImage.GetPart(0);
	if (pImagePart) {
		// Get the first image buffer
		const ImageBuffer *pImageBuffer = pImagePart->GetMipmap(0);
		if (pImageBuffer) {
			// Get API pixel format
			uint32 *pAPIPixelFormat = cRendererD3D9.GetAPIPixelFormat(m_nFormat);
			if (pAPIPixelFormat) {
				// Get Direct3D 9 device
				LPDIRECT3DDEVICE9 pDevice = static_cast<Renderer&>(cRenderer).GetDevice();
				if (pDevice) {
					D3DFORMAT nAPIFormat = static_cast<D3DFORMAT>(*pAPIPixelFormat);

					// Get the size
					m_vSize = pImageBuffer->GetSize();

					// Is this texture buffer a render target?
					if (nFlags & RenderTarget) {
						// Create Direct3D 9 texture
						m_nNumOfMipmaps = 0;
						if (D3DXCreateVolumeTexture(pDevice, m_vSize.x, m_vSize.y, m_vSize.z, 1, D3DUSAGE_RENDERTARGET, nAPIFormat,
													D3DPOOL_DEFAULT, &m_pD3D9Texture) != D3D_OK) {
							// Error!
							m_nFormat = Unknown;
							m_vSize   = Vector3i::Zero;
						}
					} else {
						// Use given mipmaps if there are any?
						bool   bMipmaps           = nFlags & Mipmaps;
						uint32 nNumOfImageMipmaps = pImagePart->GetNumOfMipmaps() - 1;
						if (bMipmaps && !nNumOfImageMipmaps) {
							// Calculate the number of mipmaps
							m_nNumOfMipmaps = static_cast<uint32>(Math::Log2(static_cast<float>(Math::Max(m_vSize.x, m_vSize.y))));
						} else {
							m_nNumOfMipmaps = nNumOfImageMipmaps;
						}

						// Create Direct3D 9 texture - we have to use 'D3DPOOL_MANAGED' if we want to use 'D3DXFilterVolumeTexture()' later
						if (pDevice->CreateVolumeTexture(m_vSize.x, m_vSize.y, m_vSize.z, m_nNumOfMipmaps, 0, nAPIFormat,
														(bMipmaps && !nNumOfImageMipmaps) ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT, &m_pD3D9Texture, nullptr) != D3D_OK) {
							// There was an error and compression was used, use no compression and try again
							bUsePreCompressedData = false;
							m_nFormat			  = nImageFormat;
							pAPIPixelFormat		  = cRendererD3D9.GetAPIPixelFormat(m_nFormat);
							if (pAPIPixelFormat) {
								nAPIFormat = static_cast<D3DFORMAT>(*pAPIPixelFormat);
								if (pDevice->CreateVolumeTexture(m_vSize.x, m_vSize.y, m_vSize.z, m_nNumOfMipmaps, 0, nAPIFormat, D3DPOOL_DEFAULT, &m_pD3D9Texture, nullptr) != D3D_OK) {
									// Error!
									m_nFormat		= Unknown;
									m_nNumOfMipmaps	= 0;
									m_vSize			= Vector3i::Zero;
								}
							} else {
								// Error!
								m_nFormat		= Unknown;
								m_nNumOfMipmaps	= 0;
								m_vSize			= Vector3i::Zero;
							}
						}

						// Upload the texture buffer
						Vector3i vSize;
						for (uint32 nLevel=0; nLevel<=nNumOfImageMipmaps; nLevel++) {
							// Get the mipmap image buffer
							const ImageBuffer *pMipmapImageBuffer = pImagePart->GetMipmap(nLevel);
							if (pMipmapImageBuffer) {
								// Get the size of this mipmap level
								vSize = pMipmapImageBuffer->GetSize();

								// Get the volume
								LPDIRECT3DVOLUME9 pDestVolume = nullptr;
								m_pD3D9Texture->GetVolumeLevel(nLevel, &pDestVolume);
								if (pDestVolume) {
									// Is the source data compressed?
									if (bUsePreCompressedData && pMipmapImageBuffer->HasCompressedData()) {
										// Compressed source data
										D3DFORMAT nSourceFormat;
										switch (pMipmapImageBuffer->GetCompression()) {
											case CompressionDXT1:
												nSourceFormat = D3DFMT_DXT1;
												break;

											// [TODO] Unsupported
											// case Image::DXTC_DXT2:
											//	nSourceFormat = D3DFMT_DXT2;
											//	break;

											case CompressionDXT3:
												nSourceFormat = D3DFMT_DXT3;
												break;

											// [TODO] Unsupported
											// case Image::DXTC_DXT4:
											//	nSourceFormat = D3DFMT_DXT4;
											//	break;

											case CompressionDXT5:
												nSourceFormat = D3DFMT_DXT5;
												break;

											case CompressionLATC1:
												nSourceFormat = static_cast<D3DFORMAT>('1ITA');
												break;

											case CompressionLATC2:
												nSourceFormat = static_cast<D3DFORMAT>('2ITA');
												break;

											// [TODO] Check Image::DXTC_RXGB
											default:
												nSourceFormat = D3DFMT_UNKNOWN;
												break;
										}
										if (nSourceFormat != D3DFMT_UNKNOWN) {
											D3DBOX sSourceBox[] = { 0, 0, vSize.x, vSize.y, 0, vSize.z };
											D3DXLoadVolumeFromMemory(pDestVolume, nullptr, nullptr, static_cast<const uint8*>(pMipmapImageBuffer->GetCompressedData()), nSourceFormat,
																	 pMipmapImageBuffer->GetCompressedDataSize()/((vSize.y+3)/4/vSize.z), pMipmapImageBuffer->GetCompressedDataSize()/vSize.z,
																	 nullptr, sSourceBox, D3DX_FILTER_NONE, 0);
										}
									} else {
										// Uncompressed source data
										D3DFORMAT  nSourceFormat;
										uint8      nNumOfComponents = static_cast<uint8>(pMipmapImageBuffer->GetComponentsPerPixel());
										uint8     *pTempData        = const_cast<uint8*>(static_cast<const uint8*>(pMipmapImageBuffer->GetData()));
										switch (nNumOfComponents) {
											case 1:
												nSourceFormat = D3DFMT_L8;
												break;

											case 2:
												nSourceFormat = D3DFMT_A8L8;
												break;

											case 3:
											{
												// Quite stupid, but we really have to invert the color order...
												uint32 nSize = pMipmapImageBuffer->GetDataSize();
												nSourceFormat = D3DFMT_R8G8B8;
												pTempData = new uint8[nSize];
													  uint8 *pTempImageData = pTempData;
												const uint8 *pImageData     = static_cast<const uint8*>(pMipmapImageBuffer->GetData());
												const uint8 *pImageDataEnd  = static_cast<const uint8*>(pMipmapImageBuffer->GetData()) + nSize;
												while (pImageData<pImageDataEnd) {
													// Copy data and swap r/b
													pTempImageData[0] = pImageData[2];
													pTempImageData[1] = pImageData[1];
													pTempImageData[2] = pImageData[0];

													// Next pixel, please
													pTempImageData += 3;
													pImageData     += 3;
												}
												break;
											}

											case 4:
												nSourceFormat = D3DFMT_A8B8G8R8;
												break;

											default:
												nSourceFormat = D3DFMT_UNKNOWN;
												break;
										}
										if (nSourceFormat != D3DFMT_UNKNOWN) {
											D3DBOX sSourceBox[] = { 0, 0, vSize.x, vSize.y, 0, vSize.z };
											uint32 nRowPitch = vSize.x*nNumOfComponents;
											D3DXLoadVolumeFromMemory(pDestVolume, nullptr, nullptr, pTempData, nSourceFormat,
																	 nRowPitch, nRowPitch*vSize.y, nullptr,
																	 sSourceBox, D3DX_FILTER_NONE, 0);
										}
										if (pTempData != static_cast<const uint8*>(pMipmapImageBuffer->GetData()))
											delete [] pTempData;
									}

									// Release the volume
									pDestVolume->Release();
								}

								// Update the total number of bytes this texture buffer requires
								m_nTotalNumOfBytes += GetNumOfBytes(nLevel);
							}
						}

						// Do we need to create mipmaps by hand?
						if (bMipmaps && !nNumOfImageMipmaps) {
							// Let the API create the mipmaps for us
							if (D3DXFilterVolumeTexture(m_pD3D9Texture, nullptr, D3DX_DEFAULT, D3DX_FILTER_LINEAR) != D3D_OK)
								PL_LOG(Error, "Failed to create 3D texture buffer mipmap data")

							// Calculate the total number of bytes this texture buffer requires (note that we already have the base level!)
							for (uint32 nLevel=1; nLevel<=m_nNumOfMipmaps; nLevel++)
								m_nTotalNumOfBytes += GetNumOfBytes(nLevel);
						} else {
							// We have to define all mipmap levels down to 1x1x1 otherwise the texture buffer is invalid when we try to use any min
							// filter that uses mipmaps. Direct3D 9 normally uses white color when invalid/incomplete texture buffer is enabled.
							if (bMipmaps && (vSize.x != 1 || vSize.y != 1 || vSize.z != 1))
								PL_LOG(Warning, String::Format("Lowest mipmap is %dx%dx%d, but should be 1x1! As result, the texture buffer is probably NOT rendered correctly!", vSize.x, vSize.y, vSize.z))
						}
					}

					// Update renderer statistics
					cRendererD3D9.GetWritableStatistics().nTextureBuffersMem += GetTotalNumOfBytes();
				}
			}
		}
	}
}


//[-------------------------------------------------------]
//[ Public virtual PLRenderer::TextureBuffer functions    ]
//[-------------------------------------------------------]
bool TextureBuffer3D::CopyDataFrom(uint32 nMipmap, EPixelFormat nFormat, const void *pData, uint8 nFace)
{
	// Check parameters and Direct3D 9 texture
	if (nMipmap <= m_nNumOfMipmaps && nFormat != Unknown && pData && !nFace && m_pD3D9Texture) {
		// Get API pixel format
		uint32 *pAPIPixelFormat = static_cast<PLRenderer::RendererBackend&>(GetRenderer()).GetAPIPixelFormat(nFormat);
		if (pAPIPixelFormat) {
			// Get the size of this mipmap level
			Vector3i vSize = GetSize(nMipmap);

			// Get the volume
			LPDIRECT3DVOLUME9 pDestVolume = nullptr;
			m_pD3D9Texture->GetVolumeLevel(nMipmap, &pDestVolume);
			if (pDestVolume) {
				// Upload
				D3DBOX sSourceBox[] = { 0, 0, vSize.x, vSize.y, 0, vSize.z };
				D3DXLoadVolumeFromMemory(pDestVolume, nullptr, nullptr, pData, static_cast<D3DFORMAT>(*pAPIPixelFormat),
										 IsCompressedFormat() ? GetNumOfBytes(nMipmap)/((vSize.y+3)/4)/vSize.z : vSize.x*GetComponentsPerPixel(),
										 IsCompressedFormat() ? GetNumOfBytes(nMipmap)/vSize.z				   : vSize.x*vSize.y*GetComponentsPerPixel(),
										 nullptr, sSourceBox, D3DX_FILTER_NONE, 0);

				// Release the volume
				pDestVolume->Release();

				// Done
				return true;
			}
		}
	}

	// Error!
	return false;
}

bool TextureBuffer3D::CopyDataTo(uint32 nMipmap, EPixelFormat nFormat, void *pData, uint8 nFace) const
{
	// Check parameters and Direct3D 9 texture
	if (nMipmap <= m_nNumOfMipmaps && nFormat != Unknown && pData && !nFace && m_pD3D9Texture) {
		// [TODO] Implement me
	}

	// Error!
	return false;
}


//[-------------------------------------------------------]
//[ Private virtual PLRenderer::TextureBuffer functions   ]
//[-------------------------------------------------------]
bool TextureBuffer3D::MakeCurrent(uint32 nStage)
{
	// Check if there are renderer information
	LPDIRECT3DDEVICE9 pDevice = static_cast<Renderer&>(GetRenderer()).GetDevice();
	if (pDevice) {
		if (GetRenderer().GetTextureBuffer(nStage) != this) {
			if (!GetRenderer().SetTextureBuffer(nStage, this))
				return true; // Done
		}

		// Set Direct3D 9 texture
		return (pDevice->SetTexture(nStage, m_pD3D9Texture) == D3D_OK);
	}

	// Error!
	return false;
}


//[-------------------------------------------------------]
//[ Private virtual PLRenderer::Resource functions        ]
//[-------------------------------------------------------]
void TextureBuffer3D::BackupDeviceData(uint8 **ppBackup)
{
	// [TODO] Backup data
}

void TextureBuffer3D::RestoreDeviceData(uint8 **ppBackup)
{
	// [TODO] Restore data
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererD3D9
