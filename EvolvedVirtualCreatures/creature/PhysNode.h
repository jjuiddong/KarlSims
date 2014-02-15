/**
@filename Node.h

*/
#pragma once


#include "PhysXSample.h"
class CEvc;
class RenderComposition;


namespace evc
{
	class CEffector;
	class CSensor;
	class CJoint;
	class CNeuralNet;
	namespace genotype_parser { struct SExpr; }

	class CPhysNode
	{
	public:
		CPhysNode(CEvc &sample);
		virtual ~CPhysNode();
		void InitBrain(const vector<double> &weights = vector<double>());
		void GetOutputNerves(OUT vector<double> &out) const;
		int GetNeuronCount() const;
		void Move(float dtime);
		PxRigidDynamic* GetBody();
		PxShape* GetShape();
		const CNeuralNet* GetBrain() const;
		void GetAllSensor(OUT vector<CSensor*> &out) const;
		void GetAllEffector(OUT vector<CEffector*> &out) const;


	protected:
		void setCollisionGroup(PxRigidActor* actor, PxU32 group);
		void UpdateNeuron(float dtime);


	public:
		CEvc &m_Sample;
		string m_Name;
		string m_ShapeName;
		//string m_MaterialName;
		PxVec3 m_MaterialDiffuse;
		bool m_IsTerminalNode;
		bool m_IsKinematic;
		PxRigidDynamic *m_pBody;
		PxShape *m_pShape;
		vector<CJoint*> m_Joints;
		vector<CSensor*> m_Sensors;
		vector<CEffector*> m_Effectors;
		CSensor *m_pParentJointSensor;
		RenderComposition *m_pShapeRenderer;
		RenderComposition *m_pOriginalShapeRenderer;
		int m_PaletteIndex; // tm palette index
		PxBounds3 m_worldBounds;
		PxVec3 m_Dimension;

		// brain
		CNeuralNet *m_pBrain;
		vector<CSensor*> m_AllConnectSensor;		// 
		vector<CEffector*> m_AllConnectEffector;	// 
		vector<double> m_Nerves;
	};

	
	inline PxRigidDynamic* CPhysNode::GetBody() { return m_pBody; }
	inline PxShape* CPhysNode::GetShape() { return m_pShape; }
	inline const CNeuralNet* CPhysNode::GetBrain() const { return m_pBrain; }
}
