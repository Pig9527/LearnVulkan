#pragma once
#include <fstream>
#include <string>
#include <vector>
class LoadFile
{
public:
  LoadFile(const std::string &path);
  ~LoadFile();

  void Open();
  void ReadFromFile(std::vector<char> &buff);
  int GetFileSize() { return m_FileSize; }

private:
  std::string m_Path;
  int m_FileSize;
  std::ifstream m_file;
};
