#include "model.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <SOIL.h>
#include <omp.h>
#include <unordered_set>

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
    
    std::vector<glm::vec3> positionList;
    std::vector<glm::vec3> colorList;
    
    
    string line, label;
    
    // for vertex list and color list
    float v0, v1, v2, c0, c1, c2;
    
    // for faces
    unsigned int index[3];
    while (getline(infile, line))
    {
        istringstream iss(line);
        iss >> label;
        switch (label[0])
        {
            case ('v'):
            {
                iss >> v0 >> v1 >> v2 >> c0 >> c1 >> c2;
                positionList.push_back(glm::vec3(v0, v1, v2));
                colorList.push_back(glm::vec3(c0, c1, c2));
                break;
            }
            case ('f'):
            {
                iss >> index[0] >> index[1] >> index[2];
                for (int i = 0; i < 3; i++)
                {
                    m_positionVector.push_back(positionList[index[i] - 1]);
                    m_colorVector.push_back(colorList[index[i] - 1]);
                }
                break;
            }
            default:
            {
                continue;
            }
        }
    }
    
    m_numVertices = m_positionVector.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &m_positionVector[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &m_colorVector[0],
                 GL_STATIC_DRAW);
    
    m_colored = true;
    
    return 0;
}

// need major edits
int Model::loadTextureOBJ(const char *objPath, const char *texturePath)
{
    cerr << "Loading texture model from file " << objPath << endl;
    std::ifstream infile(objPath);
    
    std::vector<glm::vec3> positionList;
    std::vector<glm::vec2> textureList;
    std::vector<glm::vec3> normalList;
    
    
    string line, label;
    
    // for vertex list
    float v0, v1, v2, t0, t1, n0, n1, n2;
    
    // for faces
    std::string indexTupleString;
    while (getline(infile, line))
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
                    positionList.push_back(glm::vec3(SCALE_FACE * v0, SCALE_FACE * v1, SCALE_FACE * v2));
                }
                else if (label[1] == 't')
                {
                    iss >> t0 >> t1;
                    textureList.push_back(glm::vec2(t0, t1));
                }
                else if (label[1] == 'n')
                {
                    m_normal = true;
                    iss >> n0 >> n1 >> n2;
                    normalList.push_back(glm::vec3(n0, n1, n2));
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
                unsigned int vIndex, vtIndex, vnIndex;
                string indexTupleString;
                string vIndexString, vtIndexString, vnIndexString;
                
                for (int i = 0; i < 3; i++)
                {
                    iss >> indexTupleString;
                    delimiterLoc = (unsigned int) indexTupleString.find("/");
                    vIndexString = indexTupleString.substr(0, delimiterLoc);
                    vtIndexString = indexTupleString.substr(delimiterLoc + 1);

                    if (m_normal)
                    {
                        delimiterLoc = (unsigned int) vtIndexString.find("/");
                        vnIndexString = vtIndexString.substr(delimiterLoc + 1);
                        vtIndexString = vtIndexString.substr(0, delimiterLoc);

                        vnIndex = atoi(vnIndexString.c_str());
                        m_normalVector.push_back(normalList[vnIndex - 1]);
                    }

                    vIndex = atoi(vIndexString.c_str());
                    vtIndex = atoi(vtIndexString.c_str());
                    m_positionVector.push_back(positionList[vIndex - 1]);
                    m_textureVector.push_back(textureList[vtIndex - 1]);
                }
                break;
            }
            default:
            {
                continue;
            }
        }
    }
    
    m_numVertices = m_positionVector.size();
    
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &m_positionVector[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_textureVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec2),
                 &m_textureVector[0],
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &m_normalVector[0],
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

GLfloat arctan(GLfloat x, GLfloat y)
{
    GLfloat extra = 0.0;
    if (x < 0.0)
        extra = 180.0;
    else if (y < 0.0)
        extra = 360.0;
    return glm::atan(y/x) / 3.14159 * 180.0 + extra;
}

float correspondenceScore(glm::vec3 p1, glm::vec3 p2)
{
    glm::vec3 diff = p2 - p1;
    return glm::dot(diff, diff);

    // GLfloat x1 = p1[0], y1 = p1[1], z1 = p1[2], x2 = p2[0], y2 = p2[1], z2 = p2[2];

    // GLfloat phi1 = arctan(x1, -z1);
    // GLfloat phi2 = arctan(x2, -z2);

    // GLfloat theta1 = glm::acos(y1 / glm::fastSqrt(x1*x1 + y1*y1 + z1*z1));
    // GLfloat theta2 = glm::acos(y2 / glm::fastSqrt(x2*x2 + y2*y2 + z2*z2));

    // GLfloat dPhi = phi2 - phi1;
    // GLfloat dTheta = theta2 - theta1;
    // return dPhi*dPhi + dTheta*dTheta;
}



// Use ICP to project points onto another model, O(mn)
void Model::projectOnto(Model *target)
{
    if (m_projected)
        return;

    fprintf(stderr, "Constructing projection...\n");


    // std::vector<glm::vec2> p;
    // p.push_back(glm::vec2(1.0, 0.0));
    // p.push_back(glm::vec2(0.866, 0.5));
    // p.push_back(glm::vec2(0.5, 0.866));
    // p.push_back(glm::vec2(0.0, 1.0));
    // p.push_back(glm::vec2(-0.5, 0.866));
    // p.push_back(glm::vec2(-0.866, 0.5));
    // p.push_back(glm::vec2(-1.0, 0.0));
    // p.push_back(glm::vec2(-0.866, -0.5));
    // p.push_back(glm::vec2(-0.5, -0.866));
    // p.push_back(glm::vec2(0.0, -1.0));
    // p.push_back(glm::vec2(0.5, -0.866));
    // p.push_back(glm::vec2(0.866, -0.5));
    // p.push_back(glm::vec2(1.0, 0.0));

    // for (int i = 0; i < p.size(); i++)
    // {
    //     cout << arctan(p[i][0], p[i][1]) << endl;
    // }

    // exit(1);




    std::vector<glm::vec3> *targetPositions = target->positionVector();
    std::vector<glm::vec2> *targetTextures = target->textureVector();


    int numTargetPositions = targetPositions->size();

    glm::vec3 curPosition;;
    glm::vec3 curTargetPosition;
    int bestIndex;
    glm::vec3 diff;
    float bestScore;
    float curScore;

    m_projectionPositionVector = std::vector<glm::vec3>(m_numVertices);
    m_projectionTextureVector = std::vector<glm::vec2>(m_numVertices);

    // unordered_set<int> used;

    int i = 0, j = 0;
    #pragma omp parallel for private(curPosition, curTargetPosition, bestIndex, diff, bestScore, curScore, j)
    for (i = 0; i < m_numVertices; i++)
    {
        curPosition = m_positionVector[i];
        bestScore = FLT_MAX;
        for (j = 0; j < numTargetPositions; j++)
        {
            // if (used.find(j) != used.end())
            //     continue;
            curScore = correspondenceScore(curPosition, (*targetPositions)[j]);
            if (curScore < bestScore)
            {
                bestScore = curScore;
                bestIndex = j;
            }
        }
        m_projectionPositionVector[i] = (*targetPositions)[bestIndex];
        m_projectionTextureVector[i] = (*targetTextures)[bestIndex];
        // used.insert(bestIndex);
    }
    m_projectionTexture = target->texture();

    glGenBuffers(1, &m_projectionPositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_projectionPositionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec3),
                 &m_projectionPositionVector[0],
                 GL_STATIC_DRAW);

    glGenBuffers(1, &m_projectionTextureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_projectionTextureVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_numVertices * sizeof(glm::vec2),
                 &m_projectionTextureVector[0],
                 GL_STATIC_DRAW);

    m_projected = true;
    fprintf(stderr, "DONE!\n");
}

void Model::drawProjection(GLuint program) const
{
    if (!m_projected)
        return;

    glBindBuffer(GL_ARRAY_BUFFER, m_projectionPositionVBO);
    setAttribute(program, "vertexPosition", 3, m_projectionPositionVBO);

    if (m_colored)
    {
        setAttribute(program, "vertexColor", 3, m_colorVBO);
    }

    if (m_textured)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_projectionTexture);
        //glBindTexture(GL_TEXTURE_2D, m_texture);
        glUniform1i(glGetUniformLocation(program, "textureSampler"), 0);
        setAttribute(program, "vertexTexture", 2, m_projectionTextureVBO);
        //setAttribute(program, "vertexTexture", 2, m_textureVBO);

    }
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
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
    std::vector<glm::vec3> p;
    p.push_back(center + glm::vec3(size, -size, -size));
    p.push_back(center + glm::vec3(size, -size, size));
    p.push_back(center + glm::vec3(-size, -size, size));
    p.push_back(center + glm::vec3(-size, -size, -size));
    p.push_back(center + glm::vec3(size, size, -size));
    p.push_back(center + glm::vec3(size, size, size));
    p.push_back(center + glm::vec3(-size, size, size));
    p.push_back(center + glm::vec3(-size, size, -size));

    std::vector<glm::vec3> ret;
    ret.push_back(p[4]); ret.push_back(p[0]); ret.push_back(p[3]);
    ret.push_back(p[4]); ret.push_back(p[3]); ret.push_back(p[7]);
    ret.push_back(p[2]); ret.push_back(p[6]); ret.push_back(p[7]);
    ret.push_back(p[2]); ret.push_back(p[7]); ret.push_back(p[3]);
    ret.push_back(p[1]); ret.push_back(p[5]); ret.push_back(p[2]);
    ret.push_back(p[5]); ret.push_back(p[6]); ret.push_back(p[2]);
    ret.push_back(p[0]); ret.push_back(p[4]); ret.push_back(p[1]);
    ret.push_back(p[4]); ret.push_back(p[5]); ret.push_back(p[1]);
    ret.push_back(p[4]); ret.push_back(p[7]); ret.push_back(p[5]);
    ret.push_back(p[7]); ret.push_back(p[6]); ret.push_back(p[5]);
    ret.push_back(p[0]); ret.push_back(p[1]); ret.push_back(p[2]);
    ret.push_back(p[0]); ret.push_back(p[2]); ret.push_back(p[3]);
    return ret;
}

Model::Marker::Marker(glm::vec3 position)
{
    std::vector<glm::vec3> positionBuffer = cube(position, 0.002f);
    std::vector<glm::vec3> m_colorVector;
    glm::vec3 color(1.0f, 1.0f, 1.0f);
    m_numVertices = positionBuffer.size();
    for (unsigned long i = 0; i < m_numVertices; i++)
        m_colorVector.push_back(color);
    glGenBuffers(1, &m_positionVBO);
    glGenBuffers(1, &m_colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 positionBuffer.size() * sizeof(glm::vec3),
                 &positionBuffer[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_colorVector.size() * sizeof(glm::vec3),
                 &m_colorVector[0],
                 GL_STATIC_DRAW);
}


