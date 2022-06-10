R"(
#version 460 core

layout(location=0) in vec3 inPosition;
layout(location=1) in float inMass;
layout(location=2) in vec3 lineCoordinates;
layout(location=3) in vec3 lineColor;

uniform mat4 M_model;
uniform mat4 M_view;
uniform mat4 M_projection;

uniform float maxPos;
uniform float minMass;
uniform float maxMass;

uniform uint mode; //0 = Bodies, 1 = Lines

uniform float zoomFactor = 1.0f;

const float MAX_ADD_RADIUS = 60.0f;
const float BASE_RADIUS = 1.0f;

out vec3 lColor;

out float logInMass;
out float logMinMass;
out float logMaxMass;

float log10(float x){
    return log(x) / log(10);
}

void main(){
    // Used for rendering the bodies
    if (0 == mode){
        logInMass = log10(inMass);
        logMinMass = log10(minMass);
        logMaxMass = log10(maxMass);
        gl_Position = M_projection * M_view * M_model * vec4(inPosition.x, inPosition.z, inPosition.y, maxPos);
        // Using log-min-max normalization since almost all values would be mapped to approx. 0 without log as the interval range can be really huge
        gl_PointSize = float((logInMass - logMinMass) / (logMaxMass - logMinMass) * MAX_ADD_RADIUS) + BASE_RADIUS;
        // Only zoom-in objects which are big enough
        if(zoomFactor > 1 && logInMass >= 0.7 * logMaxMass){
            gl_PointSize *= zoomFactor;
        }
        else if (zoomFactor < 1){
            gl_PointSize *= zoomFactor;
        }
    }
    // Used for rendering the coordinate system axes
    else if (1 == mode){
        gl_Position = M_projection * M_view * M_model * vec4(lineCoordinates, 1.0f);
        lColor = lineColor;
    }
}
)"