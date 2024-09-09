#version 120

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
//varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D gameTexture;
uniform float iTime;
uniform vec2 iResolution;
//uniform vec4 colDiffuse;

vec2 dist(vec2 uv) { 
    //float x = uv.x*25.+iTime;
    //float y = uv.y*25.+iTime;
    //uv.y += cos(x+y)*0.01*cos(y);
    //uv.x += sin(x-y)*0.01*sin(y);
    return uv + 150.0;
}

void main() {
    vec4 texelColor = texture2D(texture0, fragTexCoord);
    if (texelColor.a > 0.21) {
        vec3 result = texelColor.rgb;
        //if (texelColor.a < 0.28) {
        //    result = vec3(1.0, 1.0, 1.0);
        //}

        // distortion
        vec2 uv = dist(fragTexCoord.xy / iResolution.xy);
        vec4 distortColor = texture2D(gameTexture, uv);

        gl_FragColor = vec4(distortColor.rgb, 0.5);
    } else {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
