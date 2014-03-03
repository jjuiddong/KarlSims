#include "stdafx.h"
#include "phenotypenode.h"
#include "../EvolvedVirtualCreatures.h"
#include "Joint.h"
#include "NeuralNet.h"
#include "AngularSensor.h"
#include "MuscleEffector.h"
#include "../renderer/RenderComposition.h"


using namespace evc;


CPhenotypeNode::CPhenotypeNode(CEvc &sample) :
	m_Sample(sample)
,	m_pBrain(NULL)
,	m_pBody(NULL)
,	m_pShape(NULL)
,	m_pParentJointSensor(NULL)
,	m_pShapeRenderer(NULL)
,	m_pOriginalShapeRenderer(NULL)
,	m_PaletteIndex(0)
,	m_IsTerminalNode(false)
,	m_IsKinematic(false)
{

}

CPhenotypeNode::~CPhenotypeNode()
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

	m_Sample.removeRenderObject(m_pOriginalShapeRenderer);
	if (m_pOriginalShapeRenderer != m_pShapeRenderer)
		m_Sample.removeRenderObject(m_pShapeRenderer);
}


/**
 @brief 
 @date 2013-12-05
*/
void CPhenotypeNode::setCollisionGroup(PxRigidActor* actor, PxU32 group)
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
 @brief Brain is only one in Nodes
 @date 2013-12-10
*/
void CPhenotypeNode::InitBrain(const vector<double> &weights) // weights = vector<double>()
{
	//int count = GetNeuronCount();
	//RET(count <= 0);

	//if (m_pParentJointSensor)
	//	++count;

	//m_Nerves.resize(count);
	m_AllConnectSensor.clear();
	m_AllConnectEffector.clear();
	GetAllSensor(m_AllConnectSensor);
	GetAllEffector(m_AllConnectEffector);

	int inputCount = 0;
	BOOST_FOREACH (auto &sensor, m_AllConnectSensor)
	{
		inputCount += sensor->GetOutputCount();
	}
	const int outputCount = m_AllConnectEffector.size();

	SAFE_DELETE(m_pBrain);
	m_pBrain = new CNeuralNet(inputCount, outputCount, 1, inputCount); 

	if (!weights.empty())
		m_pBrain->PutWeights(weights);
}


/**
 @brief 
 @date 2013-12-03
*/
void CPhenotypeNode::Move(float dtime)
{
	BOOST_FOREACH(auto joint, m_Joints)
		joint->Move(dtime);

	if (m_pBody && m_pShape)
		m_worldBounds = PxShapeExt::getWorldBounds(*m_pShape, *m_pBody);

	UpdateNeuron(dtime);
}


/**
 @brief Update Neuron
 @date 2013-12-10
*/
void CPhenotypeNode::UpdateNeuron(float dtime)
{
	//int count = GetNeuronCount();
	//RET(count <= 0);

	//if (m_pParentJointSensor)
	//	++count;

	//vector<double> nerves;
	//nerves.reserve(count);

	//if (m_pParentJointSensor) // get parent joint sensor output
	//{
	//	const vector<double> &out = m_pParentJointSensor->GetOutput();
	//	BOOST_FOREACH (auto val, out)
	//		nerves.push_back(val);
	//}

	//GetOutputNerves(nerves);

	//if (m_pBrain)
	//{
	//	vector<double> output = m_pBrain->Update(nerves);
	//	for (u_int i=0; i < output.size(); ++i)
	//		m_Effectors[ i]->Signal(dtime, output[ i]);
	//}

	RET(!m_pBrain);

	vector<double> input;
	input.reserve(m_AllConnectSensor.size());
	BOOST_FOREACH (auto &sensor, m_AllConnectSensor)
	{
		const vector<double> &output = sensor->GetOutput();
		BOOST_FOREACH (auto &val, output)
			input.push_back(val);
	}

	vector<double> output = m_pBrain->Update(input);
	for (u_int i=0; i < output.size(); ++i)
		m_AllConnectEffector[ i]->Signal(dtime, output[ i]);
}


/**
 @brief return total neuron count
 @date 2013-12-10
*/
int CPhenotypeNode::GetNeuronCount() const
{
	int count = 0;
	BOOST_FOREACH(auto &joint, m_Joints)
	{
		count += joint->GetNeuronCount();
	}

	BOOST_FOREACH (auto &sensor, m_Sensors)
	{
		count += sensor->GetOutputCount();
	}

	return count;
}


/**
 @brief 
 @date 2013-12-10
*/
void CPhenotypeNode::GetOutputNerves(OUT vector<double> &out) const
{
	BOOST_FOREACH(auto &joint, m_Joints)
	{
		joint->GetOutputNerves(out);
	}

	BOOST_FOREACH (auto &sensor, m_Sensors)
	{
		const vector<double> &output = sensor->GetOutput();
		BOOST_FOREACH (auto &val, output)
			out.push_back(val);
	}
}


/**
 @brief 
 @date 2013-12-20
*/
void CPhenotypeNode::GetAllSensor(OUT vector<CSensor*> &out) const
{
	BOOST_FOREACH (auto &sensor, m_Sensors)
	{
		out.push_back(sensor);
	}
	BOOST_FOREACH (auto &joint, m_Joints)
	{
		joint->GetActor1()->GetAllSensor(out);
	}
}


/**
 @brief 
 @date 2013-12-20
*/
void CPhenotypeNode::GetAllEffector(OUT vector<CEffector*> &out) const
{
	BOOST_FOREACH (auto &effector, m_Effectors)
	{
		out.push_back(effector);
	}
	BOOST_FOREACH (auto &joint, m_Joints)
	{
		joint->GetActor1()->GetAllEffector(out);
	}
}

