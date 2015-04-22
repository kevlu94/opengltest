#include "model.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

Model::Model()
{
    m_numVertices = 0;
    m_yaw = 0;
    m_pitch = 0;
    m_roll = 0;
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);
}

Model::Model(const char *path, glm::vec3 position)
{
    m_numVertices = 0;
    m_yaw = 0;
    m_pitch = 0;
    m_roll = 0;
    m_position = position;
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);
    loadColorOBJ(path);
}



int Model::loadColorOBJ(const char *path)
{
    cerr << "Loading model from file " << path << endl;
    std::ifstream infile(path);
    
    std::vector<glm::vec3> pointList;
    std::vector<glm::vec3> colorList;
    std::vector<glm::vec3> pointBuffer;
    std::vector<glm::vec3> colorBuffer;
    
    
    string line, label;
    
    // for vertex list and color list
    float v0, v1, v2, c0, c1, c2;
    
    // for faces
    unsigned int index[3];
    for (int i = 0; getline(infile, line); i++)
    {
        istringstream iss(line);
        iss >> label;
        if (label.compare("v") == 0)
        {
            iss >> v0 >> v1 >> v2 >> c0 >> c1 >> c2;
            pointList.push_back(glm::vec3(v0, v1, v2));
            colorList.push_back(glm::vec3(c0, c1, c2));
        }
        else if (label.compare("f") == 0)
        {
            iss >> index[0] >> index[1] >> index[2];
            for (int i = 0; i < 3; i++)
            {
                pointBuffer.push_back(pointList[index[i] - 1]);
                colorBuffer.push_back(colorList[index[i] - 1]);
            }
        }
        else if (label.compare("vn") == 0)
            cerr << "normals not yet supported in model::loadObj" << endl;
        else
            continue;
    }
    
    m_numVertices = pointBuffer.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &pointBuffer[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &colorBuffer[0],
                 GL_STATIC_DRAW);
    
    return 0;
}


glm::mat4 Model::model() const
{
    glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), m_roll, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), m_position);
    return translateMatrix * rollMatrix * pitchMatrix * yawMatrix;
}

void Model::setMarker(glm::vec3 position)
{
    m_markers.push_back(Marker(position));
}

void Model::drawMarkers(GLuint program) const
{
    if (m_markers.empty())
        return;
    
    unsigned long numMarkers = m_markers.size();
    const Marker *marker;
    for (unsigned long i = 0; i < numMarkers; i++)
    {
        marker = &m_markers[i];
        setAttribute(program, "vertexPosition", marker->positionVBO());
        setAttribute(program, "vertexColor", marker->colorVBO());
        glBindBuffer(GL_ARRAY_BUFFER, marker->positionVBO());
        glDrawArrays(GL_TRIANGLES, 0, (int) marker->numVertices());
    }
    
}


// Private functions



void Model::setAttribute(GLuint program, const GLchar *name, GLuint vbo) const
{
    GLint attribLoc = glGetAttribLocation(program, name);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*)0);
}

vector<glm::vec3> cube(glm::vec3 center, GLfloat size)
{
    std::vector<glm::vec3> p = {
        center + glm::vec3(size, -size, -size),
        center + glm::vec3(size, -size, size),
        center + glm::vec3(-size, -size, size),
        center + glm::vec3(-size, -size, -size),
        center + glm::vec3(size, size, -size),
        center + glm::vec3(size, size, size),
        center + glm::vec3(-size, size, size),
        center + glm::vec3(-size, size, -size)
    };
    std::vector<glm::vec3> ret = {
        p[4], p[0], p[3],
        p[4], p[3], p[7],
        p[2], p[6], p[7],
        p[2], p[7], p[3],
        p[1], p[5], p[2],
        p[5], p[6], p[2],
        p[0], p[4], p[1],
        p[4], p[5], p[1],
        p[4], p[7], p[5],
        p[7], p[6], p[5],
        p[0], p[1], p[2],
        p[0], p[2], p[3]
    };
    return ret;
}

Model::Marker::Marker(glm::vec3 position)
{
    std::vector<glm::vec3> positionBuffer = cube(position, 0.002f);
    std::vector<glm::vec3> colorBuffer;
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    m_numVertices = positionBuffer.size();
    for (unsigned long i = 0; i < m_numVertices; i++)
        colorBuffer.push_back(color);
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


