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

    void SetRasterizerFrontCounterClockwise(bool isFrontCounterClockwise);
    void SetZBufferOnOff(bool isZBufferOn);

    inline int GetScreenWidth() const { return m_screenWidth; }
    inline int GetScreenHeight() const { return m_screenHeight; }

    // 포인터로 반환한다는 점에서 Get 으로 선언하는 의미가 사라짐
    // d3dclass 가 가지고 있는 자원을 shader 쪽에서 참조해서 pipeline 에 bind 하는 구조라 그런데
    // shader 쪽으로 뺄 수 있는 지 확인해봐야함.
    ID3D11ShaderResourceView** GetDepthStencilSRV();
    ID3D11ShaderResourceView** GetAlbedoSRV();
    ID3D11ShaderResourceView** GetNormalSRV();

    void UnbindDepthStencilView();
    /*
    *   OMSetRenderTargets 기본 세팅으로 변경
    */
    void BindStandard_RTV_SRV();
    void BindGBuffer();

private:
    int m_screenWidth;
    int m_screenHeight;
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RenderTargetView* m_renderTargetView;
    /*
    *   Depth Texture => RTV, SRV
    */
    ID3D11Texture2D* m_depthStencilBuffer;
    ID3D11DepthStencilState* m_depthStencilState;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11ShaderResourceView* m_depthStencilSRV;

    /*
    *   Albedo Texture => RTV, SRV
    */
    ID3D11Texture2D* m_albedoTexture;
    ID3D11RenderTargetView* m_albedoRTV;
    ID3D11ShaderResourceView* m_albedoSRV;

    /*
    *   Normal Texture => RTV, SRV
    */
    ID3D11Texture2D* m_normalTexture;
    ID3D11RenderTargetView* m_normalRTV;
    ID3D11ShaderResourceView* m_normalSRV;

    ID3D11RasterizerState* m_rasterState;
    ID3D11RasterizerState* m_outlineRasterState;
    XMMATRIX m_projectionMatrix;
    XMMATRIX m_worldMatrix;
    XMMATRIX m_orthoMatrix;
    D3D11_VIEWPORT m_viewport;
    ID3D11DepthStencilState* m_depthStencilStateNotUsingZBuffer;
};


#endif