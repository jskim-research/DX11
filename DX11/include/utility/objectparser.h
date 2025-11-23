#ifndef _OBJECTPARSER_H_
#define _OBJECTPARSER_H_

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
using namespace DirectX;

#include "tiny_gltf.h"

class ObjectParser
{
public:
	struct BaseVertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texture;  // (u, v, texture idx)
	};

	struct CommonVertexType : public BaseVertexType
	{
		XMFLOAT4 color;
		uint32_t imageIndex;
	};

	// GPU Text Instancing 용 정보
	struct TextVertexInstanceType
	{
		XMFLOAT2 offset;  // render position
		XMFLOAT2 size;
		XMFLOAT4 uvRect;  // atlas 글자 영역
	};

public:
	ObjectParser();
	bool ParseCustomFile(const char* fileName, CommonVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount);
	// from rastertek tutorial
	bool ParseCustomFile2(const char* fileName, CommonVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount);
	bool ParseGLTFFile(const char* fileName, CommonVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount, std::vector<tinygltf::Image>& images);
	bool ExtractTexturesFromGLB(const char* glbFilePath, const char* outputFolder);

private:
	void LogMessage(const char* fileName, const char* message);
	const char* logFileName = "./log/ObjectParserLogs.txt";
};


#endif