#include "stdafx.h"
#include "node.h"
#include "../EvolvedVirtualCreatures.h"
#include "Joint.h"
#include "NeuralNet.h"


using namespace evc;


void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;	// word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*)*numShapes);
	actor->getShapes(shapes, numShapes);
	for(PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	SAMPLE_FREE(shapes);
}


CNode::CNode(CEvc &sample) :
	m_Sample(sample)
//,	m_Force(1.f)
//,	m_ElapseT(0)
,	m_pBrain(NULL)
,	m_pBody(NULL)
{
	m_pBrain = new CNeuralNet(2, 1, 0, 0); 

}

CNode::~CNode()
{
	SAFE_DELETE(m_pBrain);
	BOOST_FOREACH (auto joint, m_Joints)
	{
		SAFE_DELETE(joint);
	}
}


/**
 @brief 
 @date 2013-12-05
*/
void CNode::setCollisionGroup(PxRigidActor* actor, PxU32 group)
{
	PxSceneWriteLock scopedLock(m_Sample.getActiveScene());

	PxU32 nbShapes = actor->getNbShapes();
	if( nbShapes )
	{
		SampleArray<PxShape*> shapes(nbShapes);
		actor->getShapes( &shapes[0], nbShapes);
		for( PxU32 j = 0; j < nbShapes; j++)
		{
			PxFilterData fd = shapes[j]->getSimulationFilterData();
			fd.word0 = group;
			shapes[j]->setSimulationFilterData(fd);
		}
	}
}


/**
 @brief 
 @date 2013-12-03
*/
void CNode::Move(float dtime)
{
	BOOST_FOREACH(auto joint, m_Joints)
		joint->Move(dtime);
}

