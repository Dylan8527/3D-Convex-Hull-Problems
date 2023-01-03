#include<scene.h>
#include<chrono>

int scene_id = 1;
EConvexhullAlgorithm convexhull_algorithm=EConvexhullAlgorithm::Incremental;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double x, double y);
void scroll_callback(GLFWwindow* window, double x, double y);
const int WIDTH = 1920;
const int HEIGHT = 1080;

bool firstMouse = true;
float fov = 45.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;

GLFWwindow *window;
Camera mycamera;

int main(int argc, char *argv[])
{
  if(argc != 0) scene_id = atoi(argv[1]);
  
  WindowGuard windowGuard(window, WIDTH, HEIGHT, "CS271 hw1");
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glEnable(GL_DEPTH_TEST);

  Shader shader("../src/shaders/projective.vs", "../src/shaders/phong_lighting.fs");
  ShaderParam shader_param;
  shader_param.projection = glm::perspective(glm::radians(fov), (float)WIDTH/(float)HEIGHT,  0.1f, 100.0f);

  Scene scene(scene_id, convexhull_algorithm);

  auto start = std::chrono::system_clock::now();
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_param.view = mycamera.getViewMatrix();
    shader_param.cameraPos = mycamera.Position;
    shader_param.projection = glm::perspective(glm::radians(fov), (float)WIDTH/(float)HEIGHT,  0.1f, 100.0f);
    auto end = std::chrono::system_clock::now();
    auto duration = double(std::chrono::duration_cast<std::chrono::microseconds>(end-start).count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && duration > 0.1f){
      start = end;
      scene.Solve();
    }
    if(scene.CheckCollision()) start = std::chrono::system_clock::now();
    scene.Draw(shader, shader_param);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  return 0;
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Forward);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Backward);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Leftward);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  {
    mycamera.processWalkAround(Rightward);
  }
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
  {
    std::cout << "Camera position: (" << mycamera.Position.x << ", "
              << mycamera.Position.y << ", " << mycamera.Position.z << ")" << std::endl;
  }
}

void mouse_callback(GLFWwindow *window, double x, double y)
{
  x = (float)x;
  y = (float)y;
  if (firstMouse)
  {
    lastX = x;
    lastY = y;
    firstMouse = false;
  }

  mycamera.processLookAround(x - lastX, y - lastY);

  // update record
  lastX = x;
  lastY = y;
}

void scroll_callback(GLFWwindow* window, double x, double y)
{
  if(fov >= 1.0f && fov <= 45.0f)
    fov -= y;
  if(fov <= 1.0f)
    fov = 1.0f;
  if(fov >= 45.0f)
    fov = 45.0f;
}

