#include "utility/objectparser.h"
#include <fstream>
#include <string>
#include <vector>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "utility/tiny_gltf.h"

using namespace std;

ObjectParser::ObjectParser()
{
    // log file �ʱ�ȭ
    ofstream writeFile;
    writeFile.open(logFileName);
    writeFile.close();
}

bool ObjectParser::ParseCustomFile(const char* fileName, CommonVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount)
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
                // ó�� �ʿ� X
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
                    // stoi ������ ���ɼ� �����Ű����� ��� ó��?
                    string valueString = str.substr(valueIndex + 1, str.size() - valueIndex - 1);                    
                    maxVertexCount = stoi(valueString);
                    (*vertices) = new CommonVertexType[maxVertexCount];
                    (*indices) = new unsigned long[maxVertexCount];
                }
            }
            else if (str.substr(0, 4) == "Data")
            {
                // ó�� �ʿ� X
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
                    // vertex count ���� �� ���� ���� �� ����.
                    LogMessage(logFileName, "Too many vertices exist.");
                    break;
                }
                
                vector<float> v;
                size_t spaceIdx;
                str += ' ';  // ������ �ܾ npos �� ���� ���õ��� �ʵ��� �־���

                while ((spaceIdx = str.find(' ')) != string::npos)
                {
                    if (spaceIdx == 0)
                    {
                        // �ܾ� ���� ���鸸 �ٷ� ���� ��� ����
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
                    // x y z tu tv nx ny nz ���Դ� �����ϹǷ�
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
            // ���� custom file format �� vertex, index ������ ����
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

bool ObjectParser::ParseCustomFile2(const char* fileName, CommonVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount)
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

        (*vertices) = new CommonVertexType[count];
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

            (*vertices)[i].imageIndex = 0;

            (*indices)[i] = i;
        }

        *vertexCount = count;
        *indexCount = count;
        isSuccess = true;
    }

    file.close();
    return isSuccess;
}

bool ObjectParser::ParseGLTFFile(const char* fileName, CommonVertexType** vertices, unsigned long** indices, int* vertexCount, int* indexCount, std::vector<tinygltf::Image>& images)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    // GLB �ε�
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, fileName);
    if (!ret)
    {
        // ASCII glTF �õ�
        ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);
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

    // ��ü vertex/ index ���� ���
    size_t totalVertexCount = 0;
    size_t totalIndexCount = 0;

    for (auto& mesh : model.meshes)
    {
        for (auto& prim : mesh.primitives)
        {
            auto posIt = prim.attributes.find("POSITION");
            if (posIt == prim.attributes.end())
                continue;

            totalVertexCount += model.accessors[posIt->second].count;

            if (prim.indices >= 0)
                totalIndexCount += model.accessors[prim.indices].count;
            else
                totalIndexCount += model.accessors[posIt->second].count;
        }
    }

    *vertices = new CommonVertexType[totalVertexCount];
    *indices = new unsigned long[totalIndexCount];

    size_t vertexOffset = 0;
    size_t indexOffset = 0;

    // ���� ������ ����
    for (auto& mesh : model.meshes)
    {
        for (auto& prim : mesh.primitives)
        {
            auto posIt = prim.attributes.find("POSITION");
            if (posIt == prim.attributes.end())
                continue;

            auto posAccessor = model.accessors[posIt->second];
            auto posBufferView = model.bufferViews[posAccessor.bufferView];
            const float* posBuffer = reinterpret_cast<const float*>(
                &model.buffers[posBufferView.buffer].data[posBufferView.byteOffset + posAccessor.byteOffset]);

            // Normals
            const float* normalBuffer = nullptr;
            auto normalIt = prim.attributes.find("NORMAL");
            if (normalIt != prim.attributes.end())
            {
                auto normAccessor = model.accessors[normalIt->second];
                auto normBufferView = model.bufferViews[normAccessor.bufferView];
                normalBuffer = reinterpret_cast<const float*>(
                    &model.buffers[normBufferView.buffer].data[normBufferView.byteOffset + normAccessor.byteOffset]);
            }

            // UV
            const float* uvBuffer = nullptr;
            auto uvIt = prim.attributes.find("TEXCOORD_0");
            if (uvIt != prim.attributes.end())
            {
                auto uvAccessor = model.accessors[uvIt->second];
                auto uvBufferView = model.bufferViews[uvAccessor.bufferView];
                uvBuffer = reinterpret_cast<const float*>(
                    &model.buffers[uvBufferView.buffer].data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
            }

            // Color
            const float* colorBuffer = nullptr;
            auto colorIt = prim.attributes.find("COLOR_0");
            if (colorIt != prim.attributes.end())
            {
                auto colorAccessor = model.accessors[colorIt->second];
                auto colorBufferView = model.bufferViews[colorAccessor.bufferView];
                colorBuffer = reinterpret_cast<const float*>(
                    &model.buffers[colorBufferView.buffer].data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
            }

            // Material -> Texture -> Image index
            int imgIndex = 0;
            if (prim.material >= 0)
            {
                const auto& mat = model.materials[prim.material];
                if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0)
                {
                    int texIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
                    imgIndex = model.textures[texIndex].source;
                }
            }

            size_t vertexNum = posAccessor.count;
            size_t indexNum = prim.indices >= 0 ? model.accessors[prim.indices].count : vertexNum;

            // Vertex ����
            for (size_t i = 0; i < vertexNum; i++)
            {
                (*vertices)[vertexOffset + i].imageIndex = imgIndex;

                (*vertices)[vertexOffset + i].position = XMFLOAT3(
                    posBuffer[i * 3 + 0], posBuffer[i * 3 + 1], posBuffer[i * 3 + 2]);

                if (normalBuffer)
                    (*vertices)[vertexOffset + i].normal = XMFLOAT3(
                        normalBuffer[i * 3 + 0], normalBuffer[i * 3 + 1], normalBuffer[i * 3 + 2]);
                else
                    (*vertices)[vertexOffset + i].normal = XMFLOAT3(0.f, 0.f, 0.f);

                if (uvBuffer)
                {
                    (*vertices)[vertexOffset + i].texture = XMFLOAT2(
                        uvBuffer[i * 2 + 0], uvBuffer[i * 2 + 1]);
                }
                else
                {
                    // (*vertices)[vertexOffset + i].imageIndex = 0;

                    (*vertices)[vertexOffset + i].texture = XMFLOAT2(0.f, 0.f);
                }

                if (colorBuffer)
                    (*vertices)[vertexOffset + i].color = XMFLOAT4(
                        colorBuffer[i * 4 + 0], colorBuffer[i * 4 + 1],
                        colorBuffer[i * 4 + 2], colorBuffer[i * 4 + 3]);
                else
                    (*vertices)[vertexOffset + i].color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
            }

            // Index ����
            if (prim.indices >= 0)
            {
                auto idxAccessor = model.accessors[prim.indices];
                auto idxBufferView = model.bufferViews[idxAccessor.bufferView];
                const unsigned char* rawIndices =
                    &model.buffers[idxBufferView.buffer].data[idxBufferView.byteOffset + idxAccessor.byteOffset];

                for (size_t i = 0; i < indexNum; i++)
                {
                    // ��� primitive �� ��ġ�鼭 vertex index offset �� ���������Ƿ�
                    // primitive �� rawIndices �� vertex index offset �� ���������
                    switch (idxAccessor.componentType)
                    {
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                        (*indices)[indexOffset + i] = ((unsigned char*)rawIndices)[i] + (unsigned long)vertexOffset;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                        (*indices)[indexOffset + i] = ((unsigned short*)rawIndices)[i] + (unsigned long)vertexOffset;
                        break;
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                        (*indices)[indexOffset + i] = ((unsigned int*)rawIndices)[i] + (unsigned long)vertexOffset;
                        break;
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < indexNum; i++)
                    (*indices)[indexOffset + i] = vertexOffset + i;
            }

            vertexOffset += vertexNum;
            indexOffset += indexNum;
        }
    }

    *vertexCount = (int)totalVertexCount;
    *indexCount = (int)totalIndexCount;

    return true;
}

bool ObjectParser::ExtractTexturesFromGLB(const char* glbFilePath, const char* outputFolder)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    string err;
    string warn;

    // GLB �ε�
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, glbFilePath);
    if (!ret)
    {
        return false;
    }

    // images ����
    for (size_t i = 0; i < model.images.size(); ++i)
    {
        const tinygltf::Image& image = model.images[i];
        string fileName;

        if (!image.image.empty())
        {
            // ���� �̸� ����
            if (!image.name.empty())
                fileName = string(outputFolder) + "/" + image.name;
            else
                fileName = string(outputFolder) + "/image_" + to_string(i);

            // mimeType�� ���� Ȯ���� ����
            if (image.mimeType == "image/png")
                fileName += ".png";
            else if (image.mimeType == "image/jpeg")
                fileName += ".jpg";
            else
                fileName += ".bin"; // �� �� ���� Ÿ��

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
