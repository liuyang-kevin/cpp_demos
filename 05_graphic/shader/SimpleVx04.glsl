#version 330 core
layout(location=0)in vec3 vertexPosition_modelspace;
layout(location=1)in vec3 vertexColor;
out vec3 fragmentColor;
uniform mat4 MVP;// 来自03课，追加矩阵透视，MVP3次变换
void main(){
    gl_Position=MVP*vec4(vertexPosition_modelspace,1);
    
    fragmentColor=vertexColor;
}