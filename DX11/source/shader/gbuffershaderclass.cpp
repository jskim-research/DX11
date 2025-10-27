#include "shader/gbuffershaderclass.h"
#include "shader/baseshaderinput.h"

GBufferShaderClass::GBufferShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename):
	BaseShaderClass(vsFilename, psFilename)
{
}

void GBufferShaderClass::RenderShader(BaseShaderInput* input, int indexCount)
{
	// depth SRV ����� ���ؼ� RTV ���� unbind �������
	input->d3dclass->UnbindDepthStencilView();
	input->deviceContext->PSSetShaderResources(0, 1, input->d3dclass->GetAlbedoSRV());
	input->deviceContext->PSSetShaderResources(1, 1, input->d3dclass->GetNormalSRV());
	input->deviceContext->PSSetShaderResources(2, 1, input->d3dclass->GetDepthStencilSRV());
	BaseShaderClass::RenderShader(input, indexCount);

}
