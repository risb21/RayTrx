#version 450

// layout (location = 0) in vec3 fragColour;

layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 colour;
} push;

layout (location = 0) out vec4 outColour;
void main() {
	outColour = vec4(push.colour, 1.0); // RGBA
}