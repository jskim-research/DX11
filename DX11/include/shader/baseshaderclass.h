#ifndef _BASESHADERCLASS_H_
#define _BASESHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include <vector>
using namespace DirectX;
using namespace std;

struct BaseShaderInput;

class BaseShaderClass
{
private:
	struct MatrixBuffer
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	// 복사 금지
	BaseShaderClass(const BaseShaderClass&);
	BaseShaderClass& operator=(const BaseShaderClass&);

public:
	/*
	*	const wchar_t* = > L"./cartoon.vs" 같은 형태
	*/ 
	BaseShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);
	virtual ~BaseShaderClass();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool Render(BaseShaderInput* input, int indexCount);
	void Shutdown();

protected:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	virtual UINT GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const;
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	// 자식 클래스에서 BaseShaderInput 을 dynamic_cast 등을 통해 캐스팅해서 사용해야함
	// assert 를 통해 잘못된 입력 방지하기
	virtual bool SetShaderParameters(BaseShaderInput* input);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
	virtual void ShutdownShader();

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_samplerState;
	ID3D11Buffer* m_matrixBuffer;
	const wchar_t* m_vsFilename;
	const wchar_t* m_psFilename;
};



#endif