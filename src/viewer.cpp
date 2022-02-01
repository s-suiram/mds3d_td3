#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
    : _winWidth(0), _winHeight(0), zoom(1), translation(0, 0), wireframe(false) {
}

Viewer::~Viewer() {
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h) {
  loadShaders();
  
  if (!_mesh.load(DATA_DIR"/models/lemming.off")) exit(1);
  _mesh.initVBA();
  
  reshape(w, h);
  _trackball.setCamera(&_cam);
}

void Viewer::reshape(int w, int h) {
  _winWidth = w;
  _winHeight = h;
  _cam.setViewport(w, h);
}

/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_LINE_SMOOTH);
  glClearColor(0.1, 0.1, 0.1, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  _shader.activate();
  
  glViewport(0, 0, _winWidth / 2, _winHeight);
  
  glUniform1f(_shader.getUniformLocation("zoom"), zoom);
  glUniform2f(_shader.getUniformLocation("translation"), translation.x(), translation.y());
  
  glUniform1i(_shader.getUniformLocation("sideView"), 0);
  glUniform1i(_shader.getUniformLocation("white"), 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  _mesh.draw(_shader);
  
  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform1i(_shader.getUniformLocation("white"), 1);
    _mesh.draw(_shader);
  }
  
  // Side view
  glViewport(_winWidth / 2, 0, _winWidth / 2, _winHeight);
  glUniform1i(_shader.getUniformLocation("sideView"), 1);
  
  glUniform1i(_shader.getUniformLocation("white"), 0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  _mesh.draw(_shader);
  
  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUniform1i(_shader.getUniformLocation("white"), 1);
    _mesh.draw(_shader);
  }
  
  _shader.deactivate();
}

void Viewer::updateAndDrawScene() {
  drawScene();
}

void Viewer::loadShaders() {
  // Here we can load as many shaders as we want, currently we have only one:
  _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
  checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    loadShaders();
  }
  
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    if (key == GLFW_KEY_UP) {
      translation.y() += 0.05;
    } else if (key == GLFW_KEY_DOWN) {
      translation.y() -= 0.05;
    } else if (key == GLFW_KEY_LEFT) {
      translation.x() -= 0.05;
    } else if (key == GLFW_KEY_RIGHT) {
      translation.x() += 0.05;
    } else if (key == GLFW_KEY_PAGE_UP) {
      zoom += 0.1;
    } else if (key == GLFW_KEY_PAGE_DOWN) {
      zoom -= 0.1;
      zoom = std::max(0.f, zoom);
    } else if (key == GLFW_KEY_SPACE) {
      wireframe = !wireframe;
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action) {
  if (action == GLFW_PRESS) {
    _trackingMode = TM_ROTATE_AROUND;
    _trackball.start();
    _trackball.track(_lastMousePos);
  } else if (action == GLFW_RELEASE) {
    _trackingMode = TM_NO_TRACK;
  }
}

/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y) {
  if (_trackingMode == TM_ROTATE_AROUND) {
    _trackball.track(Vector2i(x, y));
  }
  
  _lastMousePos = Vector2i(x, y);
}

void Viewer::mouseScroll(double /*x*/, double y) {
  _cam.zoom(-0.1 * y);
}

void Viewer::charPressed(int /*key*/) {
}
