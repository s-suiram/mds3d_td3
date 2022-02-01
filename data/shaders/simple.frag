#version 330 core

in vec4 col;
out vec4 out_color;

uniform bool white = false;

void main(void) {
    out_color = white ? vec4(1) : col;
}
