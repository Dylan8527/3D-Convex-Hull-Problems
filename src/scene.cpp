#include<scene.h>

Scene::Scene(
    int scene_id,
    EConvexhullAlgorithm convexhull_algorithm
)
{
    m_scene_id = scene_id;
    m_convexhull_algorithm = convexhull_algorithm;
    switch(scene_id){
        case 0: 
            // one bunny
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(0.5f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            break;
        case 1:
            // two bunny && no collision
            m_collision_detection = true;
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(5.f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            m_geometry.push_back(Geometry(
                vec3(0.5f, 0.4f, 0.3f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(0.25f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            break;
        case 2:
            // two bunny && collision obviously
            m_collision_detection = true;
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(0.25f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            m_geometry.push_back(Geometry(
                vec3(0.5f, 0.4f, 0.3f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(1.0f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            break;
        case 3:
            // two bunny && collision unobviously
            m_collision_detection = true;
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(0.25f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            m_geometry.push_back(Geometry(
                vec3(0.5f, 0.4f, 0.3f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(1.65f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            break;
        case 4:
            // two bunny && collision boundary
            m_collision_detection = true;
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(0.25f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            m_geometry.push_back(Geometry(
                vec3(0.5f, 0.4f, 0.3f), 
                "assets/stanford_bunny.obj",
                10.f, 
                vec3(1.7f, 0.f, 0.f), 
                EObjectType::StandfordBunny
            ));
            break;
        case 5: 
            // 3000 random samples within sphere
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f),
                2.f, 
                vec3(0.f, 0.f, 0.f),
                EObjectType::Sphere,
                3000
            ));
            break;
        case 6: 
            // 100 random samples within sphere
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f),
                2.f, 
                vec3(0.f, 0.f, 0.f),
                EObjectType::Sphere,
                100
            ));
            break;
        case 7: 
            // two spheres randomly
            m_collision_detection = true;
            m_geometry.push_back(Geometry(
                vec3(0.3f, 0.4f, 0.5f),
                2.f, 
                vec3(0.f, 0.f, 0.f),
                EObjectType::Sphere,
                25
            ));
            m_geometry.push_back(Geometry(
                vec3(0.5f, 0.4f, 0.3f),
                2.f, 
                vec3(1.2f, 1.2f, 1.2f),
                EObjectType::Sphere,
                25
            ));
            break;
        UNIMPLEMENTED;
    }
    
}

void Scene::Draw(Shader &shader, ShaderParam &shader_param)
{
    for(Geometry &geometry: m_geometry){
        geometry.SetUpdateIndices(m_update_indices);    
        geometry.Draw(shader, shader_param);
    }
    m_update_indices = false;
}

void Scene::Solve() 
{
    m_update_indices = true;
    std::shared_ptr<Solver> m_solver;
    switch(m_convexhull_algorithm){
        case EConvexhullAlgorithm::Incremental:
            m_solver = std::make_shared<Incremental_solver>();
            break;
        UNIMPLEMENTED;
    }

    for(Geometry &geometry: m_geometry) {
        geometry.RecursiveUpdateState();
        if(geometry.GetObjectState() == EObjectState::ConvexHull)
            geometry.UpdateConvexhullindices(m_solver->Solve(geometry.GetVertices()));
    }
}

bool Scene::CheckCollision()
{
    auto m_solver = std::make_shared<SAT_solver>();
    if(m_collision_detection){
        if(m_solver->CheckCollision(m_geometry[0], m_geometry[1])){
            std::cout << "Collision detected!" << std::endl;
            m_collision_detection = false;
        }
        else if(m_geometry[0].GetObjectState() == EObjectState::ConvexHull && m_geometry[1].GetObjectState() == EObjectState::ConvexHull){
            std::cout << "No collision detected!" << std::endl;
            m_collision_detection = false;
        }
        if(m_collision_detection == false)
            return true;
    }
    return false;
}