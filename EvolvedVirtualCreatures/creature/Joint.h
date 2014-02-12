/**
 @filename Joint.h
 
  joint class
*/
#pragma once


namespace evc
{
	class CAngularSensor;
	class CMuscleEffector;
	class CPhysNode;
	class CJoint
	{
	public:
		CJoint(CPhysNode *actor0, CPhysNode *actor1, const PxTransform &tm0, const PxTransform &tm1, 
			PxJoint *joint=NULL, float velocity=1.f, float period=1.f);
		virtual ~CJoint();
		void Move(float dtime);
		double GetAngle() const;
		const CPhysNode* GetActor0() const;
		const CPhysNode* GetActor1() const;
		void GetOutputNerves( OUT vector<double> &out) const;
		PxTransform GetTm0() const;
		PxTransform GetTm1() const;
		int GetNeuronCount() const;
		void ApplySensor(CAngularSensor &sensor);
		void ApplyEffector(CMuscleEffector &effector);


	private:
		CPhysNode *m_pActor0;
		CPhysNode *m_pActor1;
		PxJoint *m_pJoint;
		float m_Velocity;
		float m_Period; // (dtime*m_Period) =  dtime
		float m_ElapseT;
		double m_RelativeAngle; // radian, -pi ~ 0 ~ pi
		PxTransform m_Tm0; // initial tm0
		PxTransform m_Tm1; // initial tm1
	};


	inline double CJoint::GetAngle() const { return m_RelativeAngle; }
	inline const CPhysNode* CJoint::GetActor0() const { return m_pActor0; }
	inline const CPhysNode* CJoint::GetActor1() const { return m_pActor1; }
	inline PxTransform CJoint::GetTm0() const { return m_Tm0; }
	inline PxTransform CJoint::GetTm1() const { return m_Tm1; }
}
