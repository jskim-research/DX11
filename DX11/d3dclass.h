#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

// 밑을 통해 visual studio 프로젝트 설정에 추가하지 않더라도 해당 라이브러리 자동 링크 (링커에게 지시)
// 헤더파일만 include 하더라도 선언은 있기에 컴파일은 가능하지만, 실제 구현 (기계어 코드)는 라이브러리에 있기 때문에 링크하지 않으면 링커 에러 (LNK2019) 발생
// library for setting up and drawing 3D graphics in DirectX11
#pragma comment(lib, "d3d11.lib")
// library contains tools to interface with hardware on the computer to obtain information about the refresh rate of the monitor, video card being used, ...
#pragma comment(lib, "dxgi.lib")
// library contains functionality for compiling shaders 
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class D3DClass
{
public:
    D3DClass();
    D3DClass(const D3DClass&);
    ~D3DClass();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetProjectionMatrix(XMMATRIX&);
    void GetWorldMatrix(XMMATRIX&);
    void GetOrthoMatrix(XMMATRIX&);

    void GetVideoCardInfo(char*, int&);

    void SetBackBufferRenderTarget();
    void ResetViewport();

private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11RasterizerState* m_rasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    D3D11_VIEWPORT m_viewport;
};


#endif