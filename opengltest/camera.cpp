#include "camera.hpp"

Camera::Camera(GLFWwindow* window, glm::vec3 position, float yaw, float pitch)
{
    m_window = window;
    m_position = position;
    m_yaw = yaw;
    m_pitch = pitch;
}

glm::vec3 Camera::direction()
{
    
    glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec4 result = yawMatrix * pitchMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    return glm::vec3(result[0], result[1], result[2]);
}

void Camera::move(glm::vec3 distance)
{
    m_position += distance;
}

void Camera::update()
{
    GLfloat MOVESIZE = 0.05f;
    
    // Move forward
    if (glfwGetKey( m_window, GLFW_KEY_UP ) == GLFW_PRESS){
        move(glm::vec3(0.0f, 0.0f, -MOVESIZE));
    }
    // Move backward
    if (glfwGetKey( m_window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        move(glm::vec3(0.0f, 0.0f, MOVESIZE));
    }
    // Strafe left
    if (glfwGetKey( m_window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        move(glm::vec3(-MOVESIZE, 0.0f, 0.0f));
    }
    // Strafe right
    if (glfwGetKey( m_window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        move(glm::vec3(MOVESIZE, 0.0f, 0.0f));
    }
}