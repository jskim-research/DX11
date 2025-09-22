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
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

PixelInputType LightVertexShader(VertexInputType input)
{
	PixelInputType output;
	input.position.w = 1.0f;
	
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	// pixel shader 에서 다시 normalize 해야될 수도 있음 (interpolation 과정 때문에)
	output.normal = normalize(output.normal);

	return output;
}