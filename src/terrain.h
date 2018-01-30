#ifndef TERRAIN_TERRAIN_H
#define TERRAIN_TERRAIN_H

#include <GLFW/glfw3.h>
#include <Opengl/gl3.h>
#include <helper.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/vec3.hpp>
#include "perlin.h"

using namespace glm;

static const unsigned int NbCubeVertices = 36;
static const vec3 CubeVertices[] = {
        vec3(-0.5, -0.5, -0.5), vec3(-0.5, 0.5, -0.5),  vec3(0.5, -0.5, -0.5), vec3(-0.5, 0.5, -0.5),
        vec3(0.5, -0.5, -0.5),  vec3(0.5, 0.5, -0.5),   vec3(0.5, 0.5, 0.5),   vec3(0.5, -0.5, 0.5),
        vec3(0.5, 0.5, -0.5),   vec3(0.5, -0.5, 0.5),   vec3(0.5, 0.5, -0.5),  vec3(0.5, -0.5, -0.5),
        vec3(0.5, 0.5, 0.5),    vec3(-0.5, 0.5, 0.5),   vec3(0.5, -0.5, 0.5),  vec3(-0.5, 0.5, 0.5),
        vec3(0.5, -0.5, 0.5),   vec3(-0.5, -0.5, 0.5),  vec3(-0.5, -0.5, 0.5), vec3(-0.5, -0.5, -0.5),
        vec3(0.5, -0.5, 0.5),   vec3(-0.5, -0.5, -0.5), vec3(0.5, -0.5, 0.5),  vec3(0.5, -0.5, -0.5),
        vec3(-0.5, 0.5, -0.5),  vec3(-0.5, -0.5, -0.5), vec3(-0.5, 0.5, 0.5),  vec3(-0.5, -0.5, -0.5),
        vec3(-0.5, 0.5, 0.5),   vec3(-0.5, -0.5, 0.5),  vec3(0.5, 0.5, -0.5),  vec3(-0.5, 0.5, -0.5),
        vec3(0.5, 0.5, 0.5),    vec3(-0.5, 0.5, -0.5),  vec3(0.5, 0.5, 0.5),   vec3(-0.5, 0.5, 0.5)};

static const unsigned int NbCubeUVs = 36;
static const vec2 CubeUVs[] = {
        vec2(0.333, 0.75), vec2(0.666, 0.75), vec2(0.333, 0.5),  vec2(0.666, 0.75), vec2(0.333, 0.5),  vec2(0.666, 0.5),
        vec2(0.666, 0.25), vec2(0.333, 0.25), vec2(0.666, 0.5),  vec2(0.333, 0.25), vec2(0.666, 0.5),  vec2(0.333, 0.5),
        vec2(0.666, 0.25), vec2(0.666, 0.0),  vec2(0.333, 0.25), vec2(0.666, 0.0),  vec2(0.333, 0.25), vec2(0.333, 0.0),
        vec2(0.0, 0.75),   vec2(0.333, 0.75), vec2(0.0, 0.5),    vec2(0.333, 0.75), vec2(0.0, 0.5),    vec2(0.333, 0.5),
        vec2(0.666, 0.75), vec2(0.333, 0.75), vec2(0.666, 1.0),  vec2(0.333, 0.75), vec2(0.666, 1.0),  vec2(0.333, 1.0),
        vec2(0.666, 0.5),  vec2(0.666, 0.75), vec2(1.0, 0.5),    vec2(0.666, 0.75), vec2(1.0, 0.5),    vec2(1.0, 0.75)};


struct Light {
    glm::vec3 light_pos = glm::vec3(0, 400.0, -400.0);

    // pass light properties to the shader
    void Setup(GLuint program_id) {
        glUseProgram(program_id);

        // given in camera space
        glUniform3fv(glGetUniformLocation(program_id, "light_pos"), 1, glm::value_ptr(light_pos));
    }
};

class Terrain : public Light{
public:
    Perlin perlin;
    Perlin2 perlin2;

    void Init(){
        program_id = load_shaders("../src/terrain_vshader.glsl", "../src/terrain_fshader.glsl");

        if (!program_id) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id);

        // VAO
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);


        float amp = size/10.0f;
        perlin.setAmplitude(amp);

        generateGrid();

        glUniform1f(glGetUniformLocation(program_id, "size"), size);
        glUniform1f(glGetUniformLocation(program_id, "AMPLITUDE"), amp);

        model_id = glGetUniformLocation(program_id, "model");
        view_id = glGetUniformLocation(program_id, "view");
        projection_id = glGetUniformLocation(program_id, "projection");

        // Load Texture
        loadTexture("../texture/grass_texture.tga", grass_texture_id, "grass_texture", 0);
        loadTexture("../texture/rock_texture.tga", rock_texture_id, "rock_texture", 1);
        loadTexture("../texture/sand_texture.tga", sand_texture_id, "sand_texture", 2);
        loadTexture("../texture/snow_texture.tga", snow_texture_id, "snow_texture", 3);
        loadTexture("../texture/water_texture.tga", water_texture_id, "water_texture", 4);

        // Avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {

        // Use program
        glUseProgram(program_id);

        // Bind your VAO
        glBindVertexArray(vao_id);

        Light::Setup(program_id);

        // Clear the buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // enable depth test.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniformMatrix4fv(model_id, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_id, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, glm::value_ptr(projection));

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grass_texture_id);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rock_texture_id);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, sand_texture_id);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, snow_texture_id);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, water_texture_id);

        glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }

    void CleanUp(){
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vbo_position_id);
        glDeleteBuffers(1, &vbo_index_id);
        glDeleteVertexArrays(1, &vao_id);
        glDeleteProgram(program_id);
    }

    void Update() {
        cout << "Updating terrain..." << endl;
        cout << "Generate new terrain..." << endl;
        // generate grid
        num_indices = num_grid * num_grid * 6;

        vector<GLfloat> vertices;
        vector<GLfloat> normals;
        displacement = size / num_grid;
        half = size / 2.0f;

        for (int i = 0; i <= num_grid; i++) {
            for (int j = 0; j <= num_grid; j++) {
                float posx = -half + displacement * float(i);
                float posz = -half + displacement * float(j);

                float height = perlin.getHeight(i, j);
                float posy = height;

                vertices.push_back(posx);
                vertices.push_back(posy);
                vertices.push_back(posz);

                glm::vec3 normal = calculateNormal(i, j);
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);
            }
        }

        // Create triangles
        GLuint indices[num_indices];
        for (int ti = 0, vi = 0, y = 0; y < num_grid; y++, vi++) {
            for (int x = 0; x < num_grid; x++, ti += 6, vi++) {
                indices[ti] = vi;
                indices[ti + 3] = indices[ti + 2] = vi + 1;
                indices[ti + 4] = indices[ti + 1] = vi + num_grid + 1;
                indices[ti + 5] = vi + num_grid + 2;
            }
        }

        cout << "Upload terrain data..." << endl;
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_id);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);
        cout << "Finished updating." << endl;
    }

    void setResolution(int type){
        if (type == 1)
            num_grid = num_grid * 2;
        if (type == 2)
            num_grid = num_grid / 2;
    }

private:
    GLuint vao_id;
    GLuint vbo_position_id;
    GLuint vbo_normal_id;
    GLuint vbo_index_id;

    GLuint program_id;
    GLuint num_indices;
    GLuint model_id;
    GLuint view_id;
    GLuint projection_id;
    GLuint grass_texture_id;
    GLuint rock_texture_id;
    GLuint sand_texture_id;
    GLuint snow_texture_id;
    GLuint water_texture_id;

    float size = 1000.0;
    int num_grid = 512;
    float displacement;
    float half;

    void generateGrid() {
        // generate grid
        num_indices = num_grid * num_grid * 6;

        vector<GLfloat> vertices;
        vector<GLfloat> normals;
        displacement = size / num_grid;
        half = size / 2.0f;

        for (int i = 0; i <= num_grid; i++) {
            for (int j = 0; j <= num_grid; j++) {
                float posx = -half + displacement * float(i);
                float posz = -half + displacement * float(j);

                float height = perlin.getHeight(i, j);
                float posy = height;

                vertices.push_back(posx);
                vertices.push_back(posy);
                vertices.push_back(posz);

                glm::vec3 normal = calculateNormal(i, j);
                normals.push_back(normal.x);
                normals.push_back(normal.y);
                normals.push_back(normal.z);
            }
        }

        // Create triangles
        GLuint indices[num_indices];
        for (int ti = 0, vi = 0, y = 0; y < num_grid; y++, vi++) {
            for (int x = 0; x < num_grid; x++, ti += 6, vi++) {
                indices[ti] = vi;
                indices[ti + 3] = indices[ti + 2] = vi + 1;
                indices[ti + 4] = indices[ti + 1] = vi + num_grid + 1;
                indices[ti + 5] = vi + num_grid + 2;
            }
        }


        // VBO
        glGenBuffers(1, &vbo_position_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_id);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);

        // Link attibute
        GLuint loc_position = glGetAttribLocation(program_id, "position");
        glEnableVertexAttribArray(loc_position);
        glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // VBO
        glGenBuffers(1, &vbo_normal_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_id);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_DYNAMIC_DRAW);

        // Link attribute
        GLuint loc_normal = glGetAttribLocation(program_id, "normal");
        glEnableVertexAttribArray(loc_normal);
        glVertexAttribPointer(loc_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);


        glGenBuffers(1, &vbo_index_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);
    }

    glm::vec3 calculateNormal(int x, int z){
        float heightL = perlin.getHeight(x - 1, z);
        float heightR = perlin.getHeight(x + 1, z);
        float heightD = perlin.getHeight(x, z - 1);
        float heightU = perlin.getHeight(x, z + 1);

//        float heightL = perlin2.getHeight(x - 1, z, num_grid);
//        float heightR = perlin2.getHeight(x + 1, z, num_grid);
//        float heightD = perlin2.getHeight(x, z - 1, num_grid);
//        float heightU = perlin2.getHeight(x, z + 1, num_grid);

        glm::vec3 normal = glm::vec3(heightL - heightR, 2.0f, heightD - heightU);
        return glm::normalize(normal);
    }

    void loadTexture(const char *texturefile, GLuint &texture_id, const char *texture_name, int val) {
        int width, height, nrChannels;

        stbi_set_flip_vertically_on_load(1);

        unsigned char *image = stbi_load(texturefile, &width, &height, &nrChannels, 0);

        if (!image) {
            std::cout << "Failed to load texture" << std::endl;
            return;
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (nrChannels== 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (nrChannels== 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        glGenerateMipmap(GL_TEXTURE_2D);

        glUseProgram(program_id);
        GLuint loc_texture = glGetUniformLocation(program_id, texture_name);
        glUniform1i(loc_texture, val);

        stbi_image_free(image);
    }
};

class Sky{
private:
    GLuint program_id;
    GLuint vao_id;
    GLuint vbo_position_id;
    GLuint vbo_texcoord_id;
    GLuint texture_id;
    GLuint model_id;
    GLuint view_id;
    GLuint projection_id;

public:
    void Init() {
        // compile the shaders.
        program_id = load_shaders("../src/sky_vshader.glsl", "../src/sky_fshader.glsl");
        if (!program_id) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id);

        // vertex one vertex array
        glGenVertexArrays(1, &vao_id);
        glBindVertexArray(vao_id);

        // vertex coordinates
        glGenBuffers(1, &vbo_position_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_id);
        glBufferData(GL_ARRAY_BUFFER, NbCubeVertices * sizeof(vec3), &CubeVertices[0], GL_STATIC_DRAW);

        // attribute
        GLuint vertex_point_id = (GLuint)glGetAttribLocation(program_id, "position");
        glEnableVertexAttribArray(vertex_point_id);
        glVertexAttribPointer(vertex_point_id, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // texture coordinates
        glGenBuffers(1, &vbo_texcoord_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord_id);
        glBufferData(GL_ARRAY_BUFFER, NbCubeUVs * sizeof(vec2), &CubeUVs[0], GL_STATIC_DRAW);

        // attribute
        GLuint vertex_texture_coord_id = (GLuint)glGetAttribLocation(program_id, "texcoord");
        glEnableVertexAttribArray(vertex_texture_coord_id);
        glVertexAttribPointer(vertex_texture_coord_id, 2, GL_FLOAT, GL_FALSE, 0, 0);


        // load texture
        int width, height, nrChannels;

        string texture_filename = "../texture/sky_texture.tga";
        stbi_set_flip_vertically_on_load(1);

        unsigned char *image = stbi_load(texture_filename.c_str(), &width, &height, &nrChannels, 0);

        if (image == nullptr) {
            cout << "Failed to load texture" << endl;
            return;
        }

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        GLuint tex_id = (GLuint)glGetUniformLocation(program_id, "tex");
        glUniform1i(tex_id, 5);

        stbi_image_free(image);

        model_id = glGetUniformLocation(program_id, "model");
        view_id = glGetUniformLocation(program_id, "view");
        projection_id = glGetUniformLocation(program_id, "projection");
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vbo_position_id);
        glDeleteBuffers(1, &vbo_texcoord_id);
        glDeleteProgram(program_id);
        glDeleteVertexArrays(1, &vao_id);
        glDeleteTextures(1, &texture_id);
    }

    void Draw(const mat4 &view, const mat4 &projection, const mat4 &model) {
        glUseProgram(program_id);
        glBindVertexArray(vao_id);

        // bind textures
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        mat4 new_model = model * scale(model, vec3(2000.0f));

        glUniformMatrix4fv(model_id, 1, GL_FALSE, value_ptr(new_model));
        glUniformMatrix4fv(view_id, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projection_id, 1, GL_FALSE, value_ptr(projection));

        // draw
        glDrawArrays(GL_TRIANGLES, 0, NbCubeVertices);

        glBindVertexArray(0);
        glUseProgram(0);
    }
};

#endif //TERRAIN_TERRAIN_H
