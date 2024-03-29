#include "CoreGLengine.h"

#include "../Rendering/Renderer.h"
#include "../GPU/GPU_screenshot.h"
#include "../Node_engine.h"
#include "../Shader/Shader.h"
#include "../Shader/Base/Shader_obj.h"
#include "../Camera/Camera.h"
#include "../Camera/Viewport.h"
#include "../Core/Argument.h"
#include "../Core/Dimension.h"
#include "../Core/Engine.h"
#include "../Core/Configuration.h"
#include "../../Scene/Data/Data.h"
#include "../../Specific/Function/fct_math.h"

#include "../../GUI/Node_gui.h"
#include "../../GUI/Control/GUI.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GL_GLEXT_PROTOTYPES

#include <chrono>

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;


//Constructor / Destructor
CoreGLengine::CoreGLengine(){
  //---------------------------

  this->configManager = new Configuration();
  Data* data = Data::get_instance();

  this->loop_run = true;
  this->window = nullptr;
  this->flag_resized = false;

  //---------------------------
}
CoreGLengine::~CoreGLengine(){
  //---------------------------

  delete configManager;
  delete node_engine;

  glfwDestroyWindow(window);
  glfwTerminate();

  //---------------------------
}

//Opengl stuff
void CoreGLengine::arg(int argc, char* argv[]){
  Argument argManager(node_engine);
  //--------------geometric-------------

  argManager.process_arg(argc, argv);

  //---------------------------
}
void CoreGLengine::init(){
  //---------------------------

  this->init_opengl();
  this->init_object();
  this->init_rendering();

  //---------------------------
  console.AddLog("ok" ,"Program initialized...");
}
void CoreGLengine::loop(){
  bool is_timer = false;
  //---------------------------
  do{
    //First pass
    //---------------------------
    this->loop_resizing();
    renderManager->loop_rendering();

    //GUI and end
    //---------------------------
    this->loop_gui();
    this->loop_end();

    //Time loop
    auto t2 = high_resolution_clock::now();
    this->time_loop = duration_cast<milliseconds>(t2 - t1).count();
  }
  while(loop_run);

  //---------------------------
}

// Initialization stuff
void CoreGLengine::init_opengl(){
  //---------------------------

  //Parametrization
  int resolution_width = configManager->parse_json_i("window", "resolution_width");
  int resolution_height = configManager->parse_json_i("window", "resolution_height");
  bool forceVersion = configManager->parse_json_b("window", "forceVersion");
  string win_title = configManager->parse_json_s("window", "title");

  //GLFW
  glfwInit();
  if(forceVersion){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
  }
  glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  window = glfwCreateWindow(resolution_width, resolution_height, "window", NULL, NULL);
  if(window == NULL){
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetWindowTitle(window, win_title.c_str());

  //Enable OpenGL alpha channel for RGB
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // OpenGL face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  //Enable shader point size setting
  //glEnable(GL_PROGRAM_POINT_SIZE);

  //GLEW
  glewInit();

  //---------------------------
}
void CoreGLengine::init_object(){
  //---------------------------

  this->node_engine = new Node_engine(this);
  this->node_gui = node_engine->get_node_gui();
  this->dimManager = node_engine->get_dimManager();
  this->shaderManager = node_engine->get_shaderManager();
  this->cameraManager = node_engine->get_cameraManager();
  this->viewportManager = node_engine->get_viewportManager();
  this->renderManager = node_engine->get_renderManager();
  this->engineManager = node_engine->get_engineManager();
  this->screenshotManager = node_engine->get_gpu_screenshot();

  //---------------------------
}
void CoreGLengine::init_rendering(){
  //---------------------------

  dimManager->update();
  shaderManager->init_shader();

  //---------------------------
}

// Loop stuff
void CoreGLengine::loop_gui(){
  //---------------------------

  dimManager->set_is_resized(false);

  //Draw GUI on fbo 0
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  node_gui->loop();

  //---------------------------
}
void CoreGLengine::loop_resizing(){
  //---------------------------

  //Update things
  dimManager->update();
  this->flag_resized = dimManager->get_is_resized();

  if(flag_resized){
    renderManager->update_dim_texture();
    shaderManager->update_shader();
  }

  //Camera & scene runtime
  cameraManager->input_cam_mouse();
  node_engine->runtime();
  node_gui->loop_selection();

  //---------------------------
}
void CoreGLengine::loop_end(){
  //---------------------------

  //End, if needed, by a screenshot
  screenshotManager->render_screenshot_online();

  //Compteur for measuring GPU usage
  static vector<float> time_vec;
  static int cpt = 0;
  tic();

  {
    //Window display stuff
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  float time = toc_ms();
  if(false && cpt == 100){
    float mean = fct_mean(time_vec);
    cout<<"Mean time = "<<mean<< " ms" <<endl;
    time_vec.clear();
    cpt = 0;
  }else{
    time_vec.push_back(time);
    cpt++;
  }

  //Check for window termination
  if(glfwWindowShouldClose(window)){
    loop_run = false;
  }

  //---------------------------
}
