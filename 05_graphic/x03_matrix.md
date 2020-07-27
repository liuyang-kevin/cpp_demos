呃。。。原来有中文翻译。。
[矩阵](http://www.opengl-tutorial.org/cn/beginners-tutorials/tutorial-3-matrices/)
* OpenGL数学库（GLM）
    * OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
* [透视投影(Perspective Projection)变换推导](https://www.cnblogs.com/hisiqi/p/3155813.html)
# 导读&笔记：
## 模型（Model）、观察（View）和投影（Projection）矩阵
* 模型矩阵变换有顺序，而且不能变：`缩放旋转平移`，核心问题就是，这个步骤操作，不会导致计算后中心点偏移
    * 因为模型的所有点，都与中心点保持相对关系，矩阵计算后，就可以完成所有点的移动，模型也就成功移动了。
    * 模型矩阵（Model Matrix）是针对模型本身与世界坐标的关系。
        * 例如，1:1的人物模型，模型中心点在腹部，放入世界的中心点，那么人身就是半截在地下，这时就需要模型矩阵，偏移y轴。将人与世界位置摆正。
* 观察矩阵，这里的核心原理是观察世界的“相机”位置不动，世界移动。
    * *引擎推动的不是飞船而是宇宙。飞船压根就没动过。* 《飞出个未来》
    * 计算的时候，不要计算相机移动了多少，要计算世界偏移了多少。也就是观察矩阵原理。
* 投影矩阵，这里的重要的含义是，屏幕输出或者说2D平面输出的窗口是垂直景深，这里也是一个坐标系。
    * 相机的观察区是个锥形空间（无限广角，其实也是全部空间，或者说一半空间）
    * 投影矩阵与观察区形成的关系，就是最后的输出。（其实可以理解为相机的焦距。对焦点在哪里）

* 再重复一遍。`MVP` 
    1. 移动模型到世界坐标。
    2. 视野不动，世界动。（这里是因为移动视野，那么视野上就会绑定2层关系，而移动世界很简单，所以世界这边维护相机的关系就很顺。）
    3. 视野不动，投影动。（这个投影其实就是焦距，用身边的手机试试手动对焦就能理解了。）

## 代码相关： 复合变换：模型观察投影矩阵（MVP）
```c
// C++ : compute the matrix
glm::mat4 MVPmatrix = projection * view * model; // 注意，是倒序。计算机会先算后头的

// GLSL代码中 : 接收
transformed_vertex = MVP * in_vertex;
```

## 总结，升级上节课的代码

```C
// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);

// Or, for an ortho camera :
//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

// Camera matrix
glm::mat4 View = glm::lookAt(
    glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
    glm::vec3(0,0,0), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);
// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around
```

```C
// Get a handle for our "MVP" uniform
// Only during the initialisation
GLuint MatrixID = glGetUniformLocation(programID, "MVP");

// Send our transformation to the currently bound shader, in the "MVP" uniform
// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
```

```glsl
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
  // Output position of the vertex, in clip space : MVP * position
  gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}
```

`gcc ./x03_matrix.cpp -o matrix.bin -lglew -lglfw -lc++ -framework OpenGL && ./matrix.bin`

----
# 透视投影变换推导。
透视投影理解困难在于步骤繁琐，对一些基础知识过分依赖。首先介绍两个必须掌握的知识。
* 向量几何
* 矩阵
（日后补全，这里设计数学知识较多。）