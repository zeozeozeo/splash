#version 120
precision highp float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
//varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D gameTexture;
uniform sampler2D noiseTexture;
uniform float iTime;
uniform vec2 iResolution;
//uniform vec4 colDiffuse;

// constants
const float WATER_SPEED = 0.1;
const float WATER_STRENGTH = 0.05;

void main() {
    vec4 texelColor = texture2D(texture0, fragTexCoord);
    if (texelColor.a > 0.21) {
        vec3 result = texelColor.rgb;
        //if (texelColor.a < 0.28) {
        //    result = vec3(1.0, 1.0, 1.0);
        //}

        // distortion
        float noiseValue = texture2D(noiseTexture, fragTexCoord + iTime * WATER_SPEED).r;
        vec3 gameColor = texture2D(
            gameTexture,
            fragTexCoord - (WATER_STRENGTH / 2.0) + vec2(noiseValue) * WATER_STRENGTH
        ).rgb;

        gl_FragColor = vec4(
            //vec3(noiseValue, noiseValue, noiseValue),
            (result + gameColor) / 2.0,
            1.0
        );
    } else {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
