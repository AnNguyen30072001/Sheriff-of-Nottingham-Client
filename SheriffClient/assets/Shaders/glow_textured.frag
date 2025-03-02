uniform sampler2D texture;
uniform vec4 outlineColor;  // White outline color
uniform float outlineThickness; // Thickness of the outline
uniform float brightness;   // Brightness factor when hovering
uniform bool isHovered;  // 1 if hovering, 0 if not

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 texColor = texture2D(texture, texCoord);

    if (!isHovered) {
        // If not hovering, just use the normal color
        gl_FragColor = texColor;
        return;
    }

    // Sample neighboring pixels to detect edges
    float alphaCenter = texColor.a;
    float alphaLeft   = texture2D(texture, texCoord + vec2(-outlineThickness, 0)).a;
    float alphaRight  = texture2D(texture, texCoord + vec2(outlineThickness, 0)).a;
    float alphaUp     = texture2D(texture, texCoord + vec2(0, outlineThickness)).a;
    float alphaDown   = texture2D(texture, texCoord + vec2(0, -outlineThickness)).a;

    // Detect if this pixel is at the edge
    bool isEdge = (alphaCenter > 0.0) && (alphaLeft == 0.0 || alphaRight == 0.0 || alphaUp == 0.0 || alphaDown == 0.0);
    
    if (isEdge) {
        gl_FragColor = outlineColor; // Apply white outline
    } else {
        // Brighten the inside of the object
        gl_FragColor = texColor * brightness;
    }
}
