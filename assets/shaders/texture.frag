#version 330 core

out vec4 outFragColor;

uniform sampler2D image;

in vec2 uv;

void main() {
   outFragColor = texture(image, uv);
}
