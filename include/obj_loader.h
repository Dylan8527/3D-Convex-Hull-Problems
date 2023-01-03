#ifndef CS271_INCLUDE_OBJ_LOADER_H_
#define CS271_INCLUDE_OBJ_LOADER_H_
#include <geometry.h>
#include <iostream>
#include <memory>

bool loadObj(const std::string &path, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, float scale, vec3 translation);

#endif  // CS271_INCLUDE_OBJ_LOADER_H_
