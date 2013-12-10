
#include "stdafx.h"
#include "Creature.h"
#include "Node.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "Joint.h"
#include "NeuralNet.h"
#include "AngularSensor.h"
#include "MuscleEffector.h"



using namespace evc;


CCreature::CCreature(CEvc &sample) :
	m_Sample(sample)
,	m_pRoot(NULL)
{

}

CCreature::~CCreature()
{
	m_pRoot = NULL;
	BOOST_FOREACH (auto &p, m_Nodes)
	{
		SAFE_DELETE(p);
	}
}


/**
 @brief 
 @date 2013-12-09
*/
void CCreature::GenerateByGenotype(const string &genotypeScriptFileName)
{
	PxSceneWriteLock scopedLock(m_Sample.getActiveScene());

	genotype_parser::CGenotypeParser parser;
	genotype_parser::SExpr *pexpr = parser.Parse(genotypeScriptFileName);
	m_pRoot = GenerateByGenotype(pexpr, g_pDbgConfig->generationRecursiveCount);
	genotype_parser::RemoveExpression(pexpr);
}


/**
 @brief create creature by genotype script
 @date 2013-12-06
*/
CNode* CCreature::GenerateByGenotype( const genotype_parser::SExpr *pexpr, const int recursiveCnt )
{
	if (!pexpr)
		return NULL;
	if (recursiveCnt < 0)
		return NULL;

	const PxVec3 pos = m_Sample.getCamera().getPos() + (m_Sample.getCamera().getViewDir()*10.f);
	const PxVec3 vel = m_Sample.getCamera().getViewDir() * 20.f;

	CNode *pNode = new CNode(m_Sample);
	PxVec3 dimension(pexpr->dimension.x, pexpr->dimension.y, pexpr->dimension.z);
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

	PxRigidDynamic* body = pNode->m_pBody;

	genotype_parser::SJointList *pConnect = pexpr->connection;
	while (pConnect)
	{
		CNode *pChildNode = GenerateByGenotype( pConnect->joint->expr, recursiveCnt-1 );

		if (pChildNode)
		{
			PxRigidDynamic *child = pChildNode->m_pBody;
			genotype_parser::SJoint *joint = pConnect->joint;
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

			CJoint *pJoint = new CJoint(pNode, pChildNode, pxJoint, velocity.x, joint->period);
			CAngularSensor *pSensor = new CAngularSensor();
			CMuscleEffector *pEffector = new CMuscleEffector(joint->period);
			pJoint->ApplySensor(*pSensor);
			pJoint->ApplyEffector(*pEffector);

			pNode->m_Joints.push_back( pJoint );
			pNode->m_Sensors.push_back(pSensor);
			pNode->m_Effectors.push_back(pEffector);

			{ // add child
				CJoint *pChildJoint = new CJoint(pNode, NULL, pxJoint, velocity.x, joint->period);
				CAngularSensor *pChildSensor = new CAngularSensor();
				pChildJoint->ApplySensor(*pChildSensor);
				pChildNode->m_Joints.push_back(pChildJoint);
				pChildNode->m_Sensors.push_back(pChildSensor);
				pChildNode->InitNeuron();
			}
		}

		pConnect = pConnect->next;
	}

	pNode->InitNeuron();
	m_Nodes.push_back(pNode);
	return pNode;
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
}
