#pragma once

template<typename NODE>
class Tree {
  using node_t = NODE;
  using type_t = typename node_t::type_t;

private:
  node_t* root;
public:
  Tree() {

  }

  void insert(type_t&& value) {

  }
};