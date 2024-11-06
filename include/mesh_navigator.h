#pragma once

#include <stdexcept>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

// Estructura Triangle
struct Triangle {
    glm::vec3 v0 = glm::vec3(0.0f), v1 = glm::vec3(0.0f), v2 = glm::vec3(0.0f); // Vértices del triángulo
    Triangle* neighbor1;  // Triángulos vecinos
    Triangle* neighbor2;
    Triangle* neighbor3;

    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
        : neighbor1(nullptr), neighbor2(nullptr), neighbor3(nullptr) {
        v0 = a;
        v1 = b;
        v2 = c;
    }
};

float getInterpolatedHeight(const glm::vec2& pos, Triangle triangle);

// Hash para comparar aristas, sin importar el orden de los vértices
struct Edge {
    glm::vec3 v1, v2;
    Edge(const glm::vec3& a, const glm::vec3& b) : v1(a), v2(b) {
        if (!compareVertices(v1, v2)) std::swap(v1, v2);  // Ordenar para asegurar que (a,b) == (b,a)
    }

    // Función de comparación para asegurar el orden entre los vértices
    static bool compareVertices(const glm::vec3& a, const glm::vec3& b) {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }

    bool operator==(const Edge& other) const {
        return v1 == other.v1 && v2 == other.v2;
    }
};

// Hash function para Edge en unordered_map
namespace std {
    template <>
    struct hash<Edge> {
        size_t operator()(const Edge& edge) const {
            auto hash1 = hash<float>()(edge.v1.x) ^ hash<float>()(edge.v1.y) ^ hash<float>()(edge.v1.z);
            auto hash2 = hash<float>()(edge.v2.x) ^ hash<float>()(edge.v2.y) ^ hash<float>()(edge.v2.z);
            return hash1 ^ hash2;
        }
    };
}


class MeshNavigator {
    public:
    MeshNavigator(std::string filename, float scale);
    ~MeshNavigator() = default;

    std::string m_filename;
    std::vector<Triangle> triangles;
    std::vector<Triangle> loadMeshWithNeighbors(const std::string& filename, float scale);
    Triangle getTriangleFromPosition(glm::vec3 position);
    Triangle getTriangleFromPosition(const std::string& filename, glm::vec3 position);
    Triangle* getTriangleFromPosition(glm::vec3 position, Triangle* startingT);

    float m_scale;

};
