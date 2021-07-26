#pragma once
#include "./macros.hpp"

enum class Status {
  OK,
  NEEDSBALANCE
};

template<int limit>
class RTreeNode;

template<int limit = 4>
class RTreeSubNode {
  template<int>
  friend class RTreeNode;

  using node_t = RTreeNode<limit>;
  using type_t = typename node_t::type_t;

private:
  node_t* next;
  optional<type_t> value;

public:
  RTreeSubNode(): next(nullptr), value(nullopt) {}
  RTreeSubNode(type_t&& value): next(nullptr), value(move(value)) {}

  ~RTreeSubNode() {
    if(next)
      delete next;
  }

  auto setValue(type_t&& value) -> void {
    this->value = value;
  }
};

template<int limit = 4>
class RTreeNode {
  template<typename NODE>
  friend class Tree;

  template<int>
  friend class RTreeSubNode;

  using point_t = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon_t = bg::model::polygon<point_t>;
  using box_t = bg::model::box<point_t>;
  using type_t = polygon_t;
  using node_t = RTreeNode<limit>;
  using subnode_t = RTreeSubNode<limit>;

private:
  box_t box;
  vector<subnode_t> subNodes;
  bool is_leaf;

public:
  RTreeNode(type_t&& value): is_leaf(true) {
    bg::envelope(value, this->box);
    subNodes.push_back(subnode_t(move(value)));
  }

  RTreeNode(): is_leaf(true) {}

  auto insert(type_t&& value) -> void {
    Status status = this->add(move(value)).first;
    switch(status) {
      case Status::OK:
        return;
      case Status::NEEDSBALANCE:
        this->root_balance();
        break;
    }
  }

  auto add(type_t&& value) -> pair<Status, node_t*> {
    box_t b;
    bg::envelope(value, b);
    bg::set<bg::min_corner, 0>(box, min(bg::get<bg::min_corner, 0>(box), bg::get<bg::min_corner, 0>(b)));
    bg::set<bg::min_corner, 1>(box, min(bg::get<bg::min_corner, 1>(box), bg::get<bg::min_corner, 1>(b)));
    bg::set<bg::max_corner, 0>(box, max(bg::get<bg::max_corner, 0>(box), bg::get<bg::max_corner, 0>(b)));
    bg::set<bg::max_corner, 1>(box, max(bg::get<bg::max_corner, 1>(box), bg::get<bg::max_corner, 1>(b)));
    if(this->is_leaf) {
      subNodes.push_back(subnode_t(move(value)));
    } else {

    }
    if(subNodes.size() < limit) {
      return pair(Status::OK, nullptr);
    }
    return pair(Status::NEEDSBALANCE, this);
  }

  auto root_balance() -> void {
    subnode_t s1;
    subnode_t s2;
    s1.next = new node_t();
    s2.next = new node_t();
  }

  auto toString(string opt = "") -> string {
    stringstream t;
    t << bg::dsv(box);
    string value = opt + t.str();
    value += " {\n";
    for(auto sub : subNodes) {
      if (sub.value.has_value()) {
        value += opt + "\t";
        stringstream s;
        s << bg::dsv(sub.value.value());
        value += s.str();
        value += "\n";
      } else {
        if(sub.next) {
          value += sub.next->toString(opt + "\t");
        }
      }
    }
    value += opt + "}\n";
    return value;
  }
};
