#ifndef _POINTLIGHTCLASS_H_
#define _POINTLIGHTCLASS_H_

#include "lightclass.h"

class PointLightClass : public LightClass
{
public:
	void SetAttenuationFactors(float, float, float);
	XMFLOAT3 GetAttenuationFactors() const;

private:
	XMFLOAT3 m_attenuationFactors;
};

#endif