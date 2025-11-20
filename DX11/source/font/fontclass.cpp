#include "font/fontclass.h"
#include <fstream>
#include <vector>

using namespace std;

FontClass::FontClass():
	m_Chars(nullptr),
	m_fontTexture(nullptr)
{
}

FontClass::~FontClass()
{
}

void FontClass::ReleaseData()
{
	if (m_Chars)
	{
		delete[] m_Chars;
		m_Chars = nullptr;
	}

	if (m_fontTexture)
	{
		m_fontTexture->Shutdown();
		delete m_fontTexture;
		m_fontTexture = nullptr;
	}
}

bool FontClass::ImportFromFontFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* fileName, const char* textureFileName)
{
	ifstream fin;
	char temp;
	bool result;

	m_Chars = new CharType[95];

	fin.open(fileName);
	if (fin.fail()) return false;

	for (int i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ') fin.get(temp);

		fin.get(temp);
		while (temp != ' ') fin.get(temp);

		fin >> m_Chars[i].u1;  // left u
		fin >> m_Chars[i].u2;  // right u
		fin >> m_Chars[i].size;
	}

	fin.close();

	if (m_fontTexture == nullptr) m_fontTexture = new TextureClass;
	vector<const char*> fileNames;
	fileNames.push_back(textureFileName);

	// texture size is like 1024 x 32
	result = m_fontTexture->Initialize(device, deviceContext, fileNames);
	if (!result) return false;

	return true;
}

bool FontClass::GetCharData(char ch, CharType* charInfo) const
{
	if (m_Chars == nullptr) return false;
	// 32 ~ 126, 95 개의 ascii 만 현재 고려중
	if (ch < 32) return false;

	*charInfo = m_Chars[ch - 32];
	return true;
}

ID3D11ShaderResourceView* FontClass::GetTexture() const
{
	if (m_fontTexture) return m_fontTexture->GetTexture();
	return nullptr;
}
