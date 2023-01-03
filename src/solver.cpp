#include <solver.h>

std::vector<vec3> Solver::small_random_perturb(std::vector<Vertex>* vertices)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::vector<vec3> new_vertices;
    auto points = *vertices;
    new_vertices.resize(vertices->size()); 
    // small random perturb to avoid the algorithm can't identify the situation that four points on the same plane.
    for(unsigned int i = 0; i < vertices->size(); ++i) {
        new_vertices[i] = vec3(points[i].position.x+dis(gen)*m_eps,points[i].position.y+dis(gen)*m_eps,points[i].position.z+dis(gen)*m_eps);
    }
    return new_vertices;
}


std::vector<unsigned int> Incremental_solver::Solve(std::vector<Vertex>* vertices)
{
    unsigned int num_vertices = vertices->size();
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Using Incremental Algorithm solver, and # of vertices : " << num_vertices << std::endl;
    std::chrono::duration<double> elapsed_seconds;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::vector<glm::uvec3> current_extreme_indices, next_extreme_indices;
    std::vector<std::vector<bool>> vis(num_vertices, std::vector<bool>(num_vertices, false));
    auto points = small_random_perturb(vertices);
    current_extreme_indices.push_back(glm::uvec3(0,1,2));
    current_extreme_indices.push_back(glm::uvec3(2,1,0));
    for(unsigned int i=3; i< num_vertices; ++i){
        auto& p = points[i];
        for(auto &extre_point : current_extreme_indices){
            auto p0 = points[extre_point.x], p1 = points[extre_point.y], p2=points[extre_point.z];
            auto a = p1 - p0, b = p2 - p0;
            auto normal = vec3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);
            bool sgn = glm::dot(p-p0, normal) > 0;
            if(!sgn) next_extreme_indices.push_back(extre_point);
            vis[extre_point.x][extre_point.y]=sgn;
            vis[extre_point.y][extre_point.z]=sgn;
            vis[extre_point.z][extre_point.x]=sgn;
        }
        for(auto &extre_point : current_extreme_indices){
            if(vis[extre_point.x][extre_point.y] && !vis[extre_point.y][extre_point.x]) next_extreme_indices.push_back(glm::uvec3(extre_point.x, extre_point.y, i));
            if(vis[extre_point.y][extre_point.z] && !vis[extre_point.z][extre_point.y]) next_extreme_indices.push_back(glm::uvec3(extre_point.y, extre_point.z, i));
            if(vis[extre_point.z][extre_point.x] && !vis[extre_point.x][extre_point.z]) next_extreme_indices.push_back(glm::uvec3(extre_point.z, extre_point.x, i));
        }
        current_extreme_indices = next_extreme_indices;
        next_extreme_indices.clear();
    }
    std::vector<unsigned int> convexhull_indices(current_extreme_indices.size()*3);
    for(unsigned int i = 0; i < current_extreme_indices.size(); ++i){
        auto& p = current_extreme_indices[i];
        convexhull_indices[i*3]    = p.x;
        convexhull_indices[i*3 + 1]= p.y;
        convexhull_indices[i*3 + 2]= p.z;
    }
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "Incremental Algorithm solver finished, and time cost : " << elapsed_seconds.count() << "s";
    std::cout << ", # of convex hull faces: " << convexhull_indices.size()/3 << std::endl;
    return convexhull_indices;
}

bool SAT_solver::CheckSeparatingAxis(std::vector<vec3> &verticesA, std::vector<vec3> &verticesB, vec3 &axis)
{
    float minA = FLT_MAX;
    float maxA = -FLT_MAX;
    float minB = FLT_MAX;
    float maxB = -FLT_MAX;
    for(auto& v : verticesA){
        float proj = glm::dot(v, axis);
        if(proj < minA || minA == FLT_MAX)
            minA = proj;
        if(proj > maxA || maxA == -FLT_MAX)
            maxA = proj;
    }
    for(auto& v : verticesB){
        float proj = glm::dot(v, axis);
        if(proj < minB || minB == FLT_MAX)
            minB = proj;
        if(proj > maxB || maxB == -FLT_MAX)
            maxB = proj;
    }
    if(minA > maxB|| minB > maxA){
        return false; // no collision
    }
    return true; // collision
}

bool SAT_solver::CheckCollision(Geometry &one, Geometry &other)
{
    if(one.GetObjectState() != EObjectState::ConvexHull && other.GetObjectState() != EObjectState::ConvexHull)
        return false;
    std::vector<vec3> verticesA, verticesB;
    std::vector<vec3> edgesA, edgesB;
    std::vector<vec3> normalsA, normalsB;
    std::chrono::high_resolution_clock::time_point t1, t2;
    std::chrono::duration<double> time_span;
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Using SAT solver, and # of facets : " << one.GetConvexHullSize()/3 << " and " << other.GetConvexHullSize()/3 << std::endl;
    t1 = std::chrono::high_resolution_clock::now();
    verticesA = one.GetConvexHullVertices();
    verticesB = other.GetConvexHullVertices();
    edgesA = one.GetConvexHullEdges();
    edgesB = other.GetConvexHullEdges();
    normalsA = one.GetConvexHullFaceNormals();
    normalsB = other.GetConvexHullFaceNormals();
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Time to get vertices, edges and normals: " << time_span.count() << " seconds." << std::endl;
    std::cout << "# of Vertices:" << verticesA.size() << ", " << verticesB.size() << std::endl;
    std::cout << "# of Edges:" << edgesA.size() << ", " << edgesB.size() << std::endl;
    std::cout << "# of Normals:" << normalsA.size() << ", " << normalsB.size() << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    for(auto& n : normalsA){
        if(!CheckSeparatingAxis(verticesA, verticesB, n)){
            t2 = std::chrono::high_resolution_clock::now();
            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            std::cout << "Non-collision detected by NormalsA, Time to check separating axis: " << time_span.count() << " seconds." << std::endl;
            return false;
        }
    }
    printf("NormalsA passed \n");
    for(auto& n : normalsB){
        if(!CheckSeparatingAxis(verticesA, verticesB, n)){
            t2 = std::chrono::high_resolution_clock::now();
            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
            std::cout << "Non-collision detected by NormalsB, Time to check separating axis: " << time_span.count() << " seconds." << std::endl;
            return false;
        }
    }
    printf("NormalsB passed \n");
    bool find = false;
    #pragma omp parallel for schedule(dynamic) shared(find, t2, time_span) // OpenMP
    for(int i = 0; i < edgesA.size(); i++){
        auto& e1 = edgesA[i];
        if(find)
            break;
        for(auto& e2: edgesB){
            if(find)
                break;
            vec3 axis = glm::cross(e1, e2);
            if(axis.length() < 1e-8)
                continue;
            axis = glm::normalize(axis);
            if(!CheckSeparatingAxis(verticesA, verticesB, axis)){
                t2 = std::chrono::high_resolution_clock::now();
                time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                find = true;
            }
        }
    }

    if(find){
        std::cout << "Non-collision detected by EdgesAB, Time to check separating axis: " << time_span.count() << " seconds." << std::endl;
        return false;
    }
    printf("EdgesA and EdgesB passed \n");
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Time to check separating axis: " << time_span.count() << " seconds." << std::endl;
    return true;
}
