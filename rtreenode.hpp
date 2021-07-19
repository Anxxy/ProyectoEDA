#pragma once
#include "./macros.hpp"

template<int limit>
class RTreeNode;

template<int limit = 4>
class RTreeSubNode {
  using point = model::point<double, 2, cs::cartesian>;
  using polygon = model::polygon<point>;
  using type_t = polygon;
  using node_t = RTreeNode<limit>;
private:
  node_t* next;
  type_t* value;
public:
  RTreeSubNode(): next(nullptr), value(nullptr) {}

  }
};

template<int limit = 4>
class RTreeNode {
  template<typename NODE>
  friend class Tree;

  using point = model::point<double, 2, cs::cartesian>;
  using polygon = model::polygon<point>;
  using type_t = polygon;
  using node_t = RTreeNode<limit>;
  using subnode_t = RTreeSubNode<limit>;

private:
  polygon box;
  vector<subnode_t> subNodes;
public:

  RTreeNode(type_t&& value) {

  }
};
