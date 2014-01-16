
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
#include <RenderMaterial.h>


using namespace SampleRenderer;

RendererCompositionShape::~RendererCompositionShape(void)
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_mesh);
}

RendererCompositionShape::RendererCompositionShape(Renderer &renderer, const int paletteIndex, 
	const vector<PxTransform> &tmPalette, RendererShape *shape0, RenderMaterial *material0) :
	RendererShape(renderer)
,	m_PaletteIndex(paletteIndex)
,	m_TmPalette(tmPalette)
{
	GenerateCompositionShape(shape0, material0);
}


/**
 @brief 
 @date 2014-01-02
*/
RendererCompositionShape::RendererCompositionShape(Renderer &renderer, 
	const int parentShapeIndex, const int childShapeIndex, 
	const int paletteIndex, const vector<PxTransform> &tmPalette,
	RendererCompositionShape *shape0, const PxTransform &tm0, 
	RendererCompositionShape *shape1, const PxTransform &tm1, 
	const PxReal* uvs0) :
	RendererShape(renderer)
,	m_PaletteIndex(paletteIndex)
,	m_TmPalette(tmPalette)
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
	
	const PxU32 idx0Size = idx0->getMaxIndices();
	const PxU32 idx1Size = idx1->getMaxIndices();
	const PxU32 numVerts = m_vertexBuffer->getMaxVertices();

	// generate vertex buffer
	PxU32 stride = 0;
	void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
	void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
	void *colors = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_COLOR, stride);
	//void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, stride);
	void *bones = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride);
	PxU16 *indices = (PxU16*)m_indexBuffer->lock();

	set<PxU16> vtxIndices0, vtxIndices1; 
	std::pair<int,int> mostCloseFace0, mostCloseFace1;
	FindMostCloseFace(parentShapeIndex, childShapeIndex, positions, stride, normals, stride, 
		bones, stride, numVerts, indices, idx0Size, idx1Size, mostCloseFace0, mostCloseFace1, vtxIndices0, vtxIndices1);

	const PxU32 numVtx0 = vtx0[0]->getMaxVertices();
	const PxU32 numVtx1 = vtx1[0]->getMaxVertices();
	PxU32 startIndexIdx = idx0Size + idx1Size;
	PxU32 startVtxIdx = numVtx0 + numVtx1;

	PxVec3 center;
	CalculateCenterPoint( mostCloseFace0, mostCloseFace1,
		positions, stride, normals, stride, indices, center );

	vector<PxU16> outVtxIndices;
	GenerateBoxFromCloseVertex( vtxIndices0, vtxIndices1, center,
		positions, normals, bones, colors, stride, startVtxIdx, indices, startIndexIdx,
		outVtxIndices );

	CopyLocalVertexToSourceVtx(shape0, shape1, normals, stride, m_vertexBuffer->getMaxVertices(), 
		indices, m_indexBuffer->getMaxIndices(), startIndexIdx, outVtxIndices);

	// recovery original position
	ApplyTransform(positions, stride, normals, stride, m_vertexBuffer->getMaxVertices(), tm0);

	m_indexBuffer->unlock();
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_COLOR);
	//m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);
}


/**
 @brief 
 @date 2014-01-05
*/
void RendererCompositionShape::CalculateCenterPoint( 
	std::pair<int,int> closeFace0, std::pair<int,int> closeFace1,
	void *positions, PxU32 positionStride, void *normals, PxU32 normalStride,
	PxU16 *indices, OUT PxVec3 &out )
{
	PxVec3 center;
	{
		int minFaceIdx0 = closeFace0.first;
		int minFaceIdx1 = closeFace0.second;

		const PxU16 vidx00 = indices[ minFaceIdx0];
		const PxU16 vidx01 = indices[ minFaceIdx0+1];
		const PxU16 vidx02 = indices[ minFaceIdx0+2];

		PxVec3 &p00 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx00));
		PxVec3 &p01 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx01));
		PxVec3 &p02 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx02));

		PxVec3 &n00 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx00));
		PxVec3 &n01 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx01));
		PxVec3 &n02 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx02));

		const PxU16 vidx10 = indices[ minFaceIdx1];
		const PxU16 vidx11 = indices[ minFaceIdx1+1];
		const PxU16 vidx12 = indices[ minFaceIdx1+2];

		PxVec3 &p10 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx10));
		PxVec3 &p11 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx11));
		PxVec3 &p12 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx12));

		center += (p00 +p01 + p02 + p10 + p11 + p12);
	}

	{
		int minFaceIdx0 = closeFace1.first;
		int minFaceIdx1 = closeFace1.second;

		const PxU16 vidx00 = indices[ minFaceIdx0];
		const PxU16 vidx01 = indices[ minFaceIdx0+1];
		const PxU16 vidx02 = indices[ minFaceIdx0+2];

		PxVec3 &p00 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx00));
		PxVec3 &p01 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx01));
		PxVec3 &p02 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx02));

		PxVec3 &n00 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx00));
		PxVec3 &n01 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx01));
		PxVec3 &n02 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx02));

		const PxU16 vidx10 = indices[ minFaceIdx1];
		const PxU16 vidx11 = indices[ minFaceIdx1+1];
		const PxU16 vidx12 = indices[ minFaceIdx1+2];

		PxVec3 &p10 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx10));
		PxVec3 &p11 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx11));
		PxVec3 &p12 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx12));

		center += (p00 +p01 + p02 + p10 + p11 + p12);
	}

	center /= 12.f;
	out = center;
}


/**
 @brief 
 @date 2014-01-08
*/
void RendererCompositionShape::CalculateCenterPoint( const int boneIndex, void *positions, 
	PxU32 positionStride, void *bones, PxU32 boneStride, const PxU32 numVerts, OUT PxVec3 &out )
{
	int count=0;
	PxVec3 center;
	for (PxU32 i=0; i < numVerts; ++i)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (positionStride * i));
		PxU32 &b  =  *(PxU32*)(((PxU8*)bones) + (boneStride * i));
		if (boneIndex == b)
		{
			center +=p;
			++count;
		}
	}

	center /= (float)count;
	out = center;
}


/**
 @brief 
 @date 2014-01-03
 */
void RendererCompositionShape::GenerateCompositionShape( 
	RendererShape *shape0, const PxTransform &tm0, 
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

	PxU32 stride0 = 0;
	void *positions0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride0);
	void *colors0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_COLOR, stride0);
	void *normals0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride0);
	void *uvs0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, stride0);
	void *bones0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride0);

	PxU32 stride1 = 0;
	void *positions1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride1);
	void *colors1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_COLOR, stride1);
	void *normals1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride1);
	void *uvs1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, stride1);
	void *bones1 = vtx1[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride1);

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
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_COLOR]  = RendererVertexBuffer::FORMAT_COLOR_BGRA;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL] = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_BONEINDEX] = RendererVertexBuffer::FORMAT_UBYTE4;
		vbdesc.maxVertices = numVerts;
		m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
		RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");

		PxU32 stride = 0;
		void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
		void *colors = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_COLOR, stride);
		void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
		void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, stride);
		void *bones = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride);

		if (m_vertexBuffer)
		{
			// copy shape0 to current
			for (PxU32 i=0; i < numVtx0; ++i)
			{
				PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * i));
				PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (stride * i));
				PxF32 *uv  =  (PxF32*)(((PxU8*)uvs) + (stride * i));
				PxU32 &b  =  *(PxU32*)(((PxU8*)bones) + (stride * i));
				PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride * i));

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions0) + (stride0 * i));
				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals0) + (stride0 * i));
				PxF32 *uv0  =  (PxF32*)(((PxU8*)uvs0) + (stride0 * i));
				PxU32 &b0  =  *(PxU32*)(((PxU8*)bones0) + (stride0 * i));
				PxU32 &c0 = *(PxU32*)(((PxU8*)colors0) + (stride0 * i));

				PxTransform m = tm0.getInverse() * PxTransform(p0);
				p = m.p;
				n = n0;
				c = c0;
				uv[ 0] = uv0[ 0];
				uv[ 1] = uv0[ 1];
				b = b0;
			}

			// copy shape1 to current
			for (PxU32 i=0; i < numVtx1; ++i)
			{
				PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * (i+numVtx0)));
				PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (stride * (i+numVtx0)));
				PxF32 *uv  =  (PxF32*)(((PxU8*)uvs) + (stride * (i+numVtx0)));
				PxU32 &b  =  *(PxU32*)(((PxU8*)bones) + (stride * (i+numVtx0)));
				PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride  * (i+numVtx0)));

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions1) + (stride1 * i));
				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals1) + (stride1 * i));
				PxF32 *uv0  =  (PxF32*)(((PxU8*)uvs1) + (stride1 * i));
				PxU32 &b0  =  *(PxU32*)(((PxU8*)bones1) + (stride1 * i));
				PxU32 &c0 = *(PxU32*)(((PxU8*)colors1) + (stride1 * i));

				PxTransform m = tm1.getInverse() * PxTransform(p0);
				p = m.p;
				n = n0;
				c = c0;
				uv[ 0] = uv0[ 0];
				uv[ 1] = uv0[ 1];
				b = b0;
			}
		}


		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_COLOR);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);


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
				for(PxU32 i=0; i<idx0Size; i++)
					*(indices++) = indices0[ i];
				for(PxU32 i=0; i<idx1Size; i++)
					*(indices++) = numVtx0+indices1[ i];
			}

			m_indexBuffer->unlock();
		}
	}

	idx0->unlock();
	idx1->unlock();
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_COLOR);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	vtx0[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_COLOR);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	vtx1[ 0]->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);

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
void RendererCompositionShape::GenerateTriangleFrom4Vector( void *positions, void *normals, void *bones, void *colors,
	PxU32 stride, PxU32 startVtxIdx, PxU16 *indices, PxU32 startIndexIdx,
	const PxVec3 &center, PxVec3 v0, PxVec3 v1, PxVec3 v2, PxVec3 v3,
	PxU32 b0, PxU32 b1, PxU32 b2, PxU32 b3,
	PxU32 c0, PxU32 c1, PxU32 c2, PxU32 c3,
	vector<PxU16> &faceIndices, OUT vector<PxU16> &outfaceIndices )
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
			// vertex
			PxVec3 tmp = v3;
			v3 = v2;
			v2 = tmp;

			// bone
			PxU32 tmp2 = b3;
			b3 = b2;
			b2 = tmp2;

			// index
			PxU32 tmp3 = faceIndices[ 3];
			faceIndices[ 3] = faceIndices[ 2];
			faceIndices[ 2] = tmp3;
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
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v1;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)normals) + (stride * face1VtxIdx)) = -n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face1VtxIdx+1))) = -n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face1VtxIdx+2))) = -n;
			*(PxU32*)(((PxU8*)bones) + (stride * face1VtxIdx)) = b0;
			*(PxU32*)(((PxU8*)bones) + (stride * (face1VtxIdx+1))) = b1;
			*(PxU32*)(((PxU8*)bones) + (stride * (face1VtxIdx+2))) = b2;
			*(PxU32*)(((PxU8*)colors) + (stride * face1VtxIdx)) = c0;
			*(PxU32*)(((PxU8*)colors) + (stride * (face1VtxIdx+1))) = c1;
			*(PxU32*)(((PxU8*)colors) + (stride * (face1VtxIdx+2))) = c2;

			outfaceIndices.push_back( faceIndices[ 0] );
			outfaceIndices.push_back( faceIndices[ 1] );
			outfaceIndices.push_back( faceIndices[ 2] );
		}
		else
		{ // ccw
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v1;
			*(PxVec3*)(((PxU8*)normals) + (stride * face1VtxIdx)) = n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face1VtxIdx+1))) = n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face1VtxIdx+2))) = n;
			*(PxU32*)(((PxU8*)bones) + (stride * face1VtxIdx)) = b0;
			*(PxU32*)(((PxU8*)bones) + (stride * (face1VtxIdx+1))) = b2;
			*(PxU32*)(((PxU8*)bones) + (stride * (face1VtxIdx+2))) = b1;
			*(PxU32*)(((PxU8*)colors) + (stride * face1VtxIdx)) = c0;
			*(PxU32*)(((PxU8*)colors) + (stride * (face1VtxIdx+1))) = c2;
			*(PxU32*)(((PxU8*)colors) + (stride * (face1VtxIdx+2))) = c1;

			outfaceIndices.push_back( faceIndices[ 0] );
			outfaceIndices.push_back( faceIndices[ 2] );
			outfaceIndices.push_back( faceIndices[ 1] );
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
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v3;
			*(PxVec3*)(((PxU8*)normals) + (stride * face2VtxIdx)) = -n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face2VtxIdx+1))) = -n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face2VtxIdx+2))) = -n;
			*(PxU32*)(((PxU8*)bones) + (stride * face2VtxIdx)) = b0;
			*(PxU32*)(((PxU8*)bones) + (stride * (face2VtxIdx+1))) = b2;
			*(PxU32*)(((PxU8*)bones) + (stride * (face2VtxIdx+2))) = b3;
			*(PxU32*)(((PxU8*)colors) + (stride * face2VtxIdx)) = c0;
			*(PxU32*)(((PxU8*)colors) + (stride * (face2VtxIdx+1))) = c2;
			*(PxU32*)(((PxU8*)colors) + (stride * (face2VtxIdx+2))) = c3;

			outfaceIndices.push_back( faceIndices[ 0] );
			outfaceIndices.push_back( faceIndices[ 2] );
			outfaceIndices.push_back( faceIndices[ 3] );
		}
		else
		{ // ccw
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v0;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v3;
			*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx++)) = v2;
			*(PxVec3*)(((PxU8*)normals) + (stride * face2VtxIdx)) = n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face2VtxIdx+1))) = n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (face2VtxIdx+2))) = n;
			*(PxU32*)(((PxU8*)bones) + (stride * face2VtxIdx)) = b0;
			*(PxU32*)(((PxU8*)bones) + (stride * (face2VtxIdx+1))) = b3;
			*(PxU32*)(((PxU8*)bones) + (stride * (face2VtxIdx+2))) = b2;
			*(PxU32*)(((PxU8*)colors) + (stride * face2VtxIdx)) = c0;
			*(PxU32*)(((PxU8*)colors) + (stride * (face2VtxIdx+1))) = c3;
			*(PxU32*)(((PxU8*)colors) + (stride * (face2VtxIdx+2))) = c2;

			outfaceIndices.push_back( faceIndices[ 0] );
			outfaceIndices.push_back( faceIndices[ 3] );
			outfaceIndices.push_back( faceIndices[ 2] );
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
	const int findParentBoneIndex, const int findChildBoneIndex, 
	void *positions, PxU32 positionStride, void *normals, PxU32 normalStride,
	void *bones, PxU32 boneStride, const PxU32 numVerts,
	PxU16 *indices, PxU32 idx0Size, PxU32 idx1Size,
	OUT std::pair<int,int> &closeFace0, OUT std::pair<int,int> &closeFace1,
	OUT set<PxU16> &vtxIndices0, OUT set<PxU16> &vtxIndices1 )
{
	int foundCount = 0;
	set<int> checkV0, checkV1;
	vector<float> dots;
	vector<PxVec3> centers;
	vector<PxVec3> centerNorms;

	PxVec3 meshCenter0, meshCenter1;
	CalculateCenterPoint(findParentBoneIndex, positions, positionStride, bones, boneStride, numVerts, meshCenter0);
	CalculateCenterPoint(findChildBoneIndex, positions, positionStride, bones, boneStride, numVerts, meshCenter1);
	PxVec3 mesh0to1V = meshCenter1 - meshCenter0;
	mesh0to1V.normalize();
	PxVec3 mesh1to0V = meshCenter0 - meshCenter1;
	mesh1to0V.normalize();

	while (foundCount < 2)
	{
		float minLen = 100000.f;
		float maxDot = -10000;
		int minFaceIdx0 = -1;
		int minFaceIdx1 = -1;
		PxVec3 minCenter0, minCenter1;
		PxVec3 minCenterNorm0, minCenterNorm1;

		// find most close face
		for (PxU32 i=0; i<idx0Size; i+=3)
		{
			if (checkV0.find(i) != checkV0.end())
				continue; // already exist face index

			PxVec3 center0;
			PxVec3 center0Normal;
			{
				const PxU16 vidx0 = indices[ i];
				const PxU16 vidx1 = indices[ i+1];
				const PxU16 vidx2 = indices[ i+2];

				PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx0));
				PxVec3 &p1 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx1));
				PxVec3 &p2 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx2));

				PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx0));
				PxVec3 &n1 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx1));
				PxVec3 &n2 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx2));

				PxU32 &b0 = *(PxU32*)(((PxU8*)bones) + (boneStride * vidx0));
				PxU32 &b1 = *(PxU32*)(((PxU8*)bones) + (boneStride * vidx1));
				PxU32 &b2 = *(PxU32*)(((PxU8*)bones) + (boneStride * vidx2));

				if ((b0 != findParentBoneIndex) || (b1 != findParentBoneIndex) || (b2 != findParentBoneIndex))
					continue;

				center0 = p0 + p1 + p2;
				center0 /= 3.f;

				PxVec3 v0 = p1 - p0;
				v0.normalize();
				PxVec3 v1 = p2 - p0;
				v1.normalize();
				center0Normal = v1.cross(v0);
				center0Normal.normalize();

				if (mesh0to1V.dot(center0Normal) <= 0.f)
				{
					continue;
				}
			}

			for (PxU32 k=idx0Size; k<(idx0Size+idx1Size); k+=3)
			{
				if (checkV1.find(k) != checkV1.end())
					continue; // already exist face index

				PxVec3 center1;
				PxVec3 center1Normal;
				{
					const PxU16 vidx0 = indices[ k];
					const PxU16 vidx1 = indices[ k+1];
					const PxU16 vidx2 = indices[ k+2];

					PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx0));
					PxVec3 &p1 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx1));
					PxVec3 &p2 = *(PxVec3*)(((PxU8*)positions) + (positionStride * vidx2));

					PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx0));
					PxVec3 &n1 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx1));
					PxVec3 &n2 = *(PxVec3*)(((PxU8*)normals) + (normalStride * vidx2));

					PxU32 &b0 = *(PxU32*)(((PxU8*)bones) + (boneStride * vidx0));
					PxU32 &b1 = *(PxU32*)(((PxU8*)bones) + (boneStride * vidx1));
					PxU32 &b2 = *(PxU32*)(((PxU8*)bones) + (boneStride * vidx2));

					if ((b0 != findChildBoneIndex) || (b1 != findChildBoneIndex) || (b2 != findChildBoneIndex))
						continue;

					center1 = p0 + p1 + p2;
					center1 /= 3.f;

					PxVec3 v0 = p1 - p0;
					v0.normalize();
					PxVec3 v1 = p2 - p0;
					v1.normalize();
					center1Normal = v1.cross(v0);
					center1Normal.normalize();

					if (mesh1to0V.dot(center1Normal) <= 0.f)
					{
						continue;
					}
				}

				PxVec3 len = center0 - center1;
				//const float dot = center0Normal.dot(center1Normal);
				const float dot = mesh1to0V.dot(center1Normal) + mesh0to1V.dot(center0Normal);
				//if (minLen > len.magnitude())
				if (maxDot < dot)
				{
					minFaceIdx0 = i;
					minFaceIdx1 = k;
					minLen = len.magnitude();	
					minCenter0 = center0;
					minCenter1 = center1;
					minCenterNorm0 = center0Normal;
					minCenterNorm1 = center1Normal;
					maxDot = dot;
				}
			}
		}

		if (minFaceIdx0 < 0)
			break;

		{
			checkV0.insert(minFaceIdx0);
			checkV1.insert(minFaceIdx1);
			centers.push_back(minCenter0);
			centers.push_back(minCenter1);
			centerNorms.push_back(minCenterNorm0);
			centerNorms.push_back(minCenterNorm1);

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
	}

	if (centers.empty())
		return;

	// cross test
	PxVec3 v0 = centers[ 1] - centers[ 0];
	v0.normalize();
	PxVec3 v1 = centers[ 3] - centers[ 2];
	v1.normalize();
	const float dot1 = centerNorms[ 0].dot(v0);
	const float dot2 = centerNorms[ 2].dot(v1);
	if ((dot1 < 0.f) && (dot2 < 0.f))
	{
		return;
	}

	vtxIndices0.insert( indices[ closeFace0.first] );
	vtxIndices0.insert( indices[ closeFace0.first+1] );
	vtxIndices0.insert( indices[ closeFace0.first+2] );
	vtxIndices0.insert( indices[ closeFace1.first] );
	vtxIndices0.insert( indices[ closeFace1.first+1] );
	vtxIndices0.insert( indices[ closeFace1.first+2] );

	vtxIndices1.insert( indices[ closeFace0.second] );
	vtxIndices1.insert( indices[ closeFace0.second+1] );
	vtxIndices1.insert( indices[ closeFace0.second+2] );
	vtxIndices1.insert( indices[ closeFace1.second] );
	vtxIndices1.insert( indices[ closeFace1.second+1] );
	vtxIndices1.insert( indices[ closeFace1.second+2] );
}


/**
 @brief 
 @date 2014-01-05
*/
void RendererCompositionShape::GenerateBoxFromCloseVertex(
	const set<PxU16> &vtxIndices0, const set<PxU16> &vtxIndices1, PxVec3 center,
	void *positions, void *normals, void *bones, void *colors,
	PxU32 stride, PxU32 startVtxIdx,
	PxU16 *indices, PxU32 startIndexIdx,
	OUT vector<PxU16> &outVtxIndices )
{
	if (vtxIndices0.empty() || vtxIndices1.empty())
		return;

	// gen face
	vector<PxU16> indices0, indices1; 
	vector<PxVec3> v0, v1;
	vector<PxU32> b0, b1;
	vector<PxU32> c0, c1;
	BOOST_FOREACH (const auto vidx, vtxIndices0)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * vidx));
		PxU32 &b = *(PxU32*)(((PxU8*)bones) + (stride * vidx));
		PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride * vidx));
		v0.push_back(p);
		b0.push_back(b);
		c0.push_back(c);
		indices0.push_back(vidx);
	}

	BOOST_FOREACH (auto vidx, vtxIndices1)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * vidx));
		PxU32 &b = *(PxU32*)(((PxU8*)bones) + (stride * vidx));
		PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride * vidx));
		v1.push_back(p);
		b1.push_back(b);
		c1.push_back(c);
		indices1.push_back(vidx);
	}

	// find appropriate face
	vector<int> line(6);
	{
		PxVec3 startV = v0[1] - v0[ 0];
		startV.normalize();

		int findIdx = -1;
		for (u_int i=3; i >= 1; --i)
		{
			PxVec3 v = v1[ i-1] - v1[ i];
			v.normalize();

			if (startV.dot(v) > 0)
			{
				findIdx = i;
				break;
			}
		}

		if (findIdx < 0)
			return;

		findIdx += 4;
		line[ 0] = findIdx-- % 4;
		line[ 1] = findIdx-- % 4;
		line[ 2] = findIdx-- % 4;
		line[ 3] = findIdx-- % 4;
	}

 	vector<PxU16> face0Indices, outFace0Indices;
	face0Indices.push_back(indices0[ 0]);
	face0Indices.push_back(indices0[ 1]);
	face0Indices.push_back(indices1[ line[ 0]]);
	face0Indices.push_back(indices1[ line[ 1]]);

	GenerateTriangleFrom4Vector( positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, v0[ 0], v0[ 1], v1[ line[ 0]], v1[ line[ 1]],
		b0[ 0], b0[ 1], b1[ line[ 0]], b1[ line[ 1]], 
		c0[ 0], c0[ 1], c1[ line[ 0]], c1[ line[ 1]], 
		face0Indices, outFace0Indices );
	startVtxIdx += 6;
	startIndexIdx += 6;


	vector<PxU16> face1Indices, outFace1Indices;
	face1Indices.push_back(indices0[ 1]);
	face1Indices.push_back(indices0[ 2]);
	face1Indices.push_back(indices1[ line[ 1]]);
	face1Indices.push_back(indices1[ line[ 2]]);

	GenerateTriangleFrom4Vector(positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx,center, v0[ 1], v0[ 2], v1[ line[ 1]], v1[ line[ 2]],
		b0[ 1], b0[ 2], b1[ line[ 1]], b1[ line[ 2]],
		c0[ 1], c0[ 2], c1[ line[ 1]], c1[ line[ 2]],
		face1Indices, outFace1Indices );
	startVtxIdx += 6;
	startIndexIdx += 6;


	vector<PxU16> face2Indices, outFace2Indices;
	face2Indices.push_back(indices0[ 2]);
	face2Indices.push_back(indices0[ 3]);
	face2Indices.push_back(indices1[ line[ 2]]);
	face2Indices.push_back(indices1[ line[ 3]]);

	GenerateTriangleFrom4Vector(positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, v0[ 2], v0[ 3], v1[ line[ 2]], v1[ line[ 3]],
		b0[ 2], b0[ 3], b1[ line[ 2]], b1[ line[ 3]],
		c0[ 2], c0[ 3], c1[ line[ 2]], c1[ line[ 3]],
		face2Indices, outFace2Indices );
	startVtxIdx += 6;
	startIndexIdx += 6;


	vector<PxU16> face3Indices, outFace3Indices;
	face3Indices.push_back(indices0[ 3]);
	face3Indices.push_back(indices0[ 0]);
	face3Indices.push_back(indices1[ line[ 3]]);
	face3Indices.push_back(indices1[ line[ 0]]);

	GenerateTriangleFrom4Vector(positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, v0[ 3], v0[ 0], v1[ line[ 3]], v1[ line[ 0]],
		b0[ 3], b0[ 0], b1[ line[ 3]], b1[ line[ 0]],
		c0[ 3], c0[ 0], c1[ line[ 3]], c1[ line[ 0]],
		face3Indices, outFace3Indices );
	startVtxIdx += 6;
	startIndexIdx += 6;


	std::copy(outFace0Indices.begin(), outFace0Indices.end(), std::back_inserter(outVtxIndices));
	std::copy(outFace1Indices.begin(), outFace1Indices.end(), std::back_inserter(outVtxIndices));
	std::copy(outFace2Indices.begin(), outFace2Indices.end(), std::back_inserter(outVtxIndices));
	std::copy(outFace3Indices.begin(), outFace3Indices.end(), std::back_inserter(outVtxIndices));
}


/**
 @brief 
 @date 2014-01-03
 */
void RendererCompositionShape::GenerateCompositionShape( RendererShape *shape0, RenderMaterial *material0 )
{
	RendererMesh *mesh0 = shape0->getMesh();
	SampleRenderer::RendererVertexBuffer **vtx0 = mesh0->getVertexBuffersEdit();
	SampleRenderer::RendererIndexBuffer *idx0 = mesh0->getIndexBufferEdit();
	const PxU32 numVtxBuff0 = mesh0->getNumVertexBuffers();

	if (numVtxBuff0 <= 0)
		return;

	PxU32 stride0 = 0;
	void *positions0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride0);
	void *normals0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride0);
	void *uvs0 = vtx0[ 0]->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, stride0);

	PxU16 *indices0 = (PxU16*)idx0->lock();
	PxU32 idx0Size = idx0->getMaxIndices();

	const PxU32 numVerts = vtx0[0]->getMaxVertices();
	const PxU32 numIndices = idx0Size;
	const PxVec3 diffuseColor = (material0)? (material0->mDiffuseColor*255) : PxVec3(255,255,255);

	if (indices0 && positions0 && normals0)
	{
		RendererVertexBufferDesc vbdesc;
		vbdesc.hint = RendererVertexBuffer::HINT_STATIC;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_COLOR]  = RendererVertexBuffer::FORMAT_COLOR_BGRA;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL] = RendererVertexBuffer::FORMAT_FLOAT3;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
		vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_BONEINDEX] = RendererVertexBuffer::FORMAT_UBYTE4;
		vbdesc.maxVertices = numVerts;
		m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
		RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");

		PxU32 stride = 0;
		void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
		void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
		void *colors = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_COLOR, stride);
		void *uvs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, stride);
		void *bones = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride);

		// copy shape0 to current
		for (PxU32 i=0; i < numVerts; ++i)
		{
			// current node
			PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * i));
			PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (stride * i));
			PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride * i));
			PxF32 *uv  =  (PxF32*)(((PxU8*)uvs) + (stride * i));

			// source node
			PxVec3 &p0 = *(PxVec3*)(((PxU8*)positions0) + (stride0 * i));
			PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals0) + (stride0 * i));
			PxF32 *uv0  =  (PxF32*)(((PxU8*)uvs0) + (stride0 * i));
			PxU32 &bidx  =  *(PxU32*)(((PxU8*)bones) + (stride * i));

			p = p0;
			n = n0;
			c = m_renderer.convertColor(RendererColor(diffuseColor.x, diffuseColor.y, diffuseColor.z));

			uv[ 0] = uv0[ 0];
			uv[ 1] = uv0[ 1];
			bidx = m_PaletteIndex;
		}

		CopyToSourceVertex(positions, normals, stride, numVerts);

		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_COLOR);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);


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
				for(PxU32 i=0; i<idx0Size; i++)
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


/**
 @brief apply bone transform palette
 @date 2014-01-06
*/
void RendererCompositionShape::ApplyPalette()
{
	PxU32 stride = 0;
	void *positions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);
	void *normals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, stride);
	void *bones = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX, stride);

	const PxU32 numVerts = m_vertexBuffer->getMaxVertices();
	for (PxU32 i=0; i < numVerts; ++i)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * i));
		PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (stride * i));
		const PxU32 &bidx  =  *(PxU32*)(((PxU8*)bones) + (stride * i));

		PxTransform tm0 = m_TmPalette[ bidx] * PxTransform(m_SrcVertex[ i]);
		PxTransform tm1 = PxTransform(m_TmPalette[ bidx].q) * PxTransform(m_SrcNormal[ i]);

		p = tm0.p;
		n = tm1.p;
	}

	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_BONEINDEX);
}


/**
 @brief 
 @date 2014-01-06
*/
void RendererCompositionShape::CopyToSourceVertex(void *positions, void *normals, PxU32 stride, const int numVerts)
{
	m_SrcVertex.resize(numVerts);
	m_SrcNormal.resize(numVerts);
	for (int i=0; i < numVerts; ++i)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * i));
		m_SrcVertex[ i] = p;

		if (normals)
		{
			PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (stride * i));
			m_SrcNormal[ i] = n;
		}
	}
}


/**
 @brief copy local vertex position to source vertex buffer
 @date 2014-01-06
*/
void RendererCompositionShape::CopyLocalVertexToSourceVtx( const RendererCompositionShape *shape0, 
	const RendererCompositionShape *shape1, 
	void *normals, PxU32 normalStride, 
	const PxU32 numVerts, PxU16 *indices, 
	const PxU32 idxSize, const PxU32 startCloseIdx, const vector<PxU16> &closeVtxIndices )
{
	m_SrcVertex.resize(numVerts);
	m_SrcNormal.resize(numVerts);

	int vidx = 0;
	const vector<PxVec3> &srcVtx0 = shape0->GetLocalSrcVertex();
	BOOST_FOREACH (const auto &v, srcVtx0)
		m_SrcVertex[ vidx++] = v;
	const vector<PxVec3> &srcVtx1 = shape1->GetLocalSrcVertex();
	BOOST_FOREACH (const auto &v, srcVtx1)
		m_SrcVertex[ vidx++] = v;

	int nidx = 0;	
	BOOST_FOREACH (const auto &v, shape0->GetLocalSrcNormal())
		m_SrcNormal[ nidx++] = v;
	BOOST_FOREACH (const auto &v, shape1->GetLocalSrcNormal())
		m_SrcNormal[ nidx++] = v;

	// copy close mesh vertex
	const PxU32 vtx0Size = srcVtx0.size();
	BOOST_FOREACH (auto &idx, closeVtxIndices)
	{
		if (vtx0Size > idx)
		{
			m_SrcVertex[ vidx++] = srcVtx0[ idx];
		}
		else
		{
			m_SrcVertex[ vidx++] = srcVtx1[ idx-vtx0Size];
		}
	}

	const PxU32 startNormIdx = srcVtx0.size() + srcVtx1.size();
	for (PxU32 i=startNormIdx; i < numVerts; ++i)
	{
		PxVec3 &n0 = *(PxVec3*)(((PxU8*)normals) + (normalStride * i));
		m_SrcNormal[ nidx++] = n0;
	}
}


/**
 @brief 
 @date 2014-01-07
*/
void RendererCompositionShape::ApplyTransform(void *positions, PxU32 positionStride, void *normals, 
	PxU32 normalStride, const int numVerts, const PxTransform &tm)
{
	for (int i=0; i < numVerts; ++i)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (positionStride * i));
		PxVec3 &n = *(PxVec3*)(((PxU8*)normals) + (normalStride * i));

		PxTransform t0 = tm * PxTransform(p);
		PxTransform t1 = PxTransform(tm.q) * PxTransform(n);

		p = t0.p;
		n = t1.p;
	}
}
