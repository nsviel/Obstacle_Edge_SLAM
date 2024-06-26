#include "OOBB.h"


//Constructor / destructor
OOBB::OOBB(){
  //---------------------------

  this->name = "oobb";
  this->color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  this->width = 3;

  //---------------------------
  this->create_glyph();
}
OOBB::~OOBB(){}

void OOBB::create_glyph(){
  this->glyph = new Glyph();
  //---------------------------

  //Create glyph
  glyph->name = "OOBB";
  glyph->draw_line_width = width;
  glyph->is_visible = false;
  glyph->draw_type_name = "line";
  glyph->is_permanent = true;
  glyph->unicolor = color;

  //Box color
  for(int i=0; i<24; i++){
    glyph->rgb.push_back(color);
  }

  //Arrow color
  for(int i=0; i<6; i++){
    glyph->rgb.push_back(vec4(0,0,1,1));
  }

  //---------------------------
}
void OOBB::update_glyph(Glyph* glyph, mat4 transformation){
  vec3 min = vec3(-0.5f, -0.5f, -0.5f);
  vec3 max = vec3(0.5f, 0.5f, 0.5f);
  //---------------------------

  //location
  vector<vec3> box = build_box(min, max);
  for(int i=0; i<box.size(); i++){
    vec4 point = vec4(box[i].x, box[i].y, box[i].z, 1.0f);
    point = point * transformation;
    box[i] = vec3(point.x, point.y, point.z);
  }

  glyph->xyz = box;


  //---------------------------
}
vector<vec3> OOBB::build_box(vec3 min, vec3 max){
  vector<vec3> XYZ;
  vec3 l1, l2;
  //---------------------------

  for(int i=0; i<3; i++){
    l1=min;
    l2=min;
    l2[i]=max[i];
    XYZ.push_back(l1);
    XYZ.push_back(l2);

    l1=max;
    l2=max;
    l2[i]=min[i];
    XYZ.push_back(l1);
    XYZ.push_back(l2);
  }
  for(int i=0; i<2; i++){
    l1=min;
    l1.z=max.z;
    l2=min;
    l2.z=max.z;
    l2[i]=max[i];
    XYZ.push_back(l1);
    XYZ.push_back(l2);

    l1=max;
    l1.z=min.z;
    l2=max;
    l2.z=min.z;
    l2[i]=min[i];
    XYZ.push_back(l1);
    XYZ.push_back(l2);

    l1=min;
    l1[i]=max[i];
    l2=l1;
    l2.z=max.z;
    XYZ.push_back(l1);
    XYZ.push_back(l2);
  }

  //Arrow direction
  l1 = vec3(2*max.x*3/4 - max.x, -max.y, max.z);
  l2 = vec3(2*max.x*3/4 - max.x, max.y, max.z);
  XYZ.push_back(l1);
  XYZ.push_back(l2);

  //---------------------------
  return XYZ;
}
