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

void Model::setMarker(GLuint program, glm::vec3 position)
{
    m_markers.push_back(Marker(position));
    unsigned long numMarkers = m_markers.size();
    setAttribute(program, "vertexPosition", m_markers[numMarkers-1].positionVBO());
    setAttribute(program, "vertexColor", m_markers[numMarkers-1].colorVBO());
}

void Model::drawMarkers() const
{
    if (m_markers.empty())
        return;
    
    unsigned long numMarkers = m_markers.size();
    for (unsigned long i = 0; i < numMarkers; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_markers[i].positionVBO());
        glDrawArrays(GL_TRIANGLES, 0, 3);
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





