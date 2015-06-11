#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "globals.hpp"

class Camera
{
public:
    Camera(GLFWwindow* window, glm::vec3 position, float yaw, float pitch);
    ~Camera() {}
    
    void update();
    void move(glm::vec3 distance);

    // accessor functions
    GLFWwindow* window() { return m_window; }
    glm::vec3 position() { return m_position; }
    glm::vec3 direction();
    
    
private:
    GLFWwindow* m_window;
    glm::vec3 m_position;
    float m_yaw;
    float m_pitch;
};


#endif