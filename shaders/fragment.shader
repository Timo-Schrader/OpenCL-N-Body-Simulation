R"(
#version 460 core

#define M_PI 3.14159265358979323846
#define EPSILON 0.0001 // Fixes the color when calculating it for the maximum mass object
                                            
in vec3 lColor;

in float logInMass;
in float logMinMass;
in float logMaxMass;

uniform float minMass;
uniform float maxMass;

uniform uint mode; //0 = Bodies, 1 = Lines

out vec4 fragColor;

// Algorithm as described on https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB
vec3 hsv2rgb(float h, float s, float v){
    vec3 rgb;

    float C = v * s;
    h = h * 180 / M_PI;
    h /= 60;
    float X = C * (1 - abs(mod(h, 2) - 1));

    if(h >= 0 && h < 1){
        rgb.x = C;
        rgb.y = X;
        rgb.z = 0;
    }
    else if(h >= 1 && h < 2){
        rgb.x = X;
        rgb.y = C;
        rgb.z = 0;
    }
    else if(h >= 2 && h < 3){
        rgb.x = 0;
        rgb.y = C;
        rgb.z = X;
    }
    else if(h >= 3 && h < 4){
        rgb.x = 0;
        rgb.y = X;
        rgb.z = C;
    }
    else if(h >= 4 && h < 5){
        rgb.x = X;
        rgb.y = 0;
        rgb.z = C;
    }
    else if(h >= 5 && h < 6){
        rgb.x = C;
        rgb.y = 0;
        rgb.z = X;
    }
    else{
        rgb.x = 0;
        rgb.y = 0;
        rgb.z = 0;
    }

    float m = v - C;
    rgb.x += m;
    rgb.y += m;
    rgb.z += m;

    return rgb;
}

void main(){
    // Used for rendering the bodies
    if(0 == mode){
        // Calculates the degree (in radian) between min and max which is mapped to min and max color                              
        float degree = float(2 * M_PI * (logInMass - logMinMass) / (logMaxMass + EPSILON - logMinMass));
        float s = 1.0f;
        float v = 1.0f;
        vec3 pixelColor = hsv2rgb(degree, s, v);
        fragColor = vec4(pixelColor, 1.0f);
    }
    // Used for rendering the coordinate system axes
    else if (1 == mode){
        fragColor = vec4(lColor, 1.0f);
    }
}
)"