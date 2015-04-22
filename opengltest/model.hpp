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
    Model(const char *path, glm::vec3 position);
    ~Model() {}
    
    int loadColorOBJ(const char *path);

    unsigned long numVertices() { return m_numVertices; }
    glm::mat4 model() const;
    void setMarker(GLuint program, glm::vec3 position);
    void drawMarkers() const;
    
    // accessor functions
    glm::vec3 position() { return m_position; }
    GLuint positionVBO() { return m_positionVBO; }
    GLuint colorVBO() { return m_colorVBO; }
    
    // mutator functions
    void shift(glm::vec3 distance) { m_position += distance; }
    void yawBy(GLfloat angle) { m_yaw = fmod(m_yaw + angle, 2.0 * M_PI); }
    void pitchBy(GLfloat angle) { m_pitch = fmod(m_pitch + angle, 2.0 * M_PI); }
    void rollBy(GLfloat angle) { m_roll = fmod(m_roll + angle, 2.0 * M_PI); }
    void setAttribute(GLuint program, const GLchar *name, GLuint vbo) const;

    
    
    
private:
    // private functions
    
    // private variables
    unsigned long m_numVertices;
    GLuint m_positionVBO;
    GLuint m_colorVBO;
    
    glm::vec3 m_position;
    GLfloat m_yaw;
    GLfloat m_pitch;
    GLfloat m_roll;
    
    class Marker
    {
    public:
        Marker(glm::vec3 position)
        {
            std::vector<glm::vec3> positionBuffer, colorBuffer;
            positionBuffer.push_back(position);
            positionBuffer.push_back(position + glm::vec3(0.1f, 0.0f, 0.0f));
            positionBuffer.push_back(position + glm::vec3(0.0f, 0.1f, 0.0f));
            colorBuffer.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
            colorBuffer.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
            colorBuffer.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
            glGenBuffers(1, &m_positionVBO);
            glGenBuffers(1, &m_colorVBO);
            glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
            glBufferData(GL_ARRAY_BUFFER,
                         positionBuffer.size() * sizeof(glm::vec3),
                         &positionBuffer[0],
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
            glBufferData(GL_ARRAY_BUFFER,
                         colorBuffer.size() * sizeof(glm::vec3),
                         &colorBuffer[0],
                         GL_STATIC_DRAW);
        }
        ~Marker() {}
        GLuint positionVBO() const { return m_positionVBO; }
        GLuint colorVBO() const { return m_colorVBO; }
    private:
        GLuint m_positionVBO;
        GLuint m_colorVBO;
    };
    std::vector<Marker> m_markers;
    
};

#endif