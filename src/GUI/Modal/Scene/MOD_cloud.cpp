#include "MOD_cloud.h"

#include "../../../Engine/Node_engine.h"
#include "../../../Scene/Node_scene.h"
#include "../../../Scene/Data/Scene.h"
#include "../../../Operation/Node_operation.h"
#include "../../../Operation/Transformation/Attribut.h"
#include "../../../Operation/Color/Color.h"
#include "../../../Specific/Function/fct_math.h"
#include "../../../Specific/Function/fct_transtypage.h"

#include "image/IconsFontAwesome5.h"

#include "../Modal_tab.h"
extern struct Modal_tab modal_tab;


//Constructor / Destructor
MOD_cloud::MOD_cloud(Node_operation* node_ope){
  //---------------------------

  Node_engine* node_engine = node_ope->get_node_engine();
  Node_scene* node_scene = node_engine->get_node_scene();

  this->sceneManager = node_scene->get_sceneManager();
  this->attribManager = node_ope->get_attribManager();
  this->colorManager = node_ope->get_colorManager();

  //---------------------------
}
MOD_cloud::~MOD_cloud(){}

//Main function
void MOD_cloud::mod_cloud_info(){
  Collection* collection = sceneManager->get_selected_collection();
  bool* open = &modal_tab.show_modifyFileInfo;
  //---------------------------

  if(*open && collection != nullptr){
    ImGui::Begin(ICON_FA_COMMENT " Point cloud", open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav);
    Cloud* cloud = (Cloud*)collection->selected_obj;

    //Visibility
    static bool visible = true;
    if(ImGui::Checkbox("Visibility", &visible)){
      collection->set_visibility(visible);
    }
    ImGui::SameLine();

    //Uniform collection color
    static vec4 color_PC(0.0f);
    if(collection != nullptr){
      color_PC = collection->unicolor;
    }

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;
    flags |= ImGuiColorEditFlags_AlphaBar;
    if(ImGui::ColorEdit4("Color", (float*)&color_PC, flags)){
      if(collection != nullptr){
        colorManager->set_color_new(collection, color_PC);
      }
    }
    ImGui::Separator();

    //Name
    ImGui::Columns(2);
    ImGui::SetColumnWidth(-1,75);
    ImGui::Text("Name ");
    ImGui::NextColumn();
    static char str_n[256];
    strcpy(str_n, collection->name.c_str());
    if(ImGui::InputText("##name", str_n, IM_ARRAYSIZE(str_n), ImGuiInputTextFlags_EnterReturnsTrue)){
      collection->name = str_n;
    }
    ImGui::NextColumn();

    //Format
    ImGui::Text("Format ");
    ImGui::NextColumn();
    static char str_f[256];
    strcpy(str_f, collection->file_format.c_str());
    if(ImGui::InputText("##format", str_f, IM_ARRAYSIZE(str_f), ImGuiInputTextFlags_EnterReturnsTrue)){
      collection->file_format = str_f;
    }
    ImGui::NextColumn();

    //Number of cloud
    ImGui::Text("Nb cloud ");
    ImGui::NextColumn();
    string nb_obj = to_string(collection->nb_obj);
    ImGui::Text("%s", nb_obj.c_str());
    ImGui::NextColumn();

    //Number of points
  /*  ImGui::Text("Nb point ");
    ImGui::NextColumn();
    string nb_point = thousandSeparator(collection->nb_point);
    ImGui::Text("%s", nb_point.c_str());
    ImGui::NextColumn();*/

    //Root pos
    vec3& root = cloud->root;
    ImGui::Text("Root ");
    ImGui::NextColumn();
    ImGui::Text("%.2f  %.2f  %.2f", root.x, root.y, root.z);
    ImGui::SameLine();
    if(ImGui::Button("R", ImVec2(15,0))){
      root = vec3(0,0,0);
    }
    ImGui::Columns(1);
    ImGui::Separator();

    if(ImGui::Button("Transformation", ImVec2(100,0))){
      modal_tab.show_transformation = !modal_tab.show_transformation;
    }
    ImGui::SameLine();
    if(ImGui::Button("Data", ImVec2(100,0))){
      modal_tab.show_asciiData = !modal_tab.show_asciiData;
    }

    ImGui::Separator();
    if(ImGui::Button("Close")){
      *open = false;
    }
    ImGui::End();
  }

  //---------------------------
}
/*void MOD_cloud::mod_cloud_data(){
  if(modal_tab.show_asciiData){
    ImGui::Begin("Data", &modal_tab.show_asciiData);
    Collection* collection = sceneManager->get_selected_collection();
    Cloud* cloud = (Cloud*)collection->selected_obj;
    //---------------------------

    // Data vectors
    vector<vec3>& XYZ = cloud->xyz;
    vector<vec4>& RGB = cloud->rgb;
    vector<float>& Is = cloud->I;
    vector<vec3>& N = cloud->Nxyz;
    vector<float>& ts = cloud->ts;

    //Settings
    static int nbLines = 100;
    if(ImGui::SliderInt("Number of lines", &nbLines, 1, cloud->nb_point)){
      if(nbLines > XYZ.size()) nbLines = XYZ.size();
    }
    ImGui::Separator();

    // Get number of attributs
    int nb = 2;
    if(cloud->has_intensity) nb++;
    if(cloud->has_color) nb++;
    if(cloud->has_normal) nb++;
    if(cloud->has_timestamp) nb++;

    //Columns
    ImGui::Columns(nb);
    ImGui::SetColumnWidth(0, 30);
    ImGui::Text("#");
    ImGui::NextColumn();
    ImGui::Text("XYZ");
    ImGui::NextColumn();
    if(cloud->has_intensity){
      ImGui::Text("I");
      ImGui::NextColumn();
    }
    if(cloud->has_color){
      ImGui::Text("RGB");
      ImGui::NextColumn();
    }
    if(cloud->has_normal){
      ImGui::Text("Nxyz");
      ImGui::NextColumn();
    }
    if(cloud->has_timestamp){
      ImGui::Text("ts");
      ImGui::NextColumn();
    }
    ImGui::Separator();

    //Data in columns
    static int selected = -1;
    for(int i=0; i<nbLines; i++){
      // Line number
      ImGui::TextColored(ImVec4(0.4f,0.9f,0.4f,1.0f),"%i", i+1);
      ImGui::NextColumn();

      // XYZ coordinate
      ImGui::Text("%f %f %f", XYZ[i].x, XYZ[i].y, XYZ[i].z);
      ImGui::NextColumn();

      // Intensity
      if(cloud->has_intensity){
        ImGui::Text("%f", Is[i]);
        ImGui::NextColumn();
      }

      // Color
      if(cloud->has_color){
        ImGui::Text("%f %f %f", RGB[i].x, RGB[i].y, RGB[i].z);
        ImGui::NextColumn();
      }

      //Normal
      if(cloud->has_normal){
        ImGui::Text("%f %f %f", N[i].x, N[i].y, N[i].z);
        ImGui::NextColumn();
      }

      // Timestamp
      if(cloud->has_timestamp){
        ImGui::Text("%f", ts[i]);
        ImGui::NextColumn();
      }
    }
    ImGui::Columns(1);
    ImGui::TreePop();

    //---------------------------
    ImGui::End();
  }
}*/

void MOD_cloud::mod_cloud_data(){
  if(modal_tab.show_asciiData){
    ImGui::Begin("Data", &modal_tab.show_asciiData);
    Collection* collection = sceneManager->get_selected_collection();
    Cloud* cloud = (Cloud*)collection->selected_obj;
    //---------------------------

    // Data vectors
    vector<vec3>& XYZ = cloud->xyz;
    vector<vec4>& RGB = cloud->rgb;
    vector<float>& Is = cloud->I;
    vector<vec3>& N = cloud->Nxyz;
    vector<float>& ts = cloud->ts;

    //Settings
    static int nbLines = 100;
    if(ImGui::SliderInt("Number of lines", &nbLines, 1, cloud->nb_point)){
      if(nbLines > XYZ.size()) nbLines = XYZ.size();
    }
    ImGui::Separator();

    // Get number of attributs
    int nb_column = 2;
    if(cloud->has_intensity) nb_column++;
    if(cloud->has_color) nb_column++;
    if(cloud->has_normal) nb_column++;
    if(cloud->has_timestamp) nb_column++;

    ImGuiTableFlags flag = ImGuiTableFlags_Resizable | ImGuiTableColumnFlags_WidthStretch;
    if (ImGui::BeginTable("table_data", nb_column, flag)){
      // Header
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("#");
      ImGui::TableNextColumn();
      ImGui::Text("XYZ");
      ImGui::TableNextColumn();
      if(cloud->has_intensity){
        ImGui::Text("I");
        ImGui::TableNextColumn();
      }
      if(cloud->has_color){
        ImGui::Text("RGB");
        ImGui::TableNextColumn();
      }
      if(cloud->has_normal){
        ImGui::Text("Nxyz");
        ImGui::TableNextColumn();
      }
      if(cloud->has_timestamp){
        ImGui::Text("ts");
        ImGui::TableNextColumn();
      }

      // Data
      for(int i=0; i<nbLines; i++){
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        // Line number
        ImGui::TextColored(ImVec4(0.4f,0.9f,0.4f,1.0f),"%i", i);
        ImGui::TableNextColumn();

        // XYZ coordinate
        ImGui::Text("%f %f %f", XYZ[i].x, XYZ[i].y, XYZ[i].z);
        ImGui::TableNextColumn();

        // Intensity
        if(cloud->has_intensity){
          ImGui::Text("%f", Is[i]);
          ImGui::TableNextColumn();
        }

        // Color
        if(cloud->has_color){
          ImGui::Text("%f %f %f", RGB[i].x, RGB[i].y, RGB[i].z);
          ImGui::TableNextColumn();
        }

        //Normal
        if(cloud->has_normal){
          ImGui::Text("%f %f %f", N[i].x, N[i].y, N[i].z);
          ImGui::TableNextColumn();
        }

        // Timestamp
        if(cloud->has_timestamp){
          ImGui::Text("%f", ts[i]);
          ImGui::TableNextColumn();
        }
      }
      ImGui::EndTable();
    }
  }
}
