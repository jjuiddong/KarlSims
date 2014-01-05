/**
 @filename RenderCompositionShape.h
 
*/
#pragma once

#include <RendererShape.h>


namespace SampleRenderer
{
	class RendererVertexBuffer;
	class RendererIndexBuffer;

	class RendererCompositionShape : public RendererShape
	{
	public:
		RendererCompositionShape(Renderer &renderer, 
			const PxVec3& pos1, const PxVec3& extents1, 
			const PxVec3& pos2, const PxVec3& extents2,
			const PxReal* uvs=NULL);

		RendererCompositionShape(Renderer &renderer, 
			RendererShape *shape0, const PxTransform &tm0, 
			RendererShape *shape1, const PxTransform &tm1, const PxReal* uvs=NULL);

		RendererCompositionShape(Renderer &renderer, RendererShape *shape0 );

		virtual ~RendererCompositionShape(void);


	protected:
		void GenerateCompositionShape( RendererShape *shape0, const PxTransform &tm0, 
			RendererShape *shape1, const PxTransform &tm1, 
			const int additionalVtxBuffSize, const int additionalIdxBuffSize );

		void GenerateCompositionShape( RendererShape *shape0 );

		void GenerateTriangleFrom4Vector( void *positions, PxU32 positionStride, 
			void *normals, PxU32 normalStride, PxU32 startVtxIdx, 
			PxU16 *indices, PxU32 startIndexIdx,
			const PxVec3 &center, PxVec3 v0, PxVec3 v1, PxVec3 v2, PxVec3 v3 );

		void FindMostCloseFace( 
			void *positions0, PxU32 positionStride0, void *normals0, PxU32 normalStride0,
			PxU16 *indices0, PxU32 idx0Size,
			void *positions1, PxU32 positionStride1, void *normals1, PxU32 normalStride1,
			PxU16 *indices1, PxU32 idx1Size,
			OUT std::pair<int,int> &closeFace0, OUT std::pair<int,int> &closeFace1,
			OUT set<PxU16> &vtx0, OUT set<PxU16> &vtx1 );

		void GenerateBoxFromCloseVertex(
			const set<PxU16> &vtxIdx0, const set<PxU16> &vtxIdx1,
			PxVec3 center,
			void *positions0, PxU32 positionStride0, void *normals0, PxU32 normalStride0,
			PxU16 *indices0, PxU32 idx0Size,
			void *positions1, PxU32 positionStride1, void *normals1, PxU32 normalStride1,
			PxU16 *indices1, PxU32 idx1Size,
			void *positions, PxU32 positionStride, PxU32 startVtxIdx,
			void *normals, PxU32 normalStride, 
			PxU16 *indices, PxU32 startIndexIdx );

		void CalculateCenterPoint( std::pair<int,int> closeFace0, std::pair<int,int> closeFace1,
			void *positions0, PxU32 positionStride0, void *normals0, PxU32 normalStride0,
			PxU16 *indices0, PxU32 idx0Size,
			void *positions1, PxU32 positionStride1, void *normals1, PxU32 normalStride1,
			PxU16 *indices1, PxU32 idx1Size,
			OUT PxVec3 &out );

	private:
		RendererVertexBuffer *m_vertexBuffer;
		RendererIndexBuffer  *m_indexBuffer;
	};

}
