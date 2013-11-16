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

#include "GXMRendererMesh.h"

#include "GXMRendererInstanceBuffer.h"
#include "GXMRendererMaterial.h"

using namespace SampleRenderer;


GXMRendererMesh::GXMRendererMesh(GXMRenderer &renderer, const RendererMeshDesc &desc) :
	RendererMesh(desc), m_renderer(renderer), mRendererIndexBuffer(0), mCurrentRendererIndexBufferSize(0), mMaxVerts(0),
		mRendererIndexBufferUid(0)
{
	InitVertexRenderer();
}

GXMRendererMesh::~GXMRendererMesh(void)
{

	GXMRenderer::graphicsFree(mRendererIndexBufferUid);

}

void GXMRendererMesh::InitVertexRenderer()
{
	mMaxVerts = 4096*3;

	mRendererIndexBuffer = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		sizeof(PxU16) * mMaxVerts,
		8,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&mRendererIndexBufferUid);

	PX_ASSERT(mRendererIndexBuffer);

	mCurrentRendererIndexBufferSize = 0;
}

void GXMRendererMesh::renderIndices(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial *material) const
{
	if(m_renderer.getShaderEnvironment().mValid)
	{
		Primitive prim = getPrimitives();

		switch(prim)
		{
		case PRIMITIVE_LINES:
			{				
				sceGxmSetCullMode( m_renderer.getContext(), SCE_GXM_CULL_NONE);

				sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_LINE );
				sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_LINE );
				/*sceGxmSetFrontDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );
				sceGxmSetBackDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );*/

				SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES , SCE_GXM_INDEX_FORMAT_U16, m_indexBuffer->lock(),m_numIndices); 

				RENDERER_ASSERT(err == SCE_OK, "Failed to render!");				

				sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
				sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
				//sceGxmSetFrontDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );
				//sceGxmSetBackDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );
			}
			break;
		case PRIMITIVE_TRIANGLES:
			{							
				SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_indexBuffer->lock(),m_numIndices);

				if(err != SCE_OK)
				{
					RENDERER_ASSERT(err == SCE_OK, "Failed to render!");	
				}			
			}
			break;
		case PRIMITIVE_POINT_SPRITES:
			{				
				sceGxmSetFrontDepthWriteEnable(m_renderer.getContext(), SCE_GXM_DEPTH_WRITE_DISABLED);
				sceGxmSetFrontDepthFunc( m_renderer.getContext(), SCE_GXM_DEPTH_FUNC_LESS_EQUAL);

				sceGxmSetFrontPolygonMode(m_renderer.getContext(),SCE_GXM_POLYGON_MODE_POINT_01UV);

				SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_POINTS, SCE_GXM_INDEX_FORMAT_U16 , m_indexBuffer->lock(), m_numIndices);

				if(err != SCE_OK)
				{
					RENDERER_ASSERT(err == SCE_OK, "Failed to render!");
				}

				sceGxmSetFrontPolygonMode(m_renderer.getContext(),SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);

				sceGxmSetFrontDepthWriteEnable(m_renderer.getContext(), SCE_GXM_DEPTH_WRITE_ENABLED);
				sceGxmSetFrontDepthFunc( m_renderer.getContext(), SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
			}
			break;
		}
		m_renderer.getShaderEnvironment().mValid = false;
	}

}

bool GXMRendererMesh::willRender() const
{
	if(m_indexBuffer)
		return true;

	if(m_primitives == PRIMITIVE_TRIANGLES || m_primitives == PRIMITIVE_LINES)
	{
		if(m_numVertices > 0)
			return true;
	}

	return false;
}

void GXMRendererMesh::renderVertices(PxU32 numVertices, RendererMaterial *material) const
{
	Primitive prim = getPrimitives();

	switch(prim)
	{
	case PRIMITIVE_TRIANGLES:
		{
			void* currentRendererIndexBuffer = NULL;

			if((mCurrentRendererIndexBufferSize + sizeof(PxU16)*numVertices) > mMaxVerts*sizeof(PxU16))
			{
				mCurrentRendererIndexBufferSize = 0;
			}	

			currentRendererIndexBuffer = static_cast<PxU8*>(mRendererIndexBuffer) + mCurrentRendererIndexBufferSize;
			mCurrentRendererIndexBufferSize += sizeof(PxU16)*numVertices;

			PxU16* ib = (PxU16*) currentRendererIndexBuffer;

			for (PxU16 i = 0; i < numVertices; i++)
			{
				ib[i] = i;
			}

			SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, currentRendererIndexBuffer,numVertices);

			if(err != SCE_OK)
			{
				RENDERER_ASSERT(err == SCE_OK, "Failed to render!");	
			}	
		}
		break;
	case PRIMITIVE_LINES:
		{
			void* currentRendererIndexBuffer = NULL;

			if((mCurrentRendererIndexBufferSize + sizeof(PxU16)*numVertices) > mMaxVerts*sizeof(PxU16))
			{
				mCurrentRendererIndexBufferSize = 0;
			}	

			currentRendererIndexBuffer = static_cast<PxU8*>(mRendererIndexBuffer) + mCurrentRendererIndexBufferSize;
			mCurrentRendererIndexBufferSize += sizeof(PxU16)*numVertices;

			PxU16* ib = (PxU16*) currentRendererIndexBuffer;

			for (PxU16 i = 0; i < numVertices; i++)
			{
				ib[i] = i;
			}

			sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_LINE );
			sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_LINE );

			sceGxmSetRegionClip(m_renderer.getContext(), SCE_GXM_REGION_CLIP_NONE, 0, 0, 0, 0);

			SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, currentRendererIndexBuffer,numVertices);

			if(err != SCE_OK)
			{
				RENDERER_ASSERT(err == SCE_OK, "Failed to render!");	
			}	

			sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
			sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );

		}
		break;
	}



}

void GXMRendererMesh::renderIndicesInstanced(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat,RendererMaterial *material) const
{
	if(m_numInstances > 0)
	{
		SceGxmErrorCode err = sceGxmDrawInstanced(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES,	SCE_GXM_INDEX_FORMAT_U32, 
			m_indexBuffer->lock(), numIndices * m_numInstances, numIndices); 
		if(err != SCE_OK)
		{
			RENDERER_ASSERT(err == SCE_OK, "Failed to render!");
		}
	}
}

void GXMRendererMesh::renderVerticesInstanced(PxU32 numVertices,RendererMaterial *material) const
{
	//currently unsupported on PSP2
	RENDERER_ASSERT(0, "Cannot render vertices without indices?");
}

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
		
