/**
 @filename Configure.h
 
 define debug configure variabl
*/
#pragma 



DECLARE_TYPE_NAME(SDbgConfig)
struct SDbgConfig : public memmonitor::Monitor<SDbgConfig, TYPE_NAME(SDbgConfig)>
{
	bool applyJoint;
	float force;
	float value1;
	float value2;
	int generationRecursiveCount;
};
