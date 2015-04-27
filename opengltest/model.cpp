#include "model.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SOIL.h>



using namespace std;

Model::Model()
{
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);
    glGenBuffers(1, &m_textureVBO);
    glGenTextures(1, &m_texture);
}

Model::Model(const char *path, glm::vec3 position, const char *texturePath)
{
    m_position = position;
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);
    glGenBuffers(1, &m_textureVBO);
    glGenTextures(1, &m_texture);
    if (texturePath)
        loadTextureOBJ(path, texturePath);
    else
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
        switch (label[0])
        {
            case ('v'):
            {
                iss >> v0 >> v1 >> v2 >> c0 >> c1 >> c2;
                pointList.push_back(glm::vec3(v0, v1, v2));
                colorList.push_back(glm::vec3(c0, c1, c2));
                break;
            }
            case ('f'):
            {
                iss >> index[0] >> index[1] >> index[2];
                for (int i = 0; i < 3; i++)
                {
                    pointBuffer.push_back(pointList[index[i] - 1]);
                    colorBuffer.push_back(colorList[index[i] - 1]);
                }
                break;
            }
            default:
            {
                continue;
            }
        }
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
    
    m_colored = true;
    
    return 0;
}

// need major edits
int Model::loadTextureOBJ(const char *objPath, const char *texturePath)
{
    cerr << "Loading texture model from file " << objPath << endl;
    std::ifstream infile(objPath);
    
    std::vector<glm::vec3> pointList;
    std::vector<glm::vec2> textureList;
    std::vector<glm::vec3> pointBuffer;
    std::vector<glm::vec2> textureBuffer;
    
    
    string line, label;
    
    // for vertex list
    float v0, v1, v2, t0, t1;
    
    // for faces
    std::string indexTupleString;
    for (int i = 0; getline(infile, line); i++)
    {
        istringstream iss(line);
        iss >> label;
        if (!label.length())
            continue;
        switch (label[0])
        {
            case ('v'):
            {
                if (label.length() == 1)
                {
                    iss >> v0 >> v1 >> v2;
                    pointList.push_back(glm::vec3(v0, v1, v2));
                }
                else if (label[1] == 't')
                {
                    iss >> t0 >> t1;
                    textureList.push_back(glm::vec2(t0, t1));
                }
                else
                {
                    fprintf(stderr, "Error: \"v%c\" not yet supported\n", label[1]);
                    return -1;
                }
                break;
            }
            case ('f'):
            {
                unsigned int delimiterLoc;
                unsigned int vIndex, vtIndex;
                string indexTupleString;
                string vIndexString, vtIndexString;
                
                for (int i = 0; i < 3; i++)
                {
                    iss >> indexTupleString;
                    delimiterLoc = (unsigned int) indexTupleString.find("/");
                    vIndexString = indexTupleString.substr(0, delimiterLoc);
                    vtIndexString = indexTupleString.substr(delimiterLoc + 1);
                    vIndex = stoi(vIndexString);
                    vtIndex = stoi(vtIndexString);
                    pointBuffer.push_back(pointList[vIndex - 1]);
                    textureBuffer.push_back(textureList[vtIndex - 1]);
                }
                break;
            }
            default:
            {
                continue;
            }
        }
    }
    
    m_numVertices = pointBuffer.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &pointBuffer[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_textureVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec2),
                 &textureBuffer[0],
                 GL_STATIC_DRAW);
    
    // load texture
    cerr << "Loading image from file " << texturePath << endl;
    
    int img_width, img_height;
    unsigned char* img = SOIL_load_image(texturePath, &img_width, &img_height, NULL, 0);
    
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    
    m_textured = true;
    
    return 0;
}


glm::mat4 Model::model() const
{
    glm::quat quaternion = glm::quat(glm::vec3(m_pitch, m_yaw, m_roll));
    glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), m_position);
    return translateMatrix * rotationMatrix;
    
}

void Model::setMarker(glm::vec3 position)
{
    m_markers.push_back(Marker(position));
}

void Model::undoMarker()
{
    if (m_markers.size())
    {
        fprintf(stderr, "Deleted marker %lu\n", numMarkers());
        m_markers.pop_back();
    }
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
        setAttribute(program, "vertexPosition", 3, marker->positionVBO());
        setAttribute(program, "vertexColor", 3, marker->colorVBO());
        glBindBuffer(GL_ARRAY_BUFFER, marker->positionVBO());
        glDrawArrays(GL_TRIANGLES, 0, (int) marker->numVertices());
    }
    
}


// Private functions



void Model::setAttribute(GLuint program, const GLchar *name, unsigned int size, GLuint vbo) const
{
    GLint attribLoc = glGetAttribLocation(program, name);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(attribLoc);
    glVertexAttribPointer(attribLoc,
                          size,
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


