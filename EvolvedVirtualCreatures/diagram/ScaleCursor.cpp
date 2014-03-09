
#include "stdafx.h"
#include "ScaleCursor.h"
#include "../EvolvedVirtualCreatures.h"
#include "../diagram/GenotypeNode.h"


using namespace evc;

CScaleCursor::CScaleCursor(CEvc &sample) :
	m_sample(sample)
{

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

		const PxVec3 dim = utility::Vec3toPxVec3(node->m_expr->dimension);
		
		// x axis
		points1.push_back(node->GetPos());
		points1.push_back(node->GetPos());
		points1.push_back(node->GetPos()+PxVec3(1,0,0)*dim.magnitude());
		points1.push_back(node->GetPos()+PxVec3(1,0,0)*dim.magnitude());

		// y axis
		points2.push_back(node->GetPos());
		points2.push_back(node->GetPos());
		points2.push_back(node->GetPos()+PxVec3(0,1,0)*dim.magnitude());
		points2.push_back(node->GetPos()+PxVec3(0,1,0)*dim.magnitude());

		// z axis
		points3.push_back(node->GetPos());
		points3.push_back(node->GetPos());
		points3.push_back(node->GetPos()+PxVec3(0,0,1)*dim.magnitude());
		points3.push_back(node->GetPos()+PxVec3(0,0,1)*dim.magnitude());

		if (m_lines.empty())
		{
			m_lines.push_back(new RenderBezierActor(*m_sample.getRenderer(), points1, PxVec3(0.75f,0,0)));
			m_lines.push_back(new RenderBezierActor(*m_sample.getRenderer(), points2, PxVec3(0,0.75f,0)));
			m_lines.push_back(new RenderBezierActor(*m_sample.getRenderer(), points3, PxVec3(0,0,0.75f)));

			m_lines[ 0]->setRenderMaterial( m_sample.GetMaterial(PxVec3(0.75f,0,0)) );
			m_lines[ 1]->setRenderMaterial( m_sample.GetMaterial(PxVec3(0.75f,0,0)) );
			m_lines[ 2]->setRenderMaterial( m_sample.GetMaterial(PxVec3(0.75f,0,0)) );

			m_sample.addRenderObject(m_lines[0]);
			m_sample.addRenderObject(m_lines[1]);
			m_sample.addRenderObject(m_lines[2]);
		}
		else
		{
			m_lines[ 0]->setRenderMaterial( m_sample.GetMaterial(PxVec3(0.75f,0,0)) );
			m_lines[ 1]->setRenderMaterial( m_sample.GetMaterial(PxVec3(0.75f,0,0)) );
			m_lines[ 2]->setRenderMaterial( m_sample.GetMaterial(PxVec3(0.75f,0,0)) );

			m_lines[ 0]->SetBezierCurve(points1, PxVec3(0.75f,0,0));
			m_lines[ 1]->SetBezierCurve(points2, PxVec3(0,0.75f,0));
			m_lines[ 2]->SetBezierCurve(points3, PxVec3(0,0,0.75f));
		}

		BOOST_FOREACH (auto line, m_lines)
			line->setRendering(true);
	}
}
