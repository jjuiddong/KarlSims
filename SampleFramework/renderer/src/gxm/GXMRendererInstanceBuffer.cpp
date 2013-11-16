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

#include "GXMRendererInstanceBuffer.h"
#include <RendererInstanceBufferDesc.h>
#include "GXMRenderer.h"

using namespace SampleRenderer;

GXMRendererInstanceBuffer::GXMRendererInstanceBuffer(const RendererInstanceBufferDesc &desc, GXMRenderer& renderer) :
	RendererInstanceBuffer(desc), m_Renderer(renderer)
{
	m_memoryId = 0;
	m_bufferSize   = (PxU32)(desc.maxInstances * m_stride);
	m_buffer       = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		m_bufferSize,
		8,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&m_memoryId);

	RENDERER_ASSERT(m_buffer, "Failed to create Vertex Buffer Object.");
		
	m_maxInstances = desc.maxInstances;
}

GXMRendererInstanceBuffer::~GXMRendererInstanceBuffer(void)
{
	if(m_memoryId)
	{
		m_buffer = NULL;
		GXMRenderer::graphicsFree(m_memoryId);
	}
}

physx::PxMat44 GXMRendererInstanceBuffer::getModelMatrix(PxU32 index) const
{
	physx::PxMat44 model;
	model = physx::PxMat44::createIdentity();
	if(index < m_maxInstances)
	{
		const void *instance = ((PxU8*)m_buffer)+(m_stride*index);
		PxVec3 column0 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_NORMALX]);
		PxVec3 column1 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_NORMALY]);
		PxVec3 column2 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_NORMALZ]);
		PxVec3 column3 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_POSITION]);

		model = PxMat44(column0, column1, column2, column3);
	}
	return model;
}

PxVec3 GXMRendererInstanceBuffer::getInstanceColumn(const void *instance, const GXMRendererInstanceBuffer::SemanticDesc &sd) const
{
	PxVec3 col = *(PxVec3*)(((PxU8*)instance)+sd.offset);
	return col;
}

void *GXMRendererInstanceBuffer::lock(void)
{
	return m_buffer;
}

void GXMRendererInstanceBuffer::unlock(void)
{

}

void GXMRendererInstanceBuffer::bind(PxU32 streamID, PxU32 firstInstance) const
{
	sceGxmSetVertexStream( m_Renderer.getContext(), 1, m_buffer);
}

void GXMRendererInstanceBuffer::unbind(PxU32 streamID) const
{

}

#endif // #if defined(RENDERER_ENABLE_OPENGL) 
