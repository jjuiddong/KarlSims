// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2013 NVIDIA Corporation. All rights reserved.
#ifndef GXM_RENDERER_MATERIAL_H
#define GXM_RENDERER_MATERIAL_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include <RendererMaterial.h>

#include "GXMRenderer.h"


namespace SampleRenderer
{


	class GXMVariable : public RendererMaterial::Variable
	{
	public:
		GXMVariable(const char *name, RendererMaterial::VariableType type, PxU32 offset)
			: Variable(name, type, offset)
		{
		}

		uint32_t m_resourceIndex;
	};

	class GXMRendererMaterial : public RendererMaterial
	{
		friend class OGLRendererMesh;

		struct ShaderConstant
		{
			const SceGxmProgramParameter* mModelMatrixUniform;
			const SceGxmProgramParameter* mViewMatrixUniform;
			const SceGxmProgramParameter* mProjMatrixUniform;
			const SceGxmProgramParameter* mModelViewMatrixUniform;
			const SceGxmProgramParameter* mBoneMatricesUniform;
			const SceGxmProgramParameter* mAmbientColor;

			const SceGxmProgramParameter* mModelViewProjMatrixUniform;
			
			const SceGxmProgramParameter* mFogColorAndDistance;

			const SceGxmProgramParameter* mEyePosition;
			const SceGxmProgramParameter* mEyeDirection;			

			const SceGxmProgramParameter* mLightColor;
			const SceGxmProgramParameter* mLightIntensity;
			const SceGxmProgramParameter* mLightDirection;
			const SceGxmProgramParameter* mLightPosition;
			const SceGxmProgramParameter* mLightInnerRadius;
			const SceGxmProgramParameter* mLightOuterRadius;
			const SceGxmProgramParameter* mLightInnerCone;
			const SceGxmProgramParameter* mLightOuterCone;
			const SceGxmProgramParameter* mLightShadowMap;
			const SceGxmProgramParameter* mLightShadowMatrix;

			const SceGxmProgramParameter* mShadeMode;

			const SceGxmProgramParameter* mVfaceScale;
		};

		struct FragmentProgram
		{
			RendererMaterial::Pass	mPass;
			const SceGxmProgram *	mGmxFragmentProgram;
			SceGxmShaderPatcherId	mGmxFragmentProgramId;
			SceGxmFragmentProgram*	mFinalFragmentProgram;
			ShaderConstant			mShaderConstant;
		};

	public:
		GXMRendererMaterial(GXMRenderer &renderer, const RendererMaterialDesc &desc);
		virtual ~GXMRendererMaterial(void);
		virtual void setModelMatrix(const float *matrix) 
		{
			PX_FORCE_PARAMETER_REFERENCE(matrix);
			PX_ALWAYS_ASSERT();
		}

		const GXMRendererMaterial::FragmentProgram* getFramentProgram(RendererMaterial::Pass) const;
		SceGxmFragmentProgram* getFinalFramentProgram(RendererMaterial::Pass) const;
		SceGxmVertexProgram* getFinalVertexProgram() const { return mFinalVertexProgram; }		

		uint32_t getResourceIndex(const char* name) const;

	private:
		virtual const Renderer& getRenderer() const { return mRenderer; }
		virtual void bind(RendererMaterial::Pass pass, RendererMaterialInstance *materialInstance, bool instanced) const;
		virtual void bindMeshState(bool instanced) const;
		virtual void unbind(void) const;
		virtual void bindVariable(Pass pass, const Variable &variable, const void *data) const;

		void loadCustomConstants(const SceGxmProgram* program, bool fragment = false);
		void loadShaderConstants(const SceGxmProgram* program, ShaderConstant* sc);
		RendererMaterial::VariableType gxmTypetoVariableType(SceGxmParameterType type, SceGxmParameterCategory category, uint32_t componentCount, uint32_t arraysize);
		void bindEnvironment(const GXMRenderer::ShaderEnvironment& env, bool instanced) const;

		void fillShaderAttribute(const GXMRenderer::ShaderEnvironment& env, GXMVariable* var, SceGxmVertexAttribute& attribute) const;

		void parseFragmentShader(const SceGxmProgram*, RendererMaterial::Pass );

	private:
		GXMRendererMaterial &operator=(const GXMRendererMaterial&) { return *this; }

	private:
		friend class GXMRenderer;
		GXMRenderer &			mRenderer;

		const SceGxmProgram *	mGmxVertexProgram;
		SceGxmShaderPatcherId	mGmxVertexProgramId;

		std::vector<FragmentProgram> mFragmentPrograms;

		PxU16					mNumVertexAttributes;
		SceGxmBlendInfo*		mParticleBlendInfo;
		
		mutable SceGxmVertexProgram*	mFinalVertexProgram;		

		mutable void *mVertexUniformDefaultBuffer;
		mutable void *mFragmentUniformDefaultBuffer;

		char	mFragmentProgramName[512];
		char	mVertexProgramName[512];
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
#endif
