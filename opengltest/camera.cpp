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
    //EDIT: fix mouse tracking and yaw/pitch mechanics
    
    /*double dx, dy;
    glfwGetCursorPos(m_window, &dx, &dy);
    dx -= WINDOW_WIDTH / 2;
    dy -= WINDOW_HEIGHT / 2;
    glfwSetCursorPos(m_window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    
    m_yaw = fmod(m_yaw + dx, 2.0 * M_PI);
    m_pitch += dy;*/
    
    // Move forward
    if (glfwGetKey( m_window, GLFW_KEY_UP ) == GLFW_PRESS){
        m_pitch = fmod(m_pitch + 0.1, 2.0 * M_PI);
    }
    // Move backward
    if (glfwGetKey( m_window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        m_pitch = fmod(m_pitch - 0.1, 2.0 * M_PI);
    }
    // Strafe right
    if (glfwGetKey( m_window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        m_yaw = fmod(m_yaw + 0.1, 2.0 * M_PI);
    }
    // Strafe left
    if (glfwGetKey( m_window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        m_yaw = fmod(m_yaw - 0.1, 2.0 * M_PI);
    }
    
    if (m_pitch > M_PI / 2)
        m_pitch = M_PI / 2;
    else if (m_pitch < -M_PI / 2)
        m_pitch = -M_PI / 2;
}