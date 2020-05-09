//
// Created by Admin on 09.05.2020.
//

#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#ifndef SHOOTER_ICOSAHEDRON_H
#define SHOOTER_ICOSAHEDRON_H

struct IcosahedronDescriptor {
    GLuint VAO;
    GLuint VBO;
    GLuint PictureID;
    GLuint ProgramID;
    std::vector<glm::mat4> FacetTransforms;
    int NTextureUnit = 0;

    IcosahedronDescriptor();
    ~IcosahedronDescriptor();

    void InitializeFacetTransforms();
    void LoadTexture();
    void CompileShaders();
};

class Icosahedron {
public:

    Icosahedron();
    void SetPosition(glm::mat4& modelMatrix);
    void Render(glm::mat4& projMatrix, glm::mat4& viewMatrix);
    bool IsInside(glm::vec3 point);

private:
    glm::mat4 _modelMatrix{1.0};
    GLint triangleTextureLocation;
    GLint baseTransformLocation;
    GLint projectionTransformLocation;
    GLint modelTransformLocation;
    GLint viewTransformLocation;
    GLint colorFlagLocation;

    static IcosahedronDescriptor& getIcosahedronDescriptor();
};

#endif //SHOOTER_ICOSAHEDRON_H
