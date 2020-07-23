#include <GL/glew.h>  // 这个库还得放在glfw之前
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>

#include "lib/loadShader.hpp"
using namespace glm;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,  // x
    1.0f,  -1.0f, 0.0f,  // y
    0.0f,  1.0f,  0.0f,  // z
};

int main(int argc, char const* argv[]) {
  glewExperimental = true;
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                 GL_TRUE);  // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE,
                 GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL

  // Open a window and create its OpenGL context
  GLFWwindow* window;
  window = glfwCreateWindow(1024, 768, "Tutorial 02", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr,
            "Failed to open GLFW window. If you have an Intel GPU, they are "
            "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);  // Initialize GLEW
  glewExperimental = true;         // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }

  //顶点数组对象
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // 多边形的buffer对象
  GLuint vertexbuffer;
  // 生成1帧Buffer（自己翻译的，帧概念不准确）
  glGenBuffers(1, &vertexbuffer);
  // 对buffer对象绑定到gl
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  //通过gl把我们的三角形数据填充给buffer（也就填充到了刚绑定的对象）
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
               g_vertex_buffer_data, GL_STATIC_DRAW);

  //------------------------------------------------
  // 从着色器创建并编译我们的GLSL程序，要在loop之前
  GLuint programID = LoadShaders("shader/SimpleV.glsl", "shader/SimpleF.glsl");
  glUseProgram(programID);  // 告诉OpenGL您要使用着色器：
  //------------------------------------------------

  // input/output 窗口
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  do {
    // glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // 修改背景色为蓝色

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,  // attribute 0. No particular reason for 0, but
                              // must match the layout in the shader.
                          3,  // size
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,         // stride
                          (void*)0   // array buffer offset
    );
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0,
                 3);  // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  }  // Check if the ESC key was pressed or the window was closed
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0);
  return 0;
}
