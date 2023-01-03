#ifndef _SOLVER_H_
#define _SOLVER_H_

#include "defines.h"
#include <common.h>
#include <opengl_object.h>
#include <random>
#include <geometry.h>

class Solver{
protected: 
    const float m_eps = 1e-8;

public:
    Solver(){};
    ~Solver(){};

    virtual std::vector<unsigned int> Solve(std::vector<Vertex>* vertices) = 0;
    std::vector<vec3> small_random_perturb(std::vector<Vertex>* vertices); 
};

class Incremental_solver : public Solver{
public:
    Incremental_solver():Solver() {};

    std::vector<unsigned int> Solve(std::vector<Vertex>* vertices);
};

class SAT_solver : public Solver{
public:
    SAT_solver():Solver() {};

    std::vector<unsigned int> Solve(std::vector<Vertex>* vertices){ UNIMPLEMENTED; return std::vector<unsigned int>(); };

    bool CheckSeparatingAxis(std::vector<vec3> &verticesA, std::vector<vec3> &verticesB, vec3 &axis);
    bool CheckCollision(Geometry &one, Geometry &other);
};
#endif