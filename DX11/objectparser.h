#ifndef _OBJECTPARSER_H_
#define _OBJECTPARSER_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;


class ObjectParser
{
public:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct GltfVertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		// XMFLOAT4 color;
	};

public:
	ObjectParser();
	bool ParseCustomFile(const char* fileName, VertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount);
	bool ParseCustomFile2(const char* fileName, VertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount);
	bool ParseGLTFFile(const char* fileName, GltfVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount);

private:
	void LogMessage(const char* fileName, const char* message);
	const char* logFileName = "./ObjectParserLogs.txt";
};


#endif