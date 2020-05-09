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

#ifndef SHOOTER_SPHERE_H
#define SHOOTER_SPHERE_H

struct SphereDescriptor{
    GLuint VAO;
    GLuint VBO;
    GLuint PictureID;
    GLuint ProgramID;
    int VertexElementsCount;
    int NTextureUnit = 1;
    int tolerance{100};
    float radius{1.0};

    SphereDescriptor();
    ~SphereDescriptor();

    void MakeVertexBuffer(std::vector<GLfloat>& vertex_buffer);
    void LoadTexture();
    void CompileShaders();
};

class Sphere {
public:

    Sphere();
    void SetPosition(glm::mat4& modelMatrix);
    void Render(glm::mat4& projMatrix, glm::mat4& viewMatrix);

private:
    glm::mat4 _modelMatrix{1.0};
    GLint sphereTextureLocation;
    GLint projectionTransformLocation;
    GLint modelTransformLocation;
    GLint viewTransformLocation;

    static SphereDescriptor& getSphereDescriptor();
};

class FireBall : public Sphere {
public:
    FireBall(glm::vec3 position, glm::vec3 directionVector, float speed, float radius);
    void Render(glm::mat4& projMatrix, glm::mat4& viewMatrix);

    glm::vec3 GetPosition();

private:
    glm::vec3 _directionVector;
    glm::vec3 _initial_position;
    float _speed;
    float _radius;
    float _first_render_time;
};


#endif //SHOOTER_SPHERE_H
