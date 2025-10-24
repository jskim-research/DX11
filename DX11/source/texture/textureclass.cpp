#include "texture/textureclass.h"

TextureClass::TextureClass()
{
    m_targaData = 0;
    m_texture = 0;
    m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const vector<const char*>& filenames)
{
    bool result;
    int height, width;
    D3D11_TEXTURE2D_DESC textureDesc;
    HRESULT hResult;
    unsigned int rowPitch;
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

    vector<unsigned char*> images;

    for (UINT i = 0; i < filenames.size(); i++)
    {
        // Load the targa image data into memory.
        result = LoadTarga32Bit(filenames[i]);
        if (!result)
        {
            return false;
        }

        images.push_back(m_targaData);   
    }

    // Setup the description of the texture.
    textureDesc.Height = m_height;
    textureDesc.Width = m_width;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = images.size();
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    // Create the empty texture.
    hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
    if (FAILED(hResult))
    {
        return false;
    }

    // Set the row pitch of the targa image data.
    rowPitch = (m_width * 4) * sizeof(unsigned char);

    for (int i = 0; i < images.size(); i++)
    {
        // Copy the targa image data into the texture.
        deviceContext->UpdateSubresource(
            m_texture,
            D3D11CalcSubresource(0, i, textureDesc.MipLevels),
            NULL,
            images[i],
            rowPitch,
            0);
    }
    
    // Setup the shader resource view description.
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = -1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = images.size();

    // Create the shader resource view for the texture.
    hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
    if (FAILED(hResult))
    {
        return false;
    }

    // Generate mipmaps for this texture.
    deviceContext->GenerateMips(m_textureView);

    // Release the targa image data now that the image data has been loaded into the texture.
    for (unsigned char* image : images)
    {
        delete[] image;
    }
    m_targaData = 0;

    return true;
}

void TextureClass::Shutdown()
{
    // Release the texture view resource.
    if (m_textureView)
    {
        m_textureView->Release();
        m_textureView = 0;
    }

    // Release the texture.
    if (m_texture)
    {
        m_texture->Release();
        m_texture = 0;
    }

    // Release the targa data.
    if (m_targaData)
    {
        delete[] m_targaData;
        m_targaData = 0;
    }
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return m_textureView;
}

int TextureClass::GetWidth()
{
    return m_width;
}

int TextureClass::GetHeight()
{
    return m_height;
}

bool TextureClass::LoadTarga32Bit(const char* filename)
{
    int error, bpp, imageSize, index, i, j, k;
    FILE* filePtr;
    unsigned int count;
    TargaHeader targaFileHeader;
    unsigned char* targaImage;


    // Open the targa file for reading in binary.
    error = fopen_s(&filePtr, filename, "rb");
    if (error != 0)
    {
        return false;
    }

    // Read in the file header.
    count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
    if (count != 1)
    {
        return false;
    }

    // Get the important information from the header.
    m_height = (int)targaFileHeader.height;
    m_width = (int)targaFileHeader.width;
    bpp = (int)targaFileHeader.bpp;

    // Check that it is 32 bit and not 24 bit.
    if (bpp != 32)
    {
        return false;
    }

    // Calculate the size of the 32 bit image data.
    imageSize = m_width * m_height * 4;

    // Allocate memory for the targa image data.
    targaImage = new unsigned char[imageSize];

    // Read in the targa image data.
    count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
    if (count != imageSize)
    {
        return false;
    }

    // Close the file.
    error = fclose(filePtr);
    if (error != 0)
    {
        return false;
    }

    // Allocate memory for the targa destination data.
    m_targaData = new unsigned char[imageSize];

    // Initialize the index into the targa destination data array.
    index = 0;

    // Initialize the index into the targa image data.
    k = (m_width * m_height * 4) - (m_width * 4);

    // Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
    for (j = 0; j < m_height; j++)
    {
        for (i = 0; i < m_width; i++)
        {
            m_targaData[index + 0] = targaImage[k + 2];  // Red.
            m_targaData[index + 1] = targaImage[k + 1];  // Green.
            m_targaData[index + 2] = targaImage[k + 0];  // Blue
            m_targaData[index + 3] = targaImage[k + 3];  // Alpha

            // Increment the indexes into the targa data.
            k += 4;
            index += 4;
        }

        // Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
        k -= (m_width * 8);
    }

    // Release the targa image data now that it was copied into the destination array.
    delete[] targaImage;
    targaImage = 0;

    return true;
}
