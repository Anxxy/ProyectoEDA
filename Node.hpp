#pragma once

int M = 4;

//-------------------------------//

template <typename T>
struct Point
{
  T x, y;
}

//-------------------------------//

template <typename Point>
struct Node_Route 
{
   Point source;
   Point destiny;
   list<Node_Route<T>*> children;
   bool is_leaf;
   int max_children;
}

template <typename Point>
class Tree_Route
{
private:
  int max_children = M;
  Node_Route<Point>* root; 
public:
  Tree_Route(int _max_children = M) : max_children(_max_children);
  ~Tree_Route();
}

//-------------------------------//

template <typename Point>
struct Node_Neighborhood 
{
   list<Point> geometry;
   list<Node_Neighborhood<Point>*> children;
   bool is_leaf;
   int max_children;
   Tree_Route<Point> routes;
}

template <typename Point>
class Tree_Neighborhood
{
private:
  int max_children = M;
  Node_Neighborhood<Point>* root = nullptr; 
public:
  Tree_Neighborhood(int _max_children = M) : max_children(_max_children);
  ~Tree_Neighborhood();
  void Insert(list<Point> geometry)
  {
    if(!root)
    {
      root = new Node_Neighborhood<Point>();
      root->geometry = geometry;
      root->max_children = M;
      root->is_leaf = true;
    }
  }
}
