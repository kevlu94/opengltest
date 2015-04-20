#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdio.h>
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Model
{
public:
    Model();
    ~Model() {}
    
    int loadUVOBJ(const char *path);
    int loadColorOBJ(const char *path);
    
    std::vector<glm::vec3> *vertexBuffer() { return &m_vertex_buffer; }
    std::vector<glm::vec3> *colorBuffer() { return &m_color_buffer; }
    std::vector<glm::vec2> *uvBuffer() { return &m_uv_buffer; }
    std::vector<glm::vec3> *normalBuffer() { return &m_normal_buffer; }
    unsigned long numPoints() { return m_vertex_buffer.size(); }
    glm::mat4 model() const;
    
    // accessor functions
    glm::vec3 position() { return m_position; }
    GLfloat yaw() { return m_yaw; }
    GLfloat pitch() { return m_pitch; }
    GLfloat roll() { return m_roll; }
    
    // mutator functions
    void shift(glm::vec3 distance) { m_position += distance; }
    void yawBy(GLfloat angle) { m_yaw = fmod(m_yaw + angle, 2.0 * M_PI); }
    void pitchBy(GLfloat angle) { m_pitch = fmod(m_pitch + angle, 2.0 * M_PI); }
    void rollBy(GLfloat angle) { m_roll = fmod(m_roll + angle, 2.0 * M_PI); }
    
    
private:
    // private functions
    void setAttribute(GLuint program, const GLchar *name, GLuint vbo) const;
    
    // private variables
    std::vector<glm::vec3> m_vertex_buffer;
    std::vector<glm::vec3> m_color_buffer;
    std::vector<glm::vec2> m_uv_buffer;
    std::vector<glm::vec3> m_normal_buffer;
    
    GLuint m_positionVBO;
    GLuint m_colorVBO;
    
    glm::vec3 m_position;
    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_roll;
    
    
};

#endif