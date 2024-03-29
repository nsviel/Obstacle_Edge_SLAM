#ifndef VISIBILITY_H
#define VISIBILITY_H

#include "../../common.h"

class Node_operation;
class Scene;
class Onthefly;


class Visibility
{
public:
  //Constructor / Destructor
  Visibility(Node_operation* node_ope);
  ~Visibility();

public:
  void compute_visibility(Collection* collection, int& ID_object);
  void compute_visibility_range_max(Collection* collection);

  inline int* get_visibility_range(){return &visibility_range;}
  inline int get_visibility_range_max(){return visibility_range_max;}

private:
  Scene* sceneManager;
  Onthefly* flyManager;

  int visibility_range;
  int visibility_range_max;
};

#endif
