#include "scene.hpp"


Scene::Scene(Camera *camera, GLuint program)
{
    m_camera = camera;
    m_window = camera->window();
    m_program = program;
    // default projection matrices
    m_projection_matrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
}

void Scene::addModel(Model *model)
{
    addVertices(*(model->vertexBuffer()));
    addColors(*(model->colorBuffer()));
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

void Scene::draw()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    moveModel(m_models[0]);
    
    glUniformMatrix4fv(glGetUniformLocation(m_program, "MVP"), 1, GL_FALSE, &(MVP(m_models[0]))[0][0]);
    glDrawArrays(GL_POINTS, 0, (int) numPoints());
    
    // Swap buffers
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}


unsigned long Scene::vertexBufferBytes() const
{
    unsigned long length = m_vertex_buffer.size();
    return (length ? length * sizeof(m_vertex_buffer[0]) : 0);
}

unsigned long Scene::colorBufferBytes() const
{
    unsigned long length = m_color_buffer.size();
    return (length ? length * sizeof(m_color_buffer[0]) : 0);
}

unsigned long Scene::uvBufferBytes() const
{
    unsigned long length = m_uv_buffer.size();
    return (length ? length * sizeof(m_uv_buffer[0]) : 0);
}

unsigned long Scene::normalBufferBytes() const
{
    unsigned long length = m_normal_buffer.size();
    return (length ? length * sizeof(m_normal_buffer[0]) : 0);
}



glm::mat4 Scene::view() const
{
    return glm::lookAt(m_camera->position(),
                       m_camera->position() + m_camera->direction(),
                       glm::vec3(0,1,0));
}