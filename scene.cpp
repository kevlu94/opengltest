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

Scene::~Scene()
{
    while (!m_models.empty())
    {
        delete m_models.back();
        m_models.pop_back();
    }
}

void Scene::addModel(Model *model)
{
    m_models.push_back(model);
    //selectModel(m_models.size() - 1); // select most recently added model
    selectModel(0); // select first model
}

void Scene::addModel(const char *path, glm::vec3 position, const char *texturePath)
{
    m_models.push_back(new Model(path, position, texturePath));
    //selectModel(m_models.size() - 1); // select most recently added model
    selectModel(0); // select first model
}

void Scene::moveModel(Model *model)
{
    GLfloat angle = 0.05f;
    
    // pitch negative
    if (glfwGetKey( m_window, GLFW_KEY_W ) == GLFW_PRESS){
        model->pitchBy(-angle);
    }
    // pitch positive
    if (glfwGetKey( m_window, GLFW_KEY_S ) == GLFW_PRESS){
        model->pitchBy(angle);
    }
    // yaw negative
    if (glfwGetKey( m_window, GLFW_KEY_A ) == GLFW_PRESS){
        model->yawBy(-angle);
    }
    // yaw positive
    if (glfwGetKey( m_window, GLFW_KEY_D ) == GLFW_PRESS){
        model->yawBy(angle);
    }
    // roll negative
    if (glfwGetKey( m_window, GLFW_KEY_E ) == GLFW_PRESS){
        model->rollBy(-angle);
    }
    // roll positive
    if (glfwGetKey( m_window, GLFW_KEY_Q ) == GLFW_PRESS){
        model->rollBy(angle);
    }
}


void Scene::update()
{
    if (!m_selectedModel)
        return;
    
    static bool mouseDown = false;
    static bool mDown = false;
    static bool vDown = false;
    static bool bDown = false;
    static bool pDown = false;
    
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
                                         view() * m_selectedModel->model(),
                                         projection(),
                                         v);
        
        m_selectedModel->setMarker(world);
        
        printf("%f %f %f\n", world[0] / SCALE_FACE, world[1] / SCALE_FACE, world[2] / SCALE_FACE);
    }
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
        mouseDown = false;
    
    if (!mDown && glfwGetKey(m_window, GLFW_KEY_M) == GLFW_PRESS)
    {
        mDown = true;
        m_selectedModel->undoMarker();
    }
    else if (glfwGetKey(m_window, GLFW_KEY_M) == GLFW_RELEASE)
        mDown = false;

    if (!vDown && glfwGetKey(m_window, GLFW_KEY_V) == GLFW_PRESS)
    {
        vDown = true;
        m_selectedModel->toggleHide();
    }
    else if (glfwGetKey(m_window, GLFW_KEY_V) == GLFW_RELEASE)
        vDown = false;

    if (!bDown && glfwGetKey(m_window, GLFW_KEY_B) == GLFW_PRESS)
    {
        bDown = true;
        if (m_models.size() > 1)
            m_models[1]->toggleHide();
    }
    else if (glfwGetKey(m_window, GLFW_KEY_B) == GLFW_RELEASE)
        bDown = false;

    if (!pDown && glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)
    {
        pDown = true;
        if (m_models.size() > 1)
            m_models[0]->projectOnto(m_models[1]);
    }
    else if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_RELEASE)
        pDown = false;






    
    
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
        model->setAttribute(m_program, "vertexPosition", 3, model->positionVBO());
        
        if (model->colored())
        {
            model->setAttribute(m_program, "vertexColor", 3, model->colorVBO());
        }

        if (model->textured())
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model->texture());
            glUniform1i(glGetUniformLocation(m_program, "textureSampler"), 0);
            model->setAttribute(m_program, "vertexTexture", 2, model->textureVBO());
        }

        if (!model->hidden())
            glDrawArrays(GL_TRIANGLES, 0, (int) model->numVertices());
        model->drawProjection(m_program);
        model->drawMarkers(m_program);
    }
    
    glDisableVertexAttribArray(glGetAttribLocation(m_program, "vertexPosition"));
    glDisableVertexAttribArray(glGetAttribLocation(m_program, "vertexColor"));
    glDisableVertexAttribArray(glGetAttribLocation(m_program, "vertexTexture"));
    
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