#include <memory>

#ifdef _WIN32
#include <glad/glad.h>
#else
#include <OpenGL/gl3.h>
#endif

class Disk;
using DiskPtr = std::shared_ptr<Disk>; 

#ifndef DISK_H
#define DISK_H

#include "shape.h"

class Disk : public Shape {
  unsigned int m_vao;
  int m_nslice;
protected:
  Disk (int nslice);
public:
  static DiskPtr Make (int nslice=64);
  virtual ~Disk ();
  virtual void Draw (StatePtr st);
};
#endif