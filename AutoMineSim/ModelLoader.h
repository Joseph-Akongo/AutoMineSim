#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Model {
public:
    GLuint VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    Model(const char* filename);
    GLuint loadTexture(const char* filename);

    void draw();
};

bool loadOBJ(const char* path, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<unsigned int>& indices);

#endif
