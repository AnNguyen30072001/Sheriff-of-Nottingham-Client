uniform bool isHovered;
uniform vec4 outlineColor;  
uniform float glowStrength;

void main()
{
    vec4 baseColor = gl_Color; // Use SFML assigned color

    if (!isHovered) {
        gl_FragColor = baseColor; // No glow when not hovered
        return;
    }

    // Simulate an outer glow by mixing outline color based on distance
    float glowFactor = smoothstep(0.0, 1.0, glowStrength);

    // Blend glow color outside, brighten inside
    gl_FragColor = mix(baseColor, outlineColor, glowFactor) * 1.2;  
}
