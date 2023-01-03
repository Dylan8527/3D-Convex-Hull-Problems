#include<obj_loader.h>
#include<geometry.h>


Geometry::Geometry(
    glm::vec3 color, 
    const std::string &path,
    float scale,
    vec3 translation,
    EObjectType object_type
) {
    std::cout << "Generating Geometry from obj file " << std::endl;
    m_object_type = object_type;
    m_opengl_object.color = color;
    auto& vertices = m_opengl_object.vertices;
    auto& indices = m_opengl_object.indices;
    loadObj(path, vertices, indices, scale, translation);
    m_mesh_indices = indices;
    
    m_opengl_object.M = glm::mat4(1.0f);
    m_opengl_object.InitData();
}

Geometry::Geometry(
    glm::vec3 color, 
    float radius,
    glm::vec3 offset,
    EObjectType object_type,
    unsigned int num_vertices
) {
    std::cout << "Generating Geometry from random samples within sphere " << std::endl;
    m_object_type = object_type;
    m_opengl_object.color = color;
    // random samples within sphere
    auto& vertices = m_opengl_object.vertices;
    auto& indices = m_opengl_object.indices;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> theta(0., 2*PI);
    std::uniform_real_distribution<float> phi(0, PI);
    std::uniform_real_distribution<float> ra(0, radius);
    vertices.resize(num_vertices);
    for(unsigned int i = 0; i < num_vertices; ++i) {
        float t = theta(gen), p = phi(gen), r= ra(gen);
        float x = glm::sin(p)*glm::cos(t), y = glm::sin(p) * glm::sin(t), z = glm::cos(p);
        vertices[i].position = vec3(x, y, z) * r + offset;
        vertices[i].normal = vec3(x,y,z);
    }
    indices = std::vector<unsigned int>();
    m_mesh_indices = indices;
    m_opengl_object.M = glm::mat4(1.0f);
    m_opengl_object.InitData();
}

void Geometry::Draw(Shader &shader, ShaderParam &shader_param) 
{
    UpdateOpenGLObjectState();
    m_opengl_object.Draw(shader, shader_param);
}
void Geometry::UpdateOpenGLObjectState()
{
    if(m_object_state == EObjectState::PointsClouds){
        m_opengl_object.draw_option.draw_element = false;
    }
    else{
        m_opengl_object.draw_option.draw_element = true;
        if(m_object_state == EObjectState::MeshClouds)
            m_opengl_object.indices = m_mesh_indices;
        else
            m_opengl_object.indices = m_convexhull_indices;   
            
        if(m_update_indices){
            m_opengl_object.UpdateIndices();
        }
    }
};

std::vector<vec3> Geometry::GetConvexHullVertices()
{
    std::vector<vec3> vertices;
    std::unordered_set<vec3, std::hash<vec3>, std::equal_to<vec3>> vertex_set;
    for(auto index : m_convexhull_indices){
        vertex_set.emplace(m_opengl_object.vertices[index].position);
    }
    for(auto && v: vertex_set){
        vertices.emplace_back(v);
    }
    return vertices;
}

std::vector<vec3> Geometry::GetConvexHullEdges()
{
    std::vector<vec3> edges;
    std::unordered_set<vec3, std::hash<vec3>, std::equal_to<vec3>> edge_set;
    for(int i = 0; i < m_convexhull_indices.size(); i+=3)
    {
        vec3 e1 = glm::normalize(m_opengl_object.vertices[m_convexhull_indices[i+1]].position - m_opengl_object.vertices[m_convexhull_indices[i]].position);
        edge_set.emplace(e1);
        vec3 e2 = glm::normalize(m_opengl_object.vertices[m_convexhull_indices[i+2]].position - m_opengl_object.vertices[m_convexhull_indices[i+1]].position);
        edge_set.emplace(e2);
        vec3 e3 = glm::normalize(m_opengl_object.vertices[m_convexhull_indices[i]].position - m_opengl_object.vertices[m_convexhull_indices[i+2]].position);
        edge_set.emplace(e3);
    }
    for(auto && e: edge_set){
        edges.emplace_back(e);
    }
    return edges;
}

std::vector<vec3> Geometry::GetConvexHullFaceNormals()
{
    std::vector<vec3> normals;
    for(int i = 0; i < m_convexhull_indices.size(); i+=3){
        vec3 v1 = m_opengl_object.vertices[m_convexhull_indices[i]].position;
        vec3 v2 = m_opengl_object.vertices[m_convexhull_indices[i+1]].position;
        vec3 v3 = m_opengl_object.vertices[m_convexhull_indices[i+2]].position;
        vec3 n = glm::normalize(glm::cross(v2-v1, v3-v1));
        auto it = std::find(normals.begin(), normals.end(), n);
        if(it == normals.end())
            normals.push_back(n);
    }
    return normals;
}
