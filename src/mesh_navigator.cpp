#include "mesh_navigator.h"
#include <iostream>

bool isPointInTriangleXZ(const float x, const float z, const Triangle& tri) {
    glm::vec2 p(x, z);
    glm::vec2 a(tri.v0.x, tri.v0.z);
    glm::vec2 b(tri.v1.x, tri.v1.z);
    glm::vec2 c(tri.v2.x, tri.v2.z);

    // Calculamos los productos cruzados para ver si el punto está consistentemente a la izquierda de cada lado.
    auto cross = [](const glm::vec2& v1, const glm::vec2& v2) {
        return v1.x * v2.y - v1.y * v2.x;
        };

    // Verificar si el punto está a la izquierda de cada lado
    bool left1 = cross(b - a, p - a) >= 0.0f;
    bool left2 = cross(c - b, p - b) >= 0.0f;
    bool left3 = cross(a - c, p - c) >= 0.0f;

    // Si el punto está a la izquierda de los tres lados, está dentro del triángulo
    return (left1 && left2 && left3) || (!left1 && !left2 && !left3);
}

float getInterpolatedHeight(const glm::vec2& positionXZ, const Triangle tri) {
    const glm::vec3& v0 = tri.v0;
    const glm::vec3& v1 = tri.v1;
    const glm::vec3& v2 = tri.v2;

    // Convertir los puntos a 2D (usando x y z).
    glm::vec2 p0(v0.x, v0.z);
    glm::vec2 p1(v1.x, v1.z);
    glm::vec2 p2(v2.x, v2.z);

    // Calcular los vectores de los bordes
    glm::vec2 v0p = positionXZ - p0;
    glm::vec2 v0v1 = p1 - p0;
    glm::vec2 v0v2 = p2 - p0;

    // Calcular las coordenadas baricéntricas (usando la fórmula con determinantes)
    float d00 = glm::dot(v0v1, v0v1);
    float d01 = glm::dot(v0v1, v0v2);
    float d11 = glm::dot(v0v2, v0v2);
    float d20 = glm::dot(v0p, v0v1);
    float d21 = glm::dot(v0p, v0v2);

    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    // Interpolar la altura usando las coordenadas baricéntricas
    float interpolatedHeight = u * v0.y + v * v1.y + w * v2.y;

    return interpolatedHeight;
}



MeshNavigator::MeshNavigator(std::string filename, float scale) : m_filename(filename), m_scale(scale) {}


// Función para cargar el archivo .obj y establecer los vecinos
std::vector<Triangle> MeshNavigator::loadMeshWithNeighbors(const std::string& filename, float scale) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load mesh");
    }

    std::cout << "Mesh se ha cargado" << std::endl;

    std::vector<Triangle> triangles;
    std::unordered_map<Edge, Triangle*> edgeMap;
    size_t numFaces = 0;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        numFaces += scene->mMeshes[i]->mNumFaces;
    }
    triangles.reserve(numFaces);
    std::cout << numFaces << std::endl;

    for (unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
        auto face = scene->mMeshes[0]->mFaces[i];
        //std::cout << "Se lee cara" << std::endl;
        if (face.mNumIndices != 3) continue;  // Asegurarse de que es un triángulo

        glm::vec3 v0 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[0]].x * scale,
                                 scene->mMeshes[0]->mVertices[face.mIndices[0]].y * scale,
                                 scene->mMeshes[0]->mVertices[face.mIndices[0]].z * scale);
        glm::vec3 v1 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[1]].x * scale,
                                 scene->mMeshes[0]->mVertices[face.mIndices[1]].y * scale,
                                 scene->mMeshes[0]->mVertices[face.mIndices[1]].z * scale);
        glm::vec3 v2 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[2]].x * scale,
                                 scene->mMeshes[0]->mVertices[face.mIndices[2]].y * scale,
                                 scene->mMeshes[0]->mVertices[face.mIndices[2]].z * scale);

        triangles.emplace_back(v0, v1, v2);
        Triangle* currentTriangle = &triangles.back();

        // Crear las aristas del triángulo
        std::vector<Edge> edges = { Edge(v0, v1), Edge(v1, v2), Edge(v2, v0) };
        
        for (int j = 0; j < 3; j++) {
            Edge edge = edges[j];

            // Buscar si ya existe un triángulo que comparte la arista
            if (edgeMap.find(edge) != edgeMap.end()) {
                Triangle* neighbor = edgeMap[edge];
                // Asociar vecinos mutuos
                if (currentTriangle->neighbor1 == nullptr) {
                    currentTriangle->neighbor1 = neighbor;
                } else if (currentTriangle->neighbor2 == nullptr) {
                    currentTriangle->neighbor2 = neighbor;
                } else {
                    currentTriangle->neighbor3 = neighbor;
                }

                if (neighbor->neighbor1 == nullptr) {
                    neighbor->neighbor1 = currentTriangle;
                } else if (neighbor->neighbor2 == nullptr) {
                    neighbor->neighbor2 = currentTriangle;
                } else {
                    neighbor->neighbor3 = currentTriangle;
                }
            } else {
                // Si no existe, agregar el triángulo actual como dueño de la arista
                edgeMap[edge] = currentTriangle;
            }
        }
    }
    for (Triangle& tri : triangles) {
        if (tri.v0 == glm::vec3(0.0f) || tri.v1 == glm::vec3(0.0f) || tri.v2 == glm::vec3(0.0f))
            std::cout << "faltan vértices" << std::endl;
    }

    return triangles;
}


//Triangle* MeshNavigator::getTriangleFromPosition(glm::vec3 position) {
//    for (Triangle& tri : triangles) { // allTriangles representa todos los triángulos de la malla
//        //std::cout <<  tri.v0.x << ", " << tri.v0.y << ", " << tri.v0.z << std::endl;
//        if (isPointInTriangleXZ(position.x, position.z, tri)) {
//            return &tri;
//        }
//    }
//}


Triangle MeshNavigator::getTriangleFromPosition(glm::vec3 position) {
    std::cout << "Carga de archivo" << std::endl;
    std::cout << m_filename << std::endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(m_filename, aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load mesh");
    }

    std::cout << "Mesh se ha cargado" << std::endl;
    size_t numFaces = 0;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        numFaces += scene->mMeshes[i]->mNumFaces;
    }
    std::cout << "Num faces " << numFaces << std::endl;

    for (unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
        auto face = scene->mMeshes[0]->mFaces[i];
        //std::cout << "Se lee cara" << std::endl;
        if (face.mNumIndices != 3) continue;  // Asegurarse de que es un triángulo
        //std::cout << "i: " << i << std::endl;

        glm::vec3 v0 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[0]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[0]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[0]].z * m_scale);
        glm::vec3 v1 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[1]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[1]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[1]].z * m_scale);
        glm::vec3 v2 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[2]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[2]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[2]].z * m_scale);

        /*std::cout << v0.x << ", " << v0.y << ", " << v0.z << std::endl;
        std::cout << v1.x << ", " << v1.y << ", " << v1.z << std::endl;
        std::cout << v2.x << ", " << v2.y << ", " << v2.z << std::endl;*/

        Triangle T(v0, v1, v2);

        if (isPointInTriangleXZ(position.x, position.z, T)) {
            return T;
        }
        
    }
    Triangle T(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    return T;
}

Triangle MeshNavigator::getTriangleFromPosition(const std::string& filename, glm::vec3 position) {
    std::cout << "Carga de archivo" << std::endl;
    std::cout << filename << std::endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load mesh");
    }

    std::cout << "Mesh se ha cargado" << std::endl;
    size_t numFaces = 0;

    for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
        numFaces += scene->mMeshes[i]->mNumFaces;
    }
    std::cout << "Num faces " << numFaces << std::endl;

    for (unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
        auto face = scene->mMeshes[0]->mFaces[i];
        //std::cout << "Se lee cara" << std::endl;
        if (face.mNumIndices != 3) continue;  // Asegurarse de que es un triángulo
        //std::cout << "i: " << i << std::endl;

        glm::vec3 v0 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[0]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[0]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[0]].z * m_scale);
        glm::vec3 v1 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[1]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[1]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[1]].z * m_scale);
        glm::vec3 v2 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[2]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[2]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[2]].z * m_scale);

        std::cout << v0.x << ", " << v0.y << ", " << v0.z << std::endl;
        std::cout << v1.x << ", " << v1.y << ", " << v1.z << std::endl;
        std::cout << v2.x << ", " << v2.y << ", " << v2.z << std::endl;

        Triangle T(v0, v1, v2);

        if (isPointInTriangleXZ(position.x, position.z, T)) {
            return T;
        }

    }
    std::cout << "Sin triangulo" << std::endl;
    Triangle T(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    return T;
}


Triangle* MeshNavigator::getTriangleFromPosition(glm::vec3 position, Triangle* tri) {
    Triangle* current = tri;

    // Bucle de búsqueda en espiral a través de vecinos
    while (current) {
        if (isPointInTriangleXZ(position.x, position.z, *current)) {
            return current; // Punto encontrado dentro del triángulo actual
        }

        Triangle* bestNeighbor = nullptr;
        float minDist = std::numeric_limits<float>::max();

        // Explorar vecinos para encontrar el que esté más cerca del punto
        std::vector<Triangle*> neighbors = {current->neighbor1, current->neighbor2, current->neighbor3};
        for (Triangle* neighbor : neighbors) {
            if (neighbor) {
                // Calcular el centroide del vecino en el plano XZ
                glm::vec3 neighborCenter = glm::vec3(
                    (neighbor->v0.x + neighbor->v1.x + neighbor->v2.x) / 3.0f,
                    (neighbor->v0.y + neighbor->v1.y + neighbor->v2.y) / 3.0f,
                    (neighbor->v0.z + neighbor->v1.z + neighbor->v2.z) / 3.0f
                );
                std::cout << "Pos prev: " << neighborCenter.x << "," << neighborCenter.y << ", " << neighborCenter.z << std::endl;

                // Calcular la distancia entre el punto y el centro del vecino
                float dist = glm::distance(position, neighborCenter);

                // Si es el vecino más cercano, actualizar el mejor vecino
                if (dist < minDist) {
                    minDist = dist;
                    bestNeighbor = neighbor;
                }
            }
        }

        // Si no hay vecino más cercano, salir del bucle
        if (!bestNeighbor) {
            return nullptr; // No se encontró un triángulo que contenga el punto
        }

        current = bestNeighbor;
    }

    return nullptr; // No se encontró un triángulo que contenga el punto
}