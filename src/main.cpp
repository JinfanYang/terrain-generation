
#include "helper.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <iostream>
#include "stb_image.h"
#include <terrain.h>
#include <camera.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera;

enum Mode{MOVE, STOP, TUNE};

enum Parameter{OCTAVES, ROUGHNESS, RESOLUTION, NONE};

Mode mode = STOP;
Parameter parameter = NONE;

Terrain terrain;
Sky sky;

void tune(int i){
    if (parameter == OCTAVES) {
        terrain.perlin.setOctaves(i);
        terrain.Update();
    }

    if (parameter == ROUGHNESS){
        terrain.perlin.setRoughness(i/10.0f);
        terrain.Update();

    }

    if (parameter == RESOLUTION) {
        terrain.setResolution(i);
        terrain.Update();
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch(key){
        case GLFW_KEY_EQUAL: {
            if (mode == STOP)
                camera.calculateZoom(1); // zoom in
            if (mode == TUNE)
                terrain.setResolution(1);
            parameter = NONE;
            break;
        }

        case GLFW_KEY_MINUS: {
            if (mode == STOP)
                camera.calculateZoom(2); // zoom out
            if (mode == TUNE)
                terrain.setResolution(2);
            parameter = NONE;
            break;
        }

        case GLFW_KEY_UP: {
            if (mode == MOVE) {
                camera.moveForward();
            }
            if (mode == STOP) {
                camera.calculatePitch(1); // look up
            }
            parameter = NONE;
            break;
        }

        case GLFW_KEY_DOWN: {
            if (mode == MOVE) {
                camera.moveBackward();
            }
            if (mode == STOP) {
                camera.calculatePitch(2); // look down
            }
            parameter = NONE;
            break;
        }

        case GLFW_KEY_LEFT: {
            if (mode == MOVE) {
                camera.moveRight();
            }
            if (mode == STOP) {
                camera.calculateAround(1);
            }
            parameter = NONE;
            break;
        }

        case GLFW_KEY_RIGHT: {
            if (mode == MOVE) {
                camera.moveLeft();
            }
            if (mode == STOP) {
                camera.calculateAround(2);
            }

            parameter = NONE;
            break;
        }

        case GLFW_KEY_M: {
            mode = MOVE;
            parameter = NONE;
            break;
        }

        case GLFW_KEY_S: {
            mode = STOP;
            parameter = NONE;
            break;
        }

        case GLFW_KEY_T: {
            mode = TUNE;
            parameter = NONE;
            break;
        }

        case GLFW_KEY_O: {
            parameter = OCTAVES;
            break;
        }

        case GLFW_KEY_R: {
            parameter = ROUGHNESS;
            break;

        }

        case GLFW_KEY_1:{
            tune(1);
            break;
        }

        case GLFW_KEY_2:{
            tune(2);
            break;

        }
        case GLFW_KEY_3:{
            tune(3);
            break;
        }

        case GLFW_KEY_4:{
            tune(4);
            break;
        }

        case GLFW_KEY_5:{
            tune(5);
            break;
        }

        case GLFW_KEY_6:{
            tune(6);
            break;
        }

        case GLFW_KEY_7:{
            tune(7);
            break;
        }

        case GLFW_KEY_8:{
            tune(8);
            break;
        }

        case GLFW_KEY_9:{
            tune(9);
            break;
        }
    }
}


int main(int argc, char *argv[])
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "TERRAIN", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    glEnable(GL_DEPTH_TEST);

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));


    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    auto t_start = std::chrono::high_resolution_clock::now();

    terrain.Init();
    sky.Init();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect_ratio = float(height)/float(width);

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = camera.getViewMatrix();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 10000.0f);

        terrain.Draw(model, view, projection);

        sky.Draw(view, projection, model);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    terrain.CleanUp();
    sky.Cleanup();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
