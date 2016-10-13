/**
 @filename Utility.h
 
	utility functions 
*/
#pragma once


#include "../genoype/GenotypeParser.h"

namespace SampleRenderer { class RendererProjection; }

namespace utility
{
	/**
	 @brief 
	 @date 2014-02-10
	*/
	inline void quatRotationArc(PxQuat &q, const PxVec3& v0, const PxVec3& v1)
	{
		PxVec3 c = v0.cross(v1);
		if (c.isZero())
		{
			q = PxQuat(0,PxVec3(1,0,0));
			return;
		}

		double s = sqrtf((1.0f+v0.dot(v1))*2.0f);
		q.x = c.x/s;
		q.y = c.y/s;
		q.z = c.z/s;
		q.w = s/2.0f;
	}


	/**
	 @brief SVec3 -> PxVec3
	 @date 2014-02-24
	*/
	inline const PxVec3 Vec3toPxVec3(const evc::genotype_parser::SVec3 &rhs)
	{
		return PxVec3(rhs.x, rhs.y, rhs.z);
	}

		/**
	 @brief PxVec3 -> SVec3
	 @date 2014-02-24
	*/
	inline const evc::genotype_parser::SVec3 PxVec3toVec3(const  PxVec3 &rhs)
	{
		return evc::genotype_parser::SVec3(rhs.x, rhs.y, rhs.z);
	}


	/**
	 @brief convertD3D9
	 @date 2014-02-24
	*/
	inline void convertD3D9(D3DMATRIX &dxmat, const physx::PxMat44 &mat)
	{
		memcpy(&dxmat._11, mat.front(), 4 * 4 * sizeof (float));
	}


	void convertD3D9(D3DMATRIX &dxmat, const SampleRenderer::RendererProjection &mat);


	void lerp(OUT PxVec3 &out, const PxVec3 &a, const PxVec3 &b, float t);
	void bezier(OUT PxVec3 &out, const vector<PxVec3> &points, const float t);

}
