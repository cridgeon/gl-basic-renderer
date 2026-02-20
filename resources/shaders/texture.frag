#version 130

in vec2 pixelCoord;
in vec2 fragCoord;
out vec4 fragColor;

uniform sampler2D textureSampler;
uniform vec4 color; // Tint color (default white for no tint)
uniform vec4 subtexture; // x, y, w, h in texture coordinates (0-1)

void main() {
    // Calculate texture coordinates based on pixel position within the rect   
    // Map to subtexture region
    vec2 texCoord = subtexture.xy + fragCoord * subtexture.zw;
    
    vec4 texColor = texture(textureSampler, texCoord);
    fragColor = texColor * color;
}