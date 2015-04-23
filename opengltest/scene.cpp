#include "scene.hpp"


Scene::Scene(Camera *camera, GLuint program)
{
    m_camera = camera;
    m_window = camera->window();
    m_program = program;
    m_selectedModel = (Model*) 0;
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


void Scene::update()
{
    static bool mouseDown = false;
    static bool mDown = false;
    
    if (!mouseDown && glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        mouseDown = true;
        GLdouble xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        ypos = WINDOW_HEIGHT - ypos; // flip to make (0,0) the bottom left
        GLfloat zpos;
        glReadPixels((int) xpos,
                     (int) ypos,
                     1,
                     1,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     &zpos);
        glm::vec4 v(0.0, 0.0, WINDOW_WIDTH, WINDOW_HEIGHT);
        
        glm::vec3 world = glm::unProject(glm::vec3(xpos, ypos, zpos),
                                         view() * m_models[0]->model(),
                                         projection(),
                                         v);
        
        m_models[0]->setMarker(world);
        
        fprintf(stderr, "(%f, %f, %f)\n", world[0], world[1], world[2]);
    }
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
        mouseDown = false;
    
    if (!mDown && glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
    {
        mDown = true;
        m_models[0]->undoMarker();
    }
    else if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_RELEASE)
        mDown = false;
    
    
    
}

void Scene::draw()
{
    int error = glGetError();
    if (error)
        fprintf(stderr, "error: %x\n", error);
    
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
        model->setAttribute(m_program, "vertexPosition", model->positionVBO());
        model->setAttribute(m_program, "vertexColor", model->colorVBO());
        glDrawArrays(GL_TRIANGLES, 0, (int) model->numVertices());
        model->drawMarkers(m_program);
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