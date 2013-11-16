// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include "GXMRendererTexture2D.h"
#include <RendererTexture2DDesc.h>
#include <gxm.h>
#include <gxt.h>
#include "GXMRenderer.h"

using namespace SampleRenderer;

void GXMRendererTexture2D::GetGXMTextureFormat(RendererTexture2D::Format f, SceGxmTextureFormat& format)
{
	switch (f)
	{
	case FORMAT_B8G8R8A8:
		{
			format = SCE_GXM_TEXTURE_FORMAT_A8R8G8B8;
		}
		break;
	case FORMAT_DXT1:
		{
			format = SCE_GXM_TEXTURE_FORMAT_UBC1_ABGR;
		}
		break;
	default:
		RENDERER_ASSERT(0, "Implement!");
		break;
	}
}

SceGxmTextureAddrMode GXMRendererTexture2D::GetAddressMode(RendererTexture2D::Addressing add)
{
	switch(add)
	{
	case ADDRESSING_WRAP:
		{
			return SCE_GXM_TEXTURE_ADDR_REPEAT;
		}
		break;
	case ADDRESSING_CLAMP:
		{
			return SCE_GXM_TEXTURE_ADDR_CLAMP;
		}
		break;
	case ADDRESSING_MIRROR:
		{
			return SCE_GXM_TEXTURE_ADDR_MIRROR;
		}
		break;
	}

	RENDERER_ASSERT(0,"Format unknown!");
	return SCE_GXM_TEXTURE_ADDR_REPEAT;
}

GXMRendererTexture2D::GXMRendererTexture2D(const RendererTexture2DDesc &desc, GXMRenderer& renderer) :
RendererTexture2D(desc),m_Renderer(renderer), m_Texture(NULL), m_TextureMemoryId(0)
{	
	PxU32 m_width     = desc.width;
	PxU32 m_height    = desc.height;

	m_UAddrsMode = GetAddressMode(desc.addressingU);
	m_VAddrsMode = GetAddressMode(desc.addressingV);
	
	m_NumLevels = desc.numLevels;
	m_Data = new SceUInt8*[m_NumLevels];
	memset(m_Data, 0, sizeof(SceUInt8*)*m_NumLevels);
	
	if(desc.format == SampleRenderer::RendererTexture2D::FORMAT_GXT)
	{
		for(PxU32 i=0; i<desc.numLevels; i++)
		{
			// get the size of the texture data
			const uint32_t dataSize = sceGxtGetDataSize(desc.data);

			// allocate memory
			m_TextureMemoryId = new SceUID();
			m_Data[i] = (SceUInt8 *) GXMRenderer::graphicsAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
				dataSize,
				SCE_GXM_TEXTURE_ALIGNMENT,
				SCE_GXM_MEMORY_ATTRIB_READ,
				m_TextureMemoryId);

			// copy texture data
			const void *dataSrc = sceGxtGetDataAddress(desc.data);
			memcpy(m_Data[i], dataSrc, dataSize);

			m_Texture = new SceGxmTexture();

			SceGxmErrorCode err = SCE_OK;

			PX_UNUSED(err);
			// set up the texture control words
			err = sceGxtInitTexture(m_Texture, desc.data, m_Data[i], 0);
			RENDERER_ASSERT(err == SCE_OK, "Init texture fail");

			// set linear filtering
			err = sceGxmTextureSetMagFilter(
				m_Texture,
				SCE_GXM_TEXTURE_FILTER_LINEAR);
			RENDERER_ASSERT(err == SCE_OK, "Set filter fail");
			err = sceGxmTextureSetMinFilter(
				m_Texture,
				SCE_GXM_TEXTURE_FILTER_LINEAR);
			RENDERER_ASSERT(err == SCE_OK, "Set filter fail");

			sceGxmTextureSetUAddrMode(m_Texture, m_UAddrsMode);
			sceGxmTextureSetVAddrMode(m_Texture, m_VAddrsMode);
		}		
	}
	else
	{		
		GetGXMTextureFormat(desc.format, m_Format);

		for(PxU32 i=0; i<desc.numLevels; i++)
		{
			PxU32 w = getLevelDimension(m_width,  i);
			PxU32 h = getLevelDimension(m_height, i);
			PxU32 levelSize = computeImageByteSize(w, h, 1, desc.format);

			// allocate memory
			m_TextureMemoryId = new SceUID();
			m_Data[i] = (SceUInt8 *) GXMRenderer::graphicsAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
				levelSize,
				SCE_GXM_TEXTURE_ALIGNMENT,
				SCE_GXM_MEMORY_ATTRIB_READ,
				m_TextureMemoryId);

			//m_Data[i]       = new SceUInt8[levelSize];
			memset(m_Data[i], 0, levelSize);
			
			// create the texture
			m_Texture = new SceGxmTexture();

			SceGxmErrorCode err;

			err = sceGxmTextureInitLinear(m_Texture,m_Data[i], m_Format,getWidth(), getHeight(), 0);
			if(err != SCE_OK)
			{
				RENDERER_ASSERT(err == SCE_OK, "Failed to initialize texture!");
			}

			err = sceGxmTextureSetMagFilter(m_Texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
			RENDERER_ASSERT(err == SCE_OK, "Failed to initialize texture!");
			err = sceGxmTextureSetMinFilter(m_Texture, SCE_GXM_TEXTURE_FILTER_LINEAR);
			RENDERER_ASSERT(err == SCE_OK, "Failed to initialize texture!");

			sceGxmTextureSetUAddrMode(m_Texture, m_UAddrsMode);
			sceGxmTextureSetVAddrMode(m_Texture, m_VAddrsMode);
		}
	}


}

GXMRendererTexture2D::~GXMRendererTexture2D(void)
{
	if(m_TextureMemoryId)
	{
		GXMRenderer::graphicsFree(*m_TextureMemoryId);
		delete m_TextureMemoryId;
	}

	if(m_Texture)
	{
		delete m_Texture;			
	}

	if(m_Data)
	{
		delete [] m_Data;
	}
}

void *GXMRendererTexture2D::lockLevel(PxU32 level, PxU32 &pitch)
{
	void *buffer = 0;
	RENDERER_ASSERT(level < m_NumLevels, "Level out of range!");
	if(level < m_NumLevels)
	{
		buffer = m_Data[level];
	}

	pitch = 0xFFFFFFFF;
	return buffer;
}

void GXMRendererTexture2D::unlockLevel(PxU32 level)
{
	RENDERER_ASSERT(level < m_NumLevels, "Level out of range!");
	if(level < m_NumLevels)
	{
		sceGxmTextureSetData(m_Texture,m_Data[level]);		
	}	
}

void GXMRendererTexture2D::bind(PxU32 textureUnit)
{
	sceGxmSetFragmentTexture(m_Renderer.getContext(), textureUnit, m_Texture);	
}

#endif // #if defined(RENDERER_ENABLE_OPENGL)
