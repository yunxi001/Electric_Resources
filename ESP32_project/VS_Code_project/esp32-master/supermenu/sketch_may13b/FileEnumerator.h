#include <utility>
#pragma once
#include "SdFat.h"
#include <string>
#include <tuple>

class FileEnumerator
{
private:
  FsFile file;
  int count = 0;     //有文件的数量
  int current = 0;
  char buffer[128];

public:
  bool Init();
  FileEnumerator();
  const char* Next();
  const char* Prev();
  void Add(const char*filename);
  void Add2(const char*filename,char filekind[128]);
  void Add3(const char*filename);
  void Flush();
  ~FileEnumerator();

  const char* CurrentFileName() {return buffer;}
};
