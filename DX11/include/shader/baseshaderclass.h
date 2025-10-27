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

	// ���� ����
	BaseShaderClass(const BaseShaderClass&);
	BaseShaderClass& operator=(const BaseShaderClass&);

public:
	/*
	*	const wchar_t* = > L"./cartoon.vs" ���� ����
	*/ 
	BaseShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename);
	virtual ~BaseShaderClass();

	/*
	*	���� �������̽��� �ڽ� Ŭ�������� ���� override ���� ����
	*/
	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool Render(BaseShaderInput* input, int indexCount);
	void Shutdown();

protected:
	virtual bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	virtual UINT GetInputElementDesc(D3D11_INPUT_ELEMENT_DESC** pInputElementDesc) const;
	// �ڽ� Ŭ�������� BaseShaderInput �� dynamic_cast ���� ���� ĳ�����ؼ� ����ؾ���
	virtual bool SetShaderParameters(BaseShaderInput* input);
	virtual void ShutdownShader();

	virtual void RenderShader(BaseShaderInput* input, int indexCount);
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

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