#include "mesh_navigator.h"
#include <iostream>

bool isPointInTriangleXZ(const float x, const float z, Triangle& tri) {
    // Convertir el triángulo y el punto a 2D (solo coordenadas x y z)
    glm::vec2 p(x, z);
    glm::vec2 a(tri.v0.x, tri.v0.z);
    glm::vec2 b(tri.v1.x, tri.v1.z);
    glm::vec2 c(tri.v2.x, tri.v2.z);

    // Vectores del triángulo en 2D
    glm::vec2 v0v1 = b - a;
    glm::vec2 v0v2 = c - a;
    glm::vec2 v0p = p - a;

    // Calcular productos cruzados en 2D
    float cross1 = v0v1.x * v0v2.y - v0v1.y * v0v2.x; // Determinante para el área del triángulo
    float cross2 = v0v1.x * v0p.y - v0v1.y * v0p.x;   // Área entre v0, v1 y p
    float cross3 = v0v2.x * v0p.y - v0v2.y * v0p.x;   // Área entre v0, v2 y p

    // Usar coordenadas baricéntricas en 2D
    float u = cross3 / cross1;
    float v = cross2 / cross1;
    float w = 1.0f - u - v;

    // El punto está dentro del triángulo si u, v y w están en el rango [0, 1]
    return (u >= 0.0f && u <= 1.0f) && (v >= 0.0f && v <= 1.0f) && (w >= 0.0f && w <= 1.0f);
}

MeshNavigator::MeshNavigator(const std::string& filename) {
    triangles = loadMeshWithNeighbors(filename);
}


// Función para cargar el archivo .obj y establecer los vecinos
std::vector<Triangle> MeshNavigator::loadMeshWithNeighbors(const std::string& filename) {
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

        glm::vec3 v0 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[0]].x,
                                 scene->mMeshes[0]->mVertices[face.mIndices[0]].y,
                                 scene->mMeshes[0]->mVertices[face.mIndices[0]].z);
        glm::vec3 v1 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[1]].x,
                                 scene->mMeshes[0]->mVertices[face.mIndices[1]].y,
                                 scene->mMeshes[0]->mVertices[face.mIndices[1]].z);
        glm::vec3 v2 = glm::vec3(scene->mMeshes[0]->mVertices[face.mIndices[2]].x,
                                 scene->mMeshes[0]->mVertices[face.mIndices[2]].y,
                                 scene->mMeshes[0]->mVertices[face.mIndices[2]].z);

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

    return triangles;
}


Triangle* MeshNavigator::getTriangleFromPosition(glm::vec3 position) {
    for (Triangle& tri : triangles) { // allTriangles representa todos los triángulos de la malla
        if (isPointInTriangleXZ(position.x, position.z, tri)) {
            return &tri;
        }
    }
}

Triangle* MeshNavigator::getTriangleFromPosition(glm::vec3 position, Triangle* tri) {
    Triangle* current = tri;

    // Bucle de búsqueda en espiral a través de vecinos
    while (current) {
        if (isPointInTriangleXZ(position.x, position.y, *current)) {
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