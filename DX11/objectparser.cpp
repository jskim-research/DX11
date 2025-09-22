#include "objectparser.h"
#include <fstream>
#include <string>
#include <vector>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

using namespace std;

ObjectParser::ObjectParser()
{
    // log file 초기화
    ofstream writeFile;
    writeFile.open(logFileName);
    writeFile.close();
}

bool ObjectParser::ParseCustomFile(const char* fileName, VertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount)
{
    ifstream readFile;
    bool isSuccess = false;
    int maxVertexCount = 0;
    int curVertexIndex = 0;

    readFile.open(fileName);

    if (readFile.is_open())
    {
        while (!readFile.eof())
        {
            string str;
            getline(readFile, str);

            if (str == "")
            {
                // 처리 필요 X
            }
            else if (str.substr(0, 12) == "Vertex Count")
            {
                size_t valueIndex = str.find(':', 0);

                if (valueIndex == string::npos)
                {
                    LogMessage(logFileName, "Vertex Count format is wrong");
                    break;
                }
                else
                {
                    // stoi 실패할 가능성 있을거같은데 어떻게 처리?
                    string valueString = str.substr(valueIndex + 1, str.size() - valueIndex - 1);                    
                    maxVertexCount = stoi(valueString);
                    (*vertices) = new VertexType[maxVertexCount];
                    (*indices) = new unsigned long[maxVertexCount];
                }
            }
            else if (str.substr(0, 4) == "Data")
            {
                // 처리 필요 X
            }
            else
            {
                if (vertexCount == 0)
                {
                    LogMessage(logFileName, "Vertex Count is zero.");
                    break;

                }
                if (maxVertexCount <= curVertexIndex)
                {
                    // vertex count 보다 더 많이 들어올 수 없다.
                    LogMessage(logFileName, "Too many vertices exist.");
                    break;
                }
                
                vector<float> v;
                size_t spaceIdx;
                str += ' ';  // 마지막 단어가 npos 에 의해 무시되지 않도록 넣어줌

                while ((spaceIdx = str.find(' ')) != string::npos)
                {
                    if (spaceIdx == 0)
                    {
                        // 단어 없이 공백만 바로 나온 경우 제거
                        str = str.substr(1, str.size() - 1);
                    }
                    else
                    {
                        v.push_back(stof(str.substr(0, spaceIdx)));
                        str = str.substr(spaceIdx + 1, str.size());
                    }
                }

                if (v.size() != 8)
                {
                    // x y z tu tv nx ny nz 들어왔다 가정하므로
                    LogMessage(logFileName, "Some vertex does not have exactly 8 elements.");
                    break;
                }

                (*vertices)[curVertexIndex].position = XMFLOAT3(v[0], v[1], v[2]);
                (*vertices)[curVertexIndex].texture = XMFLOAT2(v[3], v[4]);
                (*vertices)[curVertexIndex].normal = XMFLOAT3(v[5], v[6], v[7]);

                (*indices)[curVertexIndex] = curVertexIndex;

                curVertexIndex++;
            }
        }

        if (vertexCount != 0 && maxVertexCount == curVertexIndex)
        {
            isSuccess = true;
            // 현재 custom file format 상 vertex, index 개수가 같음
            *vertexCount = maxVertexCount;
            *indexCount = maxVertexCount;
        }
    }
    else
    {
        LogMessage(logFileName, "Can't open custom file.");
    }

    readFile.close();
    return isSuccess;
}

bool ObjectParser::ParseCustomFile2(const char* fileName, VertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount)
{
    ifstream file;
    char c;
    int count = 0;
    int test;
    bool isSuccess = false;

    file.open(fileName);

    if (file.is_open())
    {
        file.get(c);

        while (c != ':') file.get(c);

        file >> count;

        (*vertices) = new VertexType[count];
        (*indices) = new unsigned long[count];

        file.get(c);
        while (c != ':') file.get(c);

        file.get(c);
        file.get(c);

        for (int i = 0; i < count; i++)
        {
            file >> (*vertices)[i].position.x;
            file >> (*vertices)[i].position.y;
            file >> (*vertices)[i].position.z;

            file >> (*vertices)[i].texture.x;
            file >> (*vertices)[i].texture.y;

            file >> (*vertices)[i].normal.x;
            file >> (*vertices)[i].normal.y;
            file >> (*vertices)[i].normal.z;

            (*indices)[i] = i;
        }

        *vertexCount = count;
        *indexCount = count;
        isSuccess = true;
    }

    file.close();
    return isSuccess;
}

bool ObjectParser::ParseGLTFFile(const char* fileName, GltfVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount, std::vector<tinygltf::Image>& images)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    string err;
    string warn;
    bool isBinary = false;

    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, fileName); // GLB
    if (!ret)
    {
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName); // glTF
        if (!ret)
        {
            LogMessage(logFileName, ("Failed to load glTF/GLB file: " + err).c_str());
            return false;
        }
    }

    images = model.images;

    if (!warn.empty())
    {
        LogMessage(logFileName, warn.c_str());
    }

    if (model.meshes.empty())
    {
        LogMessage(logFileName, "No mesh in glTF file.");
        return false;
    }

    // 첫 번째 mesh, 첫 번째 primitive 사용
    const tinygltf::Mesh& mesh = model.meshes[0];
    if (mesh.primitives.empty())
    {
        LogMessage(logFileName, "Mesh has no primitives.");
        return false;
    }

    const tinygltf::Primitive& primitive = mesh.primitives[0];

    // Positions
    auto posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
    auto posBufferView = model.bufferViews[posAccessor.bufferView];
    const float* posBuffer = reinterpret_cast<const float*>(&model.buffers[posBufferView.buffer].data[posBufferView.byteOffset + posAccessor.byteOffset]);
    int vertexNum = posAccessor.count;

    // Normals
    const float* normalBuffer = nullptr;
    auto normalIt = primitive.attributes.find("NORMAL");
    if (normalIt != primitive.attributes.end())
    {
        auto normAccessor = model.accessors[normalIt->second];
        auto normBufferView = model.bufferViews[normAccessor.bufferView];
        normalBuffer = reinterpret_cast<const float*>(&model.buffers[normBufferView.buffer].data[normBufferView.byteOffset + normAccessor.byteOffset]);
    }

    // UV
    const float* uvBuffer = nullptr;
    auto uvIt = primitive.attributes.find("TEXCOORD_0");
    if (uvIt != primitive.attributes.end())
    {
        auto uvAccessor = model.accessors[uvIt->second];
        auto uvBufferView = model.bufferViews[uvAccessor.bufferView];
        uvBuffer = reinterpret_cast<const float*>(&model.buffers[uvBufferView.buffer].data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
    }

    // Color
    const float* colorBuffer = nullptr;
    auto colorIt = primitive.attributes.find("COLOR_0");
    if (colorIt != primitive.attributes.end())
    {
        auto colorAccessor = model.accessors[colorIt->second];
        auto colorBufferView = model.bufferViews[colorAccessor.bufferView];
        colorBuffer = reinterpret_cast<const float*>(&model.buffers[colorBufferView.buffer].data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
    }

    // Indices
    int indexNum = primitive.indices >= 0 ? model.accessors[primitive.indices].count : vertexNum;
    *vertices = new GltfVertexType[vertexNum];
    *indices = new unsigned long[indexNum];

    for (int i = 0; i < vertexNum; i++)
    {
        (*vertices)[i].position = XMFLOAT3(posBuffer[i * 3 + 0], posBuffer[i * 3 + 1], posBuffer[i * 3 + 2]);

        if (normalBuffer)
            (*vertices)[i].normal = XMFLOAT3(normalBuffer[i * 3 + 0], normalBuffer[i * 3 + 1], normalBuffer[i * 3 + 2]);
        else
            (*vertices)[i].normal = XMFLOAT3(0.f, 0.f, 0.f);

        // specify uv from which model.textures
        int texIndex = model.materials[primitive.material].pbrMetallicRoughness.baseColorTexture.index;
        // specify uv from which model.images => 실제로 이미지 담고 있음
        int imgIndex = model.textures[texIndex].source;

        if (uvBuffer)
            (*vertices)[i].texture = XMFLOAT3(uvBuffer[i * 2 + 0], uvBuffer[i * 2 + 1], imgIndex);
        else
            (*vertices)[i].texture = XMFLOAT3(0.f, 0.f, 0.f);
        
        if (colorBuffer)
            (*vertices)[i].color = XMFLOAT4(colorBuffer[i * 4 + 0], colorBuffer[i * 4 + 1], colorBuffer[i * 4 + 2], colorBuffer[i * 4 + 3]);
        else
            (*vertices)[i].color = XMFLOAT4(1.f, 1.f, 1.f, 1.f); // 기본 흰색
       
    }

    // Index 처리
    if (primitive.indices >= 0)
    {
        auto idxAccessor = model.accessors[primitive.indices];
        auto idxBufferView = model.bufferViews[idxAccessor.bufferView];
        const unsigned char* rawIndices = &model.buffers[idxBufferView.buffer].data[idxBufferView.byteOffset + idxAccessor.byteOffset];

        for (int i = 0; i < indexNum; i++)
        {
            switch (idxAccessor.componentType)
            {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                (*indices)[i] = ((unsigned char*)rawIndices)[i];
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                (*indices)[i] = ((unsigned short*)rawIndices)[i];
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                (*indices)[i] = ((unsigned int*)rawIndices)[i];
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < indexNum; i++)
            (*indices)[i] = i;
    }

    *vertexCount = vertexNum;
    *indexCount = indexNum;

    return true;
}

bool ObjectParser::ExtractTexturesFromGLB(const char* glbFilePath, const char* outputFolder)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    string err;
    string warn;

    // GLB 로드
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, glbFilePath);
    if (!ret)
    {
        return false;
    }

    // images 추출
    for (size_t i = 0; i < model.images.size(); ++i)
    {
        const tinygltf::Image& image = model.images[i];
        string fileName;

        if (!image.image.empty())
        {
            // 파일 이름 생성
            if (!image.name.empty())
                fileName = string(outputFolder) + "/" + image.name;
            else
                fileName = string(outputFolder) + "/image_" + to_string(i);

            // mimeType에 따라 확장자 결정
            if (image.mimeType == "image/png")
                fileName += ".png";
            else if (image.mimeType == "image/jpeg")
                fileName += ".jpg";
            else
                fileName += ".bin"; // 알 수 없는 타입

            stbi_write_png(fileName.c_str(), image.width, image.height, image.component,
                image.image.data(), image.width * image.component);
        }
        
    }

    return true;
}

void ObjectParser::LogMessage(const char* fileName, const char* message)
{
    ofstream writeFile;
    writeFile.open(fileName, fstream::out | fstream::app);

    string str = message;
    str += "\n";

    if (writeFile.is_open())
    {
        writeFile.write(str.c_str(), str.size());
    }
    
    writeFile.close();
}
