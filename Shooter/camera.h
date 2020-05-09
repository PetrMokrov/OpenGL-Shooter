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

#include <GLFW/glfw3.h>

#ifndef SHOOTER_CAMERA_H
#define SHOOTER_CAMERA_H

class Camera {
public:
    Camera(GLFWwindow* window);
    void SetPos(glm::vec3 cam_pos);
    void SetFront(glm::vec3 cam_front);
    void SetUp(glm::vec3 cam_up);
    void SetForwardSpeed(float speed);
    void SetPitchTolerance(float pitch_tolerance);
    void SetYawTolerance(float yaw_tolerance);
    void SetMouseTolerance(float mouse_tolerance);
    glm::vec3 GetCamPosition();
    glm::vec3 GetLookDirection();
    void Process();
    glm::mat4 RenderView();

private:
    glm::vec3 _cam_pos = glm::vec3(0., 0., 1.);
    glm::vec3 _cam_front = glm::vec3(0., 0., -1.0);
    glm::vec3 _cam_up = glm::vec3(0., 1., 0.);
    GLFWwindow* _window;

    void update_key_activity(bool& key_active, int action);
    void update_pitch(GLfloat delta_time, bool positive);
    void update_yaw(GLfloat delta_time, bool positive);
    void update_forward(GLfloat delta_time);

    bool kp8_key_active = false;
    bool kp4_key_active = false;
    bool kp2_key_active = false;
    bool kp6_key_active = false;
    bool kp5_key_active = false;

    GLfloat _last_render_time = -1.0;

    float _pitch_tolerance = 0.1;
    float _yaw_tolerance = 0.1;
    float _forward_tolerance = 10.0;

    float mouse_offset_tolerance = 0.001;

    float theta = 0.0;
    float phi = 0.0;
    void get_final_cam_position(glm::vec3& cam_pos, glm::vec3& cam_front, glm::vec3& cam_up);
    void update_theta_phi();

};

#endif //SHOOTER_CAMERA_H
