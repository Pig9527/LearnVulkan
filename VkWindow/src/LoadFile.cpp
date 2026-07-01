#include "LoadFile.h"

LoadFile::LoadFile(const std::string &path)
:m_Path(path)
{

}

LoadFile::~LoadFile()
{
  m_file.close();
}

void LoadFile::Open()
{
  m_file = std::ifstream(m_Path,std::ios::ate|std::ios::binary);

  m_FileSize = m_file.tellg();
}

void LoadFile::ReadFromFile(std::vector<char> &buff)
{
  m_file.seekg(0);
  m_file.read(buff.data(),m_FileSize);
}
