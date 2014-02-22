
#pragma once


namespace SampleRenderer
{
	void CalculateCenterPoint( const int boneIndex, void *positions, void *bones, PxU32 stride, const PxU32 numVerts, OUT PxVec3 &out );

	void GenerateTriangleFrom3Vector( void *positions, void *normals, PxU32 stride, const PxVec3 &center,
		const vector<PxU16> &triangle, OUT vector<PxU16> &outTriangle );

	void GenerateTriangleFrom4Vector2( void *positions, void *normals, void *bones, void *colors, PxU32 stride, PxU32 startVtxIdx, 
		PxU16 *indices, PxU32 startIndexIdx, const PxVec3 &center, 
		const vector<PxU16> &faceIndices, OUT vector<PxU16> &outfaceIndices );

	void GenerateBoxFromCloseVertex( const set<PxU16> &vtxIdx0, const set<PxU16> &vtxIdx1, const PxVec3 &center,
		void *positions, void *normals, void *bones, void *colors,
		PxU32 stride, PxU32 startVtxIdx, PxU16 *indices, PxU32 startIndexIdx, 
		OUT vector<PxU16> &outVtxIndices );

}
