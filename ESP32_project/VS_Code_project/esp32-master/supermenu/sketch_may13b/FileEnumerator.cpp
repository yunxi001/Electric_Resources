#include "FileEnumerator.h"
using namespace std;

FileEnumerator::~FileEnumerator()
{
  if(file)
  {
    file.close();
  }
}

FileEnumerator::FileEnumerator()
{
}

bool FileEnumerator::Init()
{
  if(file.isOpen())
    file.close();
  file.remove("/index.idx");
  if(!file.open("/index.idx", O_RDWR|O_CREAT))
  {
    Serial.println("crate file error");
    return false;
  }
  return true;
}

void FileEnumerator::Add(const char*filename)
{
  String tm = filename;
  tm.toLowerCase();
  if(tm.endsWith(".mp3"))
  {
    memset(buffer, 0, 128);
    strcpy(buffer, filename);
    file.write(buffer, 128);
    count++;
  }
}
void FileEnumerator::Add2(const char*filename,char filekind[128])
{
  String tm = filename;
  tm.toLowerCase();
  if(tm.endsWith(filekind))
  {
    memset(buffer, 0, 128);
    strcpy(buffer, filename);
    file.write(buffer, 128);
    count++;
  }
}
void FileEnumerator::Add3(const char*filename)
{
  String tm = filename;
  tm.toLowerCase();
  if(tm.endsWith(".txt"))
  {
    memset(buffer, 0, 128);
    strcpy(buffer, filename);
    file.write(buffer, 128);
    count++;
  }
}

void FileEnumerator::Flush()
{
  file.sync();
  file.flush();
}

const char* FileEnumerator::Next()
{
  if(!file)
    return NULL;
  current++;
  if(current >= count)
    current = 0;
  int pos = current * 128;
  file.seek(pos);
  memset(buffer, 0, 128);
  file.read(buffer, 128);
  return buffer;
}

const char* FileEnumerator::Prev()
{
  if(!file)
    return NULL;
  current--;
  if(current < 0)
    current = count -1;
  int pos = current * 128;
  file.seek(pos);
  memset(buffer, 0, 128);
  file.read(buffer, 128);
  return buffer;
}
