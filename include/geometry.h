#ifndef _geometry_
#define _geometry_

#include <shader.h>
#include <opengl_object.h>
#include <common.h>
#include <algorithm>
#include <unordered_set>
#include <random>

namespace std {
    template <typename T>
    struct hash;

    template <>
    struct hash<vec3> {
        std::size_t operator()(vec3 const& v) const {
            return std::hash<float>{}(v.x) + std::hash<float>{}(v.y) + std::hash<float>{}(v.z);
        }
    };

    template <typename T>
    struct equal_to;

    template <>
    struct equal_to<vec3> {
        std::size_t operator()(vec3 const& lhs, vec3 const& rhs) const {
            return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
        }
    };
}

class Geometry
{
protected:
    EObjectState m_object_state = EObjectState::PointsClouds;
    EObjectType  m_object_type = EObjectType::StandfordBunny;
    OpenGLObject m_opengl_object; // OpenGL object for rendering.
    std::vector<unsigned int> m_mesh_indices;
    std::vector<unsigned int> m_convexhull_indices;
    bool m_update_indices = false;

public:
    Geometry(
        glm::vec3 color, 
        const std::string &path,
        float scale,
        vec3 translation,
        EObjectType object_type
    ); // read from obj file

    Geometry(
        glm::vec3 color, 
        float radius,
        glm::vec3 offset,
        EObjectType object_type,
        unsigned int num_vertices
    ); // random samples within sphere

    ~Geometry() = default;

    // Draw the geometry based on the OpenGLObject.
    void Draw(Shader &shader, ShaderParam &shader_param);
    void UpdateOpenGLObjectState();
    void UpdateConvexhullindices(std::vector<unsigned int> convexhull_indices) {m_convexhull_indices = convexhull_indices; m_object_state = EObjectState::ConvexHull;};
    std::vector<Vertex>* GetVertices() {return &m_opengl_object.vertices;};
    unsigned int GetConvexHullSize() {return m_convexhull_indices.size();};
    EObjectState GetObjectState() const {return m_object_state;};
    void RecursiveUpdateState() {int num=3; if (m_object_type==EObjectType::Sphere) num--; m_object_state = EObjectState((int(m_object_state) + 1)%num);}
    void SetUpdateIndices(bool value) {m_update_indices = value;};
    std::vector<vec3> GetConvexHullVertices();
    std::vector<vec3> GetConvexHullEdges();
    std::vector<vec3> GetConvexHullFaceNormals();
    // Calculate convex hull w.r.t algorithm ''
    // void calculate_convex_hull(EConvexhullAlgorithm algorithm);
};

#endif 