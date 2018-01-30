#version 150 core

in vec3 position;
in vec3 normal;

out vec3 frag_pos;
out vec3 vnormal;
out vec2 uv;
out float height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float size;

void main()
{

    gl_Position = projection * view * model * vec4(position, 1.0);

    frag_pos = vec3(model * vec4(position, 1.0));

    vnormal = normal;

    vec2 xz_position = vec2(position.x, position.z);
    uv = (xz_position / size) + vec2(0.5);

    height = position.y;
}
