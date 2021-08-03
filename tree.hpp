#pragma once
#include "./macros.hpp"

template<typename NODE>
class Tree {
  using node_t = NODE;
  using point_t = typename node_t::point_t;
  using box_t = typename node_t::box_t;
  using type_t = typename node_t::type_t;

private:
  node_t* root;

public:
  Tree(): root(nullptr) {}

  ~Tree() {
    if(root) 
      delete root;
  }

  auto insert(string id, type_t&& value) -> void {
    if(root) {
      root->insert(id, move(value));
      return;
    }
    root = new node_t(id, move(value));
  }

  auto insert(string id, type_t& value) -> void {
    this->insert(id, move(value));
  }

  auto insert_route(string id, type_t value) -> void {
    if(root) {
      root->insert_route(id, value);
      return;
    }
  }

  auto getRoot() -> node_t* {
    return root;
  }

  template<typename ROUTE>
  auto routesInTheNeighborhood() -> vector<pair<string,Tree<ROUTE>*>> {
    return root->routesInTheNeighborhood();
  }

  auto topNeighborhoodWithMoreRoutes() -> vector<pair<string,int>> {    
    auto result = root->topNeighborhoodWithMoreRoutes();
    
    sort(result.begin(), result.end(), [](auto &left, auto &right) {
      return left.second < right.second;
    });

    vector<pair<string,int>> result_finish;
    for(int i = 0; i < 5; i++){
      result_finish.push_back(result.back());
      result.pop_back();
    }
    
    return result_finish;
    
  }

  auto rangeRoutes(box_t box) -> int {
    bg::correct(box);
    auto result = root->rangeRoutes(box);
    return result.size();
  }

  auto rangeRoutesCircle(double x, double y, double r) -> int {
    auto result = root->rangeRoutesCircle(x, y, r);
    return result.size();
  }

};

