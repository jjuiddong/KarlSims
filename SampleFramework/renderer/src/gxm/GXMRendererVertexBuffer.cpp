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

#include "RendererConfig.h"
#include <SamplePlatform.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include "GXMRendererVertexBuffer.h"

#include <RendererVertexBufferDesc.h>
#include "GXMRenderer.h"


using namespace SampleRenderer;

GXMRendererVertexBuffer::GXMRendererVertexBuffer(const RendererVertexBufferDesc &desc, GXMRenderer& renderer) 
	:RendererVertexBuffer(desc),m_Renderer(renderer), m_Buffer(0) , m_MemoryId(0)
{			
	RENDERER_ASSERT(m_stride && desc.maxVertices, "Unable to create Vertex Buffer of zero size.");
	if(m_stride && desc.maxVertices)
	{		
		m_Buffer = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
			m_stride * desc.maxVertices,
			8,
			SCE_GXM_MEMORY_ATTRIB_READ,
			&m_MemoryId);
		
		RENDERER_ASSERT(m_Buffer, "Failed to create Vertex Buffer Object.");

		m_maxVertices = desc.maxVertices;
	}
}

GXMRendererVertexBuffer::~GXMRendererVertexBuffer(void)
{
	if(m_MemoryId)
	{
		m_Buffer = NULL;
		GXMRenderer::graphicsFree(m_MemoryId);
	}
}

PxI32 GXMRendererVertexBuffer::GetSemanticOffset(const Semantic& semantic) const
{
	if(m_semanticDescs[semantic].format != RendererVertexBuffer::NUM_FORMATS)
	{
		return m_semanticDescs[semantic].offset;
	}
	else
	{
		return -1;
	}
}

void GXMRendererVertexBuffer::swizzleColor(void *colors, PxU32 stride, PxU32 numColors, RendererVertexBuffer::Format inFormat)
{
}

void *GXMRendererVertexBuffer::lock(void)
{
	void *buffer = m_Buffer;
	return buffer;
}

void GXMRendererVertexBuffer::unlock(void)
{
}


void GXMRendererVertexBuffer::bind(PxU32 streamID, PxU32 firstVertex)
{
	sceGxmSetVertexStream(m_Renderer.getContext(), 0, m_Buffer);
}

void GXMRendererVertexBuffer::unbind(PxU32 streamID)
{
}

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
