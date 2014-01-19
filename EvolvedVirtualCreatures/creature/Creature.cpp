
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
#include "../renderer/RenderComposition.h"
#include "../renderer/RenderCompositionShape.h"


using namespace evc;

CCreature::CCreature(CEvc &sample) :
	m_Sample(sample)
,	m_pRoot(NULL)
,	m_pGenotypeExpr(NULL)
,	m_IncreaseTime(0)
,	m_GrowCount(0)
,	m_IsDispSkinning(true)
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
	genotype_parser::RemoveExpression(m_pGenotypeExpr);
}


/**
 @brief 
 @date 2013-12-09
*/
void CCreature::GenerateByGenotype(const string &genotypeScriptFileName, const PxVec3 &initialPos, 
	const int recursiveCount, const bool isDispSkinning) 
	//recursivCount=2, isDispSkinning=true
{
	PxSceneWriteLock scopedLock(m_Sample.getActiveScene());

	m_IsDispSkinning = isDispSkinning;

	m_pRoot = GenerateByGenotype(NULL, m_pGenotypeExpr, recursiveCount, initialPos);
	RET(!m_pRoot);
	m_pRoot->InitBrain();

	m_Genome.fitness = 0;
	m_Genome.chromo.clear();
	m_Genome.chromo.reserve(64);
	GetChromo(this, m_pGenotypeExpr, m_Genome.chromo);

	m_InitialPos = m_pRoot->GetBody()->getGlobalPose().p;
	m_InitialPos.y = 0;


	// generate skinning mesh
	m_TmPalette.resize(m_Nodes.size()); // generate tm palette
	if (isDispSkinning)
	{
		GenerateRenderComposition(m_pRoot);

		PxRigidDynamic *rigidActor0 = m_pRoot->GetBody();
		PxShape *shape;
		if (1 == rigidActor0->getShapes(&shape, 1))
			m_Sample.link(m_pRoot->m_pRenderComposition, shape, m_pRoot->GetBody());
	}
}


/**
 @brief generate creature by genome 
 @date 2013-12-13
*/
void CCreature::GenerateByGenome(const SGenome &genome, const PxVec3 &initialPos)
{
	m_Genome = genome;

	vector<double> weights;
	genotype_parser::SExpr *p = BuildExpr(genome.chromo, weights);
	m_pRoot = GenerateByGenotype(NULL, p, g_pDbgConfig->generationRecursiveCount, initialPos);
	m_pRoot->InitBrain(weights);

	m_InitialPos = m_pRoot->GetBody()->getGlobalPose().p;
	m_InitialPos.y = 0;

	genotype_parser::RemoveExpression(p);
}


/**
 @brief progressive generate creature
 @date 2014-01-17
*/
void CCreature::GenerateProgressive(const string &genotypeScriptFileName, const PxVec3 &initialPos,  const bool isDispSkinning) 
	//isDispSkinning=true
{
	if (m_pRoot)
	{
		GenerateProgressive(m_pRoot, m_pGenotypeExpr);
	}
	else
	{
		genotype_parser::CGenotypeParser parser;
		m_pGenotypeExpr = parser.Parse(genotypeScriptFileName);
		MakeExprSymbol(m_pGenotypeExpr, m_GenotypeSymbols);
		GenerateByGenotype(genotypeScriptFileName, initialPos, 1, isDispSkinning);
	}
}


/**
 @brief 
 @date 2014-01-17
*/
void CCreature::GenerateProgressive( CNode *currentNode, const genotype_parser::SExpr *expr )
{
	RET(!currentNode);

	BOOST_FOREACH (auto &joint, currentNode->m_Joints)
	{
		GenerateProgressive((CNode*)joint->GetActor1(), expr);
	}

	if (currentNode->m_Joints.empty())
	{
		// Generate parts
		const genotype_parser::SExpr *nodeExpr = FindExpr(currentNode->m_Name);
		GenerateByGenotype(currentNode, nodeExpr, 1, PxVec3(), false);
	}
}


/**
 @brief create creature by genotype script
 @date 2013-12-06
*/
CNode* CCreature::GenerateByGenotype( CNode* parentNode, const genotype_parser::SExpr *pexpr, const int recursiveCnt, 
	const PxVec3 &initialPos, const bool isGenerateBody, const PxVec3 &randPos, const float dimensionRate, 
	const PxVec3 &parentDim, const bool IsTerminal ) 
	// isGenerateBody=true, randPos=Px(0,0,0), dimensionRate=1, randPos=PxVec3(0,0,0), IsTerminal=false
{
	if (!pexpr)
		return NULL;
	if (recursiveCnt < 0)
		return GenerateTerminalNode(parentNode, pexpr, initialPos, dimensionRate, parentDim);

	// Generate Body
	CNode *pNode = NULL;
	if (isGenerateBody)
	{
		PxVec3 tmp;
		pNode = CreateBody(pexpr, initialPos, randPos, dimensionRate, parentDim, tmp);
		RETV(!pNode, NULL);

		pNode->m_PaletteIndex = m_Nodes.size();
		pNode->m_IsTerminalNode = IsTerminal;
		m_Nodes.push_back(pNode);
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
			PxVec3 conPos(connection->pos.x, connection->pos.y, connection->pos.z);
			PxVec3 randPos(connection->randPos.x, connection->randPos.y, connection->randPos.z);
			PxVec3 nodePos = pos - conPos;

			CNode *pChildNode = GenerateByGenotype( pNode, connection->expr, recursiveCnt-1, nodePos, true, randPos, 
				dimensionRate*0.7f, dimension);
			if (pChildNode && !pChildNode->m_IsTerminalNode)
			{
				PxVec3 newConPos = pos - pChildNode->m_pBody->getGlobalPose().p;
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
CNode* CCreature::CreateBody(const genotype_parser::SExpr *expr, const PxVec3 &initialPos, const PxVec3 &randPos, 
	const float dimensionRate, const PxVec3 &parentDim, OUT PxVec3 &outDimension)
{
	// Generate Body
	PxVec3 pos = initialPos;
	pos += RandVec3(randPos, 1.f);

	CNode *pNode = new CNode(m_Sample);
	pNode->m_Name = expr->id;

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

	if (dimension.magnitude() < .1f)
	{
		SAFE_DELETE(pNode);
		return NULL;
	}

	MaterialIndex material = GetMaterialType(expr->material);
	pNode->m_MaterialName = expr->material;
	const float mass = expr->mass;

	if (boost::iequals(expr->shape, "box"))
	{
		pNode->m_pBody = m_Sample.createBox(pos, dimension, NULL, m_Sample.getManageMaterial(material), mass);
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		pNode->m_pBody = m_Sample.createSphere(pos, dimension.x, NULL, m_Sample.getManageMaterial(material), mass);
	}
	else if (boost::iequals(expr->shape, "root"))
	{ // root node size 0.1, 0.1, 0.1
		pos = PxVec3(pos.x,0,pos.z);
		//dimension = PxVec3(0.1f,0.1f,0.1f);
		pNode->m_pBody = m_Sample.createBox(pos,  PxVec3(0.1f,0.1f,0.1f), NULL, m_Sample.getManageMaterial(material), mass);
		pNode->m_pBody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
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
CNode* CCreature::GenerateTerminalNode( CNode *parentNode, const genotype_parser::SExpr *pexpr, 
	const PxVec3 &initialPos, const float dimensionRate, const PxVec3 &parentDim ) 
{
	RETV(!pexpr, NULL);

	CNode *pNode = NULL;
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

				CNode *pChildNode = GenerateByGenotype( parentNode, connection->expr, g_pDbgConfig->generationRecursiveCount, 
					nodePos, true, randPos, dimensionRate, parentDim, true);
				if (pChildNode)
				{
					PxVec3 newConPos = pos - pChildNode->m_pBody->getGlobalPose().p;
					CreateJoint(parentNode, pChildNode, connection, newConPos);
				}
				pNode = pChildNode;
			}
			else if(boost::iequals(connection->conType, "sensor"))
			{
				CNode *pChildNode = CreateSensor(parentNode, connection, pos, true);
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
void CCreature::CreateJoint( CNode *parentNode, CNode *childNode, genotype_parser::SConnection *connect, const PxVec3 &conPos )
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

	PxJoint* pxJoint = NULL;
	if (boost::iequals(joint->type, "fixed"))
	{
		PxFixedJoint *j = PxFixedJointCreate(m_Sample.getPhysics(), body, tm0, child, tm1 );
		pxJoint = j;
	}
	else if(boost::iequals(joint->type, "spherical"))
	{
		if (PxSphericalJoint *j = PxSphericalJointCreate(m_Sample.getPhysics(), body, tm0, child, tm1) )
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
		if (PxRevoluteJoint*j = PxRevoluteJointCreate(m_Sample.getPhysics(), body, tm0, child, tm1) )
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
CNode* CCreature::CreateSensor(CNode *parentNode, genotype_parser::SConnection *connect, const PxVec3 &initialPos, const bool IsTerminal)
	// IsTerminal = false
{
	RETV(!parentNode, NULL);

	CNode *childNode = NULL;

	CNode *pNode = new CNode(m_Sample);
	pNode->m_PaletteIndex = m_Nodes.size();
	pNode->m_IsTerminalNode = IsTerminal;

	PxVec3 dimension(0.05f, 0.2f, 0.05f);
	MaterialIndex material = GetMaterialType("red");
	{
		pNode->m_pBody = m_Sample.createBox(initialPos, dimension, NULL, m_Sample.getManageMaterial(material), 1.f);
	}
	m_Nodes.push_back(pNode);
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
	
	CJoint *pJoint = new CJoint(parentNode, childNode, tm0, tm1, pxJoint, velocity.x, joint->period);

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
	m_IncreaseTime += dtime;

	if (m_GrowCount < 3)
	{
		if (m_IncreaseTime > 1.f)
		{
			++m_GrowCount;
			m_IncreaseTime = 0;
			GenerateProgressive(m_pRoot, m_pGenotypeExpr);
			m_TmPalette.resize(m_Nodes.size());




		}
	}

	BOOST_FOREACH (auto &node, m_Nodes)
		node->Move(dtime);

	// update palette
	BOOST_FOREACH (auto &node, m_Nodes)
	{
		if (node->GetBody())
			m_TmPalette[ node->m_PaletteIndex] = node->GetBody()->getGlobalPose();
	}

	// update worldbound
	if (m_pRoot && m_pRoot->m_pRenderComposition)
	{
		PxBounds3 worldBound = m_pRoot->m_worldBounds;
		BOOST_FOREACH (auto &node, m_Nodes)
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
		m_pRoot->m_pRenderComposition->setWorldBounds(worldBound);
	}

	// fitness
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


/**
 @brief composite all node shape
 @date 2014-01-05
*/
void CCreature::GenerateRenderComposition( CNode *node )
{
	RET(!node);

	BOOST_FOREACH (auto joint, node->m_Joints)
	{
		GenerateRenderComposition((CNode*)joint->GetActor1());
	}

	const MaterialIndex materialIndex = GetMaterialType(node->m_MaterialName);
	RenderMaterial *material = m_Sample.getManageMaterial(materialIndex);

	PxRigidDynamic *rigidActor0 = node->GetBody();
	PxU32 nbShapes0 = rigidActor0->getNbShapes();
	if (!nbShapes0)
		return;
	PxShape** shapes0 = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*)*nbShapes0);
	PxU32 nb0 = rigidActor0->getShapes(shapes0, nbShapes0);
	PX_ASSERT(nb0==nbShapes0);

	RenderBaseActor *renderActor0 = m_Sample.getRenderActor(rigidActor0, shapes0[ 0]);
	if (renderActor0)
	{
		node->m_pRenderComposition = new RenderComposition(*m_Sample.getRenderer(), node->m_PaletteIndex, 
			m_TmPalette, renderActor0->getRenderShape(), material);

		renderActor0->setRendering(false);
		node->m_pRenderComposition->setEnableCameraCull(true);

		m_Sample.addRenderObject( node->m_pRenderComposition );
		m_Sample.removeRenderObject(renderActor0);
		m_Sample.unlink(renderActor0, shapes0[ 0], rigidActor0);
	}
	SAMPLE_FREE(shapes0);


	// composite child shape
	BOOST_FOREACH (auto joint, node->m_Joints)
	{
		CNode *child = (CNode*)joint->GetActor1();

		RenderComposition *p = node->m_pRenderComposition;
		node->m_pRenderComposition = new RenderComposition(*m_Sample.getRenderer(), 
			node->m_PaletteIndex, child->m_PaletteIndex,
			node->m_PaletteIndex, m_TmPalette, 
			(SampleRenderer::RendererCompositionShape*)p->getRenderShape(), joint->GetTm0(), 
			(SampleRenderer::RendererCompositionShape*)child->m_pRenderComposition->getRenderShape(), joint->GetTm1());

		node->m_pRenderComposition->setEnableCameraCull(true);

		m_Sample.addRenderObject( node->m_pRenderComposition );
		m_Sample.removeRenderObject(p);
		m_Sample.removeRenderObject(child->m_pRenderComposition);
		child->m_pRenderComposition = NULL;
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
