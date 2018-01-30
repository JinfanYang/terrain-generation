#version 150 core

in vec3 vnormal;
in vec2 uv;
in float height;
in vec3 frag_pos;

out vec4 outColor;

uniform vec3 light_pos;
uniform float AMPLITUDE;

uniform sampler2D grass_texture;
uniform sampler2D rock_texture;
uniform sampler2D sand_texture;
uniform sampler2D snow_texture;
uniform sampler2D water_texture;

void main()
{
    vec2 tile_coords = uv * 100.0;

    vec4 grass = texture(grass_texture, tile_coords);
    vec4 rock = texture(rock_texture, tile_coords);
    vec4 sand = texture(sand_texture, tile_coords);
    vec4 snow = texture(snow_texture, tile_coords);
    vec4 water = texture(water_texture, tile_coords);

    vec3 normal = vnormal;
    vec3 texture_color;

    float ration = height / AMPLITUDE;
    if (ration >= 0.8) {
        float mix_ration = (ration - 0.8) / 0.2;
        texture_color = mix(rock.rgb, snow.rgb, mix_ration);
    }
    else if (ration >= 0.3) {
        float mix_ration = (ration - 0.3) / 0.5;
        texture_color = mix(grass.rgb, rock.rgb, mix_ration);
    }
    else if (ration >= 0.0) {
        float mix_ration = ration / 0.3;
        texture_color = mix(sand.rgb, grass.rgb, mix_ration);
    }
    else if (ration >= -1.0){
        texture_color = sand.rgb;
    }
    else {
        texture_color = water.rgb;
        normal = vec3(0.0, 1.0, 0.0);
    }

    vec4 brouillard = vec4(0.8, 0.8, 0.8, 1);
    vec3 color = mix(vec4(texture_color, 1.0), brouillard, 1 - 0.8).rgb;

    // ambient
    vec3 light_color = vec3(1.0, 1.0, 1.0);
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * light_color;

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light_pos - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 color_l = (ambient + diffuse) * color;

    outColor = vec4(color_l, 1.0);
}
