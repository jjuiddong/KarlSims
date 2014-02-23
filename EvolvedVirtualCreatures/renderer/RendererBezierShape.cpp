
#include "stdafx.h"
#include "RendererBezierShape.h"
#include "MeshCompositionUtility.h"


using namespace SampleRenderer;

RendererBezierShape::RendererBezierShape(Renderer& renderer, const vector<PxVec3> &points) :
	RendererShape(renderer)
{
	const int NODE_COUNT = 30;
	const int CONER_COUNT = 5;
	const PxU32 numVerts = (NODE_COUNT-1)*CONER_COUNT + 1; // +1 is head vertex point

	RendererVertexBufferDesc vbdesc;
	vbdesc.hint = RendererVertexBuffer::HINT_STATIC;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.maxVertices = numVerts;
	m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
	RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");
	if (m_vertexBuffer)
	{
		PxU32 stride = 0;
		void* vertPositions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, stride);

		PxVec3 oldPos;
		int vtxOffset = 0;
		for (int i=0; i < NODE_COUNT-1; ++i)
		{
			PxVec3 pos;
			utility::bezier(pos, points, (float)i/(float)(NODE_COUNT-1));

			PxQuat q;
			if (i > 0)
			{
				PxVec3 curve = pos - oldPos;
				curve.normalize();
				utility::quatRotationArc(q, curve, PxVec3(1,0,0));
			}
			oldPos = pos;

			float DEPTH = .03f;
			if (i == NODE_COUNT-2)
				DEPTH += DEPTH*3; // arrow head

			for (int k=0; k < CONER_COUNT; ++k)
			{
				const float rad = -PxPi * 2.f * ((float)k/(float)CONER_COUNT);
				PxVec3 dir = PxQuat(rad, PxVec3(1,0,0)).rotate(PxVec3(0,1,0));
				if (i > 0)
					dir = q.rotate(dir);
				*(PxVec3*)(((PxU8*)vertPositions) + vtxOffset) = pos + dir*DEPTH;
				vtxOffset += stride;
			}

			// arrow head
			DEPTH += DEPTH; // head length
			if (i == NODE_COUNT-2)
			{
				utility::bezier(pos, points, 1);
				PxVec3 dir = q.rotate(PxVec3(1,0,0));
				//*(PxVec3*)(((PxU8*)vertPositions) + vtxOffset) = (pos + dir*DEPTH);
				*(PxVec3*)(((PxU8*)vertPositions) + vtxOffset) = pos;
			}
		}
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	}


	// make index buffer
	// ((CONER_COUNT-2) * 3) : arrow front triangle
	// (CONER_COUNT*3) : arrow head triangle
	const PxU32 numIndices = (6*CONER_COUNT)*(NODE_COUNT-2) + ((CONER_COUNT-2) * 3) + (CONER_COUNT*3);
	const PxU32 numFaces = numIndices/3;

	RendererIndexBufferDesc ibdesc;
	ibdesc.hint = RendererIndexBuffer::HINT_STATIC;
	ibdesc.format = RendererIndexBuffer::FORMAT_UINT16;
	ibdesc.maxIndices = numIndices;
	m_indexBuffer = m_renderer.createIndexBuffer(ibdesc);
	RENDERER_ASSERT(m_indexBuffer, "Failed to create Index Buffer.");
	if (m_indexBuffer)
	{
		PxU16* indices = (PxU16*)m_indexBuffer->lock();
		if (indices)
		{
			PxU32 writeStartIndiceIdx = 0;

			// front
			for (int k=0; k < CONER_COUNT-2; ++k)
			{
				indices[ writeStartIndiceIdx++] = 0;
				indices[ writeStartIndiceIdx++] = k+2;
				indices[ writeStartIndiceIdx++] = k+1;
			}
			// arrow head
			const int lastVtxIdx = numVerts-CONER_COUNT-1;
			for (int k=0; k < CONER_COUNT; ++k)
			{
				indices[ writeStartIndiceIdx++] = lastVtxIdx+k;
				indices[ writeStartIndiceIdx++] = lastVtxIdx+(k+1)%CONER_COUNT;
				indices[ writeStartIndiceIdx++] = numVerts-1; // arrow head vertex
			}

			for (int i=0; i < NODE_COUNT-2; ++i)
			{
				vector<PxU16> vtxIdx0, vtxIdx1;
				for (int f=0; f<CONER_COUNT; ++f)
				{
					vtxIdx0.push_back(i*CONER_COUNT + f);
					vtxIdx1.push_back(i*CONER_COUNT + CONER_COUNT+f);
				}
				
				for (int k=0; k < CONER_COUNT; ++k)
				{
					indices[ writeStartIndiceIdx++] = vtxIdx0[ (k+0)%CONER_COUNT];
					indices[ writeStartIndiceIdx++] = vtxIdx0[ (k+1)%CONER_COUNT];
					indices[ writeStartIndiceIdx++] = vtxIdx1[ (k+0)%CONER_COUNT];

					indices[ writeStartIndiceIdx++] = vtxIdx0[ (k+1)%CONER_COUNT];
					indices[ writeStartIndiceIdx++] = vtxIdx1[ (k+1)%CONER_COUNT];
					indices[ writeStartIndiceIdx++] = vtxIdx1[ (k+0)%CONER_COUNT];
				}
			}
		}
		m_indexBuffer->unlock();
	}

	if(m_vertexBuffer && m_indexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives  = RendererMesh::PRIMITIVE_TRIANGLES;
		meshdesc.vertexBuffers = &m_vertexBuffer;
		meshdesc.numVertexBuffers = 1;
		meshdesc.firstVertex = 0;
		meshdesc.numVertices = numVerts;
		meshdesc.indexBuffer = m_indexBuffer;
		meshdesc.firstIndex = 0;
		meshdesc.numIndices = numIndices;
		m_mesh = m_renderer.createMesh(meshdesc);
		RENDERER_ASSERT(m_mesh, "Failed to create Mesh.");
	}
}

RendererBezierShape::~RendererBezierShape()
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_mesh);
}
