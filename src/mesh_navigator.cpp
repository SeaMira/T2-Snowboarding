#include "mesh_navigator.h"
#include <iostream>

// Función auxiliar para verificar si un punto está dentro de un triángulo en el plano XZ
bool isPointInTriangleXZ(const glm::vec2& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec2 a(v0.x, v0.z);
    glm::vec2 b(v1.x, v1.z);
    glm::vec2 c(v2.x, v2.z);

    auto cross = [](const glm::vec2& v1, const glm::vec2& v2) {
        return v1.x * v2.y - v1.y * v2.x;
        };

    bool left1 = cross(b - a, p - a) >= 0.0f;
    bool left2 = cross(c - b, p - b) >= 0.0f;
    bool left3 = cross(a - c, p - c) >= 0.0f;

    return (left1 && left2 && left3) || (!left1 && !left2 && !left3);
}

// Función para interpolar la altura dentro de un triángulo
float interpolateHeightInTriangle(const glm::vec2& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec2 a(v0.x, v0.z);
    glm::vec2 b(v1.x, v1.z);
    glm::vec2 c(v2.x, v2.z);

    glm::vec2 v0v1 = b - a;
    glm::vec2 v0v2 = c - a;
    glm::vec2 v0p = p - a;

    float d00 = glm::dot(v0v1, v0v1);
    float d01 = glm::dot(v0v1, v0v2);
    float d11 = glm::dot(v0v2, v0v2);
    float d20 = glm::dot(v0p, v0v1);
    float d21 = glm::dot(v0p, v0v2);

    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return u * v0.y + v * v1.y + w * v2.y;
}



MeshNavigator::MeshNavigator(std::string filename, float scale) : m_filename(filename), m_scale(scale) {}





void MeshNavigator::loadMeshToMap(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_JoinIdenticalVertices);
    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error("Failed to load mesh");
    }

    for (unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
        auto face = scene->mMeshes[0]->mFaces[i];
        if (face.mNumIndices != 4) continue; // Asegurar que es un quad

        glm::vec3 v0(scene->mMeshes[0]->mVertices[face.mIndices[0]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[0]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[0]].z * m_scale);
        glm::vec3 v1(scene->mMeshes[0]->mVertices[face.mIndices[1]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[1]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[1]].z * m_scale);
        glm::vec3 v2(scene->mMeshes[0]->mVertices[face.mIndices[2]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[2]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[2]].z * m_scale);
        glm::vec3 v3(scene->mMeshes[0]->mVertices[face.mIndices[3]].x * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[3]].y * m_scale,
            scene->mMeshes[0]->mVertices[face.mIndices[3]].z * m_scale);

        // Verificar si algún valor es NaN
        if (glm::isnan(v0.x) || glm::isnan(v0.y) || glm::isnan(v0.z) ||
            glm::isnan(v1.x) || glm::isnan(v1.y) || glm::isnan(v1.z) ||
            glm::isnan(v2.x) || glm::isnan(v2.y) || glm::isnan(v2.z) ||
            glm::isnan(v3.x) || glm::isnan(v3.y) || glm::isnan(v3.z)) {

            std::cout << "NaN detected in face " << i << " vertices:" << std::endl;
            std::cout << "v0: (" << v0.x << ", " << v0.y << ", " << v0.z << ")" << std::endl;
            std::cout << "v1: (" << v1.x << ", " << v1.y << ", " << v1.z << ")" << std::endl;
            std::cout << "v2: (" << v2.x << ", " << v2.y << ", " << v2.z << ")" << std::endl;
            std::cout << "v3: (" << v3.x << ", " << v3.y << ", " << v3.z << ")" << std::endl;
            continue; // Saltar este quad si tiene valores NaN
        }
        
        
        // Crear y agregar el quad al vector quads
        Quad* newQuad = new Quad(v0, v1, v2, v3);
        quads.push_back(newQuad);

        //// Mensaje de depuración para verificar los vértices del quad
        //std::cout << "Quad " << i << " vertices:" << std::endl;
        //std::cout << "v0: (" << quads[i]->v0.x << ", " << quads[i]->v0.y << ", " << quads[i]->v0.z << ")" << std::endl;
        //std::cout << "v1: (" << quads[i]->v1.x << ", " << quads[i]->v1.y << ", " << quads[i]->v1.z << ")" << std::endl;
        //std::cout << "v2: (" << quads[i]->v2.x << ", " << quads[i]->v2.y << ", " << quads[i]->v2.z << ")" << std::endl;
        //std::cout << "v3: (" << quads[i]->v3.x << ", " << quads[i]->v3.y << ", " << quads[i]->v3.z << ")" << std::endl;

    }
}


Quad* MeshNavigator::getQuadAtPosition(float x, float z) {
    for (int i = 0; i < quads.size(); i++) {
        glm::vec3 v0 = quads[i]->v0;
        glm::vec3 v1 = quads[i]->v1;
        glm::vec3 v2 = quads[i]->v2;
        glm::vec3 v3 = quads[i]->v3;

        float minx = std::min({ v0.x, v1.x, v2.x, v3.x });
        float maxx = std::max({ v0.x, v1.x, v2.x, v3.x });
        float minz = std::min({ v0.z, v1.z, v2.z, v3.z });
        float maxz = std::max({ v0.z, v1.z, v2.z, v3.z });

        /*std::cout << "minx: " << minx << std::endl;
        std::cout << "maxx: " << maxx << std::endl;
        std::cout << "minz: " << minz << std::endl;
        std::cout << "maxz: " << maxz << std::endl;*/

        if ((minx <= x && x <= maxx) && (minz <= z && z <= maxz)) return quads[i];
    }
    return nullptr;
}

// Función principal para interpolar la altura dado un punto y un quad
float getHeightInQuad(const glm::vec2& positionXZ, Quad* quad) {
    if (isPointInTriangleXZ(positionXZ, quad->v0, quad->v1, quad->v2)) {
        return interpolateHeightInTriangle(positionXZ, quad->v0, quad->v1, quad->v2);
    }
    else if (isPointInTriangleXZ(positionXZ, quad->v0, quad->v1, quad->v2)) {
        return interpolateHeightInTriangle(positionXZ, quad->v0, quad->v1, quad->v2);
    }
    else {
        // El punto está fuera del quad
        throw std::runtime_error("El punto está fuera del quad");
    }
}
