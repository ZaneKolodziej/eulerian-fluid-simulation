#version 430 core

in vec4 fragColor;
in vec2 fragTexCoordinate;

out vec4 finalColor;

uniform sampler2D texture0;
uniform int displayMode;

void main () {
    vec4 data = texture(texture0, fragTexCoordinate);
    vec2 vel = data.rg;

    if (displayMode == 0) {
        finalColor = vec4(data.rgb, 1.0);
    }
    else if (displayMode == 1) {
        // Velocity visualization
        float speed = length(vel);
        vec3 col = vec3(abs(vel.x) * 2.0, abs(vel.y) * 2.0, speed * 0.5);
        finalColor = vec4(col, 1.0);
    }
    else if (displayMode == 2) {
        // Divergence visualization
        float divergence = data.b; // Or whatever channel holds divergence in mode 2
        vec3 col = vec3(0.0);
        if (divergence > 0.0) col.r = divergence * 10.0;
        else col.b = abs(divergence) * 10.0;
        finalColor = vec4(col, 1.0);
    }
}