
#include "stdafx.h"
#include "Creature.h"
#include "Node.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "Joint.h"
#include "NeuralNet.h"
#include "AngularSensor.h"
#include "PhotoSensor.h"
#include "VisionSensor.h"
#include "MuscleEffector.h"



using namespace evc;


CCreature::CCreature(CEvc &sample) :
	m_Sample(sample)
,	m_pRoot(NULL)
{

}

CCreature::~CCreature()
{
	//m_pRoot = NULL;
	//BOOST_FOREACH (auto &p, m_Nodes)
	//{
	//	SAFE_DELETE(p);
	//}
	m_Nodes.clear();
	SAFE_DELETE(m_pRoot);
}


/**
 @brief 
 @date 2013-12-09
*/
void CCreature::GenerateByGenotype(const string &genotypeScriptFileName, const PxVec3 &initialPos)
{
	PxSceneWriteLock scopedLock(m_Sample.getActiveScene());

	genotype_parser::CGenotypeParser parser;
	genotype_parser::SExpr *pexpr = parser.Parse(genotypeScriptFileName);
	m_pRoot = GenerateByGenotype(pexpr, g_pDbgConfig->generationRecursiveCount, initialPos);
	m_pRoot->InitBrain();

	m_Genome.fitness = 0;
	m_Genome.chromo.clear();
	m_Genome.chromo.reserve(64);
	GetChromo(this, pexpr, m_Genome.chromo);

	m_InitialPos = m_pRoot->GetBody()->getGlobalPose().p;
	m_InitialPos.y = 0;

	genotype_parser::RemoveExpression(pexpr);
}


/**
 @brief 
 @date 2013-12-13
*/
void CCreature::GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos)
{
	m_Genome = genome;

	vector<double> weights;
	genotype_parser::SExpr *p = BuildExpr(genome.chromo, weights);
	m_pRoot = GenerateByGenotype(p, g_pDbgConfig->generationRecursiveCount, initialPos);
	m_pRoot->InitBrain(weights);

	m_InitialPos = m_pRoot->GetBody()->getGlobalPose().p;
	m_InitialPos.y = 0;

	genotype_parser::RemoveExpression(p);
}


/**
 @brief create creature by genotype script
 @date 2013-12-06
*/
CNode* CCreature::GenerateByGenotype( const genotype_parser::SExpr *pexpr, const int recursiveCnt, const PxVec3 &initialPos )
{
	if (!pexpr)
		return NULL;
	if (recursiveCnt < 0)
		return NULL;

	// Generate Body
	const PxVec3 pos = initialPos;
	CNode *pNode = new CNode(m_Sample);
	pNode->m_Name = pexpr->id;
	PxVec3 dimension(pexpr->dimension.x, pexpr->dimension.y, pexpr->dimension.z);
	{ // rand dimension
		PxVec3 dimRand1 = dimension * 0.2f;
		PxVec3 dimRand(RandFloat()*dimRand1.x, RandFloat()*dimRand1.y, RandFloat()*dimRand1.z);
		if (RandFloat() > 0.5f) dimRand.x = -dimRand.x;
		if (RandFloat() > 0.5f) dimRand.y = -dimRand.y;
		if (RandFloat() > 0.5f) dimRand.z = -dimRand.z;

		dimension += dimRand;
		dimension.x = max(dimension.x, 0.1f);
		dimension.y = max(dimension.y, 0.1f);
		dimension.z = max(dimension.z, 0.1f);
	}

	MaterialIndex material = GetMaterialType(pexpr->material);
	const float mass = pexpr->mass;

	if (boost::iequals(pexpr->shape, "box"))
	{
		pNode->m_pBody = m_Sample.createBox(pos+PxVec3(0,0,0), dimension, NULL, m_Sample.getManageMaterial(material), mass);
	}
	else if (boost::iequals(pexpr->shape, "sphere"))
	{
		pNode->m_pBody = m_Sample.createSphere(pos+PxVec3(0,0,0), dimension.x, NULL, m_Sample.getManageMaterial(material), mass);
	}

	// Generate Connection 
	PxRigidDynamic* body = pNode->m_pBody;
	genotype_parser::SConnectionList *pConnect = pexpr->connection;
	while (pConnect)
	{
		if (boost::iequals(pConnect->connect->conType, "joint"))
		{
			CNode *pChildNode = GenerateByGenotype( pConnect->connect->expr, recursiveCnt-1, initialPos );
			if (pChildNode)
			{
				 CreateJoint(pNode, pChildNode, pConnect->connect);
			 }
		}
		else if(boost::iequals(pConnect->connect->conType, "sensor"))
		{
			CreateSensor(pNode, pConnect->connect, pos);
		}

		pConnect = pConnect->next;
	}

	//pNode->InitNeuron();
	m_Nodes.push_back(pNode);
	return pNode;
}


/**
 @brief 
 @date 2013-12-19
*/
void CCreature::CreateJoint( CNode *parentNode, CNode *childNode, genotype_parser::SConnection *connect)
{
	RET(!childNode);
	RET(!parentNode);

	PxRigidDynamic* body = parentNode->m_pBody;
	PxRigidDynamic *child = childNode->m_pBody;
	genotype_parser::SConnection *joint = connect;
	PxVec3 dir0(joint->parentOrient.dir.x, joint->parentOrient.dir.y, joint->parentOrient.dir.z);
	PxVec3 dir1(joint->orient.dir.x, joint->orient.dir.y, joint->orient.dir.z);
	PxVec3 pos(joint->pos.x, joint->pos.y, joint->pos.z);
	PxVec3 limit(joint->limit.x, joint->limit.y, joint->limit.z);
	PxVec3 velocity(joint->velocity.x, joint->velocity.y, joint->velocity.z);

	PxTransform tm0 = (dir0.isZero())? PxTransform::createIdentity() : PxTransform(PxQuat(joint->parentOrient.angle, dir0));
	PxTransform tm1 = (dir1.isZero())? PxTransform(PxVec3(pos)) : 
		(PxTransform(PxQuat(joint->orient.angle, dir1)) * PxTransform(PxVec3(pos)));

	PxJoint* pxJoint = NULL;

	if (boost::iequals(joint->type, "fixed"))
	{
		PxFixedJoint *j = PxFixedJointCreate(m_Sample.getPhysics(), 
			body, tm0, 
			child, tm1 );
		pxJoint = j;
	}
	else if(boost::iequals(joint->type, "spherical"))
	{
		if (PxSphericalJoint *j = PxSphericalJointCreate(m_Sample.getPhysics(), 
			body, tm0,
			child, tm1) )
		{
			if (!limit.isZero())
			{
				j->setLimitCone(PxJointLimitCone(limit.x, limit.y, PxSpring(0,0)));
				j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
			}

			j->setProjectionLinearTolerance(0.0f);
			j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
			pxJoint = j;
		}
	}
	else if (boost::iequals(joint->type, "revolute"))
	{
		if (PxRevoluteJoint*j = PxRevoluteJointCreate(m_Sample.getPhysics(), 
			body, tm0,
			child, tm1) )
		{
			if (!limit.isZero())
			{
				j->setLimit(PxJointAngularLimitPair(limit.x, limit.y, limit.z)); // upper, lower, tolerance
				j->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
			}

			if (!velocity.isZero())
			{
				j->setDriveVelocity(velocity.x);
				j->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
			}

			j->setProjectionLinearTolerance(0.0f);
			j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
			pxJoint = j;
		}
	}

	CJoint *pJoint = new CJoint(parentNode, childNode, pxJoint, velocity.x, joint->period);
	CAngularSensor *pSensor = new CAngularSensor();
	CMuscleEffector *pEffector = new CMuscleEffector(joint->period);
	pJoint->ApplySensor(*pSensor);
	pJoint->ApplyEffector(*pEffector);

	parentNode->m_Joints.push_back(pJoint);
	parentNode->m_Sensors.push_back(pSensor);
	parentNode->m_Effectors.push_back(pEffector);

	{ // add child
		CAngularSensor *pChildSensor = new CAngularSensor();
		pJoint->ApplySensor(*pChildSensor);
		childNode->m_pParentJointSensor = pChildSensor;
		//childNode->InitNeuron();
	}
}


/**
 @brief 
 @date 2013-12-19
*/
CNode* CCreature::CreateSensor(CNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos)
{
	RETV(!parentNode, NULL);

	CNode *childNode = NULL;

	CNode *pNode = new CNode(m_Sample);
	PxVec3 dimension(0.05f, 0.2f, 0.05f);
	MaterialIndex material = GetMaterialType("red");
	{
		pNode->m_pBody = m_Sample.createBox(initialPos, dimension, NULL, m_Sample.getManageMaterial(material), 1.f);
	}
	childNode = pNode;

	PxRigidDynamic* body = parentNode->m_pBody;
	PxRigidDynamic *child = childNode->m_pBody;
	genotype_parser::SConnection *joint = connect;
	PxVec3 dir0(joint->parentOrient.dir.x, joint->parentOrient.dir.y, joint->parentOrient.dir.z);
	PxVec3 dir1(joint->orient.dir.x, joint->orient.dir.y, joint->orient.dir.z);
	PxVec3 pos(joint->pos.x, joint->pos.y, joint->pos.z);
	PxVec3 limit(joint->limit.x, joint->limit.y, joint->limit.z);
	PxVec3 velocity(joint->velocity.x, joint->velocity.y, joint->velocity.z);

	PxTransform tm0 = (dir0.isZero())? PxTransform::createIdentity() : PxTransform(PxQuat(joint->parentOrient.angle, dir0));
	PxTransform tm1 = (dir1.isZero())? PxTransform(PxVec3(pos)) : 
		(PxTransform(PxQuat(joint->orient.angle, dir1)) * PxTransform(PxVec3(pos)));

	PxJoint* pxJoint = NULL;
	if (PxFixedJoint *j = PxFixedJointCreate(m_Sample.getPhysics(), body, tm0, child, tm1 ))
	{
		pxJoint = j;
	}
	
	CJoint *pJoint = new CJoint(parentNode, childNode, pxJoint, velocity.x, joint->period);

	CSensor *sensor = NULL;
	if (boost::iequals(joint->type, "photo"))
	{
		childNode->m_Name = "photo sensor";
		sensor = new CPhotoSensor(m_Sample);
		((CPhotoSensor*)sensor)->SetSensorInfo(childNode, PxVec3(0,1,0), 10);
	}
	else if (boost::iequals(joint->type, "vision"))
	{
		childNode->m_Name = "vision sensor";
		sensor = new CVisionSensor(m_Sample);
		((CVisionSensor*)sensor)->SetSensorInfo(childNode, 10);
	}
	
	parentNode->m_Joints.push_back(pJoint);
	parentNode->m_Sensors.push_back(sensor);

	return childNode;
}


/**
 @brief genotype script material field to MaterialIndex value
 @date 2013-12-07
*/
MaterialIndex CCreature::GetMaterialType(const string &materialStr)
{
	const static string materials[] = {"grey", "red", "green", "blue", "yellow"};
	const int size = sizeof(materials) / sizeof(string);

	for (int i=0; i < size; ++i)
	{
		if (materialStr == materials[ i])
		{
			return (MaterialIndex)(i + MATERIAL_GREY);
		}
	}
	return MATERIAL_GREY;
}


/**
 @brief 
 @date 2013-12-09
*/
void CCreature::Move(float dtime)
{
	BOOST_FOREACH (auto &node, m_Nodes)
		node->Move(dtime);

	if (m_pRoot)
	{
		PxVec3 pos = m_pRoot->GetBody()->getGlobalPose().p;
		pos.y = 0;
		PxVec3 len =  pos - m_InitialPos;
		m_Genome.fitness = len.magnitude();
	}
}


/**
 @brief 
 @date 2013-12-19
*/
PxVec3 CCreature::GetPos() const
{
	RETV(!m_pRoot, PxVec3());
	return m_pRoot->GetBody()->getGlobalPose().p;
}


/**
 @brief 
 @date 2013-12-20
*/
const CNeuralNet* CCreature::GetBrain() const
{
	RETV(!m_pRoot, NULL);
	return m_pRoot->GetBrain();
}
