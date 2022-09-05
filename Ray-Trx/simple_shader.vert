#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 colour;

// layout(location = 0) out vec3 fragColour;

layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 colour;
} push;

// no relation b/w in and out, therefore position can be the same

void main() {
	gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0); // 4D vector, y coords sign flipped
    // third vec layer index, 4th vec divisor
    // fragColour = colour;
}