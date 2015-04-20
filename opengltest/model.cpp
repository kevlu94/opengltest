#include "model.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

Model::Model()
{
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);
}




int Model::loadUVOBJ(const char *path)
{
    cerr << "Loading model from file " << path << endl;
    std::ifstream infile(path);
    
    std::vector<glm::vec3> vertexList;
    std::vector<glm::vec2> uvList;
    
    string line, label;
    
    // for vertex list and uv list
    float v0, v1, v2, u, v;
    
    // for faces
    unsigned int delimiterLoc;
    unsigned int vIndex, vtIndex;
    string indexTupleString[3];
    string vIndexString, vtIndexString;
    for (int i = 0; getline(infile, line); i++)
    {
        istringstream iss(line);
        iss >> label;
        if (label.compare("v") == 0)
        {
            iss >> v0 >> v1 >> v2;
            vertexList.push_back(glm::vec3(v0, v1, v2));
        }
        else if (label.compare("vt") == 0)
        {
            iss >> u >> v;
            uvList.push_back(glm::vec2(u, v));
        }
        else if (label.compare("f") == 0)
        {
            iss >> indexTupleString[0] >> indexTupleString[1] >> indexTupleString[2];
            for (int i = 0; i < 3; i++)
            {
                delimiterLoc = (unsigned int) indexTupleString[i].find("/");
                vIndexString = indexTupleString[i].substr(0, delimiterLoc);
                vtIndexString = indexTupleString[i].substr(delimiterLoc + 1);
                vIndex = stoi(vIndexString);
                vtIndex = stoi(vtIndexString);
                m_vertex_buffer.push_back(vertexList[vIndex]);
                m_uv_buffer.push_back(uvList[vtIndex]);
                m_color_buffer.push_back(glm::vec3(0.9f, 0.9f, 0.9f));
            }
        }
        else if (label.compare("vn") == 0)
            cerr << "normals not yet supported in model::loadObj" << endl;
        else
            continue;
    }
    return 0;
}




int Model::loadColorOBJ(const char *path)
{
    cerr << "Loading model from file " << path << endl;
    std::ifstream infile(path);
    
    std::vector<glm::vec3> vertexList;
    std::vector<glm::vec3> colorList;
    
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
            vertexList.push_back(glm::vec3(v0, v1, v2));
            colorList.push_back(glm::vec3(c0, c1, c2));
        }
        else if (label.compare("f") == 0)
        {
            iss >> index[0] >> index[1] >> index[2];
            for (int i = 0; i < 3; i++)
            {
                if (colorList[index[i]] == glm::vec3(0.0f))
                    continue;
                m_vertex_buffer.push_back(vertexList[index[i]]);
                m_color_buffer.push_back(colorList[index[i]]);
            }
        }
        else if (label.compare("vn") == 0)
            cerr << "normals not yet supported in model::loadObj" << endl;
        else
            continue;
    }
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





