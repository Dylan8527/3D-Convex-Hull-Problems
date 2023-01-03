#ifndef _SCENE_
#define _SCENE_

#include <shader.h>
#include <common.h>
#include <geometry.h>
#include <thread>
#include <camera.h>
#include <utils.h>
#include <solver.h>

class Scene
{
protected:
    EConvexhullAlgorithm m_convexhull_algorithm = EConvexhullAlgorithm::Extreme_edges;
    int m_scene_id;
    std::vector<Geometry> m_geometry;
    std::shared_ptr<Solver> m_solver;
    bool m_update_indices = false;
    bool m_collision_detection = false;
public:
    Scene(
        int scene_id,
        EConvexhullAlgorithm convexhull_algorithm
    );
    // Draw the current scene.
    void Draw(Shader &shader, ShaderParam &shader_param);

    // Solve to calculate the convex hull 
    void Solve();

    // Check collision between two objects.
    bool CheckCollision();

    bool GetCollisionDetection() const { return m_collision_detection; }
};

#endif // _SCENE_
