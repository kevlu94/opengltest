#include "scene.hpp"


Scene::Scene(Camera *camera, GLuint program)
{
    m_camera = camera;
    m_window = camera->window();
    m_program = program;
    // default projection matrix
    m_projection_matrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
}

void Scene::addModel(Model *model)
{
    m_models.push_back(model);
}

void Scene::moveModel(Model *model)
{
    // pitch positive
    if (glfwGetKey( m_window, GLFW_KEY_W ) == GLFW_PRESS){
        model->pitchBy(0.1f);
    }
    // pitch negative
    if (glfwGetKey( m_window, GLFW_KEY_S ) == GLFW_PRESS){
        model->pitchBy(-0.1f);
    }
    // yaw positive
    if (glfwGetKey( m_window, GLFW_KEY_D ) == GLFW_PRESS){
        model->yawBy(0.1f);
    }
    // yaw negative
    if (glfwGetKey( m_window, GLFW_KEY_A ) == GLFW_PRESS){
        model->yawBy(-0.1f);
    }
    // roll positive
    if (glfwGetKey( m_window, GLFW_KEY_E ) == GLFW_PRESS){
        model->rollBy(0.1f);
    }
    // roll negative
    if (glfwGetKey( m_window, GLFW_KEY_Q ) == GLFW_PRESS){
        model->rollBy(-0.1f);
    }
}

void Scene::prepareModel(Model *model, GLuint program)
{
    model->setAttribute(program, "vertexPosition", model->positionVBO());
    model->setAttribute(program, "vertexColor", model->colorVBO());
}

void Scene::prepareAllModels(GLuint program)
{
    unsigned long numModels = m_models.size();
    for (unsigned long i = 0; i < numModels; i++)
        prepareModel(m_models[i], program);
}

void Scene::draw()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Model *model;
    unsigned long numModels = m_models.size();
    for (unsigned long i = 0; i < numModels; i++)
    {
        model = m_models[i];
        moveModel(model);
        glBindBuffer(GL_ARRAY_BUFFER, model->positionVBO());
        glUniformMatrix4fv(glGetUniformLocation(m_program, "MVP"), 1, GL_FALSE, &(MVP(model))[0][0]);
        glDrawArrays(GL_POINTS, 0, (int) model->numVertices());
    }

    // Swap buffers
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}


glm::mat4 Scene::view() const
{
    return glm::lookAt(m_camera->position(),
                       m_camera->position() + m_camera->direction(),
                       glm::vec3(0,1,0));
}