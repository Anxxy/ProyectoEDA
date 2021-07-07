#pragma once
#include "./macros.hpp"

template<int limit = 4>
class RTreeNode {
  template<typename NODE>
  friend class Tree;

  using point = model::point<double, 2, cs::cartesian>;
  using polygon = model::polygon<point>;
  using type_t = polygon;
  using node_t = RTreeNode<limit>;

private:
public:
};