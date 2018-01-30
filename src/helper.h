#ifndef TERRAIN_HELPER_H
#define TERRAIN_HELPER_H

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>

// we use GLM for linear algebra
#define GLM_FORCE_CXX11
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

// these namespaces assumed by default
using namespace std;

// on some OSs the exit flags are not defined
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

void _check_gl_error(const char *file, int line)
{
    GLenum err (glGetError());

    while(err!=GL_NO_ERROR)
    {
        std::string error;

        switch(err)
        {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }

        std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
        err = glGetError();
    }
}

#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

GLuint create_shader(GLint type, const char *shader_string)
{
    if (shader_string == NULL)
        return (GLint)0;

    GLint id = glCreateShader(type);
    glShaderSource(id, 1, &shader_string, NULL);
    glCompileShader(id);

    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        char buffer[512];
        if (type == GL_VERTEX_SHADER)
            cerr << "Vertex shader:" << endl;
        else if (type == GL_FRAGMENT_SHADER)
            cerr << "Fragment shader:" << endl;
        else if (type == GL_GEOMETRY_SHADER)
            cerr << "Geometry shader:" << endl;
        cerr << shader_string << endl << endl;
        glGetShaderInfoLog(id, 512, NULL, buffer);
        cerr << "Error: " << endl << buffer << endl;
        return (GLuint) 0;
    } else {
        if (type == GL_VERTEX_SHADER)
            cout << "Success: Compiled vertex shader" << endl;
        else if (type == GL_FRAGMENT_SHADER)
            cout << "Success: Compiled fragment shader" << endl;
        else if (type == GL_GEOMETRY_SHADER)
            cout << "Success: Compiled geometry shader" << endl;
    }

    check_gl_error();

    return id;
}


GLuint compile_shaders(const char *vshader, const char *fshader, const char *gshader=NULL) {

    // Create shaders
    cout << "Compiling shaders ===>" << endl;
    GLuint vertex_shader_id = create_shader(GL_VERTEX_SHADER, vshader);
    GLuint fragment_shader_id = create_shader(GL_FRAGMENT_SHADER, fshader);
    GLuint geometry_shader_id;
    if (gshader != NULL)
        geometry_shader_id = create_shader(GL_GEOMETRY_SHADER, gshader);

    if (gshader == NULL) {
        if (!vertex_shader_id || !fragment_shader_id)
            return (GLuint) 0;
    } else {
        if (!vertex_shader_id || !fragment_shader_id || !geometry_shader_id)
            return (GLuint) 0;
    }

    // Link the program
    cout << "Linking programs ===>" << endl;

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    if (gshader != NULL)
        glAttachShader(program_id, geometry_shader_id);

    glBindFragDataLocation(program_id, 0, "color");
    glLinkProgram(program_id);

    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);

    if (status != GL_TRUE)
    {
        char buffer[512];
        glGetProgramInfoLog(program_id, 512, NULL, buffer);
        cerr << "Linker error: " << endl << buffer << endl;
        program_id = 0;
    } else {
        cout << "Success: Linked program." << endl;
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    check_gl_error();

    return program_id;
}

string read_shader(const char* shader_file_path) {
    string shader_string;
    ifstream shader_file;

    // ensure ifstream objects can throw exceptions
    shader_file.exceptions(ifstream::failbit | ifstream::badbit);

    try
    {
        shader_file.open(shader_file_path);
        stringstream shader_stream;
        shader_stream << shader_file.rdbuf();
        shader_file.close();
        shader_string = shader_stream.str();
    }
    catch (ifstream::failure e)
    {
        cout << "ERROR: Shader file not succesfully read!" << endl;
        cout << e.what() << endl;
        return "";
    }

    return shader_string;
}

GLuint load_shaders(const char *vertex_file_path, const char *fragment_file_path)
{
    string vertex_shader_code = read_shader(vertex_file_path);
    string fragment_shader_code = read_shader(fragment_file_path);

    // compile them
    char const *vertex_source = vertex_shader_code.c_str();
    char const *fragment_source = fragment_shader_code.c_str();
    char const *geometry_source = NULL;

    int program_id = compile_shaders(vertex_source, fragment_source, geometry_source);
    return program_id;
}


#endif //TERRAIN_HELPER_H