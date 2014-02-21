
#include "stdafx.h"
#include "RendererSkinModelShape.h"


using namespace SampleRenderer;

RendererSkinModelShape::RendererSkinModelShape(Renderer& renderer, const SBMMLoader &loader, Matrix44 *palette) :
	RendererShape(renderer)
,	m_loader(loader)
,	m_palette(palette)
{
	//RENDERER_ASSERT(faces16 || faces32, "Needs either 16bit or 32bit indices.");
	PxU32 numVerts = loader.m.pMesh->vnt.size;
	SVtxNormTex *verts = loader.m.pMesh->vnt.pV;

	RendererVertexBufferDesc vbdesc;
	vbdesc.hint = RendererVertexBuffer::HINT_STATIC;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL]    = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
	vbdesc.maxVertices = numVerts;
	m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
	RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");
	if(m_vertexBuffer)
	{
		PxU32 positionStride = 0;
		void* vertPositions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);

		PxU32 normalStride = 0;
		void* vertNormals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride);

		PxU32 uvStride = 0;
		void* vertUVs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride);

		if(vertPositions && vertNormals && vertUVs)
		{
			for(PxU32 i=0; i<numVerts; i++)
			{
				memcpy(vertPositions, &verts[ i].v, sizeof(verts[ i].v));
				memcpy(vertNormals, &verts[ i].n, sizeof(verts[ i].n));
				memcpy(vertUVs, &verts[ i].tu, sizeof(PxReal)*2);

				vertPositions = (void*)(((PxU8*)vertPositions) + positionStride);
				vertNormals = (void*)(((PxU8*)vertNormals) + normalStride);
				vertUVs = (void*)(((PxU8*)vertUVs) + uvStride);
			}
		}
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	}

	const PxU32 numIndices = loader.m.pMesh->i.size*3;
	const PxU32 numFaces = numIndices/3;
	SVector3s *faces16 = loader.m.pMesh->i.pI;

	RendererIndexBufferDesc ibdesc;
	ibdesc.hint       = RendererIndexBuffer::HINT_STATIC;
	//ibdesc.format     = faces16 ? RendererIndexBuffer::FORMAT_UINT16 : RendererIndexBuffer::FORMAT_UINT32;
	ibdesc.format     = RendererIndexBuffer::FORMAT_UINT16;
	ibdesc.maxIndices = numIndices;
	m_indexBuffer = m_renderer.createIndexBuffer(ibdesc);
	RENDERER_ASSERT(m_indexBuffer, "Failed to create Index Buffer.");
	if(m_indexBuffer)
	{
		PxU16* indices = (PxU16*)m_indexBuffer->lock();
		if(indices)
		{
			//if(flipWinding)
			//{
			//	for(PxU32 i=0;i<numFaces;i++)
			//	{
			//		indices[i*3+0] = faces16[i*3+0];
			//		indices[i*3+1] = faces16[i*3+2];
			//		indices[i*3+2] = faces16[i*3+1];
			//	}
			//}
			//else
			{
				memcpy(indices, faces16, sizeof(*faces16)*numFaces);
			}
		}
		m_indexBuffer->unlock();
	}

	if(m_vertexBuffer && m_indexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives       = RendererMesh::PRIMITIVE_TRIANGLES;
		meshdesc.vertexBuffers    = &m_vertexBuffer;
		meshdesc.numVertexBuffers = 1;
		meshdesc.firstVertex      = 0;
		meshdesc.numVertices      = numVerts;
		meshdesc.indexBuffer      = m_indexBuffer;
		meshdesc.firstIndex       = 0;
		meshdesc.numIndices       = numIndices;
		m_mesh = m_renderer.createMesh(meshdesc);
		RENDERER_ASSERT(m_mesh, "Failed to create Mesh.");
	}
}

RendererSkinModelShape::~RendererSkinModelShape()
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_mesh);
}


void RendererSkinModelShape::ApplyPalette()
{
	PxU32 stride = 0;
	void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
	void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
	void *bones = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride);

	const PxU32 paletteSize = m_TmPalette.size();
	const PxU32 numVerts = m_vertexBuffer->getMaxVertices();
	for (PxU32 i=0; i < numVerts; ++i)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * i));
		PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (stride * i));
		const PxU32 &bidx  =  *(PxU32*)(((PxU8*)bones) + (stride * i));
		if (bidx >= paletteSize)
			continue;

#ifdef _DEBUG
		if (!m_TmPalette[ bidx].isSane()) continue;
#endif

		PxTransform tm0 = m_TmPalette[ bidx] * PxTransform(m_SrcVertex[ i]);
		PxTransform tm1 = PxTransform(m_TmPalette[ bidx].q) * PxTransform(m_SrcNormal[ i]);

		p = tm0.p;
		n = tm1.p;
	}

	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);
}
