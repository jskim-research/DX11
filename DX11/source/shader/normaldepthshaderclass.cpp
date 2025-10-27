#include "shader/normaldepthshaderclass.h"
#include "shader/baseshaderinput.h"

NormalDepthShaderClass::NormalDepthShaderClass(const wchar_t* vsFilename, const wchar_t* psFilename):
	BaseShaderClass(vsFilename, psFilename)
{
}

void NormalDepthShaderClass::RenderShader(BaseShaderInput* input, int indexCount)
{
	input->d3dclass->UnbindDepthStencilView();
	input->deviceContext->PSSetShaderResources(0, 1, input->d3dclass->GetDepthStencilSRV());
	
	BaseShaderClass::RenderShader(input, indexCount);

	input->d3dclass->BindStandard_RTV_SRV();
}
