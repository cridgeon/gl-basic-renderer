#version 130
in vec2 position;
uniform vec2 resolution;
uniform vec4 rect;
out vec2 pixelCoord;
out vec2 fragCoord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    pixelCoord = ((position + 1.0) / 2.0) * resolution;
    fragCoord = (pixelCoord - rect.xy) / rect.zw;
}