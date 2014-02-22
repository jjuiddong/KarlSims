
#include "stdafx.h"
#include "RendererSkinModelShape.h"


using namespace SampleRenderer;

RendererSkinModelShape::RendererSkinModelShape(Renderer& renderer, const SBMMLoader &loader, Matrix44 *palette) :
	RendererShape(renderer)
,	m_loader(loader)
,	m_palette(palette)
,	m_physiq(NULL)
{
	//RENDERER_ASSERT(faces16 || faces32, "Needs either 16bit or 32bit indices.");
	if (0 < loader.m.size)
		m_physiq = &loader.m.pMesh->physiq;

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
	RET(m_loader.m.size <= 0);
	RET(!m_palette);

	PxU32 stride = 0;
	void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
	void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
	
	SVtxNormTex *srcVerts = m_loader.m.pMesh->vnt.pV;
	for (int i=0; i < m_physiq->size; ++i)
	{
		const int idx = m_physiq->p[ i].vtx;
		const int offset = idx * stride;
		Vector3 *pVtx = (Vector3*)(((PxU8*)positions) + offset);
		*pVtx = Vector3(0,0,0);

		for( int k=0; k < m_physiq->p[ i].size; ++k )
		{
			SWeight *w = &m_physiq->p[ i].w[ k];
			Vector3 v = *(Vector3*)&srcVerts[ idx] * m_palette[ w->bone];
			*pVtx += v * w->weight;
		}
	}

	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
}
