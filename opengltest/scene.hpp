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
    void handleMouse();
    void draw();
    glm::mat4 MVP(Model *model) const { return projection() * view() * model->model(); }
    
    
    
    glm::mat4 view() const;
    glm::mat4 projection() const { return m_projection_matrix; }
    
private:
    
    // private functions
    template<typename Type>
    void appendVecToVec(std::vector<Type> &left, const std::vector<Type> &right);
    template<typename Vector, typename Type>
    void appendVecToVecVec(std::vector<Vector> &left, const std::vector<Type> &right);
    
    void moveModel(Model *model);


    // private variables
    GLuint m_program;
    GLFWwindow* m_window;
    glm::mat4 m_projection_matrix;
    Camera *m_camera;
    std::vector<Model*> m_models;
    Model *m_selectedModel;
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