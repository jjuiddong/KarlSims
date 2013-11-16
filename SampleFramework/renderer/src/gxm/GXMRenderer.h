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
#ifndef GXM_RENDERER_H
#define GXM_RENDERER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

class SceGxmContext;
class SceGxmRenderTarget;

#include <Renderer.h>
#include <GraphicsUtility.h>
#include <vectormath.h>

namespace SampleFramework {
	class SamplePlatform;
}

namespace SampleRenderer
{
	class GXMRendererMaterial;
	class GXMRenderer;
	class GXMRendererVertexBuffer;
	class GXMRendererInstanceBuffer;
	class GXMRendererIndexBuffer;

	typedef struct DisplayData{
		void *			m_address;
		GXMRenderer*	m_renderer;
	};


	class GXMRenderer : public Renderer
	{
	public:
		GXMRenderer(const RendererDesc &desc, const char* assetDir);
		virtual ~GXMRenderer(void);



		const PxMat44 &getViewMatrix(void) const { return m_viewMatrix; }

		void                       setCurrentMaterial(const GXMRendererMaterial *cm) { m_currMaterial = cm;   }
		const GXMRendererMaterial *getCurrentMaterial(void)                          { return m_currMaterial; }

		class ShaderEnvironment
		{
		public:
			sce::Vectormath::Scalar::Aos::Matrix4   mModelMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mViewMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mProjMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mModelViewMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mModelViewProjMatrix;

			sce::Vectormath::Scalar::Aos::Matrix4	mModelViewProjMatrixUniform;

			float									mFogColorAndDistance;

			sce::Vectormath::Scalar::Aos::Vector4	mLightColor;
			float									mLightIntensity;
			float									mLightDirection[4];
			float									mLightPosition[4];
			float									mLightInnerRadius;
			float									mLightOuterRadius;
			float									mLightInnerCone;
			float									mLightOuterCone;

			float									mEyePosition[4];
			float									mEyeDirection[4];

			sce::Vectormath::Scalar::Aos::Vector4   mAmbientColor;

			SceGxmPrimitiveType						mFillMode;

			const GXMRendererVertexBuffer*			mVertexBuffer;
			const GXMRendererInstanceBuffer*		mInstanceBuffer;
			const GXMRendererIndexBuffer*			mIndexBufffer;
			PxU32									mNumIndices;
			PxU32									mNumVertices;
			PxU32									mNumInstances;

			bool									mValid;
			
		public:
			ShaderEnvironment(void);
		};

	private:
		bool begin(void);
		void end(void);
		void checkResize(void);

	public:
		// clears the offscreen buffers.
		virtual void clearBuffers(void);

		// presents the current color buffer to the screen.
		// returns true on device reset and if buffers need to be rewritten.
		virtual bool swapBuffers(void);

		// get the device pointer (void * abstraction)
		virtual void *getDevice(); 

		virtual void finishRendering();

		// get the window size
		void getWindowSize(PxU32 &width, PxU32 &height) const;

		virtual RendererVertexBuffer   *createVertexBuffer(  const RendererVertexBufferDesc   &desc);
		virtual RendererIndexBuffer    *createIndexBuffer(   const RendererIndexBufferDesc    &desc);
		virtual RendererInstanceBuffer *createInstanceBuffer(const RendererInstanceBufferDesc &desc);
		virtual RendererTexture2D      *createTexture2D(     const RendererTexture2DDesc      &desc);
		virtual RendererTexture3D      *createTexture3D(     const RendererTexture3DDesc      &desc);
		virtual RendererTarget         *createTarget(        const RendererTargetDesc         &desc);
		virtual RendererMaterial       *createMaterial(      const RendererMaterialDesc       &desc);
		virtual RendererMesh           *createMesh(          const RendererMeshDesc           &desc);
		virtual RendererLight          *createLight(         const RendererLightDesc          &desc);

		virtual bool captureScreen(const char* filename);

		void renderDebugText( const DisplayData* displayData );

		static void *graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid);
		static void graphicsFree(SceUID uid);

		SceGxmShaderPatcher* getShaderPatcher() const { return m_shaderPatcher; }
		SceGxmContext*	getContext() const { return m_graphicsData.pContext; }

		const ShaderEnvironment& getShaderEnvironment() const { return m_environment; }
		ShaderEnvironment& getShaderEnvironment() { return m_environment; }

	private:		
		virtual void bindViewProj(const physx::PxMat44 &eye, const RendererProjection &proj);
		virtual void bindAmbientState(const RendererColor &ambientColor);
		virtual void bindFogState(const RendererColor &fogColor, float fogDistance);
		virtual void bindDeferredState(void);
		virtual void bindMeshContext(const RendererMeshContext &context);
		virtual void beginMultiPass(void);
		virtual void endMultiPass(void);
		virtual void renderDeferredLight(const RendererLight &light);
		virtual PxU32 convertColor(const RendererColor& color) const;
		virtual void beginTransparentMultiPass(void)		{}
		virtual void endTransparentMultiPass(void)			{}
		virtual void setVsync(bool on)						{}
		virtual bool captureScreen(PxU32 &width, PxU32& height, PxU32& sizeInBytes, const void*& screenshotData) { return false; }

		virtual bool isOk(void) const;

		virtual	void setupTextRenderStates();
		virtual	void resetTextRenderStates();
		virtual	void renderTextBuffer(const void* vertices, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material);
		virtual void renderLines2D(const void* vertices, PxU32 nbVerts, RendererMaterial* material);  
		virtual	void setupScreenquadRenderStates();
		virtual	void resetScreenquadRenderStates();
		virtual bool initTexter();

		// GXM implementation
		void initGXM();

	private:
		SampleFramework::SamplePlatform*			   m_platform;


		GraphicsUtilContextData		m_graphicsData;
		SceGxmRenderTarget *		m_renderTarget;
		DisplayData					m_displayData;
		SceGxmShaderPatcher	*		m_shaderPatcher;

		const GXMRendererMaterial *m_currMaterial;

		PxU32                      m_displayWidth;
		PxU32                      m_displayHeight;

		void*						m_TextRendererVertexBuffer;
		void*						m_TextRendererIndexBuffer;

		PxU32						m_CurrentTextRendererVertexBufferSize;
		PxU32						m_CurrentTextRendererIndexBufferSize;
		PxU32						m_TextMaxVerts;		
		PxU32						m_TextMaxIndices;
		SceUID						m_TextRendererVertexBufferUid;
		SceUID						m_TextRendererIndexBufferUid;

		ShaderEnvironment              m_environment;

		PxMat44 m_viewMatrix;
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
#endif
