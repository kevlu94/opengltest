#ifndef SCENE_HPP
#define SCENE_HPP

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "model.hpp"


class Scene
{
public:
    Scene(Camera *camera, GLuint program);
    ~Scene() {}
    
    void addModel(Model *model);
    void moveModel(Model *model);
    void draw();
    
    // accessor functions
    GLvoid *vertexBuffer() { return &m_vertex_buffer[0]; }
    GLvoid *colorBuffer() { return &m_color_buffer[0]; }
    GLvoid *uvBuffer() { return &m_uv_buffer[0]; }
    GLvoid *normalBuffer() { return &m_normal_buffer[0]; }
    unsigned long vertexBufferBytes() const;
    unsigned long colorBufferBytes() const;
    unsigned long uvBufferBytes() const;
    unsigned long normalBufferBytes() const;
    unsigned long numPoints() const { return vertexBufferBytes() / sizeof(glm::vec3); }
    glm::mat4 view() const;
    glm::mat4 projection() const { return m_projection_matrix; }
    glm::mat4 MVP(Model *model) const { return projection() * view() * model->model(); }

    // mutator functions
    void addVertices(const std::vector<glm::vec3> &newData) { appendVecToVec(m_vertex_buffer, newData); }
    void addVertices(const std::vector<GLfloat> &newData) { appendVecToVecVec(m_vertex_buffer, newData); }
    void addColors(std::vector<glm::vec3> &newData) { appendVecToVec(m_color_buffer, newData); }
    void addColors(std::vector<GLfloat> &newData) { appendVecToVecVec(m_color_buffer, newData); }
    void setModel(glm::mat4 matrix) { m_model_matrix = matrix; }
    void setProjection(glm::mat4 matrix) { m_projection_matrix = matrix; }
    
    
    
private:
    
    // private functions
    template<typename Type>
    void appendVecToVec(std::vector<Type> &left, const std::vector<Type> &right);
    template<typename Vector, typename Type>
    void appendVecToVecVec(std::vector<Vector> &left, const std::vector<Type> &right);
    
    // variables
    GLuint m_program;
    GLFWwindow* m_window;
    std::vector<glm::vec3> m_vertex_buffer;
    std::vector<glm::vec3> m_color_buffer;
    std::vector<glm::vec2> m_uv_buffer;
    std::vector<glm::vec3> m_normal_buffer;
    glm::mat4 m_model_matrix;
    glm::mat4 m_projection_matrix;
    Camera *m_camera;
    std::vector<Model*> m_models;
};

// templated functions

template<typename Type>
void Scene::appendVecToVec(std::vector<Type> &left, const std::vector<Type> &right)
{
    left.reserve(left.size() + right.size());
    left.insert(left.end(), right.begin(), right.end());
}

template<typename Vector, typename Type>
void Scene::appendVecToVecVec(std::vector<Vector> &left, const std::vector<Type> &right)
{
    // determine the dimensions of Type
    unsigned int dim = sizeof(Vector) / sizeof(Type);
    
    int i, j;
    Vector vec;
    for (i = 0; i < right.size(); i+= dim)
    {
        for (j = 0; j < dim; j++)
            vec[j] = right[i + j];
        left.push_back(vec);
    }
}




#endif