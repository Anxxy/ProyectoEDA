#pragma once
#include "./macros.hpp"

template<typename NODE>
class Tree {
  using node_t = NODE;
  using type_t = typename node_t::type_t;

private:
  node_t* root;
public:
  Tree(): root(nullptr) {

  }

  ~Tree() {
    if(root) 
      delete root;
  }

  auto insert(type_t&& value) -> void {
    if(root) {
      root->insert(move(value));
      return;
    }
    root = new node_t(move(value));
  }

  auto insert(type_t& value) -> void {
    this->insert(move(value));
  }

  auto getRoot() -> node_t* {
    return root;
  }
};
