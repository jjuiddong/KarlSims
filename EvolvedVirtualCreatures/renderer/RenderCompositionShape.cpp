
#include "stdafx.h"
#include "RenderCompositionShape.h"

#include <Renderer.h>

#include <RendererVertexBuffer.h>
#include <RendererVertexBufferDesc.h>

#include <RendererIndexBuffer.h>
#include <RendererIndexBufferDesc.h>

#include <RendererMesh.h>
#include <RendererMeshDesc.h>

#include <RendererMemoryMacros.h>
#include <boost/foreach.hpp>


using namespace SampleRenderer;

typedef struct
{
	PxVec3 positions[4];
	PxVec3 normal;
} BoxFace;

static const BoxFace g_BoxFaces1[] =
{
	{ // Z+
		{PxVec3(-1,-1, 1), PxVec3(-1, 1, 1), PxVec3( 1, 1, 1), PxVec3( 1,-1, 1)},
			PxVec3( 0, 0, 1)
	},
	{ // X+
		{PxVec3( 1,-1, 1), PxVec3( 1, 1, 1), PxVec3( 1, 1,-1), PxVec3( 1,-1,-1)},
			PxVec3( 1, 0, 0)
		},
		{ // Z-
			{PxVec3( 1,-1,-1), PxVec3( 1, 1,-1), PxVec3(-1, 1,-1), PxVec3(-1,-1,-1)},
				PxVec3( 0, 0,-1)
		},
		{ // X-
			{PxVec3(-1,-1,-1), PxVec3(-1, 1,-1), PxVec3(-1, 1, 1), PxVec3(-1,-1, 1)},
				PxVec3(-1, 0, 0)
			},
			{ // Y+
				{PxVec3(-1, 1, 1), PxVec3(-1, 1,-1), PxVec3( 1, 1,-1), PxVec3( 1, 1, 1)},
					PxVec3( 0, 1, 0)
			},
			{ // Y-
				{PxVec3(-1,-1,-1), PxVec3(-1,-1, 1), PxVec3( 1,-1, 1), PxVec3( 1,-1,-1)},
					PxVec3( 0,-1, 0)
				}
};

static const PxVec3 g_BoxUVs1[] =
{
	PxVec3(0,1,0), PxVec3(0,0,0),
	PxVec3(1,0,0), PxVec3(1,1,0),
};

namespace physx
{
	static PxVec3 operator*(const PxVec3 &a, const PxVec3 &b)
	{
		return PxVec3(a.x*b.x, a.y*b.y, a.z*b.z);
	}
}

RendererCompositionShape::RendererCompositionShape(Renderer &renderer, 
	const PxVec3& pos1, const PxVec3& extents1, 
	const PxVec3& pos2, const PxVec3& extents2,
	const PxReal* userUVs) :
	RendererShape(renderer)
{
	const PxU32 numVerts = 24;
	const PxU32 numIndices = 36;

	RendererVertexBufferDesc vbdesc;
	vbdesc.hint = RendererVertexBuffer::HINT_STATIC;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL] = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
	vbdesc.maxVertices = numVerts;
	m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
	RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");
	if(m_vertexBuffer)
	{
		PxU32 positionStride = 0;
		void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);
		PxU32 normalStride = 0;
		void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride);
		PxU32 uvStride = 0;
		void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride);
		if(positions && normals && uvs)
		{
			for(PxU32 i=0; i<6; i++)
			{
				const BoxFace &bf = g_BoxFaces1[i];
				for(PxU32 j=0; j<4; j++)
				{
					PxVec3 &p  = *(PxVec3*)positions; 
					positions = ((PxU8*)positions) + positionStride;

					PxVec3 &n  = *(PxVec3*)normals;   
					normals   = ((PxU8*)normals)   + normalStride;

					PxF32 *uv  =  (PxF32*)uvs;        
					uvs       = ((PxU8*)uvs)       + uvStride;

					n = bf.normal;
					p = bf.positions[j] * extents1;
					if(userUVs)
					{
						uv[0] = *userUVs++;
						uv[1] = *userUVs++;
					}
					else
					{
						uv[0] = g_BoxUVs1[j].x;
						uv[1] = g_BoxUVs1[j].y;
					}
				}
			}
		}
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	}

	RendererIndexBufferDesc ibdesc;
	ibdesc.hint       = RendererIndexBuffer::HINT_STATIC;
	ibdesc.format     = RendererIndexBuffer::FORMAT_UINT16;
	ibdesc.maxIndices = numIndices;
	m_indexBuffer = m_renderer.createIndexBuffer(ibdesc);
	RENDERER_ASSERT(m_indexBuffer, "Failed to create Index Buffer.");
	if(m_indexBuffer)
	{
		PxU16 *indices = (PxU16*)m_indexBuffer->lock();
		if(indices)
		{
			for(PxU8 i=0; i<6; i++)
			{
				const PxU16 base = i*4;
				*(indices++) = base+0;
				*(indices++) = base+1;
				*(indices++) = base+2;
				*(indices++) = base+0;
				*(indices++) = base+2;
				*(indices++) = base+3;
			}
		}
		m_indexBuffer->unlock();
	}

	if (m_vertexBuffer && m_indexBuffer)
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

RendererCompositionShape::~RendererCompositionShape(void)
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_mesh);
}

RendererCompositionShape::RendererCompositionShape(Renderer &renderer, RendererShape *shape0) :
	RendererShape(renderer)
{
	GenerateCompositionShape(shape0);
}


/**
 @brief 
 @date 2014-01-02
*/
RendererCompositionShape::RendererCompositionShape(Renderer &renderer, 
	RendererShape *shape0, const PxTransform &tm0, 
	RendererShape *shape1, const PxTransform &tm1, 
	const PxReal* uvs0) :
	RendererShape(renderer)
{
	GenerateCompositionShape(shape0, tm0, shape1, tm1, 24, 24);

	RendererMesh *mesh0 = shape0->getMesh();
	RendererMesh *mesh1 = shape1->getMesh();
	
	SampleRenderer::RendererVertexBuffer **vtx0 = mesh0->getVertexBuffersEdit();
	SampleRenderer::RendererIndexBuffer *idx0 = mesh0->getIndexBufferEdit();
	const PxU32 numVtxBuff0 = mesh0->getNumVertexBuffers();

	SampleRenderer::RendererVertexBuffer **vtx1 = mesh1->getVertexBuffersEdit();
	SampleRenderer::RendererIndexBuffer *idx1 = mesh1->getIndexBufferEdit();
	const PxU32 numVtxBuff1 = mesh1->getNumVertexBuffers();

	if ((numVtxBuff0 <= 0) || (numVtxBuff1 <= 0))
		return;

	PxU32 positionStride0 = 0;
	void *positions0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride0);
	PxU32 normalStride0 = 0;
	void *normals0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride0);

	PxU32 positionStride1 = 0;
	void *positions1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride1);
	PxU32 normalStride1 = 0;
	void *normals1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride1);
	
	const PxU32 idx0Size = idx0->getMaxIndices();
	const PxU32 idx1Size = idx1->getMaxIndices();
	PxU16 *indices0 = (PxU16*)idx0->lock();
	PxU16 *indices1 = (PxU16*)idx1->lock();

	if (indices0 && positions0 && normals0 && indices1 && positions1 && normals1)
	{
		set<PxU16> vtxIdx0, vtxIdx1; 
		std::pair<int,int> mostCloseFace0, mostCloseFace1;
		FindMostCloseFace(positions0, positionStride0, normals0, normalStride0, indices0, idx0Size,
			positions1, positionStride1, normals1, normalStride1, indices1, idx1Size,
			mostCloseFace0, mostCloseFace1, vtxIdx0, vtxIdx1);

		// generate vertex buffer
		PxU32 positionStride = 0;
		void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);
		PxU32 normalStride = 0;
		void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride);
		PxU32 uvStride = 0;
		void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride);

		PxU16 *indices = (PxU16*)m_indexBuffer->lock();
		const PxU32 numVtx0 = vtx0[0]->getMaxVertices();
		const PxU32 numVtx1 = vtx1[0]->getMaxVertices();
		PxU32 startIndexIdx = idx0Size + idx1Size;
		PxU32 startVtxIdx = numVtx0 + numVtx1;

		PxVec3 center;
		CalculateCenterPoint( mostCloseFace0, mostCloseFace1,
			positions0, positionStride0, normals0, normalStride0, indices0, idx0Size,
			positions1, positionStride1, normals1, normalStride1, indices1, idx1Size,
			center );

		GenerateBoxFromCloseVertex( vtxIdx0, vtxIdx1, center,
			positions0, positionStride0, normals0, normalStride0, indices0, idx0Size,
			positions1, positionStride1, normals1, normalStride1, indices1, idx1Size,
			positions, positionStride, startVtxIdx, normals, normalStride, indices, startIndexIdx );

		m_indexBuffer->unlock();
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	}


	idx0->unlock();
	idx1->unlock();
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);

}


/**
 @brief 
 @date 2014-01-05
*/
void RendererCompositionShape::CalculateCenterPoint( std::pair<int,int> closeFace0, std::pair<int,int> closeFace1,
	void *positions0, PxU32 positionStride0, void *normals0, PxU32 normalStride0,
	PxU16 *indices0, PxU32 idx0Size,
	void *positions1, PxU32 positionStride1, void *normals1, PxU32 normalStride1,
	PxU16 *indices1, PxU32 idx1Size,
	OUT PxVec3 &out )
{
	PxVec3 center;
	{
		int minFaceIdx0 = closeFace0.first;
		int minFaceIdx1 = closeFace0.second;

		const PxU16 vidx00 = indices0[ minFaceIdx0];
		const PxU16 vidx01 = indices0[ minFaceIdx0+1];
		const PxU16 vidx02 = indices0[ minFaceIdx0+2];

		PxVec3 &p00 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx00));
		PxVec3 &p01 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx01));
		PxVec3 &p02 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx02));

		PxVec3 &n00 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx00));
		PxVec3 &n01 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx01));
		PxVec3 &n02 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx02));

		const PxU16 vidx10 = indices1[ minFaceIdx1];
		const PxU16 vidx11 = indices1[ minFaceIdx1+1];
		const PxU16 vidx12 = indices1[ minFaceIdx1+2];

		PxVec3 &p10 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx10));
		PxVec3 &p11 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx11));
		PxVec3 &p12 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx12));

		center += (p00 +p01 + p02 + p10 + p11 + p12);
	}

	{
		int minFaceIdx0 = closeFace1.first;
		int minFaceIdx1 = closeFace1.second;

		const PxU16 vidx00 = indices0[ minFaceIdx0];
		const PxU16 vidx01 = indices0[ minFaceIdx0+1];
		const PxU16 vidx02 = indices0[ minFaceIdx0+2];

		PxVec3 &p00 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx00));
		PxVec3 &p01 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx01));
		PxVec3 &p02 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx02));

		PxVec3 &n00 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx00));
		PxVec3 &n01 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx01));
		PxVec3 &n02 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx02));

		const PxU16 vidx10 = indices1[ minFaceIdx1];
		const PxU16 vidx11 = indices1[ minFaceIdx1+1];
		const PxU16 vidx12 = indices1[ minFaceIdx1+2];

		PxVec3 &p10 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx10));
		PxVec3 &p11 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx11));
		PxVec3 &p12 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx12));

		center += (p00 +p01 + p02 + p10 + p11 + p12);
	}

	center /= 12.f;
	out = center;
}


/**
 @brief 
 @date 2014-01-03
*/
void RendererCompositionShape::GenerateCompositionShape( RendererShape *shape0, const PxTransform &tm0, 
	RendererShape *shape1, const PxTransform &tm1, 
	const int additionalVtxBuffSize, const int additionalIdxBuffSize )
{
	RendererMesh *mesh0 = shape0->getMesh();
	RendererMesh *mesh1 = shape1->getMesh();

	SampleRenderer::RendererVertexBuffer **vtx0 = mesh0->getVertexBuffersEdit();
	SampleRenderer::RendererIndexBuffer *idx0 = mesh0->getIndexBufferEdit();
	const PxU32 numVtxBuff0 = mesh0->getNumVertexBuffers();

	SampleRenderer::RendererVertexBuffer **vtx1 = mesh1->getVertexBuffersEdit();
	SampleRenderer::RendererIndexBuffer *idx1 = mesh1->getIndexBufferEdit();
	const PxU32 numVtxBuff1 = mesh1->getNumVertexBuffers();

	if ((numVtxBuff0 <= 0) || (numVtxBuff1 <= 0))
		return;

	PxU32 positionStride0 = 0;
	void *positions0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride0);
	PxU32 normalStride0 = 0;
	void *normals0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride0);
	PxU32 uvStride0 = 0;
	void *uvs0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride0);

	PxU32 positionStride1 = 0;
	void *positions1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride1);
	PxU32 normalStride1 = 0;
	void *normals1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride1);
	PxU32 uvStride1 = 0;
	void *uvs1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride1);


	PxU16 *indices0 = (PxU16*)idx0->lock();
	PxU16 *indices1 = (PxU16*)idx1->lock();
	PxU32 idx0Size = idx0->getMaxIndices();
	PxU32 idx1Size = idx1->getMaxIndices();

	const PxU32 numVtx0 = vtx0[0]->getMaxVertices();
	const PxU32 numVtx1 = vtx1[0]->getMaxVertices();

	const PxU32 numVerts =  numVtx0 + numVtx1 + additionalVtxBuffSize;
	const PxU32 numIndices = idx0Size + idx1Size + additionalIdxBuffSize;

	if (indices0 && positions0 && normals0 && indices1 && positions1 && normals1)
	{
		RendererVertexBufferDesc vbdesc;
		vbdesc.hint = RendererVertexBuffer::HINT_STATIC;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL] = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
		vbdesc.maxVertices = numVerts;
		m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
		RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");

		PxU32 positionStride = 0;
		void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);
		PxU32 normalStride = 0;
		void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride);
		PxU32 uvStride = 0;
		void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride);

		if (m_vertexBuffer)
		{
			// copy shape0 to current
			for (PxU32 i=0; i < numVtx0; ++i)
			{
				PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (positionStride * i));
				PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (normalStride * i));
				PxF32 *uv  =  (PxF32*)(((PxU8*)uvs) + (uvStride * i));

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * i));
				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * i));
				PxF32 *uv0  =  (PxF32*)(((PxU8*)uvs0) + (uvStride0 * i));

				p = p0;
				n = n0;
				uv[ 0] = uv0[ 0];
				uv[ 1] = uv0[ 1];
			}

			// copy shape1 to current
			for (PxU32 i=0; i < numVtx1; ++i)
			{
				PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (positionStride * (i+numVtx0)));
				PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (normalStride * (i+numVtx0)));
				PxF32 *uv  =  (PxF32*)(((PxU8*)uvs) + (uvStride * (i+numVtx0)));

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * i));
				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals1) + (normalStride1 * i));
				PxF32 *uv0  =  (PxF32*)(((PxU8*)uvs1) + (uvStride1 * i));

				p = p0;
				//p += PxVec3(1,0,0);

				n = n0;
				uv[ 0] = uv0[ 0];
				uv[ 1] = uv0[ 1];
			}
		}

		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);



		// copy index buffer
		RendererIndexBufferDesc ibdesc;
		ibdesc.hint = RendererIndexBuffer::HINT_STATIC;
		ibdesc.format = RendererIndexBuffer::FORMAT_UINT16;
		ibdesc.maxIndices = numIndices;
		m_indexBuffer = m_renderer.createIndexBuffer(ibdesc);
		RENDERER_ASSERT(m_indexBuffer, "Failed to create Index Buffer.");
		if (m_indexBuffer)
		{
			PxU16 *indices = (PxU16*)m_indexBuffer->lock();
			if (indices)
			{
				for(PxU16 i=0; i<idx0Size; i++)
					*(indices++) = indices0[ i];
				for(PxU16 i=0; i<idx1Size; i++)
					*(indices++) = numVtx0+indices1[ i];
			}

			m_indexBuffer->unlock();
		}
	}

	idx0->unlock();
	idx1->unlock();
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);

	if (m_vertexBuffer && m_indexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives = RendererMesh::PRIMITIVE_TRIANGLES;
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


/**
 @brief 
 @date 2014-01-04
*/
void RendererCompositionShape::GenerateTriangleFrom4Vector( void *positions, PxU32 positionStride, 
	void *normals, PxU32 normalStride, PxU32 startVtxIdx, PxU16 *indices, PxU32 startIndexIdx,
	const PxVec3 &center, PxVec3 v0, PxVec3 v1, PxVec3 v2, PxVec3 v3 )
{
	// test cw
	{
		PxVec3 n0 = v2 - v0;
		n0.normalize();
		PxVec3 n1 = v3 - v0;
		n1.normalize();
		PxVec3 n = n0.cross(n1);
		n.normalize();

		PxVec3 faceCenter = v0 + v2 + v3;
		faceCenter /= 3.f;
		PxVec3 cn = center - faceCenter;
		cn.normalize();
		const float d = n.dot(cn);
		if (d >= 0)
		{ // cw
		}
		else
		{ // ccw
			// switching
			PxVec3 tmp = v3;
			v3 = v2;
			v2 = tmp;
		}
	}

	// triangle 1
	{
		PxU32 face1VtxIdx = startVtxIdx;

		PxVec3 n0 = v1 - v0;
		n0.normalize();
		PxVec3 n1 = v2 - v0;
		n1.normalize();
		PxVec3 n = n0.cross(n1);
		n.normalize();
		PxVec3 faceCenter = v0 + v1 + v2;
		faceCenter /= 3.f;
		PxVec3 cn = center - faceCenter;
		cn.normalize();

		const float d = n.dot(cn);
		if (d >= 0)
		{ // cw
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v1;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * face1VtxIdx)) = -n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face1VtxIdx+1))) = -n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face1VtxIdx+2))) = -n;
		}
		else
		{ // ccw
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v1;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * face1VtxIdx)) = n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face1VtxIdx+1))) = n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face1VtxIdx+2))) = n;
		}

		indices[ startIndexIdx++] = face1VtxIdx;
		indices[ startIndexIdx++] = face1VtxIdx+1;
		indices[ startIndexIdx++] = face1VtxIdx+2;
	}


	// triangle2
	{
		PxU32 face2VtxIdx = startVtxIdx;

		PxVec3 n0 = v2 - v0;
		n0.normalize();
		PxVec3 n1 = v3 - v0;
		n1.normalize();
		PxVec3 n = n0.cross(n1);
		n.normalize();
		PxVec3 faceCenter = v0 + v2 + v3;
		faceCenter /= 3.f;
		PxVec3 cn = center - faceCenter;
		cn.normalize();

		const float d = n.dot(cn);
		if (d >= 0)
		{ // cw
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v3;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * face2VtxIdx)) = -n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face2VtxIdx+1))) = -n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face2VtxIdx+2))) = -n;
		}
		else
		{ // ccw
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v3;
			*(PxVec3*)(((PxU8*)positions) + (positionStride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * face2VtxIdx)) = n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face2VtxIdx+1))) = n;
			*(PxVec3*)(((PxU8*)normals) + (normalStride * (face2VtxIdx+2))) = n;
		}

		indices[ startIndexIdx++] = face2VtxIdx;
		indices[ startIndexIdx++] = face2VtxIdx+1;
		indices[ startIndexIdx++] = face2VtxIdx+2;
	}
}


/**
 @brief 
 @date 2014-01-05
*/
void RendererCompositionShape::FindMostCloseFace( 
	void *positions0, PxU32 positionStride0, void *normals0, PxU32 normalStride0,
	PxU16 *indices0, PxU32 idx0Size,
	void *positions1, PxU32 positionStride1, void *normals1, PxU32 normalStride1,
	PxU16 *indices1, PxU32 idx1Size,
	OUT std::pair<int,int> &closeFace0, OUT std::pair<int,int> &closeFace1,
	OUT set<PxU16> &vtx0, OUT set<PxU16> &vtx1 )
{
	int foundCount = 0;
	set<int> checkV0, checkV1;

	while (foundCount < 2)
	{
		float minLen = 100000.f;
		int minFaceIdx0 = -1;
		int minFaceIdx1 = -1;

		// find most close face
		for (PxU16 i=0; i<idx0Size; i+=3)
		{
			if (checkV0.find(i) != checkV0.end())
				continue; // already exist face index

			PxVec3 center0;
			PxVec3 center0Normal;
			{
				const PxU16 vidx0 = indices0[ i];
				const PxU16 vidx1 = indices0[ i+1];
				const PxU16 vidx2 = indices0[ i+2];

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx0));
				PxVec3 &p1 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx1));
				PxVec3 &p2 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx2));

				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx0));
				PxVec3 &n1 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx1));
				PxVec3 &n2 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * vidx2));

				center0 = p0 + p1 + p2;
				center0 /= 3.f;

				center0Normal = n0;
			}

			for (PxU16 k=0; k<idx1Size; k+=3)
			{
				if (checkV1.find(k) != checkV1.end())
					continue; // already exist face index

				PxVec3 center1;
				PxVec3 center1Normal;
				{
					const PxU16 vidx0 = indices1[ k];
					const PxU16 vidx1 = indices1[ k+1];
					const PxU16 vidx2 = indices1[ k+2];

					PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx0));
					PxVec3 &p1 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx1));
					PxVec3 &p2 = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx2));

					PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals1) + (normalStride1 * vidx0));
					PxVec3 &n1 = *(PxVec3*)(((PxU8*)normals1) + (normalStride1 * vidx1));
					PxVec3 &n2 = *(PxVec3*)(((PxU8*)normals1) + (normalStride1 * vidx2));

					center1 = p0 + p1 + p2;
					center1 /= 3.f;

					center1Normal = n0;
				}

				PxVec3 len = center0 - center1;
				if ((minLen > len.magnitude()) && (center0Normal.dot(center1Normal) < 0) )
				{
					minFaceIdx0 = i;
					minFaceIdx1 = k;
					minLen = len.magnitude();	
				}
			}
		}

		checkV0.insert(minFaceIdx0);
		checkV1.insert(minFaceIdx1);

		if (foundCount == 0)
		{
			closeFace0 = std::pair<int,int>(minFaceIdx0, minFaceIdx1);
		}
		else
		{
			closeFace1 = std::pair<int,int>(minFaceIdx0, minFaceIdx1);
		}

		++foundCount;
	}


	vtx0.insert( indices0[ closeFace0.first] );
	vtx0.insert( indices0[ closeFace0.first+1] );
	vtx0.insert( indices0[ closeFace0.first+2] );
	vtx0.insert( indices0[ closeFace1.first] );
	vtx0.insert( indices0[ closeFace1.first+1] );
	vtx0.insert( indices0[ closeFace1.first+2] );

	vtx1.insert( indices1[ closeFace0.second] );
	vtx1.insert( indices1[ closeFace0.second+1] );
	vtx1.insert( indices1[ closeFace0.second+2] );
	vtx1.insert( indices1[ closeFace1.second] );
	vtx1.insert( indices1[ closeFace1.second+1] );
	vtx1.insert( indices1[ closeFace1.second+2] );
}


/**
 @brief 
 @date 2014-01-05
*/
void RendererCompositionShape::GenerateBoxFromCloseVertex(
	const set<PxU16> &vtxIdx0, const set<PxU16> &vtxIdx1,
	PxVec3 center,
	void *positions0, PxU32 positionStride0, void *normals0, PxU32 normalStride0,
	PxU16 *indices0, PxU32 idx0Size,
	void *positions1, PxU32 positionStride1, void *normals1, PxU32 normalStride1,
	PxU16 *indices1, PxU32 idx1Size,
	void *positions, PxU32 positionStride, PxU32 startVtxIdx,
	void *normals, PxU32 normalStride, 
	PxU16 *indices, PxU32 startIndexIdx )
{
	// gen face
	vector<PxVec3> v0, v1;
	BOOST_FOREACH (const auto vidx, vtxIdx0)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * vidx));
		v0.push_back(p);
	}

	BOOST_FOREACH (auto vidx, vtxIdx1)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions1) + (positionStride1 * vidx));
		v1.push_back(p);
	}

	vector<int> line(6);
	for (unsigned int i=0; i < v0.size(); ++i)
	{
		float minLen = 10000;
		for (unsigned int k=0; k < v1.size(); ++k)
		{
			PxVec3 v = v0[ i] - v1[ k];
			const float len = v.magnitude();
			if (minLen > len)
			{
				line[ i] = k;
				minLen = len;
			}
		}
	}

	GenerateTriangleFrom4Vector(positions, positionStride, normals, normalStride, startVtxIdx, indices, startIndexIdx,
		center, v0[ 0], v0[ 1], v1[ line[ 0]], v1[ line[ 1]] );
	startVtxIdx += 6;
	startIndexIdx += 6;

	GenerateTriangleFrom4Vector(positions, positionStride, normals, normalStride, startVtxIdx, indices, startIndexIdx,
		center, v0[ 1], v0[ 2], v1[ line[ 1]], v1[ line[ 2]] );
	startVtxIdx += 6;
	startIndexIdx += 6;

	GenerateTriangleFrom4Vector(positions, positionStride, normals, normalStride, startVtxIdx, indices, startIndexIdx,
		center, v0[ 2], v0[ 3], v1[ line[ 2]], v1[ line[ 3]] );
	startVtxIdx += 6;
	startIndexIdx += 6;

	GenerateTriangleFrom4Vector(positions, positionStride, normals, normalStride, startVtxIdx, indices, startIndexIdx,
		center, v0[ 3], v0[ 0], v1[ line[ 3]], v1[ line[ 0]] );
	startVtxIdx += 6;
	startIndexIdx += 6;

}


/**
 @brief 
 @date 2014-01-03
*/
void RendererCompositionShape::GenerateCompositionShape( RendererShape *shape0 )
{
	RendererMesh *mesh0 = shape0->getMesh();

	SampleRenderer::RendererVertexBuffer **vtx0 = mesh0->getVertexBuffersEdit();
	SampleRenderer::RendererIndexBuffer *idx0 = mesh0->getIndexBufferEdit();
	const PxU32 numVtxBuff0 = mesh0->getNumVertexBuffers();

	if (numVtxBuff0 <= 0)
		return;

	PxU32 positionStride0 = 0;
	void *positions0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride0);
	PxU32 normalStride0 = 0;
	void *normals0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride0);
	PxU32 uvStride0 = 0;
	void *uvs0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride0);

	PxU16 *indices0 = (PxU16*)idx0->lock();
	PxU32 idx0Size = idx0->getMaxIndices();

	const PxU32 numVtx0 = vtx0[0]->getMaxVertices();
	const PxU32 numVerts =  numVtx0;
	const PxU32 numIndices = idx0Size;

	if (indices0 && positions0 && normals0)
	{
		RendererVertexBufferDesc vbdesc;
		vbdesc.hint = RendererVertexBuffer::HINT_STATIC;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL] = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
		vbdesc.maxVertices = numVerts;
		m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
		RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");

		PxU32 positionStride = 0;
		void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);
		PxU32 normalStride = 0;
		void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride);
		PxU32 uvStride = 0;
		void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride);

		if (m_vertexBuffer)
		{
			// copy shape0 to current
			for (PxU32 i=0; i < numVtx0; ++i)
			{
				PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (positionStride * i));
				PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (normalStride * i));
				PxF32 *uv  =  (PxF32*)(((PxU8*)uvs) + (uvStride * i));

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions0) + (positionStride0 * i));
				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals0) + (normalStride0 * i));
				PxF32 *uv0  =  (PxF32*)(((PxU8*)uvs0) + (uvStride0 * i));

				p = p0;
				n = n0;
				uv[ 0] = uv0[ 0];
				uv[ 1] = uv0[ 1];
			}
		}

		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);



		// copy index buffer
		RendererIndexBufferDesc ibdesc;
		ibdesc.hint = RendererIndexBuffer::HINT_STATIC;
		ibdesc.format = RendererIndexBuffer::FORMAT_UINT16;
		ibdesc.maxIndices = numIndices;
		m_indexBuffer = m_renderer.createIndexBuffer(ibdesc);
		RENDERER_ASSERT(m_indexBuffer, "Failed to create Index Buffer.");
		if (m_indexBuffer)
		{
			PxU16 *indices = (PxU16*)m_indexBuffer->lock();
			if (indices)
			{
				for(PxU16 i=0; i<idx0Size; i++)
					*(indices++) = indices0[ i];
			}

			m_indexBuffer->unlock();
		}
	}

	idx0->unlock();
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);

	if (m_vertexBuffer && m_indexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives = RendererMesh::PRIMITIVE_TRIANGLES;
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
