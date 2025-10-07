cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer UtilityVariableBufferType : register(b1)
{
	bool isOutline;
	float3 util_padding1;
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

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	float3 viewNormal = mul(output.normal, (float3x3)viewMatrix);
	viewNormal = normalize(viewNormal);

	if (isOutline && viewNormal.z >= 0)
	{
		float k;
		float M = pow((output.position.x * viewNormal.z - output.position.z * viewNormal.x), 2);
		// screen width = 1280
		// screen height = 720
		// FOV = pi / 4 가정
		// d3dclass.cpp Initialize 함수 참고
		float alpha = 1280 / 720;
		float FOV = 3.141592654f / 4.0f;
		float focalLength = 1 / tan(FOV / 2);
		M += pow(alpha, 2) * pow((output.position.y * viewNormal.z - output.position.z * viewNormal.y), 2);
		
		float outlineDistance = 0.005;
		outlineDistance /= focalLength;
		float denorm = (sqrt(M) - output.position.z * viewNormal.z * outlineDistance);
		if (denorm < 0.000001) k = 0;
		else k = (pow(output.position.z, 2) * outlineDistance) / denorm;

		// 거리에 따른 position 조정
		output.position += k * float4(viewNormal, 0);
		// 거리 상관 없이 position 조정
		// output.position += outlineDistance * float4(viewNormal, 0);
		output.imageIndex = -1;
	}
	else
	{
		output.color = input.color;
		output.tex = input.tex;
		output.imageIndex = input.imageIndex;
	}

	output.position = mul(output.position, projectionMatrix);



	return output;
}