#define STB_IMAGE_IMPLEMENTATION
#include "ModelLoader.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>

Model::Model(const char* filename) {
    if (!loadOBJ(filename, vertices, normals, indices) || vertices.empty() || indices.empty()) {
        std::cerr << "Error: Model failed to load or is empty: " << filename << std::endl;
        return;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.empty() ? nullptr : &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.empty() ? nullptr : &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);
}

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

void Model::draw() {
    if (indices.empty()) return; // Prevents drawing with invalid data
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool loadOBJ(const char* path, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }


    std::vector<glm::vec3> temp_vertices, temp_normals;
    std::vector<unsigned int> temp_indices;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::string vertex1, vertex2, vertex3;
            ss >> vertex1 >> vertex2 >> vertex3;

            try {
                unsigned int v1 = std::stoi(vertex1), v2 = std::stoi(vertex2), v3 = std::stoi(vertex3);
                if (v1 == 0 || v2 == 0 || v3 == 0 || v1 > temp_vertices.size() || v2 > temp_vertices.size() || v3 > temp_vertices.size()) {
                    std::cerr << "Error: Invalid face indices in OBJ file" << std::endl;
                    continue;
                }

                temp_indices.push_back(v1 - 1);
                temp_indices.push_back(v2 - 1);
                temp_indices.push_back(v3 - 1);
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing face indices: " << e.what() << std::endl;
            }
        }
    }
    vertices = temp_vertices;
    normals = temp_normals;
    indices = temp_indices;

    return true;
}
