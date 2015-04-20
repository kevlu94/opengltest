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

GLuint MVPID;
GLuint VERTEX_BUFFER;
GLuint COLOR_BUFFER;


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
    window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Tutorial 03 - Matrices", NULL, NULL);
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
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    return 0;
}






int main( void )
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
    Model faceModel;
    faceModel.loadColorOBJ("ALEX/alex.obj");
    scene.addModel(&faceModel);
    
    // create vertex array
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // get the location of the uniform "MVP" in the vertex shader
    MVPID = glGetUniformLocation(program, "MVP");

    
    
    // set up vertex buffer and pass in vertex data
    glGenBuffers(1,                             // Generate 1 buffer
                 &VERTEX_BUFFER);               // with the name VERTEX_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER,               // Binding target: GL_ARRAY_BUFFER
                 VERTEX_BUFFER);                // Bind the generated buffer (VERTEX_BUFFER)
    glBufferData(GL_ARRAY_BUFFER,               // Destination buffer: GL_ARRAY_BUFFER
                 scene.vertexBufferBytes(),     // Amount (bytes) of data to send to buffer
                 scene.vertexBuffer(),          // Source buffer containing the data
                 GL_STATIC_DRAW);               // Usage of the data
    GLint ploc = glGetAttribLocation(program, "vertexPosition");
    glEnableVertexAttribArray(ploc);               // Enable vertex attribute array at index 0
    glVertexAttribPointer(ploc,                    // For the array at index 0, let GL know that...
                          3,                    // each vertex has three numbers
                          GL_FLOAT,             // of type GL_FLOAT
                          GL_FALSE,             // that should not be normalized,
                          0,                    // and these numbers are consecutive in the buffer
                          (void*)0);            // starting from the beginning of the buffer

    
    // set up color buffer and pass in color data
    glGenBuffers(1,                             // Generate 1 buffer
                 &COLOR_BUFFER);                // with the name COLOR_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER,               // Binding target: GL_ARRAY_BUFFER
                 COLOR_BUFFER);                 // Bind the generated buffer (COLOR_BUFFER)
    glBufferData(GL_ARRAY_BUFFER,               // Destination buffer: GL_ARRAY_BUFFER
                 scene.colorBufferBytes(),      // Amount (bytes) of data to send to buffer
                 scene.colorBuffer(),           // Source buffer containing the data
                 GL_STATIC_DRAW);               // Usage of the data
    GLint cloc = glGetAttribLocation(program, "vertexColor");
    glEnableVertexAttribArray(cloc);               // Enable vertex attribute array at index 1
    glVertexAttribPointer(cloc,                    // For the array at index 0, let GL know that...
                          3,                    // each vertex has three numbers
                          GL_FLOAT,             // of type GL_FLOAT
                          GL_FALSE,             // that should not be normalized,
                          0,                    // and these numbers are consecutive in the buffer
                          (void*)0);            // starting from the beginning of the buffer
    
    
    // continuously draw the scene we've created
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0)
    {
        camera.update();
        scene.draw();
    }
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &VERTEX_BUFFER);
    glDeleteBuffers(1, &COLOR_BUFFER);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}