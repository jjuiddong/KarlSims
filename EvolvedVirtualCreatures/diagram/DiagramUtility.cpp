
#include "stdafx.h"
#include "DiagramUtility.h"
#include "GenotypeController.h"
#include "GenotypeNode.h"
#include "../EvolvedVirtualCreatures.h"


using namespace evc;


/**
 @brief create CDiagramNode
 @date 2014-02-25
*/
CGenotypeNode* evc::CreateGenotypeNode(CEvc &sample, const genotype_parser::SExpr *expr)
{
	const bool IsSensorNode = !expr;
	CGenotypeNode *node = new CGenotypeNode(sample);
	node->m_name = expr? expr->id : "sensor";
	if (expr)
	{
		node->m_expr = new genotype_parser::SExpr();
		*node->m_expr = *expr;
	}
	else
	{ // sensor
		node->m_expr = new genotype_parser::SExpr();
		node->m_expr->id = "sensor";
		node->m_expr->dimension = genotype_parser::SVec3(0.4f,0.4f,0.4f);
		node->m_expr->material = genotype_parser::SVec3(0,0.75f,0);
		node->m_expr->isSensor = true;
	}

	PxVec3 dimension = expr? utility::Vec3toPxVec3(expr->dimension) : PxVec3(0.4f,0.4f,0.4f);

	if (IsSensorNode)
	{ // Sensor
		node->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*sample.getRenderer(), dimension);
	}
	else if (boost::iequals(expr->shape, "sphere"))
	{
		node->m_renderNode = SAMPLE_NEW(RenderSphereActor)(*sample.getRenderer(), dimension.x);
	}
	else
	{
		node->m_renderNode = SAMPLE_NEW2(RenderBoxActor)(*sample.getRenderer(), dimension);
	}

	PxVec3 material = expr? utility::Vec3toPxVec3(expr->material) : PxVec3(0,0.75f,0);
	node->m_renderNode->setRenderMaterial( sample.GetMaterial(material, false) );
	node->m_material = material;

	return node;
}


/**
 @brief calcuate transition bezier curve position
 @date 2014-02-26
*/
void evc::CalcuateTransitionPositions(CGenotypeNode *from, CGenotypeNode *to, const u_int order, OUT vector<PxVec3> &out)
{
	PxVec3 pos = from->m_renderNode->getTransform().p;

	if (from == to)
	{
		const float dimensionY = to->m_expr? to->m_expr->dimension.y : 1.f;
		PxVec3 arrowPos = pos + PxVec3(-0.f,dimensionY+0.1f,0);

		const float offset = 0.7f;
		out.push_back( arrowPos );
		out.push_back( arrowPos + PxVec3(-offset,offset,0) );
		out.push_back( arrowPos + PxVec3(offset,offset,0) );
		out.push_back( arrowPos );
	}
	else
	{
		PxVec3 newNodePos = to->m_renderNode->getTransform().p;

		PxVec3 dir = newNodePos - pos;
		float len = dir.magnitude();
		dir.normalize();

		PxVec3 interSectPos;
		if (to->m_renderNode->IntersectTri(pos, dir, interSectPos))
		{
			dir = interSectPos - pos;
			len = dir.magnitude();
			dir.normalize();
			newNodePos = pos + dir*len;
		}

		PxVec3 c = dir.cross(PxVec3(0,0,((order % 2)? -1 : 1)));
		const float curveH = 0.4f + ((float)order/2.f) * 0.5f;

		out.push_back( pos );
		out.push_back( pos + (dir*len*0.5f) - c*curveH );
		out.push_back( out[1] );
		out.push_back( newNodePos );
	}
}


/**
 @brief Create Transition
 @date 2014-02-25
*/
RenderBezierActor* evc::CreateTransition(CEvc &sample, CGenotypeNode *from, CGenotypeNode *to, const PxVec3 &color, const u_int order)//order=0
{
	vector<PxVec3> points;
	CalcuateTransitionPositions(from, to, order, points);
	RenderBezierActor *arrow = new RenderBezierActor(*sample.getRenderer(), points, color);
	arrow->setRenderMaterial(sample.GetMaterial(color));
	return arrow;
}


/**
 @brief Move Transition Arrow
 @date 2014-02-26
*/
void evc::MoveTransition(RenderBezierActor *transition, CGenotypeNode *from, CGenotypeNode *to, const u_int order)
{
	RET(!transition);
	vector<PxVec3> points;
	CalcuateTransitionPositions(from, to, order, points);
	transition->SetBezierCurve(points);
}


/**
 @brief remove diagram
 @date 2014-02-12
*/
void RemoveGenotypeTreeRec(CEvc &sample, CGenotypeNode *node, set<CGenotypeNode*> &diagrams)
{
	RET(!node);
	if (diagrams.end() != diagrams.find(node))
		return; // already removed

	diagrams.insert(node);

	BOOST_FOREACH (auto conNode, node->m_connectDiagrams)
	{
		sample.removeRenderObject(conNode.transitionArrow);
		RemoveGenotypeTreeRec(sample, conNode.connectNode, diagrams);
	}
	node->m_connectDiagrams.clear();

	SAFE_DELETE(node);
}


/**
 @brief remove diagram
 @date 2014-03-03
*/
void evc::RemoveGenotypeTree(CEvc &sample, CGenotypeNode *node)
{
	set<CGenotypeNode*> symbols;
	RemoveGenotypeTreeRec(sample, node, symbols);
}


/**
 @brief calcuate diagram poisition
 @date 2014-02-25
*/
PxVec3 evc::GetDiagramPosition(CGenotypeNode *parent, CGenotypeNode *dispNode, OUT u_int &order)
{
	RETV(!parent, PxVec3(0,0,0));
	RETV(!dispNode, PxVec3(0,0,0));

	const PxVec3 dimension = utility::Vec3toPxVec3(parent->m_expr->dimension);
	const float radius = max(dimension.x*2.f +1, 2);

	set<string> symbols;
	PxVec3 val(radius,0,0);
	BOOST_FOREACH (auto child, parent->m_connectDiagrams)
	{
		CGenotypeNode *childNode = child.connectNode;
		if (boost::iequals(parent->m_name, child.connectNode->m_name))
			continue;
		if (symbols.end() != symbols.find(childNode->m_name))
			continue; // already exist
		if (child.connectNode == dispNode)
			break;

		symbols.insert(childNode->m_name);
		const PxVec3 childDimension = utility::Vec3toPxVec3(childNode->m_expr->dimension);
		const float height = max(childDimension.y*2.f +1, 2);
		val += PxVec3(0,height,0);
	}

	// find order
	order = 0;
	BOOST_FOREACH (auto child, parent->m_connectDiagrams)
	{
		if (child.connectNode == dispNode)
			++order;
	}

	return val + parent->m_renderNode->getTransform().p;
}


/**
@brief calcuate diagram poisition
 @date 2014-02-25
*/
PxVec3 evc::GetDiagramPositionByIndex(const genotype_parser::SExpr *parent_expr, const PxVec3 &parentPos, 
	const u_int index, OUT u_int &order)
{
	RETV(!parent_expr, PxVec3(0,0,0));

	using namespace genotype_parser;
	const PxVec3 dimension = utility::Vec3toPxVec3(parent_expr->dimension);
	const float radius = max(dimension.x*2.f +1, 2);

	order = 0;
	set<string> symbols;
	PxVec3 val(radius,0,0);

	int i=-1;
	SConnectionList *conList = parent_expr->connection;
	while (conList)
	{
		++i;
		
		SConnection *con = conList->connect;
		if (!con->expr) {
			conList = conList->next; // next node
			continue;
		}
		if (boost::iequals(parent_expr->id, con->expr->id)) {
			conList = conList->next; // next node
			continue;
		}
		if (symbols.end() != symbols.find(con->expr->id))
		{
			++order;
			if (i >= (int)index)
			{
				break;
			}
			else
			{
				conList = conList->next; // next node
				continue; // already exist
			}
		}
		else
		{
			order = 0;
		}

		if (i >= (int)index)
			break;
		symbols.insert(con->expr->id);

		const PxVec3 childDimension = utility::Vec3toPxVec3(con->expr->dimension);
		const float height = max(childDimension.y*2.f +1, 2);
		val += PxVec3(0,height,0);

		conList = conList->next; // next node
	}

	return val + parentPos;
}


/**
 @brief Calculate Joint Transfrom
 @date 2014-03-03
*/
void evc::GetJointTransform(const PxVec3 *pos, const genotype_parser::SConnection *joint, OUT PxTransform &tm0, OUT PxTransform &tm1, 
	const bool applyRandom)// applyRandom = false
{
	//const PxVec3 dir0(joint->parentOrient.dir.x, joint->parentOrient.dir.y, joint->parentOrient.dir.z);
	PxQuat moveToJointAxis;
	PxVec3 jointAxis = utility::Vec3toPxVec3(joint->jointAxis);
	utility::quatRotationArc(moveToJointAxis, jointAxis, PxVec3(1,0,0));

	//PxVec3 dir1 = utility::Vec3toPxVec3(joint->orient.dir);
	//PxVec3 localPos = utility::Vec3toPxVec3(joint->orient);
	//dir1 = moveToJointAxis.rotate(dir1);
	PxQuat rotate(joint->rotate.angle, utility::Vec3toPxVec3(joint->rotate.dir));
	PxVec3 conPos = (pos? *pos : utility::Vec3toPxVec3(joint->orient));

	if (applyRandom)
	{
		// random position
		PxVec3 randPos = utility::Vec3toPxVec3(joint->randPos);
		conPos += RandVec3(randPos, 1.f);

		// random orientation
		PxVec3 randOrient = utility::Vec3toPxVec3(joint->randOrient);
		//if (!localPos.isZero())
		//{
		//	localPos += RandVec3(randOrient, 1.f);
		//	localPos.normalize();
		//}
	}

	//tm0 = (dir0.isZero())? PxTransform::createIdentity() : PxTransform(PxQuat(joint->parentOrient.angle, dir0));
	tm0 = PxTransform(moveToJointAxis);
	tm1 = PxTransform(moveToJointAxis) * PxTransform(conPos) * PxTransform(rotate);
		//PxTransform(dir1);
		//PxTransform(PxQuat(joint->orient.angle, dir1));

	//(PxTransform(PxQuat(joint->orient.angle, dir1)) * PxTransform(PxVec3(conPos)));

	tm0 = tm0.getInverse();
	tm1 = tm1.getInverse();
}


/**
 @brief random vector from vector value and rate
 @date 2014-01-13
*/
PxVec3 evc::RandVec3( const PxVec3 &vec, const float rate )
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
PxVec3 evc::MaximumVec3( const PxVec3 &vec0, const PxVec3 &vec1 )
{
	PxVec3 val;
	val.x = max(vec0.x, vec1.x);
	val.y = max(vec0.y, vec1.y);
	val.z = max(vec0.z, vec1.z);
	return val;
}


/**
 @brief recursive find joint transform from to to node
 @date 2014-03-04
*/
bool GetJointTransformAccumulateRec(const PxTransform &tm, CGenotypeNode *from, CGenotypeNode *to, 
	set<CGenotypeNode*> &symbols, OUT PxTransform &out)
{
	using namespace genotype_parser;

	RETV(!from, false);
	RETV(!to, false);
	if (from == to)
	{
		out = tm;
		return true;
	}

	if (symbols.end() != symbols.find(from))
		return false;

	symbols.insert(from);

	BOOST_FOREACH (auto &con, from->m_connectDiagrams)
	{
		const SConnection *joint = from->GetJoint(con.connectNode);
		if (!joint)
			continue;

		PxTransform tm0, tm1;
		GetJointTransform(NULL, joint, tm0, tm1);
		tm0 = tm0.getInverse(); // genotype script return inverse transform to use joint parameter
		tm1 = tm1.getInverse();

		if (GetJointTransformAccumulateRec(tm * tm0.getInverse() * tm1, con.connectNode, to, symbols, out))
			return true;
	}

	return false;
}


/**
 @brief GetJointTransformAccumulate
 @date 2014-03-04
*/
PxTransform evc::GetJointTransformAccumulate(CGenotypeNode *from, CGenotypeNode *to)
{
	set<CGenotypeNode*> symbols;
	PxTransform val = PxTransform::createIdentity();
	GetJointTransformAccumulateRec(PxTransform::createIdentity(), from, to, symbols, val);
	return val;
}


/**
 @brief mouse position x,y to check node 3d object
 @date 2014-03-06
*/
CGenotypeNode* evc::PickupNodes(CEvc &sample, vector<CGenotypeNode*> &nodes, physx::PxU32 x, physx::PxU32 y, 
	const bool isShowHighLight)
{
	PxVec3 orig, dir, pickOrig;
	sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);

	float minLength = FLT_MAX;
	evc::CGenotypeNode *pickNode = NULL;
	BOOST_FOREACH(auto node, nodes)
	{
		PxVec3 out;
		const bool isHighLight = node->m_renderNode->IntersectTri(pickOrig, dir, out);
		if (isShowHighLight)
			node->SetHighLight(false);
		if (isHighLight)
		{
			const float len = (out - pickOrig).magnitude();
			if (len < minLength)
			{
				pickNode = node;
				minLength = len;
			}
		}
	}

	if (pickNode)
		pickNode->SetHighLight(true);

	return pickNode;
}
