#pragma once

#include <stdexcept>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>


bool isPointInTriangleXZ(const glm::vec2& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);
float interpolateHeightInTriangle(const glm::vec2& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);



class Quad {
public:
    glm::vec3 v0, v1, v2, v3; // Vértices del quad

    Quad() = default;

    Quad(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
        std::vector<glm::vec3> vertices = { a, b, c, d };
        orderVerticesCCW(vertices);
        v0 = vertices[0];
        v1 = vertices[1];
        v2 = vertices[2];
        v3 = vertices[3];
    }

    // Calcular la altura en el punto (x, z) si el quad es un plano
    float getHeightAt(float x, float z) const {
        // Calcular el vector normal del plano usando v0, v1 y v2
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        // Ecuación del plano: Ax + By + Cz + D = 0
        // Desglosamos en A, B, C y calculamos D usando uno de los vértices (v0)
        float A = normal.x;
        float B = normal.y;
        float C = normal.z;
        float D = -(A * v0.x + B * v0.y + C * v0.z);

        // Usando la ecuación del plano para resolver y en función de x y z
        // y = -(Ax + Cz + D) / B
        if (B == 0) {
            throw std::runtime_error("Plano paralelo al eje Y; la altura no está definida.");
        }
        return -(A * x + C * z + D) / B;
    }

    glm::vec3 calculateQuadNormal() const {
        // Crear dos vectores a partir de los vértices del quad
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v3 - v0;

        // Calcular la normal usando el producto cruzado de los dos vectores
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        return normal;
    }

private:
    void orderVerticesCCW(std::vector<glm::vec3>& vertices) {
        // Calcula el centro del quad en el plano XZ
        glm::vec3 center(0.0f);
        for (const auto& v : vertices) {
            center += v;
        }
        center /= vertices.size();

        // Calcula los ángulos de cada vértice respecto al centro en el plano XZ
        std::sort(vertices.begin(), vertices.end(), [&center](const glm::vec3& a, const glm::vec3& b) {
            float angleA = atan2(a.z - center.z, a.x - center.x);
            float angleB = atan2(b.z - center.z, b.x - center.x);
            return angleA < angleB; // Orden CCW
            });
    }
};

float getHeightInQuad(const glm::vec2& positionXZ, Quad* quad);



class MeshNavigator {
    public:
    MeshNavigator(std::string filename, float scale);
    ~MeshNavigator() = default;

    std::string m_filename;

    std::vector<Quad*> quads;

    void loadMeshToMap(const std::string& filename);
    Quad* getQuadAtPosition(float x, float z);

    float m_scale;

};


