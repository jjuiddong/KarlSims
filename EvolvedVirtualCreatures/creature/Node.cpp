#include "stdafx.h"
#include "node.h"
#include "../EvolvedVirtualCreatures.h"
#include "Joint.h"
#include "NeuralNet.h"
#include "AngularSensor.h"
#include "MuscleEffector.h"


using namespace evc;


CNode::CNode(CEvc &sample) :
	m_Sample(sample)
,	m_pBrain(NULL)
,	m_pBody(NULL)
,	m_pParentJointSensor(NULL)
{

}

CNode::~CNode()
{
	m_pBody = NULL;
	SAFE_DELETE(m_pBrain);
	BOOST_FOREACH (auto joint, m_Joints)
	{
		SAFE_DELETE(joint);
	}
	BOOST_FOREACH (auto p, m_Sensors)
	{
		SAFE_DELETE(p);
	}
	BOOST_FOREACH (auto p, m_Effectors)
	{
		SAFE_DELETE(p);
	}
	SAFE_DELETE(m_pParentJointSensor);
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
 @date 2013-12-10
*/
void CNode::InitNeuron()
{
	int count = GetNeuronCount();
	RET(count <= 0);

	if (m_pParentJointSensor)
		++count;

	m_Nerves.resize(count);
	
	SAFE_DELETE(m_pBrain);
	const int outputCount = m_Effectors.size();
	m_pBrain = new CNeuralNet(count, outputCount, 1, count+1); 
}


/**
 @brief 
 @date 2013-12-03
*/
void CNode::Move(float dtime)
{
	BOOST_FOREACH(auto joint, m_Joints)
		joint->Move(dtime);

	UpdateNeuron(dtime);
}


/**
 @brief Update Neuron
 @date 2013-12-10
*/
void CNode::UpdateNeuron(float dtime)
{
	int count = GetNeuronCount();
	RET(count <= 0);

	if (m_pParentJointSensor)
		++count;

	vector<double> nerves;
	nerves.reserve(count);

	if (m_pParentJointSensor) // get parent joint sensor output
		nerves.push_back(m_pParentJointSensor->GetOutput());

	GetOutputNerves(nerves);

	if (m_pBrain)
	{
		vector<double> output = m_pBrain->Update(nerves);
		for (u_int i=0; i < output.size(); ++i)
			m_Effectors[ i]->Signal(dtime, output[ i]);
	}
}


/**
 @brief return total neuron count
 @date 2013-12-10
*/
int CNode::GetNeuronCount() const
{
	int count = 0;
	BOOST_FOREACH(auto &joint, m_Joints)
	{
		count += joint->GetNeuronCount();
	}

	count += m_Sensors.size();

	return count;
}


/**
 @brief 
 @date 2013-12-10
*/
void CNode::GetOutputNerves(OUT vector<double> &out) const
{
	BOOST_FOREACH(auto &joint, m_Joints)
	{
		joint->GetOutputNerves(out);
	}

	BOOST_FOREACH (auto &sensor, m_Sensors)
	{
		out.push_back( sensor->GetOutput() );
	}
}

