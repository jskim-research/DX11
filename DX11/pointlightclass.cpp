#include "pointlightclass.h"

void PointLightClass::SetAttenuationFactors(float a0, float a1, float a2)
{
    m_attenuationFactors = XMFLOAT3(a0, a1, a2);
}

XMFLOAT3 PointLightClass::GetAttenuationFactors() const
{
    return m_attenuationFactors;
}
