//
// Created by Admin on 09.05.2020.
//

#include "icosahedron.h"
#include "utilities.h"
#include "shader.h"
#include <assert.h>
#include <glm/gtx/norm.hpp>

void IcosahedronDescriptor::LoadTexture() {
    assert(NTextureUnit == 0);
    load_texture("..//textures//sierpinskiTriangleLight.png", GL_TEXTURE0, PictureID);
}

void IcosahedronDescriptor::CompileShaders() {
    fprintf(stdout, "Shader loading: Icosahedron starts load shaders\n");
    ProgramID = LoadShaders(
            "..//shaders//icosahedron_vertex_shader.glsl",
            "..//shaders//icosahedron_fragment_shader.glsl" );
    fprintf(stdout, "Shader loading: Icosahedron shaders are loaded\n");
}

void IcosahedronDescriptor::InitializeFacetTransforms() {
    float val = 1./sqrt(3.);
    float r_i = (sqrt(3.0)/(6.))*(3. + sqrt(5));
    glm::vec3 v_inscribed = glm::vec3(0., 0., r_i);
    glm::vec4 bt_first_vtx = glm::vec4(0.0f,  2.f* val, 0.0f, 1.f);
    glm::vec4 bt_second_vtx = glm::vec4(1.0f, -val, 0.0f, 1.f);
    glm::vec4 bt_third_vts = glm::vec4(-1.0f, -val, 0.0f, 1.f);
    glm::mat4 base_transform = glm::mat4(1.0f);
    glm::mat4 trans_transform = glm::translate(base_transform, v_inscribed);
    std::vector<glm::vec3> axis = {
            glm::normalize((trans_transform * bt_first_vtx).xyz()),
            glm::normalize((trans_transform * bt_second_vtx).xyz()),
            glm::normalize((trans_transform * bt_third_vts).xyz())
    };

    std::vector<glm::mat4> facets_transforms(20, trans_transform);
    GLfloat _2pi5 = glm::radians(360./5.);
    std::vector<GLfloat> rotate_angles {0., _2pi5, 2.f*_2pi5, 3.f*_2pi5, 4.f*_2pi5};
    for(int i = 0 ; i < 3 ; ++i) {
        GLfloat angle = rotate_angles[i + 1];
        for(int rot_index = 0 ; rot_index < 3 ; ++rot_index) {
            int transform_ind = i + rot_index * 3 + 1;
            glm::mat4 rot = glm::rotate(base_transform, angle, axis[rot_index]);
            facets_transforms[transform_ind] = rot * facets_transforms[transform_ind];
        }
    }

    GLfloat  _pi = glm::radians(180.);
    glm::mat4 rotate_xpi = glm::rotate(base_transform, _pi, glm::vec3(1., 0., 0.));

    for(int i = 10; i < 20; ++i) {
        facets_transforms[i] = rotate_xpi*facets_transforms[i-10];
    }

    FacetTransforms.swap(facets_transforms);
}

IcosahedronDescriptor::IcosahedronDescriptor() {
    std::printf("Icosahedron creating\n");
    InitializeFacetTransforms();
    LoadTexture();
    CompileShaders();

    float val = 1./sqrt(3.);

    // Данные о треугольнике
    static const GLfloat triangle_vertices[] = {
            0.0f,  2.f* val, 0.5f, 0.0f,
            1.0f, -val, 0.0f, 1.0f,
            -1.0f, -val, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

IcosahedronDescriptor::~IcosahedronDescriptor() {
    std::printf("Icosahedron deleting\n");
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(ProgramID);
}

IcosahedronDescriptor& Icosahedron::getIcosahedronDescriptor() {
    static IcosahedronDescriptor icosahedronDescriptor;
    return icosahedronDescriptor;
}

void Icosahedron::SetPosition(glm::mat4 &modelMatrix) {
    _modelMatrix = modelMatrix;
}

Icosahedron::Icosahedron() {
    auto& descriptor = getIcosahedronDescriptor();
    triangleTextureLocation = glGetUniformLocation(descriptor.ProgramID, "triangleTexture");
    baseTransformLocation = glGetUniformLocation(descriptor.ProgramID, "baseTransform");
    projectionTransformLocation = glGetUniformLocation(descriptor.ProgramID, "projectionTransform");
    modelTransformLocation = glGetUniformLocation(descriptor.ProgramID, "modelTransform");
    viewTransformLocation = glGetUniformLocation(descriptor.ProgramID, "viewTransform");
    colorFlagLocation = glGetUniformLocation(descriptor.ProgramID, "colorFlag");
}

void Icosahedron::Render(glm::mat4& projMatrix, glm::mat4& viewMatrix) {
    auto& descriptor = getIcosahedronDescriptor();
    GLint64 curr_program = 0;
    glGetInteger64v(GL_CURRENT_PROGRAM, &curr_program);
    if( (GLint64) descriptor.ProgramID != curr_program) {
        glUseProgram(descriptor.ProgramID);
    }

    // привязываем текстурку
    glUniform1i(triangleTextureLocation, descriptor.NTextureUnit);
    // привязываем проекционную матрицу
    glUniformMatrix4fv(projectionTransformLocation, 1, GL_FALSE, glm::value_ptr(projMatrix));
    // привязываем модельную матрицу
    glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
    // привязываем view
    glUniformMatrix4fv(viewTransformLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    for(int i = 0 ; i < 20; ++i) {
        glm::mat4 trans = descriptor.FacetTransforms[i];
        if(i == 19) {
            glUniform1i(colorFlagLocation, true);
        } else {
            glUniform1i(colorFlagLocation, false);
        }
        glUniformMatrix4fv(baseTransformLocation, 1, GL_FALSE, glm::value_ptr(trans));
        glBindVertexArray(descriptor.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

}

bool Icosahedron::IsInside(glm::vec3 point) {
    glm::vec3 center = glm::vec3(_modelMatrix[3][0], _modelMatrix[3][1], _modelMatrix[3][2]);
    return glm::l2Norm(center - point) < (sqrt(3.0)/(6.))*(3. + sqrt(5));
}
