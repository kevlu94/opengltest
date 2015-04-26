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
    int loadTextureOBJ(const char *objPath, const char *texturePath);

    unsigned long numVertices() { return m_numVertices; }
    glm::mat4 model() const;
    void setMarker(glm::vec3 position);
    void undoMarker();
    void drawMarkers(GLuint program) const;
    
    // accessor functions
    glm::vec3 position() const { return m_position; }
    GLuint positionVBO() const { return m_positionVBO; }
    GLuint colorVBO() const { return m_colorVBO; }
    bool colored() const { return m_colored; }
    GLuint textureVBO() const { return m_textureVBO; }
    GLuint texture() const { return m_texture; }
    bool textured() const { return m_textured; }
    unsigned long numMarkers() const { return m_markers.size(); }
    
    // mutator functions
    void shift(glm::vec3 distance) { m_position += distance; }
    void yawBy(GLfloat angle) { m_yaw = fmod(m_yaw + angle, 2.0 * M_PI); }
    void pitchBy(GLfloat angle) { m_pitch = fmod(m_pitch + angle, 2.0 * M_PI); }
    void rollBy(GLfloat angle) { m_roll = fmod(m_roll + angle, 2.0 * M_PI); }
    void setAttribute(GLuint program, const GLchar *name, GLuint vbo) const;

    
    
    
private:
    // private functions
    
    // private variables
    unsigned long m_numVertices = 0;
    GLuint m_positionVBO = 0;
    GLuint m_colorVBO = 0;
    bool m_colored = false;
    GLuint m_textureVBO = 0;
    GLuint m_texture = 0;
    bool m_textured = false;
    
    glm::vec3 m_position;
    glm::vec4 m_quaternion;
    GLfloat m_yaw = 0.0f;
    GLfloat m_pitch = 0.0f;
    GLfloat m_roll = 0.0f;
    
    class Marker
    {
    public:
        Marker(glm::vec3 position);
        ~Marker() {}
        unsigned long numVertices() const { return m_numVertices; }
        GLuint positionVBO() const { return m_positionVBO; }
        GLuint colorVBO() const { return m_colorVBO; }
    private:
        glm::vec3 m_center;
        unsigned long m_numVertices = 0;
        GLuint m_positionVBO = 0;
        GLuint m_colorVBO = 0;
    };
    std::vector<Marker> m_markers;
    
};

#endif