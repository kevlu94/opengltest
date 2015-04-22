//
//  main.cpp
//  opengltest
//
//  Created by Kevin on 4/5/15.
//  Copyright (c) 2015 Kevin. All rights reserved.
//

#include "scene.hpp"
#include "common/shader.hpp"
#include "globals.hpp"
#include "model.hpp"

using namespace glm;

GLFWwindow* window;

int initializeGL()
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Picking Test", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Dark blue background
    glClearColor(0.0f, 0.8f, 0.9f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable depth buffer for writing
    glDepthMask(GL_TRUE);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_ALWAYS);
    // Set range
    glDepthRange(0.0f, 1.0f);
    
    return 0;
}


int main(int argc, const char *argv[])
{
    // prepare GL environment
    if (initializeGL() == -1)
        return -1;
    
    // prepare vertex/fragment shader programs
    GLuint program;
    program = LoadShaders( "shaders/vertexshader", "shaders/fragmentshader" );
    glUseProgram(program);
    
    // initialize camera, scene, and objects to draw
    Camera camera(window, vec3(0,0,4), 0.0f, 0.0f);
    Scene scene(&camera, program);
    Model model1(argv[1], glm::vec3(0.0f, 0.0f, 0.0f));
    //Model model2(argv[2], glm::vec3(1.0f, 0.0f, 0.0f));
    
    scene.addModel(&model1);
    //scene.addModel(&model2);
    
    
    // create vertex array
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // specify the model characteristics for the shader program
    scene.prepareAllModels(program);

    fprintf(stderr, "error code before loop: %x\n", glGetError());
    
    // continuously draw the scene we've created
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        camera.update();
        scene.handleMouse();
        scene.draw();
    }
    
    // Cleanup VBO and shader
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}