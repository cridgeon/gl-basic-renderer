#version 130

in vec2 pixelCoord;
out vec4 fragColor;

uniform sampler2D textureSampler;
uniform vec4 color; // Tint color (default white for no tint)
uniform vec4 rect; // x, y, w, h in screen coordinates
uniform vec4 subtexture; // x, y, w, h in texture coordinates (0-1)

void main() {
    // Calculate texture coordinates based on pixel position within the rect
    vec2 texCoord;
    texCoord.x = (pixelCoord.x - rect.x) / rect.z;
    texCoord.y = (pixelCoord.y - rect.y) / rect.w;
    
    // Clamp to ensure we're within the rect bounds
    if (texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0) {
        discard;
    }
    
    // Map to subtexture region
    vec2 finalTexCoord = subtexture.xy + texCoord * subtexture.zw;
    
    vec4 texColor = texture(textureSampler, finalTexCoord);
    fragColor = texColor * color;
}