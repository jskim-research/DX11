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
	// gbuffer combine 시 (-1, -1) 부터 (1, 1) 까지 덮는 형태의 triangle vertices 입력됨
	// 이에 따라 pixel shader 에서 모든 픽셀이 그려질 수 있도록 함
	output.position = input.position;
	output.normal = input.normal;
	output.tex = input.tex;

	return output;
}