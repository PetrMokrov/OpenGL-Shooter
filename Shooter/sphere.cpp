//
// Created by Admin on 09.05.2020.
//

#include "sphere.h"
#include "utilities.h"
#include "shader.h"
#include <assert.h>
#include <GLFW/glfw3.h>

void SphereDescriptor::LoadTexture() {
    assert(NTextureUnit == 1);
    load_texture("..//textures//fire.jpg", GL_TEXTURE1, PictureID);
}

void SphereDescriptor::CompileShaders() {
    fprintf(stdout, "Shader loading: Sphere starts load shaders\n");
    ProgramID = LoadShaders(
            "..//shaders//sphere_vertex_shader.glsl",
            "..//shaders//sphere_fragment_shader.glsl" );
    fprintf(stdout, "Shader loading: Sphere shaders are loaded\n");
}

void computeTriangle(std::vector<GLfloat> &vertex_buffer, int h, int h_tol, int r, int r_tol, float radius) {
    float th = (float)glm::pi<float>() * h / h_tol;
    float phi = 2.0f * (float)glm::pi<float>() * r / r_tol + (float)glm::pi<float>();
    GLfloat x = radius * sin(th) * cos(phi);
    GLfloat y = radius * sin(th) * sin(phi);
    GLfloat z = radius * cos(th);
    GLfloat tx = float(r) / r_tol;
    GLfloat ty = 1.0 - float(h) / h_tol;
    vertex_buffer.push_back(x);
    vertex_buffer.push_back(y);
    vertex_buffer.push_back(z);
    vertex_buffer.push_back(tx);
    vertex_buffer.push_back(ty);
}

SphereDescriptor::SphereDescriptor() {
    std::printf("Sphere creating\n");
    LoadTexture();
    CompileShaders();
    std::vector<GLfloat> vertex_buffer;
    MakeVertexBuffer(vertex_buffer);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size() * sizeof(GLfloat), &vertex_buffer.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void SphereDescriptor::MakeVertexBuffer(std::vector<GLfloat> &vertex_buffer) {
    int h_tol = tolerance / 2;
    int r_tol = tolerance;

    vertex_buffer.clear();

    for (int i = 0; i < h_tol; i++)
    {
        for (int j = 0; j < r_tol; j++)
        {
            computeTriangle(vertex_buffer, i, h_tol, j, r_tol, radius);
            computeTriangle(vertex_buffer, i + 1, h_tol, j, r_tol, radius);
            computeTriangle(vertex_buffer, i, h_tol, j + 1, r_tol, radius);

            computeTriangle(vertex_buffer, i + 1, h_tol, j, r_tol, radius);
            computeTriangle(vertex_buffer, i, h_tol, j + 1, r_tol, radius);
            computeTriangle(vertex_buffer, i + 1, h_tol, j + 1, r_tol, radius);
        }
    }
    VertexElementsCount = int(vertex_buffer.size() / 5);
}

SphereDescriptor::~SphereDescriptor() {
    std::printf("Sphere deleting\n");
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(ProgramID);
}

Sphere::Sphere() {
    auto& descriptor = getSphereDescriptor();
    sphereTextureLocation = glGetUniformLocation(descriptor.ProgramID, "sphereTexture");
    projectionTransformLocation = glGetUniformLocation(descriptor.ProgramID, "projectionTransform");
    modelTransformLocation = glGetUniformLocation(descriptor.ProgramID, "modelTransform");
    viewTransformLocation = glGetUniformLocation(descriptor.ProgramID, "viewTransform");
}

SphereDescriptor& Sphere::getSphereDescriptor() {
    static SphereDescriptor sphereDescriptor;
    return sphereDescriptor;
}

void Sphere::SetPosition(glm::mat4 &modelMatrix) {
    _modelMatrix = modelMatrix;
}

void Sphere::Render(glm::mat4 &projMatrix, glm::mat4 &viewMatrix) {
    auto& descriptor = getSphereDescriptor();
    GLint64 curr_program = 0;
    glGetInteger64v(GL_CURRENT_PROGRAM, &curr_program);
    if( (GLint64) descriptor.ProgramID != curr_program) {
        glUseProgram(descriptor.ProgramID);
    }

    // привязываем текстурку
    glUniform1i(sphereTextureLocation, descriptor.NTextureUnit);
    // привязываем проекционную матрицу
    glUniformMatrix4fv(projectionTransformLocation, 1, GL_FALSE, glm::value_ptr(projMatrix));
    // привязываем модельную матрицу
    glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
    // привязываем view
    glUniformMatrix4fv(viewTransformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glBindVertexArray(descriptor.VAO);
    glDrawArrays(GL_TRIANGLES, 0, descriptor.VertexElementsCount);
    glBindVertexArray(0);
}

FireBall::FireBall(glm::vec3 position, glm::vec3 directionVector, float speed, float radius)
    : Sphere() {
    _directionVector = glm::normalize(directionVector);
    _speed = speed;
    _radius = radius;
    _initial_position = position;
    _first_render_time = -1.;
}

void FireBall::Render(glm::mat4& projMatrix, glm::mat4& viewMatrix) {
    glm::mat4 _base = glm::mat4(1.0f);
    glm::mat4 init_scale = glm::scale(_base, glm::vec3(_radius, _radius, _radius));
    glm::mat4 init_pos_move = glm::translate(_base, _initial_position);
    GLfloat curr_time = glfwGetTime();
    if(_first_render_time < 0) {
        _first_render_time = curr_time;
    }
    GLfloat time_offset = curr_time - _first_render_time;
    glm::mat4 fly_offset = glm::translate(_base, _directionVector * _speed * time_offset);
    glm::mat4 positionMatrix = fly_offset * init_pos_move*init_scale;
    SetPosition(positionMatrix);
    Sphere::Render(projMatrix, viewMatrix);
}

glm::vec3 FireBall::GetPosition() {
    GLfloat curr_time = glfwGetTime();
    if(_first_render_time < 0) {
        _first_render_time = curr_time;
    }
    GLfloat time_offset = curr_time - _first_render_time;
    return _initial_position + time_offset * _directionVector * _speed;
}

