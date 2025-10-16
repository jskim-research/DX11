#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <directxmath.h>
using namespace DirectX;

class LightClass
{
public:
    LightClass();
    LightClass(const LightClass&);
    ~LightClass();

    void SetDiffuseColor(float, float, float, float);
    void SetDirection(float, float, float);
    void SetPosition(float, float, float);

    XMFLOAT4 GetDiffuseColor();
    XMFLOAT3 GetDirection();
    XMFLOAT3 GetPosition();

private:
    XMFLOAT4 m_diffuseColor;
    XMFLOAT3 m_direction;
    XMFLOAT3 m_position;
};

#endif
