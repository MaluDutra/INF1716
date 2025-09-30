
#ifdef _WIN32
#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "scene.h"
#include "state.h"
#include "camera2d.h"
#include "color.h"
#include "transform.h"
#include "error.h"
#include "shader.h"
#include "quad.h"
#include "disk.h"
#include "triangle.h"

#include <iostream>

static ScenePtr scene;
static CameraPtr camera;

class RotationEngine;
using RotationEnginePtr = std::shared_ptr<RotationEngine>;
class RotationEngine : public Engine
{
    TransformPtr m_trf; 
    float m_speed;      

protected:
    RotationEngine(TransformPtr trf, float speed)
        : m_trf(trf), m_speed(speed)
    {
    }

public:
    static RotationEnginePtr Make(TransformPtr trf, float speed)
    {
        return RotationEnginePtr(new RotationEngine(trf, speed));
    }

    virtual void Update(float dt) 
    {
        float angle = m_speed * dt;
        m_trf->Rotate(angle, 0, 0, 1);
    }
};

static void initialize (void)
{
  // set background color: black 
  glClearColor(0, 0, 0, 1.0f);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);

  // create objects
  camera = Camera2D::Make(0,10,0,10);

  // constructing the sun's transformations
  auto trfSun = Transform::Make();
  trfSun->Translate(5.0f, 5.0f, 0.0f);
  trfSun->Scale(0.7f, 0.7f, 1.0f);

  // constructing the earth's transformations
  auto trfEarthOrbit = Transform::Make();
  auto trfEarthTranslation = Transform::Make();
  trfEarthTranslation->Translate(5.0f, 0.0f, 0.0f);
  auto trfEarthRotation = Transform::Make();
  auto trfEarthScale = Transform::Make();
  trfEarthScale->Scale(0.5f, 0.5f, 1.0f);

  // constructing the moon's transformations
  auto trfMoonOrbit = Transform::Make();
  auto trfMoonTranslation = Transform::Make();
  trfMoonTranslation->Translate(1.3f, 0.0f, 0.0f);
  auto trfMoonScale = Transform::Make();
  trfMoonScale->Scale(0.2f, 0.2f, 1.0f);

  // constructiong the nodes
  auto moonScaleNode = Node::Make(trfMoonScale, { Color::Make(7.0f, 7.0f, 7.0f) }, { Disk::Make() });
  auto moonTranslationNode = Node::Make(trfMoonTranslation, { moonScaleNode });
  auto moonOrbitNode = Node::Make(trfMoonOrbit, { moonTranslationNode });

  auto earthScaleNode = Node::Make(trfEarthScale, { Color::Make(0.0f, 0.0f, 0.5f) }, { Disk::Make() });
  auto earthRotationNode = Node::Make(trfEarthRotation, { earthScaleNode });
  auto earthTranslationNode = Node::Make(trfEarthTranslation, { earthRotationNode });
  earthTranslationNode->AddNode(moonOrbitNode);
  auto earthOrbitNode = Node::Make(trfEarthOrbit, { earthTranslationNode });

  auto sun = Node::Make(trfSun, { Color::Make(1.0f, 8.0f, 0.3f) }, { Disk::Make() });
  sun->AddNode(earthOrbitNode);

  auto shader = Shader::Make();
  shader->AttachVertexShader("shaders/vertex.glsl");
  shader->AttachFragmentShader("shaders/fragment.glsl");
  shader->Link();

  // build scene
  auto root = Node::Make(shader, { sun });
  scene = Scene::Make(root);
  scene->AddEngine(RotationEngine::Make(trfEarthOrbit, 20.0f));
  scene->AddEngine(RotationEngine::Make(trfEarthRotation, 100.0f));
  scene->AddEngine(RotationEngine::Make(trfMoonOrbit, 60.0f));
}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(camera);
  Error::Check("after render");
}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void update (float dt)
{
  scene->Update(dt);
}

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);       // required for mac os
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);  // option for mac os
#endif

    glfwSetErrorCallback(error);

    GLFWwindow* win = glfwCreateWindow(600, 400, "Sistema Solar", nullptr, nullptr);
    assert(win);
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback

    glfwMakeContextCurrent(win);
#ifdef _WIN32
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD OpenGL context\n");
        exit(1);
    }
#endif
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t-t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

