#include "Obstacle.h"
#include "Warning.h"
#include "Prediction.h"

#include "../Module_obstacle.h"

#include "../../../Engine/Node_engine.h"
#include "../../../Scene/Node_scene.h"
#include "../../../Scene/Data/Scene.h"
#include "../../../Scene/Glyph/Object.h"
#include "../../../Scene/Glyph/Glyphs.h"
#include "../../../Scene/Glyph/Cloud/OOBB.h"
#include "../../../Operation/Transformation/Pose.h"
#include "../../../Specific/color.h"
#include "../../../Specific/File/Directory.h"
#include "../../../Specific/File/Zenity.h"

#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>


//Constructor / Destructor
Obstacle::Obstacle(Module_obstacle* module){
  //---------------------------

  Node_engine* node_engine = module->get_node_engine();
  Node_scene* node_scene = node_engine->get_node_scene();

  this->predManager = module->get_predManager();
  this->sceneManager = node_scene->get_sceneManager();
  this->objectManager = node_scene->get_objectManager();
  this->warningManager = module->get_warningManager();
  this->glyphManager = node_scene->get_glyphManager();
  this->oobbManager = new OOBB();
  this->poseManager = new Pose();
  this->data = Data::get_instance();

  this->with_warning = false;
  this->with_prediction = true;

  //---------------------------
}
Obstacle::~Obstacle(){}

//Main functions
void Obstacle::runtime(){
  Collection* collection = sceneManager->get_selected_collection();
  //---------------------------

  if(with_prediction && collection != nullptr){
    //Check for new prediction (ground thruth or prediction)
    bool* is_new_pred = predManager->runtime_prediction();

    if(*is_new_pred){
      this->build_cloud_obstacle(collection);

      //Reverse flag
      *is_new_pred = false;
    }
  }

  //---------------------------
}
void Obstacle::online(Collection* collection, int subset_ID){
  //---------------------------

  this->artificial_obstacle(collection, subset_ID);

  //---------------------------
}

//Manual obstacle adding
void Obstacle::add_detectioned(){
  Collection* collection = sceneManager->get_selected_collection();
  //---------------------------

  if(collection == nullptr){
    cout<<"[error] no cloud in engine"<<endl;
    return;
  }

  //Get prediction file paths
  vector<string> path_vec = zenity_file_vec("Prediction loading");

  //Parses predictions files
  predManager->compute_prediction(collection, path_vec);

  //Build glyphs
  this->build_cloud_obstacle(collection);

  //---------------------------
}
void Obstacle::add_detectioned(string path_dir){
  Collection* collection = sceneManager->get_selected_collection();
  //---------------------------

  //Get prediction file paths
  vector<string> path_vec = list_all_path(path_dir);

  //Parses predictions files
  predManager->compute_prediction(collection, path_vec);

  //Build glyphs
  this->build_cloud_obstacle(collection);

  //---------------------------
}
void Obstacle::add_obstacle_grTr(){
  Collection* collection = sceneManager->get_selected_collection();
  //---------------------------

  //Get prediction file paths
  vector<string> path_vec = zenity_file_vec("Ground truth loading");

  //Parses predictions files
  predManager->compute_groundTruth(collection, path_vec);

  //Build glyphs
  this->build_cloud_obstacle(collection);

  //---------------------------
}

//Subfunctions
void Obstacle::build_cloud_obstacle(Collection* collection){
  //---------------------------
  /*
  //Process prediction if the ieme cloud are not already processed
  for(int i=0; i<collection->list_obj.size(); i++){
    Cloud* cloud = (Cloud*)*next(collection->list_obj.begin(), i);

    if(cloud->detection.is_predicted == false){
      //Build obstacle glyph
      this->build_obstacleGlyph_pr(cloud);
      this->build_obstacleGlyph_gt(cloud);

      //Send MQTT warning
      if(with_warning){
        warningManager->send_warning(cloud);
      }
    }
  }*/

  //---------------------------
}
void Obstacle::build_obstacleGlyph_pr(Cloud* cloud){
  /*Data_pred* detection = &cloud->detection;
  //---------------------------

  for(int i=0; i<detection->name.size(); i++){
    Glyph* glyph = objectManager->create_glyph_ostacle();

    vec3 To = detection->position[i];
    vec3 Ro = vec3(0, 0, detection->heading[i]);
    vec3 So = detection->dimension[i];
    mat4 transf = poseManager->compute_transformMatrix(To, Ro, So);
    vec4 color = AI_color_dic.find(detection->name[i])->second;

    oobbManager->update_glyph(glyph, transf);
    objectManager->update_glyph(glyph, color);
    detection->oobb.push_back(*glyph);

    detection->is_predicted == true;
    delete glyph;
  }
  */
  //---------------------------
}
void Obstacle::build_obstacleGlyph_gt(Cloud* cloud){
  /*Data_pred* detection_gt = &cloud->detection_gt;
  //---------------------------

  for(int j=0; j<detection_gt->name.size(); j++){
    Glyph* glyph = objectManager->create_glyph_ostacle();

    vec3 To = detection_gt->position[j];
    vec3 Ro = vec3(0, 0, detection_gt->heading[j]);
    vec3 So = detection_gt->dimension[j];
    mat4 transf = poseManager->compute_transformMatrix(To, Ro, So);

    oobbManager->update_glyph(glyph, transf);
    objectManager->update_glyph(glyph);
    detection_gt->oobb.push_back(*glyph);
    delete glyph;
  }*/

  //---------------------------
}

void Obstacle::artificial_obstacle(Collection* collection, int subset_ID){
  //---------------------------

  Cloud* cloud = (Cloud*)collection->get_obj_byID(subset_ID);
  cloud->draw_point_size = 5;

  vector<string> vec_path = list_all_path("/home/aeter/Desktop/pred");

  for(int i=0; i<vec_path.size(); i++){
    string path = vec_path[i];

    if(get_number(collection->selected_obj->name) == get_number(path)){
      //Parse prediction json file
      std::ifstream ifs(path);
      Json::Reader reader;
      Json::Value obj;
      reader.parse(ifs, obj);

      //For each hierarchical set
      vec3 pos = vec3(0);
      vec3 dim = vec3(0);
      float head = 0;
      const Json::Value& json = obj["detections"];
      for(int i=0; i<json.size(); i++){
        // Name
        string name = json[i]["name"].asString();

        if(name == "Car"){
          // Position
          const Json::Value& json_pos = json[i]["position"];
          for (int j=0; j<json_pos.size(); j++){
            pos[j] = json_pos[j].asFloat();
          }

          // Dimension
          const Json::Value& json_dim = json[i]["dimensions"];
          for (int j=0; j<json_dim.size(); j++){
            dim[j] = json_dim[j].asFloat();
          }

          // Heading
          const Json::Value& json_head = json[i]["heading"];
          head = json_head.asFloat();
        }
      }

      Glyph* glyph = oobbManager->glyph;
      vec3 To = pos;
      vec3 Ro = vec3(0, 0, head);
      vec3 So = dim;
      mat4 transf = poseManager->compute_transformMatrix(To, Ro, So);
      vec4 color = vec4(1,0,0,1);
      Cloud* object = (Cloud*)collection->get_obj_byID(subset_ID);
      mat4 translation = mat4(1);
      translation[0][3] = object->pose_T(0);
      translation[1][3] = object->pose_T(1);
      translation[2][3] = object->pose_T(2);
      for(int j=0; j<3; j++){
        for(int k=0; k<3; k++){
          translation[j][k] = object->pose_R(j, k);
        }
      }
      transf = transf * translation;

      glyphManager->insert_into_gpu(glyph);
      oobbManager->update_glyph(glyph, transf);
      glyphManager->create_glyph_object((Cloud*)collection->selected_obj, glyph);

      glyphManager->update_glyph_location(glyph);
      glyphManager->update_glyph_color(glyph, color);
      glyph->is_visible = true;

      Collection* col_glyph = data->get_collection_byName("glyph", "glyph_scene");
      col_glyph->list_obj.push_back(glyph);

    }
  }

  //---------------------------
}
string Obstacle::get_number(string truc){
  //---------------------------

  auto pos = truc.find_last_of('_');
  auto point = truc.find_last_of('.');
  string result = truc.substr(pos + 1, point - pos - 1);

  //---------------------------
  return result;
}
void Obstacle::reset(){
  //---------------------------

  Glyph* glyph = oobbManager->glyph;
  glyph->is_visible = false;

  //---------------------------
}
