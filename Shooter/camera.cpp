//
// Created by Admin on 09.05.2020.
//

#include "camera.h"

Camera::Camera(GLFWwindow* window) {
    _window = window;
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Camera::SetPos(glm::vec3 cam_pos) {
    _cam_pos = cam_pos;
}
void Camera::SetFront(glm::vec3 cam_front) {
    _cam_front = cam_front;
}
void Camera::SetUp(glm::vec3 cam_up) {
    _cam_up = cam_up;
}
void Camera::SetForwardSpeed(float speed) {
    _forward_tolerance = speed;
}
void Camera::SetPitchTolerance(float pitch_tolerance) {
    _pitch_tolerance = pitch_tolerance;
}
void Camera::SetYawTolerance(float yaw_tolerance) {
    _yaw_tolerance = yaw_tolerance;
}

void Camera::SetMouseTolerance(float mouse_tolerance) {
    mouse_offset_tolerance = mouse_tolerance;
}

void Camera::update_key_activity(bool& key_active, int action) {
    if(action == GLFW_PRESS) {
        key_active = true;
    }
    if(action == GLFW_RELEASE) {
        key_active = false;
    }
}

void Camera::update_yaw(GLfloat delta_time, bool positive) {
    if(!positive) {
        delta_time *= -1.;
    }
    GLfloat angle = delta_time * _yaw_tolerance;
    _cam_front = glm::rotate(_cam_front , angle, _cam_up);
}

void Camera::update_pitch(GLfloat delta_time, bool positive) {
    if(!positive) {
        delta_time *= -1.;
    }
    GLfloat angle = delta_time * _pitch_tolerance;
    glm::vec3 normal = glm::normalize(glm::cross(_cam_front, _cam_up));
    _cam_front = glm::rotate(_cam_front, angle, normal);
    _cam_up = glm::rotate(_cam_up, angle, normal);
}

void Camera::update_forward(GLfloat delta_time) {
    glm::vec3 _cam_front_normal = glm::normalize(_cam_front);
    _cam_pos += _cam_front_normal * delta_time * _forward_tolerance;
}

void Camera::get_final_cam_position(glm::vec3 &cam_pos, glm::vec3 &cam_front, glm::vec3& cam_up) {
    cam_pos = _cam_pos;
    cam_front = _cam_front;
    cam_up = _cam_up;

    cam_front = glm::rotate(cam_front , phi, _cam_up);

    glm::vec3 normal = glm::normalize(glm::cross(_cam_front, _cam_up));
    cam_front = glm::rotate(cam_front, theta, normal);
    cam_up = glm::rotate(cam_up, theta, normal);
}

void Camera::update_theta_phi() {
    double xpos = 0.0;
    double ypos = 0.0;
    glfwGetCursorPos(_window, &xpos, &ypos);
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    float x_offset = mouse_offset_tolerance*((double)width/2 - xpos);
    float y_offset = ((double)height/2 - ypos)*mouse_offset_tolerance;
    phi = x_offset;
    theta = y_offset;
}

void Camera::Process() {
    update_key_activity(kp8_key_active, glfwGetKey(_window, GLFW_KEY_KP_8));
    update_key_activity(kp6_key_active, glfwGetKey(_window, GLFW_KEY_KP_6));
    update_key_activity(kp4_key_active, glfwGetKey(_window, GLFW_KEY_KP_4));
    update_key_activity(kp2_key_active, glfwGetKey(_window, GLFW_KEY_KP_2));
    update_key_activity(kp5_key_active, glfwGetKey(_window, GLFW_KEY_KP_5));
}

glm::mat4 Camera::RenderView() {
    GLfloat current_time = glfwGetTime();
    if(_last_render_time < 0) {
        _last_render_time = current_time;
    }
    GLfloat time_offset = current_time - _last_render_time;

    // do all the stuff
    if(kp5_key_active) update_forward(time_offset);
    if(kp4_key_active) update_yaw(time_offset, true);
    if(kp6_key_active) update_yaw(time_offset, false);
    if(kp8_key_active) update_pitch(time_offset, true);
    if(kp2_key_active) update_pitch(time_offset, false);

    // mouse stuff
    update_theta_phi();
    glm::vec3 final_cam_pos;
    glm::vec3 final_cam_front;
    glm::vec3 final_cam_up;
    get_final_cam_position(final_cam_pos, final_cam_front, final_cam_up);

    _last_render_time = current_time;

    return glm::lookAt(final_cam_pos, final_cam_front + final_cam_pos, final_cam_up);
}

glm::vec3 Camera::GetCamPosition() {
    return _cam_pos;
}
glm::vec3 Camera::GetLookDirection() {
    glm::vec3 final_cam_pos;
    glm::vec3 final_cam_front;
    glm::vec3 final_cam_up;
    get_final_cam_position(final_cam_pos, final_cam_front, final_cam_up);
    return final_cam_front;
}

