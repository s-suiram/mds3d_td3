#version 330 core

in vec3 vtx_position;
in vec4 vtx_color;

out vec4 col;

uniform float zoom = 1;
uniform vec2 translation = vec2(0);
uniform bool sideView = false;

void main() {
    col = vtx_color;
    float r = sideView ? 3.1415926535897932384 / 2 : 0;
    vec3 tmp = vtx_position.xyz * zoom;
    tmp.z *= -1;

    vec3 tmp2 = tmp;
    tmp.x = cos(r) * tmp2.x + sin(r) * tmp2.z;
    tmp.z = -sin(r) * tmp2.x + cos(r) * tmp2.z;

    gl_Position = vec4(tmp + vec3(translation, 0), 1.);
}
