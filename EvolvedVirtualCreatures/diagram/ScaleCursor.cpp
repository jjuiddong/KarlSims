
#include "stdafx.h"
#include "ScaleCursor.h"
#include "../EvolvedVirtualCreatures.h"
#include "../diagram/GenotypeNode.h"


using namespace evc;

CScaleCursor::CScaleCursor(CEvc &sample) :
	m_sample(sample)
,	m_isEditScale(false)
{
	m_colors.push_back(PxVec3(0.75f,0,0));
	m_colors.push_back(PxVec3(0,0.75f,0));
	m_colors.push_back(PxVec3(0,0,0.75f));

	m_axises.push_back(PxVec3(1,0,0));
	m_axises.push_back(PxVec3(0,1,0));
	m_axises.push_back(PxVec3(0,0,1));
}

CScaleCursor::~CScaleCursor()
{
	BOOST_FOREACH (auto obj, m_lines)
		m_sample.removeRenderObject(obj);
	m_lines.clear();
}


/**
 @brief select node event
 @date 2014-03-07
*/
void CScaleCursor::onSelectNode(CGenotypeNode *node)
{
	if (!node)
	{
		BOOST_FOREACH (auto line, m_lines)
			line->setRendering(false);
	}
	else
	{
		vector<PxVec3> points1;
		vector<PxVec3> points2;
		vector<PxVec3> points3;

		const float offset = 0.1f;
		const PxVec3 dim = utility::Vec3toPxVec3(node->m_expr->dimension) + PxVec3(1,1,1)*offset;
		
		PxQuat q = m_selectNode->GetLocalTransform().q;
		const PxVec3 xAxis = q.rotate(m_axises[0]);
		const PxVec3 yAxis = q.rotate(m_axises[1]);
		const PxVec3 zAxis = q.rotate(m_axises[2]);

		// x axis
		points1.push_back(node->GetPos());
		points1.push_back(node->GetPos());
		points1.push_back(node->GetPos()+xAxis*dim.magnitude());
		points1.push_back(node->GetPos()+xAxis*dim.magnitude());

		// y axis
		points2.push_back(node->GetPos());
		points2.push_back(node->GetPos());
		points2.push_back(node->GetPos()+yAxis*dim.magnitude());
		points2.push_back(node->GetPos()+yAxis*dim.magnitude());

		// z axis
		points3.push_back(node->GetPos());
		points3.push_back(node->GetPos());
		points3.push_back(node->GetPos()+zAxis*dim.magnitude());
		points3.push_back(node->GetPos()+zAxis*dim.magnitude());

		if (m_lines.empty())
		{
			m_lines.push_back(new RenderBezierActor(*m_sample.getRenderer(), points1, m_colors[0]));
			m_lines.push_back(new RenderBezierActor(*m_sample.getRenderer(), points2, m_colors[1]));
			m_lines.push_back(new RenderBezierActor(*m_sample.getRenderer(), points3, m_colors[2]));

			m_lines[ 0]->setRenderMaterial( m_sample.GetMaterial(m_colors[0], false) );
			m_lines[ 1]->setRenderMaterial( m_sample.GetMaterial(m_colors[1], false) );
			m_lines[ 2]->setRenderMaterial( m_sample.GetMaterial(m_colors[2], false) );

			m_sample.addRenderObject(m_lines[0]);
			m_sample.addRenderObject(m_lines[1]);
			m_sample.addRenderObject(m_lines[2]);
		}
		else
		{
			m_lines[ 0]->SetBezierCurve(points1);
			m_lines[ 1]->SetBezierCurve(points2);
			m_lines[ 2]->SetBezierCurve(points3);
		}

		BOOST_FOREACH (auto line, m_lines)
			line->setRendering(true);
	}
}


/**
 @brief Pick scale axis
 @date 2014-03-10
*/
bool CScaleCursor::PickScaleAxis(physx::PxU32 x, physx::PxU32 y, OUT AXIS &out)
{
	PxVec3 orig, dir, pickOrig;
	m_sample.GetPicking()->computeCameraRay(orig, dir, pickOrig, x, y);
	for (u_int i=0; i < m_lines.size(); ++i)
	{
		PxVec3 pos;
		if (m_lines[ i]->IntersectTri(orig, dir, pos))
		{
			out = (AXIS)i;
			return true;
		}
	}
	return false;
}


/**
 @brief select axis
 @date 2014-03-10
*/
void CScaleCursor::SelectAxis(const bool isSelect, const AXIS axis)
{
	m_isEditScale = isSelect;
	m_selectAxis = axis;

	// initialize
	for (u_int i=0; i < m_lines.size(); ++i)
		m_lines[ i]->setRenderMaterial( m_sample.GetMaterial(m_colors[i], false) );

	// setting select color
	if (isSelect)
		m_lines[ axis]->setRenderMaterial( m_sample.GetMaterial(m_colors[axis]+PxVec3(0.25f,0.25f,0.25f), false) );															
}


/**
 @brief Edit Scale by mouse delta position
 @date 2014-03-10
*/
void CScaleCursor::EditScale(CGenotypeNode *node, const Int2 initialPos, const Int2 curPos, const AXIS axis)
{
	RET(!node);
	RET(initialPos == curPos);

	PxVec3 orig0, dir0, pickOrig0;
	m_sample.GetPicking()->computeCameraRay(orig0, dir0, pickOrig0, initialPos.x, initialPos.y);

	PxVec3 orig1, dir1, pickOrig1;
	m_sample.GetPicking()->computeCameraRay(orig1, dir1, pickOrig1, curPos.x, curPos.y);

	const float dist = (node->GetPos() - orig1).magnitude();
	pickOrig0 += dir0 * dist;
	pickOrig1 += dir1 * dist;

	const PxQuat q = node->GetLocalTransform().q;
	const PxVec3 vAxis = q.rotate(m_axises[axis]);
	const float deltaLen = (pickOrig1 - pickOrig0).dot(vAxis);

	const float offset = 0.1f;
	const PxVec3 dim = utility::Vec3toPxVec3(node->m_expr->dimension);	
	float axisLength = dim.magnitude() + offset + deltaLen;

	vector<PxVec3> points;
	const PxVec3 nodePos = m_selectNode->GetPos();
	points.push_back(nodePos);
	points.push_back(nodePos);
	points.push_back(nodePos+vAxis*axisLength);
	points.push_back(nodePos+vAxis*axisLength);
	m_lines[ axis]->SetBezierCurve(points);

	// setting dimension
	float dimensionValue = 0.01f;
	switch (axis)
	{
	case X_AXIS: 
		dimensionValue = max(dim.x + deltaLen, 0.01f);
		node->m_expr->dimension.x = dimensionValue;
		break;
	case Y_AXIS: 
		dimensionValue = max(dim.y + deltaLen, 0.01f);
		node->m_expr->dimension.y = dimensionValue; 
		break;
	case Z_AXIS: 
		dimensionValue = max(dim.z + deltaLen, 0.01f);
		node->m_expr->dimension.z = dimensionValue; 
		break;
	}

	// update dimension
	if (boost::iequals(node->m_expr->shape, "box"))
	{
		((RenderBoxActor*)node->m_renderNode)->SetBoxShape(utility::Vec3toPxVec3(node->m_expr->dimension), NULL);
	}
	else if (boost::iequals(node->m_expr->shape, "sphere"))
	{
		node->m_expr->dimension.x = dimensionValue; // sphere always store radius in x
		((RenderSphereActor*)node->m_renderNode)->setDimensions(0, dimensionValue,dimensionValue);
	}
}


/**
 @brief Mouse LButton Down
 @date 2014-03-09
*/
bool CScaleCursor::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{
	AXIS axis;
	const bool isPick = PickScaleAxis(x,y,axis);
	if (isPick)
	{
		SelectAxis(true, axis);
		m_mousePos = Int2(x,y);
	}
	return isPick;
}


/**
 @brief MouseLButtonUp Event
 @date 2014-03-10
*/
bool CScaleCursor::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{
	if (m_isEditScale)
	{
		SelectAxis(false, X_AXIS);
		return true;
	}
	else
	{
		return false;
	}
}


/**
 @brief Mouse Move
 @date 2014-03-10
*/
bool CScaleCursor::MouseMove(physx::PxU32 x, physx::PxU32 y)
{
	if (m_isEditScale)
	{
		const Int2 curPos(x,y);
		EditScale(m_selectNode, m_mousePos, curPos, m_selectAxis);
		m_mousePos = curPos;
	}
	return m_isEditScale;
}
