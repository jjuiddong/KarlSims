
#include "stdafx.h"
#include "RenderComposition.h"
#include "RenderCompositionShape.h"
#include "RendererShape.h"
#include "RaycastCCD.h"


using namespace physx;
using namespace SampleRenderer;


/**
 @brief 
 @date 2014-01-06
*/
RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	const int parentShapeIndex, const int childShapeIndex, 
	const int paletteIndex,
	const vector<PxTransform> &tmPalette,
	RendererCompositionShape *shape0, const PxTransform &tm0, 
	RendererCompositionShape *shape1, const PxTransform &tm1, 
	const PxReal* uvs) :
	m_PaletteIndex(paletteIndex)
{
	RendererShape* rs = new RendererCompositionShape(renderer, parentShapeIndex, childShapeIndex, paletteIndex, tmPalette, shape0, tm0, shape1, tm1, uvs);
	setRenderShape(rs);
}


/**
 @brief 
 @date 2014-01-06
*/
RenderComposition::RenderComposition(SampleRenderer::Renderer& renderer, 
	const int paletteIndex, const vector<PxTransform> &tmPalette, SampleRenderer::RendererShape *shape0,
	RenderMaterial *material0 ) : 
	m_PaletteIndex(paletteIndex)
{
	RendererShape* rs = new RendererCompositionShape(renderer, paletteIndex, tmPalette, shape0, material0);
	setRenderShape(rs);
}


RenderComposition::RenderComposition(const RenderComposition& src) : RenderBaseActor(src)
{
}

RenderComposition::~RenderComposition()
{
}


/**
 @brief 
 @date 2014-01-05
*/
void RenderComposition::render(SampleRenderer::Renderer& renderer, RenderMaterial* material, bool wireFrame)
{
	if (!mEnableRender)
		return;

	((RendererCompositionShape*)getRenderShape())->ApplyPalette();
	mScaledTransform = PxMat44::createIdentity();
	RenderBaseActor::render(renderer, material, wireFrame);
}


/**
 @brief 
 @date 2014-01-10
*/
void RenderComposition::update(float deltaTime)
{
	// Setup render transform from physics transform, if physics object exists
	if(mPhysicsShape)
	{
		if(!mArticulationLink && ( !mDynamicActor || mDynamicActor->isSleeping()))
			return;

		PxTransform newPose = PxShapeExt::getGlobalPose(*mPhysicsShape, *mPhysicsActor);
		PxVec3 newShapeCenter = getShapeCenter(mPhysicsActor, mPhysicsShape, mCCDWitnessOffset);

		bool updateCCDWitness = true;
		if(mEnableCCD)
			updateCCDWitness = doRaycastCCD(mPhysicsActor, mPhysicsShape, newPose, newShapeCenter, mCCDWitness, mCCDWitnessOffset);

		// Copy physics pose to graphics pose
		setTransform(PxTransform(newPose.p, newPose.q * mPhysicsToGraphicsRot));

		if(updateCCDWitness)
			mCCDWitness = newShapeCenter;

		// update worldBounds In Creature class
		//setWorldBounds(PxShapeExt::getWorldBounds(*mPhysicsShape, *mPhysicsActor));
	}
}
