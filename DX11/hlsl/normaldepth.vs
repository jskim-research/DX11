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
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};



PixelInputType VSMain(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.normal = input.normal;
	output.tex = input.tex;

	return output;
}