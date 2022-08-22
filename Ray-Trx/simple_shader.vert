#version 450

vec2 positions[3] = vec2[] (
    vec2(0.0, -0.5),    // Coords of triangle, hardcoded
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
void main() {
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0); // 4D vector, y coords sign flipped
    // third vec layer index, 4th vec divisor
}