cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	// BaseVertexType 변수들 및 Input Layout 순서와 맞춰줄 것
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
	nointerpolation uint imageIndex : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
	nointerpolation uint imageIndex : TEXCOORD1;
};

PixelInputType VSMain(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	// Bitmap 은 일부러 CPU 단에서 값 조정해서 바로 화면에 뜨도록 함
	// projectionMatrix 도 Orthomatrix 임
	output.position = mul(input.position, projectionMatrix);
	output.normal = input.normal;
	output.tex = input.tex;
	output.imageIndex = input.imageIndex;
	output.color = input.color;

	return output;
}