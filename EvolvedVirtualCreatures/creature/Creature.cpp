
#include "stdafx.h"
#include "Creature.h"
#include "physnode.h"
#include "../EvolvedVirtualCreatures.h"
#include "../genoype/GenotypeParser.h"
#include "Joint.h"
#include "NeuralNet.h"
#include "AngularSensor.h"
#include "PhotoSensor.h"
#include "VisionSensor.h"
#include "MuscleEffector.h"
#include "../renderer/RenderComposition.h"
#include "../renderer/RendererCompositionShape.h"


using namespace evc;

CCreature::CCreature(CEvc &sample) :
	m_sample(sample)
,	m_pRoot(NULL)
,	m_pGenotypeExpr(NULL)
,	m_IncreaseTime(0)
,	m_GrowCount(0)
,	m_IsDispSkinning(true)
{

}

CCreature::~CCreature()
{
	m_nodes.clear();
	SAFE_DELETE(m_pRoot);
	genotype_parser::RemoveExpression(m_pGenotypeExpr);
}


/**
 @brief Generate Immediate Creature
 @date 2013-12-09
*/
void CCreature::GenerateImmediate(const string &genotypeScriptFileName, const PxVec3 &initialPos, 
	const PxVec3 *linVel, const int recursiveCount, const bool isDispSkinning) 
	//recursivCount=2, isDispSkinning=true
{
	genotype_parser::CGenotypeParser parser;
	genotype_parser::SExpr *expr = parser.Parse(genotypeScriptFileName);

	m_GenotypeSymbols.clear();
	MakeExprSymbol(expr, m_GenotypeSymbols);
	FirstStepToGenerateByGenotype(expr, initialPos, linVel, recursiveCount, isDispSkinning);

	m_GrowCount = recursiveCount;
}


/**
 @brief Generate Immediate Creature
 @date 2014-02-27
*/
void CCreature::GenerateImmediate(genotype_parser::SExpr *expr, const PxVec3 &initialPos, 
	const PxVec3 *linVel, const int recursiveCount, const bool isDispSkinning)
	//recursivCount=2, isDispSkinning=true
{
	m_GenotypeSymbols.clear();
	MakeExprSymbol(expr, m_GenotypeSymbols);
	FirstStepToGenerateByGenotype(expr, initialPos, linVel, recursiveCount, isDispSkinning);

	m_GrowCount = recursiveCount;
}


/**
 @brief FirstStep to generate by genotype
 @date 2013-12-09
*/
void CCreature::FirstStepToGenerateByGenotype(genotype_parser::SExpr *expr, const PxVec3 &initialPos, 
	const PxVec3 *linVel, const int recursiveCount, const bool isDispSkinning) 
	//recursivCount=2, isDispSkinning=true
{
	PxSceneWriteLock scopedLock(m_sample.getActiveScene());

	m_nodes.clear();
	SAFE_DELETE(m_pRoot);

	genotype_parser::RemoveExpression(m_pGenotypeExpr);
	m_pGenotypeExpr = expr;
	m_IsDispSkinning = isDispSkinning;

	m_pRoot = GenerateByGenotype(NULL, expr, recursiveCount, initialPos, linVel);
	RET(!m_pRoot);
	m_pRoot->InitBrain();

	m_genome.fitness = 0;
	m_genome.chromo.clear();
	m_genome.chromo.reserve(64);
	GetChromo(this, expr, m_genome.chromo);

	m_initialPos = m_pRoot->GetBody()->getGlobalPose().p;
	m_initialPos.y = 0;

	m_tmPalette.resize(m_nodes.size());
	GenerateSkinningMesh();
}


/**
 @brief generate creature by genome 
 @date 2013-12-13
*/
void CCreature::GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos)
{
	m_genome = genome;

	vector<double> weights;
	genotype_parser::SExpr *p = BuildExpr(genome.chromo, weights);
	m_pRoot = GenerateByGenotype(NULL, p, g_pDbgConfig->generationRecursiveCount, initialPos, NULL);
	m_pRoot->InitBrain(weights);

	m_initialPos = m_pRoot->GetBody()->getGlobalPose().p;
	m_initialPos.y = 0;

	genotype_parser::RemoveExpression(p);
}


/**
 @brief progressive generate creature
 @date 2014-01-17
*/
void CCreature::GenerateProgressive(const string &genotypeScriptFileName, const PxVec3 &initialPos,  const PxVec3 *linVel, 
	const bool isDispSkinning) 
	//isDispSkinning=true
{
	//if (m_pRoot)
	//{
	//	GenerateProgressive(m_pRoot, m_pGenotypeExpr);
	//}
	//else
	{
		genotype_parser::CGenotypeParser parser;
		genotype_parser::SExpr *expr = parser.Parse(genotypeScriptFileName);

		m_GenotypeSymbols.clear();
		MakeExprSymbol(expr, m_GenotypeSymbols);
		FirstStepToGenerateByGenotype(expr, initialPos, linVel, 1, isDispSkinning);
	}
}


/**
 @brief progressive generate creature
 @date 2014-01-17
*/
void CCreature::GenerateProgressive(genotype_parser::SExpr *expr, const PxVec3 &initialPos,  const PxVec3 *linVel, 
	const bool isDispSkinning) 
	//isDispSkinning=true
{
	//if (m_pRoot)
	//{
	//	GenerateProgressive(m_pRoot, m_pGenotypeExpr);
	//}
	//else
	{
		m_GenotypeSymbols.clear();
		MakeExprSymbol(expr, m_GenotypeSymbols);
		FirstStepToGenerateByGenotype(expr, initialPos, linVel, 1, isDispSkinning);

		m_GrowCount = 0;
	}
}


/**
 @brief 
 @date 2014-01-17
*/
void CCreature::GenerateProgressive( CPhysNode *currentNode, const genotype_parser::SExpr *expr )
{
	RET(!currentNode);

	BOOST_FOREACH (auto &joint, currentNode->m_Joints)
	{
		GenerateProgressive((CPhysNode*)joint->GetActor1(), expr);
	}

	// only generate terminal node
	if (currentNode->m_Joints.empty())
	{
		// Generate parts
		const genotype_parser::SExpr *nodeExpr = FindExpr(currentNode->m_Name);
		GenerateByGenotype(currentNode, nodeExpr, 1, PxVec3(), NULL, false);
	}
}


/**
 @brief create creature by genotype script
 @date 2013-12-06
*/
CPhysNode* CCreature::GenerateByGenotype( CPhysNode* parentNode, const genotype_parser::SExpr *pexpr, const int recursiveCnt, 
	const PxVec3 &initialPos, const PxVec3 *linVel, const bool isGenerateBody, const PxVec3 &randPos, const float dimensionRate, 
	const PxVec3 &parentDim, const bool IsTerminal ) 
	// isGenerateBody=true, randPos=Px(0,0,0), dimensionRate=1, randPos=PxVec3(0,0,0), IsTerminal=false
{
	if (!pexpr)
		return NULL;
	if (recursiveCnt < 0)
		return NULL;
		//return GenerateTerminalNode(parentNode, pexpr, initialPos, dimensionRate, parentDim);

	// Generate Body
	CPhysNode *pNode = NULL;
	if (isGenerateBody)
	{
		PxVec3 tmp;
		pNode = CreateBody(pexpr, initialPos, linVel, randPos, dimensionRate, parentDim, tmp, IsTerminal);
		//RETV(!pNode, NULL);
		if (!pNode)
		{
			return GenerateTerminalNode(parentNode, pexpr, initialPos, linVel, dimensionRate, parentDim);
		}

		pNode->m_PaletteIndex = m_nodes.size();
		//pNode->m_IsTerminalNode = IsTerminal;
		m_nodes.push_back(pNode);
	}
	else
	{
		pNode = parentNode;
	}

	RETV(!pNode, NULL);

	const PxVec3 pos = pNode->m_pBody->getGlobalPose().p;
	PxVec3 dimension = pNode->m_Dimension;

	// Generate Connection (None Terminal Connection)
	PxRigidDynamic* body = pNode->m_pBody;
	genotype_parser::SConnectionList *pConnectList = pexpr->connection;
	while (pConnectList)
	{
		genotype_parser::SConnection *connection = pConnectList->connect;
		if (connection->terminalOnly)
		{
			pConnectList = pConnectList->next;
			continue;
		}

		if (boost::iequals(connection->conType, "joint"))
		{
			const PxVec3 conPos(connection->pos.x, connection->pos.y, connection->pos.z);
			const PxVec3 randPos(connection->randPos.x, connection->randPos.y, connection->randPos.z);
			const PxVec3 nodePos = pos - conPos;

			CPhysNode *pChildNode = GenerateByGenotype( pNode, connection->expr, recursiveCnt-1, nodePos, linVel, true, randPos, 
				dimensionRate*0.7f, dimension, IsTerminal);
			if (pChildNode && !pChildNode->m_IsTerminalNode)
			{
				const PxVec3 newConPos = pos - pChildNode->m_pBody->getGlobalPose().p;
				CreateJoint(pNode, pChildNode, connection, newConPos);
			}
		}
		else if(boost::iequals(connection->conType, "sensor"))
		{
			CreateSensor(pNode, connection, pos);
		}

		pConnectList = pConnectList->next;
	}

	return pNode;
}


/**
 @brief create body
 @date 2014-01-17
*/
CPhysNode* CCreature::CreateBody(const genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 *linVel, const PxVec3 &randPos, 
	const float dimensionRate, const PxVec3 &parentDim, OUT PxVec3 &outDimension, const bool isTerminal) //isTerminal=false
{
	// Generate Body
	PxVec3 pos = initialPos;
	pos += RandVec3(randPos, 1.f);

	PxVec3 dimension(expr->dimension.x, expr->dimension.y, expr->dimension.z);
	const PxVec3 randShape(expr->randShape.x, expr->randShape.y, expr->randShape.z);
	const PxVec3 randRange(dimension.x*randShape.x, dimension.y*randShape.y, dimension.z*randShape.z);
	dimension += RandVec3(randRange, 1.f);
	dimension = MaximumVec3(dimension, PxVec3(0.1f, 0.1f, 0.1f));
	dimension *= dimensionRate;

	if (!parentDim.isZero())
	{
		const float parentVolume = parentDim.x * parentDim.y * parentDim.z;
		const float childVolume = dimension.x * dimension.y * dimension.z;
		if ((childVolume*1.3f) >= parentVolume)
		{
			dimension /= ((childVolume*1.3f) / parentVolume);
		}
	}

	//const bool IsParentTerminal = (parentNode && parentNode->m_IsTerminalNode);
	//if (!IsTerminal && (!parentDim.isZero() && parentDim.magnitude() < .2f))
	//{
	//	SAFE_DELETE(pNode);
	//	return GenerateTerminalNode(parentNode, expr, initialPos, dimensionRate, parentDim);
	//}

	if (isTerminal)
	{
		if (dimension.magnitude() < .05f)
			return NULL;
	}
	else
	{
		if (dimension.magnitude() <= 0.15f) // minimum size
			return NULL;

		//if (dimension.magnitude() <= 0.01f) // minimum size
		//	return NULL;

		//if (dimension.magnitude() < 0.15f)
		//{
		//	if (HasTerminalNode(expr))
		//		return NULL;
		//	dimension = PxVec3(0.01f, 0.01f, 0.01f); // minimum size
		//}
	}

	CPhysNode *pNode = new CPhysNode(m_sample);
	pNode->m_Name = expr->id;
	pNode->m_ShapeName = expr->shape;

	//MaterialIndex material = GetMaterialType(expr->material);
	RenderMaterial *material = m_sample.GetMaterial(PxVec3(expr->material.x, expr->material.y, expr->material.z));
	pNode->m_MaterialDiffuse = PxVec3(expr->material.x, expr->material.y, expr->material.z);
	const float mass = expr->mass;

	if (boost::iequals(expr->shape, "box"))
	{
		pNode->m_pBody = m_sample.createBox(pos, dimension, linVel, material, mass);
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		pNode->m_pBody = m_sample.createSphere(pos, dimension.x, linVel, material, mass);
	}
	else if (boost::iequals(expr->shape, "root"))
	{ // root node size 0.1, 0.1, 0.1
		//pos = PxVec3(pos.x,pos.y,pos.z);
		//dimension = PxVec3(0.1f,0.1f,0.1f);
		pNode->m_pBody = m_sample.createBox(pos,  PxVec3(0.1f,0.1f,0.1f), NULL, material, mass);
		pNode->m_pBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		pNode->m_IsKinematic = true;
	}

	if (pNode->m_pBody)
	{
		PxShape *shape;
		if (1 == pNode->m_pBody->getShapes(&shape, 1))
			pNode->m_pShape = shape;
	}

	pNode->m_Dimension = dimension;
	outDimension = dimension;
	return pNode;
}


/**
 @brief 
 @date 2014-01-13
*/
CPhysNode* CCreature::GenerateTerminalNode( CPhysNode *parentNode, const genotype_parser::SExpr *pexpr, 
	const PxVec3 &initialPos, const PxVec3 *linVel, const float dimensionRate, const PxVec3 &parentDim ) 
{
	RETV(!pexpr, NULL);

	CPhysNode *pNode = NULL;
	PxVec3 pos = initialPos;

	// Generate Terminal Connection
	genotype_parser::SConnectionList *pConnectList = pexpr->connection;
	while (pConnectList)
	{
		genotype_parser::SConnection *connection = pConnectList->connect;
		if (connection->terminalOnly)
		{
			if (boost::iequals(connection->conType, "joint"))
			{
				PxVec3 conPos(connection->pos.x, connection->pos.y, connection->pos.z);
				PxVec3 randPos(connection->randPos.x, connection->randPos.y, connection->randPos.z);
				PxVec3 nodePos = pos - conPos;

				CPhysNode *pChildNode = GenerateByGenotype( parentNode, connection->expr, g_pDbgConfig->generationRecursiveCount, 
					nodePos, linVel, true, randPos, dimensionRate, parentDim, true);
				if (pChildNode)
				{
					pChildNode->m_IsTerminalNode = true;
					PxVec3 newConPos = pos - pChildNode->m_pBody->getGlobalPose().p;
					CreateJoint(parentNode, pChildNode, connection, newConPos);
				}
				pNode = pChildNode;
			}
			else if(boost::iequals(connection->conType, "sensor"))
			{
				CPhysNode *pChildNode = CreateSensor(parentNode, connection, pos, true);
				pNode = pChildNode;
			}
		}

		pConnectList = pConnectList->next;
	}

	return pNode;
}


/**
 @brief 
 @date 2013-12-19
*/
void CCreature::CreateJoint( CPhysNode *parentNode, CPhysNode *childNode, genotype_parser::SConnection *connect, const PxVec3 &conPos )
{
	RET(!childNode);
	RET(!parentNode);

	PxRigidDynamic* body = parentNode->m_pBody;
	PxRigidDynamic *child = childNode->m_pBody;
	genotype_parser::SConnection *joint = connect;
	PxVec3 dir0(joint->parentOrient.dir.x, joint->parentOrient.dir.y, joint->parentOrient.dir.z);
	PxVec3 dir1(joint->orient.dir.x, joint->orient.dir.y, joint->orient.dir.z);

	PxVec3 pos = conPos;

	// random position
	PxVec3 randPos(connect->randPos.x, connect->randPos.y, connect->randPos.z);
	pos += RandVec3(randPos, 1.f);

	// random orientation
	PxVec3 randOrient(connect->randOrient.x, connect->randOrient.y, connect->randOrient.z);
	if (!dir1.isZero())
	{
		dir1 += RandVec3(randOrient, 1.f);
		dir1.normalize();
	}

	PxVec3 limit(joint->limit.x, joint->limit.y, joint->limit.z);
	PxVec3 velocity(joint->velocity.x, joint->velocity.y, joint->velocity.z);

	PxTransform tm0 = (dir0.isZero())? PxTransform::createIdentity() : PxTransform(PxQuat(joint->parentOrient.angle, dir0));
	PxTransform tm1 = (dir1.isZero())? PxTransform(PxVec3(pos)) : 
		(PxTransform(PxQuat(joint->orient.angle, dir1)) * PxTransform(PxVec3(pos)));


	 //apply gravity direction only have root genotype
	if (boost::iequals(parentNode->m_ShapeName, "root"))
	{
		PxVec3 gravDir = parentNode->m_pBody->getGlobalPose().p;
		gravDir.normalize();
		gravDir = PxVec3(0,1,0);
		PxQuat gravQ;
		utility::quatRotationArc(gravQ, PxVec3(0,1,0), gravDir);
		tm1 = tm1 * PxTransform(gravQ);
	}


	PxJoint* pxJoint = NULL;
	const PxReal tolerance = 0.2f;
	if (boost::iequals(joint->type, "fixed"))
	{
		PxFixedJoint *j = PxFixedJointCreate(m_sample.getPhysics(), body, tm0, child, tm1);

		j->setProjectionLinearTolerance(tolerance);
		j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
		pxJoint = j;
	}
	else if (boost::iequals(joint->type, "spherical"))
	{
		if (PxSphericalJoint *j = PxSphericalJointCreate(m_sample.getPhysics(), body, tm0, child, tm1) )
		{
			if (!limit.isZero())
			{
				j->setLimitCone(PxJointLimitCone(limit.x, limit.y, PxSpring(0,0)));
				j->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
			}

			j->setProjectionLinearTolerance(tolerance);
			j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
			pxJoint = j;
		}
	}
	else if (boost::iequals(joint->type, "revolute"))
	{
		if (PxRevoluteJoint*j = PxRevoluteJointCreate(m_sample.getPhysics(), body, tm0, child, tm1) )
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

			j->setProjectionLinearTolerance(tolerance);
			j->setConstraintFlag(PxConstraintFlag::ePROJECTION, true);
			pxJoint = j;
		}
	}

	CJoint *pJoint = new CJoint(parentNode, childNode, tm0, tm1, pxJoint, velocity.x, joint->period);
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
	}
}


/**
 @brief 
 @date 2013-12-19
*/
CPhysNode* CCreature::CreateSensor(CPhysNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos, 
	const bool IsTerminal) // IsTerminal = false
{
	RETV(!parentNode, NULL);

	CPhysNode *childNode = NULL;
	CPhysNode *pNode = new CPhysNode(m_sample);
	pNode->m_PaletteIndex = m_nodes.size();
	//pNode->m_IsTerminalNode = IsTerminal;

	PxVec3 dimension(0.05f, 0.2f, 0.05f);
	if (connect->expr)
		dimension = utility::Vec3toPxVec3(connect->expr->dimension);
	//MaterialIndex material = GetMaterialType("red");
	pNode->m_pBody = m_sample.createBox(initialPos, dimension, NULL, m_sample.GetMaterial(PxVec3(0.75f,0,0)), 1.f);
	m_nodes.push_back(pNode);
	childNode = pNode;

	if (connect->expr)
		connect->expr->isSensor = true;

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
	if (PxFixedJoint *j = PxFixedJointCreate(m_sample.getPhysics(), body, tm0, child, tm1 ))
	{
		pxJoint = j;
	}
	
	CJoint *pJoint = new CJoint(parentNode, childNode, tm0, tm1, pxJoint, velocity.x, joint->period);

	CSensor *sensor = NULL;
	if (boost::iequals(joint->type, "photo"))
	{
		childNode->m_Name = "photo sensor";
		sensor = new CPhotoSensor(m_sample);
		((CPhotoSensor*)sensor)->SetSensorInfo(childNode, PxVec3(0,1,0), 10);
	}
	else if (boost::iequals(joint->type, "vision"))
	{
		childNode->m_Name = "vision sensor";
		sensor = new CVisionSensor(m_sample);
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
//MaterialIndex CCreature::GetMaterialType(const string &materialStr)
//{
//	const static string materials[] = {"grey", "red", "green", "blue", "yellow"};
//	const int size = sizeof(materials) / sizeof(string);
//
//	for (int i=0; i < size; ++i)
//	{
//		if (materialStr == materials[ i])
//		{
//			return (MaterialIndex)(i + MATERIAL_GREY);
//		}
//	}
//	return MATERIAL_GREY;
//}


/**
 @brief 
 @date 2013-12-09
*/
void CCreature::Move(float dtime)
{
	m_IncreaseTime += dtime;

	// grow creature
	if (m_GrowCount < 3)
	{
		if (m_IncreaseTime > 1.f)
		{
			++m_GrowCount;
			m_IncreaseTime = 0;
			
			//PxVec3 pos = GetPos();
			//pos += PxVec3(0,5,0);
			//SetPos(pos);

			GenerateProgressive(m_pRoot, m_pGenotypeExpr);
			m_tmPalette.resize(m_nodes.size());
			GenerateSkinningMesh();
		}
	}

	BOOST_FOREACH (auto &node, m_nodes)
		node->Move(dtime);

	// update palette
	BOOST_FOREACH (auto &node, m_nodes)
	{
		if (node->GetBody())
			m_tmPalette[ node->m_PaletteIndex] = node->GetBody()->getGlobalPose();
	}

	// update worldbound
	if (m_pRoot && m_pRoot->m_pShapeRenderer)
	{
		PxBounds3 worldBound = m_pRoot->m_worldBounds;
		BOOST_FOREACH (auto &node, m_nodes)
		{
			if (node == m_pRoot)
				continue;

			PxBounds3 bound = node->m_worldBounds;
			worldBound.minimum.x = min(worldBound.minimum.x, bound.minimum.x);
			worldBound.minimum.y = min(worldBound.minimum.y, bound.minimum.y);
			worldBound.minimum.z = min(worldBound.minimum.z, bound.minimum.z);
			worldBound.maximum.x = max(worldBound.maximum.x, bound.maximum.x);
			worldBound.maximum.y = max(worldBound.maximum.y, bound.maximum.y);
			worldBound.maximum.z = max(worldBound.maximum.z, bound.maximum.z);
		}
		m_pRoot->m_pShapeRenderer->setWorldBounds(worldBound);
	}

	// fitness
	if (m_pRoot)
	{
		PxVec3 pos = m_pRoot->GetBody()->getGlobalPose().p;
		pos.y = 0;
		PxVec3 len =  pos - m_initialPos;
		m_genome.fitness = len.magnitude();
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
 @brief set position
 @date 2014-03-01
*/
void CCreature::SetPos(const PxVec3 &pos)
{
	RET(!m_pRoot);
	const PxVec3 rootPos = m_pRoot->GetBody()->getGlobalPose().p;
	const PxVec3 offset = pos - rootPos;

	BOOST_FOREACH (auto node, m_nodes)
	{
		PxTransform tm = node->GetBody()->getGlobalPose();
		tm.p += offset;
		 node->GetBody()->setGlobalPose(tm);
	}
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


/**
 @brief generate skinning mesh from m_pRoot Node
 @date 2014-01-19
*/
void CCreature::GenerateSkinningMesh()
{
	PxSceneWriteLock scopedLock(m_sample.getActiveScene());

	GenerateRenderComposition(m_pRoot);
	//PxRigidDynamic *rigidActor0 = m_pRoot->GetBody();
	//PxShape *shape;
	//if (1 == rigidActor0->getShapes(&shape, 1))
	//	m_Sample.link(m_pRoot->m_pShapeRenderer, shape, m_pRoot->GetBody());

	if (m_pRoot && m_pRoot->m_pOriginalShapeRenderer)
	{
		if (m_pRoot->m_pShapeRenderer)
		{
			m_sample.addRenderObject( m_pRoot->m_pShapeRenderer );
		}
		else
		{
			m_pRoot->m_pShapeRenderer = m_pRoot->m_pOriginalShapeRenderer;
			m_sample.addRenderObject( m_pRoot->m_pShapeRenderer );
		}
		m_tmPalette[ m_pRoot->m_PaletteIndex] = m_pRoot->GetBody()->getGlobalPose();
	}

}


/**
 @brief composite all node shape
 @date 2014-01-05
*/
void CCreature::GenerateRenderComposition( CPhysNode *node )
{
	RET(!node);

	BOOST_FOREACH (auto joint, node->m_Joints)
	{
		GenerateRenderComposition((CPhysNode*)joint->GetActor1());
	}

	//const MaterialIndex materialIndex = GetMaterialType(node->m_MaterialName);
	//RenderMaterial *material = m_Sample.getManageMaterial(materialIndex);
	RenderMaterial *material = m_sample.GetMaterial(PxVec3(node->m_MaterialDiffuse.x, node->m_MaterialDiffuse.y, node->m_MaterialDiffuse.z));

	// generate renderer
	if (!node->m_pOriginalShapeRenderer)
	{
		PxRigidDynamic *rigidActor0 = node->GetBody();
		PxShape *shape0;
		if (1 == rigidActor0->getShapes(&shape0,1))
		{
			RenderBaseActor *renderActor0 = m_sample.getRenderActor(rigidActor0, shape0);
			if (renderActor0)
			{
				RenderComposition *newRenderer = new RenderComposition(*m_sample.getRenderer(), node->m_PaletteIndex, 
					m_tmPalette, renderActor0->getRenderShape(), material);

				renderActor0->setRendering(false);
				newRenderer->setEnableCameraCull(true);

				node->m_pOriginalShapeRenderer = newRenderer;
				//m_Sample.addRenderObject( node->m_pOriginalShapeRenderer );
				m_sample.removeRenderObject(renderActor0);
				m_sample.unlink(renderActor0, shape0, rigidActor0);
			}
		}
	}

	// remove shape renderer
	if (node->m_pShapeRenderer && (node->m_pShapeRenderer != node->m_pOriginalShapeRenderer))
	{
		m_sample.removeRenderObject(node->m_pShapeRenderer);
		node->m_pShapeRenderer = NULL;
	}

	// composite child shape
	BOOST_FOREACH (auto joint, node->m_Joints)
	{
		CPhysNode *child = (CPhysNode*)joint->GetActor1();

		RenderComposition *parentRenderer = (node->m_pShapeRenderer)? node->m_pShapeRenderer : node->m_pOriginalShapeRenderer;
		RenderComposition *childRenderer = (child->m_pShapeRenderer)? child->m_pShapeRenderer : child->m_pOriginalShapeRenderer;

		RenderComposition *newRenderer = new RenderComposition(*m_sample.getRenderer(), 
			node->m_PaletteIndex, child->m_PaletteIndex,
			node->m_PaletteIndex, m_tmPalette, 
			(SampleRenderer::RendererCompositionShape*)parentRenderer->getRenderShape(), joint->GetTm0(), 
			(SampleRenderer::RendererCompositionShape*)childRenderer->getRenderShape(), joint->GetTm1());

		newRenderer->setEnableCameraCull(true);

		// remove shape renderer
		if (node->m_pShapeRenderer && (node->m_pShapeRenderer != node->m_pOriginalShapeRenderer))
			m_sample.removeRenderObject(node->m_pShapeRenderer);

		node->m_pShapeRenderer = newRenderer;

		//m_Sample.addRenderObject( node->m_pShapeRenderer );
		//m_Sample.removeRenderObject(parentRenderer);
		//m_Sample.removeRenderObject(child->m_pRenderComposition);
		//child->m_pRenderComposition = NULL;
	}

}


/**
 @brief random vector from vector value and rate
 @date 2014-01-13
*/
PxVec3 CCreature::RandVec3( const PxVec3 &vec, const float rate )
{
	if (rate <= 0.f)
		return vec;
	if (vec.isZero())
		return vec;

	PxVec3 dimRand1 = vec * rate;
	PxVec3 dimRand(RandFloat()*dimRand1.x, RandFloat()*dimRand1.y, RandFloat()*dimRand1.z);
	if (RandFloat() > 0.5f) dimRand.x = -dimRand.x;
	if (RandFloat() > 0.5f) dimRand.y = -dimRand.y;
	if (RandFloat() > 0.5f) dimRand.z = -dimRand.z;
	return dimRand;
}


/**
 @brief return maximum value
 @date 2014-01-13
*/
PxVec3 CCreature::MaximumVec3( const PxVec3 &vec0, const PxVec3 &vec1 )
{
	PxVec3 val;
	val.x = max(vec0.x, vec1.x);
	val.y = max(vec0.y, vec1.y);
	val.z = max(vec0.z, vec1.z);
	return val;
}


/**
 @brief find exression by name
 @date 2014-01-17
*/
const genotype_parser::SExpr* CCreature::FindExpr( const string &name)
{
	auto it = m_GenotypeSymbols.find(name);
	if (m_GenotypeSymbols.end() != it)
		return it->second;
	else
		return NULL;
}


/**
 @brief make expression symbol table
 @date 2014-01-17
*/
void CCreature::MakeExprSymbol( const genotype_parser::SExpr *expr, OUT map<string, const genotype_parser::SExpr*> &symbols )
{
	using namespace genotype_parser;
	RET(!expr);

	auto it = symbols.find(expr->id);
	if (symbols.end() != it)
		return; // already exist

	symbols[ expr->id] = expr;

	SConnectionList *connectList = expr->connection;
	while (connectList)
	{
		const SConnection *connection = connectList->connect;
		MakeExprSymbol(connection->expr, symbols);
		connectList = connectList->next;
	}
}


/**
 @brief return true if Has Terminal Node 
 @date 2014-02-01
*/
bool CCreature::HasTerminalNode(const genotype_parser::SExpr *expr) const
{
	genotype_parser::SConnectionList *pConnectList = expr->connection;
	while (pConnectList)
	{
		genotype_parser::SConnection *connection = pConnectList->connect;
		if (connection->terminalOnly)
			return true;
		pConnectList = pConnectList->next;
	}
	return false;
}


/**
 @brief 
 @date 2014-02-10
*/
void CCreature::SetGravity(const PxVec3 &centerOfGravity)
{
	BOOST_FOREACH (auto node, m_nodes)
	{
		if (node->m_IsKinematic)
			continue;

		if (node->m_pBody)
		{
			PxTransform pose = node->m_pBody->getGlobalPose();
			PxVec3 up = pose.p - centerOfGravity;
			up.normalize();
		
			const PxVec3 force = -up * 9.81f * 1.f;
			node->m_pBody->addForce(force, PxForceMode::eACCELERATION, false);
		}
	}
}
