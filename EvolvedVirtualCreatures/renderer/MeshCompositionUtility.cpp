
#include "stdafx.h"
#include "MeshCompositionUtility.h"

using namespace SampleRenderer;



/**
 @brief generate triangle index buffer
 @date 2014-01-30
*/
void SampleRenderer::GenerateTriangleFrom3Vector( void *positions, void *normals, PxU32 stride, 
	const PxVec3 &center, const vector<PxU16> &triangle, OUT vector<PxU16> &outTriangle )
{
	const PxVec3 p0 = *(PxVec3*)(((PxU8*)positions) + (stride * triangle[ 0]));
	const PxVec3 p1 = *(PxVec3*)(((PxU8*)positions) + (stride * triangle[ 1]));
	const PxVec3 p2 = *(PxVec3*)(((PxU8*)positions) + (stride * triangle[ 2]));

	PxVec3 faceCenter = (p0 + p1 + p2) / 3.f;
	PxVec3 n = faceCenter - center;
	n.normalize();

	// ccw check
	//
	//            p0
	//          /   \
	//        /        \
	//    p2  ------  p1
	//

	PxVec3 v10 = p1 - p0;
	v10.normalize();
	PxVec3 v20 = p2 - p0;
	v20.normalize();

	PxVec3 crossV = v10.cross(v20);
	crossV.normalize();

	if (n.dot(crossV) >= 0)
	{
		outTriangle.push_back( triangle[ 0] );
		outTriangle.push_back( triangle[ 2] );
		outTriangle.push_back( triangle[ 1] );
	}
	else
	{
		outTriangle.push_back( triangle[ 0] );
		outTriangle.push_back( triangle[ 1] );
		outTriangle.push_back( triangle[ 2] );
	}
}



/**
 @brief vector 4개로 triangle 2개를 그리는 index buffer를 생성한다.
 @date 2014-01-30
*/
void SampleRenderer::GenerateTriangleFrom4Vector2( void *positions, void *normals, void *bones, void *colors, 
	PxU32 stride, PxU32 startVtxIdx, PxU16 *indices, PxU32 startIndexIdx, const PxVec3 &center, 
	const vector<PxU16> &faceIndices, OUT vector<PxU16> &outfaceIndices )
{
	vector<PxU16> triangle0; triangle0.reserve(3);
	vector<PxU16> triangle1; triangle1.reserve(3);
	triangle0.push_back( faceIndices[ 0] );
	triangle0.push_back( faceIndices[ 2] );
	triangle0.push_back( faceIndices[ 3] );

	triangle1.push_back( faceIndices[ 0] );
	triangle1.push_back( faceIndices[ 1] );
	triangle1.push_back( faceIndices[ 3] );

	GenerateTriangleFrom3Vector(positions, normals, stride, center, triangle0, outfaceIndices);
	GenerateTriangleFrom3Vector(positions, normals, stride, center, triangle1, outfaceIndices);

	if (outfaceIndices.size() != 6)
		return; // error occur
	
	for (u_int i=0; i < outfaceIndices.size();)
	{
		const PxVec3 p0 = *(PxVec3*)(((PxU8*)positions) + (stride * outfaceIndices[ i]));
		const PxVec3 p1 = *(PxVec3*)(((PxU8*)positions) + (stride * outfaceIndices[ i+1]));
		const PxVec3 p2 = *(PxVec3*)(((PxU8*)positions) + (stride * outfaceIndices[ i+2]));

		*(PxVec3*)(((PxU8*)positions) + (stride * startVtxIdx)) = p0;
		*(PxVec3*)(((PxU8*)positions) + (stride * (startVtxIdx+1))) = p1;
		*(PxVec3*)(((PxU8*)positions) + (stride * (startVtxIdx+2))) = p2;

		if (bones)
		{
			const PxU32 b0 = *(PxU32*)(((PxU8*)bones) + (stride * outfaceIndices[ i]));
			const PxU32 b1 = *(PxU32*)(((PxU8*)bones) + (stride * outfaceIndices[ i+1]));
			const PxU32 b2 = *(PxU32*)(((PxU8*)bones) + (stride * outfaceIndices[ i+2]));

			*(PxU32*)(((PxU8*)bones) + (stride * startVtxIdx)) = b0;
			*(PxU32*)(((PxU8*)bones) + (stride * (startVtxIdx+1))) = b1;
			*(PxU32*)(((PxU8*)bones) + (stride * (startVtxIdx+2))) = b2;
		}

		if (colors)
		{
			const PxU32 c0 = *(PxU32*)(((PxU8*)colors) + (stride * outfaceIndices[ i]));
			const PxU32 c1 = *(PxU32*)(((PxU8*)colors) + (stride * outfaceIndices[ i+1]));
			const PxU32 c2 = *(PxU32*)(((PxU8*)colors) + (stride * outfaceIndices[ i+2]));

			*(PxU32*)(((PxU8*)colors) + (stride * startVtxIdx)) = c0;
			*(PxU32*)(((PxU8*)colors) + (stride * (startVtxIdx+1))) = c1;
			*(PxU32*)(((PxU8*)colors) + (stride * (startVtxIdx+2))) = c2;
		}

		if (normals)
		{
			PxVec3 v01 = p1 - p0;
			PxVec3 v02 = p2 - p0;
			v01.normalize();
			v02.normalize();
			PxVec3 n = v01.cross(v02);
			n.normalize();
			n = -n;

			*(PxVec3*)(((PxU8*)normals) + (stride * startVtxIdx)) = n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (startVtxIdx+1))) = n;
			*(PxVec3*)(((PxU8*)normals) + (stride * (startVtxIdx+2))) = n;
		}		

		indices[ startIndexIdx] = startVtxIdx;
		indices[ startIndexIdx+1] = startVtxIdx+1;
		indices[ startIndexIdx+2] = startVtxIdx+2;

		startIndexIdx += 3;
		startVtxIdx += 3;
		i += 3;
	}
}


/**
 @brief 
 @date 2014-01-05
*/
void SampleRenderer::GenerateBoxFromCloseVertex(
	const set<PxU16> &vtxIndices0, const set<PxU16> &vtxIndices1, const PxVec3 &center,
	void *positions, void *normals, void *bones, void *colors,
	PxU32 stride, PxU32 startVtxIdx, PxU16 *indices, PxU32 startIndexIdx, OUT vector<PxU16> &outVtxIndices )
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
		v0.push_back(p);
		indices0.push_back(vidx);

		if (bones)
		{
			PxU32 &b = *(PxU32*)(((PxU8*)bones) + (stride * vidx));
			b0.push_back(b);
		}
		if (colors)
		{
			PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride * vidx));
			c0.push_back(c);
		}
	}

	BOOST_FOREACH (auto vidx, vtxIndices1)
	{
		PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * vidx));
		v1.push_back(p);
		indices1.push_back(vidx);

		if (bones)
		{
			PxU32 &b = *(PxU32*)(((PxU8*)bones) + (stride * vidx));
			b1.push_back(b);
		}
		if (colors)
		{
			PxU32 &c = *(PxU32*)(((PxU8*)colors) + (stride * vidx));
			c1.push_back(c);
		}
	}

	// find appropriate face
	vector<int> line(6);
	{
		int findIdx = -1;
		int findIdx2 = -1;
		for (u_int k=0; k < 3; ++k)
		{
			PxVec3 startV = v0[ k+1] - v0[ k];
			startV.normalize();

			for (u_int i=3; i >= 1; --i)
			{
				PxVec3 v = v1[ i-1] - v1[ i];
				v.normalize();
				const float dot = startV.dot(v);
				if (dot > 0)
				{
					findIdx = i;
					findIdx2 = k;
					break;
				}
			}
			if (findIdx >= 0)
				break;
		}

		if (findIdx < 0)
			return;

		findIdx += 4;
		line[ findIdx2++ % 4] = findIdx-- % 4;
		line[ findIdx2++ % 4] = findIdx-- % 4;
		line[ findIdx2++ % 4] = findIdx-- % 4;
		line[ findIdx2++ % 4] = findIdx-- % 4;
	}

 	vector<PxU16> face0Indices, outFace0Indices;
	face0Indices.push_back(indices0[ 0]);
	face0Indices.push_back(indices0[ 1]);
	face0Indices.push_back(indices1[ line[ 0]]);
	face0Indices.push_back(indices1[ line[ 1]]);

	GenerateTriangleFrom4Vector2( positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, face0Indices, outFace0Indices );

	startVtxIdx += 6;
	startIndexIdx += 6;


	vector<PxU16> face1Indices, outFace1Indices;
	face1Indices.push_back(indices0[ 1]);
	face1Indices.push_back(indices0[ 2]);
	face1Indices.push_back(indices1[ line[ 1]]);
	face1Indices.push_back(indices1[ line[ 2]]);

	GenerateTriangleFrom4Vector2( positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, face1Indices, outFace1Indices );

	startVtxIdx += 6;
	startIndexIdx += 6;


	vector<PxU16> face2Indices, outFace2Indices;
	face2Indices.push_back(indices0[ 2]);
	face2Indices.push_back(indices0[ 3]);
	face2Indices.push_back(indices1[ line[ 2]]);
	face2Indices.push_back(indices1[ line[ 3]]);

	GenerateTriangleFrom4Vector2( positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, face2Indices, outFace2Indices );

	startVtxIdx += 6;
	startIndexIdx += 6;


	vector<PxU16> face3Indices, outFace3Indices;
	face3Indices.push_back(indices0[ 3]);
	face3Indices.push_back(indices0[ 0]);
	face3Indices.push_back(indices1[ line[ 3]]);
	face3Indices.push_back(indices1[ line[ 0]]);

	GenerateTriangleFrom4Vector2( positions, normals, bones, colors, stride, 
		startVtxIdx, indices, startIndexIdx, center, face3Indices, outFace3Indices );

	startVtxIdx += 6;
	startIndexIdx += 6;


	std::copy(outFace0Indices.begin(), outFace0Indices.end(), std::back_inserter(outVtxIndices));
	std::copy(outFace1Indices.begin(), outFace1Indices.end(), std::back_inserter(outVtxIndices));
	std::copy(outFace2Indices.begin(), outFace2Indices.end(), std::back_inserter(outVtxIndices));
	std::copy(outFace3Indices.begin(), outFace3Indices.end(), std::back_inserter(outVtxIndices));
}


/**
 @brief Calculate CenterPoint
 @date 2014-01-08
*/
void SampleRenderer::CalculateCenterPoint( const int boneIndex, void *positions, 
	void *bones, PxU32 stride, const PxU32 numVerts, OUT PxVec3 &out )
{
	int count=0;
	PxVec3 center(0,0,0);
	for (PxU32 i=0; i < numVerts; ++i)
	{
		const PxVec3 &p = *(PxVec3*)(((PxU8*)positions) + (stride * i));
		if (bones)
		{
			const PxU32 &b  =  *(PxU32*)(((PxU8*)bones) + (stride * i));
			if (boneIndex == b)
			{
				center +=p;
				++count;
			}
		}
		else
		{
			center +=p;
			++count;
		}
	}

	center /= (float)count;
	out = center;
}
