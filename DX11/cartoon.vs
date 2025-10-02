cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 tex : TEXCOORD0;
	nointerpolation uint imageIndex : TEXCOORD1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 worldPosition : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 tex : TEXCOORD0;
	nointerpolation uint imageIndex : TEXCOORD1;
};

PixelInputType CartoonVertexShader(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	
	output.position = mul(input.position, worldMatrix);
	output.worldPosition = output.position;
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	
	/*
	float3 testNormal = normalize(mul(output.normal, (float3x3)viewMatrix));
	if (dot(testNormal, float3(0, 0, 1)) > 0)
	{
		output.position = output.position + float4(output.normal, 0) * 0.05;
		output.tex = -1;
		output.normal = -output.normal;
	}
	*/

	output.tex = input.tex;
	output.imageIndex = input.imageIndex;
	return output;
}