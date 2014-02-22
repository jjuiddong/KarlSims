/**
 @filename RenderCompositionShape.h
 
*/
#pragma once

#include <RendererShape.h>


class RenderMaterial;
namespace SampleRenderer
{
	class RendererVertexBuffer;
	class RendererIndexBuffer;

	class RendererCompositionShape : public RendererShape
	{
	public:
		RendererCompositionShape(Renderer &renderer, 
			const int parentShapeIndex, const int childShapeIndex, 
			const int paletteIndex, const vector<PxTransform> &tmPalette,
			RendererCompositionShape *shape0, const PxTransform &tm0, 
			RendererCompositionShape *shape1, const PxTransform &tm1, const PxReal* uvs=NULL);

		RendererCompositionShape(Renderer &renderer, const int paletteIndex, const vector<PxTransform> &tmPalette,
			RendererShape *shape0, RenderMaterial *material0 );

		virtual ~RendererCompositionShape(void);

		void ApplyPalette();

		const vector<PxVec3>& GetLocalSrcVertex() const;
		const vector<PxVec3>& GetLocalSrcNormal() const;


	protected:
		void GenerateCompositionShape( RendererShape *shape0, const PxTransform &tm0, 
			RendererShape *shape1, const PxTransform &tm1, 
			const int additionalVtxBuffSize, const int additionalIdxBuffSize );

		void GenerateCompositionShape( RendererShape *shape0, RenderMaterial *material0 );

		//void GenerateTriangleFrom4Vector( void *positions, void *normals, void *bones, void *colors, 
		//	PxU32 stride, PxU32 startVtxIdx, 
		//	PxU16 *indices, PxU32 startIndexIdx,
		//	const PxVec3 &center, PxVec3 v0, PxVec3 v1, PxVec3 v2, PxVec3 v3,
		//	PxU32 b0, PxU32 b1, PxU32 b2, PxU32 b3,
		//	PxU32 c0, PxU32 c1, PxU32 c2, PxU32 c3,
		//	vector<PxU16> &faceIndices, OUT vector<PxU16> &outfaceIndices
		//	);

		void FindMostCloseFace( 
			const int findParentBoneIndex, const int findChildBoneIndex,
			void *positions, PxU32 positionStride, void *normals, PxU32 normalStride,
			void *bones, PxU32 boneStride, const PxU32 numVerts,
			PxU16 *indices, PxU32 idx0Size, PxU32 idx1Size,
			OUT std::pair<int,int> &closeFace0, OUT std::pair<int,int> &closeFace1,
			OUT set<PxU16> &vtxIndices0, OUT set<PxU16> &vtxIndices1 );


		void CalculateCenterPoint( std::pair<int,int> closeFace0, std::pair<int,int> closeFace1,
			void *positions, PxU32 stride, PxU16 *indices, OUT PxVec3 &out );

		void CopyToSourceVertex(void *positions, void *normals, PxU32 stride, const int numVerts);

		void CopyLocalVertexToSourceVtx( const RendererCompositionShape *shape0, const RendererCompositionShape *shape1,
			void *normals, PxU32 normalStride, const PxU32 numVerts, PxU16 *indices, const PxU32 idxSize, const PxU32 startCloseIdx,
			const vector<PxU16> &closeVtxIndices);
		
		//void CalculateNormal( const vector<PxU16> &indicesBuff, vector<PxVec3> &vtxBuff, vector<PxVec3> &normBuff );
		void ApplyTransform(void *positions, PxU32 positionStride, void *normals, PxU32 normalStride, const int numVerts, 
			const PxTransform &tm);


	private:
		vector<PxVec3> m_SrcVertex;
		vector<PxVec3> m_SrcNormal;
		int m_PaletteIndex;
		const vector<PxTransform> &m_TmPalette;
		RendererVertexBuffer *m_vertexBuffer;
		RendererIndexBuffer  *m_indexBuffer;
	};


	inline const vector<PxVec3>& RendererCompositionShape::GetLocalSrcVertex() const { return m_SrcVertex; }
	inline const vector<PxVec3>& RendererCompositionShape::GetLocalSrcNormal() const { return m_SrcNormal; }
}
