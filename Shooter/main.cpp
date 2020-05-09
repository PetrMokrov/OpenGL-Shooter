#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <vector>

// Включаем GLEW
#include <GL/glew.h>

// Включаем GLFW
#include <GLFW/glfw3.h>

// Включаем glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "shader.h"
#include "utilities.h"
#include "icosahedron.h"
#include "sphere.h"
#include "camera.h"
#include <random>
#include <chrono>
#include <list>


const int windowHeight = 1080;
const int windowWidth = 1920;
const char * windowName = "Shooter";

void default_set_viewport(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
}

void processEscPress(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool processMouseButtonPress(GLFWwindow * window) {
    return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)== GLFW_PRESS;
}

int main() {

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);

    std::uniform_real_distribution<double> distribution (-50.0,50.0);

    /////////////////////////////
    // Libraries initialization

    if( !glfwInit() )
    {
        std::printf("Ошибка при инициализации GLFWn" );
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Мы хотим использовать OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Мы не хотим старый OpenGL

    GLFWwindow* window;
    window = glfwCreateWindow( windowWidth, windowHeight, windowName, nullptr, nullptr);

    if( window == nullptr ){
        std::printf( "Невозможно открыть окно GLFW" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::printf("Error: %s\n", glewGetErrorString(err));
    }

    //////////////////////////
    // Debug information

    std::printf("Status: Using GLFW %d.%d.%d\n", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    std::printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
    std::printf("Status: VideoCard vendor: %s\n", (char*)vendor);
    std::printf("Status: VideoCard model: %s\n", (char*)renderer);
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::printf("Status: Maximum number of vertex attributes supported : %d \n", nrAttributes);

    //////////////////////////

    Icosahedron icn1;
    std::list<FireBall> fireBalls;
    std::list<Icosahedron> enemies;

    glm::mat4 projMatrix = glm::perspective(glm::radians(45.0f), (float)windowWidth/(float)windowHeight, 0.1f, 200.0f);
    glEnable(GL_DEPTH_TEST);

    Camera camera(window);
    camera.SetPos(glm::vec3(0.0, 0.0, 6.0));
    camera.SetFront(glm::vec3(0.0, 0.0, -1.0));
    camera.SetUp(glm::vec3(0.0, 1.0, 0.0));
    camera.SetForwardSpeed(20.0);
    camera.SetPitchTolerance(0.5);
    camera.SetYawTolerance(0.5);
    camera.SetMouseTolerance(0.0005);

    GLfloat lastFireTime = glfwGetTime() - 10.0;
    GLfloat lastEnemyTime = lastFireTime;

    while(!glfwWindowShouldClose(window)){

        processEscPress(window);
        camera.Process();

        GLfloat timeValue = glfwGetTime();

        // Приделываем вращения и переносы
        GLfloat rotate_angle = sin(timeValue*0.87432)* 5;
        //GLfloat rotate_angle = timeValue;
        glm::vec3 rotate_vec = glm::normalize(
                glm::vec3(cos(timeValue*1.3672), sin(timeValue*1.9382), cos(timeValue*1.09056)));
        // Создаём единичную матрицу
        glm::mat4 model_trans = glm::mat4(1.0f);
        // теперь фигачим поворот в 3D
        //model_trans = glm::rotate(model_trans, rotate_angle, rotate_vec);
        model_trans = glm::rotate(model_trans, rotate_angle, rotate_vec);

        // чистим буфер
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 camView = camera.RenderView();
        if (processMouseButtonPress(window) && lastFireTime + 1. < timeValue) {
            glm::vec3 camPosition = camera.GetCamPosition();
            glm::vec3 lookDirection = camera.GetLookDirection();
            glm::vec3 norm_look_direction = glm::normalize(lookDirection);
            fireBalls.emplace_back(FireBall(camPosition + norm_look_direction, lookDirection, 10.0, 0.3));
            lastFireTime = timeValue;
        }

        if (lastEnemyTime + 10.0 < timeValue) {
            GLfloat x = distribution(generator);
            GLfloat y = distribution(generator);
            GLfloat z = distribution(generator);
            glm::vec3 enemy_position = glm::vec3(x, y, z);

            GLfloat rot_x = distribution(generator);
            GLfloat rot_y = distribution(generator);
            GLfloat rot_z = distribution(generator);
            glm::vec3 enemy_rotate_vec = glm::normalize(glm::vec3(rot_x, rot_y, rot_z));
            GLfloat  enemy_angle = distribution(generator) / 10.0;
            glm::mat4 base = glm::mat4(1.0f);
            glm::mat4 enemy_rotate = glm::rotate(base, enemy_angle, enemy_rotate_vec);
            glm::mat4 enemy_transl = glm::translate(base, enemy_position);
            glm::mat4 enemy_transform = enemy_transl * enemy_rotate;
            enemies.emplace_back(Icosahedron());
            enemies.back().SetPosition(enemy_transform);
            lastEnemyTime = timeValue;
        }

        for(auto it = fireBalls.begin(); it != fireBalls.end() ; ++it) {
            it->Render(projMatrix, camView);
        }

        for(auto it = enemies.begin(); it != enemies.end(); ++it) {
            it->Render(projMatrix, camView);
        }

        for(auto fb = fireBalls.begin(); fb != fireBalls.end() ; ++fb) {
            for(auto e = enemies.begin(); e != enemies.end(); ++e) {
                if (e->IsInside(fb->GetPosition())) {
                    fireBalls.erase(fb);
                    enemies.erase(e);
                    break;
                }
            }
        }


        glfwSwapBuffers(window);

        // Проверяем события и вызываем функции обратного вызова.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}