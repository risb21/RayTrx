#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 colour;

layout(location = 0) out vec3 fragColour;
// no relation b/w in and out, therefore position can be the same

void main() {
	gl_Position = vec4(position, 0.0, 1.0); // 4D vector, y coords sign flipped
    // third vec layer index, 4th vec divisor
    fragColour = colour;
}