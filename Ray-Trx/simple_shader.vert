#version 450

layout(location = 0) in vec2 position;


void main() {
	gl_Position = vec4(position, 0.0, 1.0); // 4D vector, y coords sign flipped
    // third vec layer index, 4th vec divisor
}