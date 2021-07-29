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
  using subnode_t = RTreeSubNode<limit>;
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

  auto getPolygon() -> type_t {
    if(value.has_value()) {
      return value.value();
    } else {
      type_t p;
      bg::convert(next->box, p);
      return p;
    }
  }

  auto operator=(subnode_t &b) -> void {
    next = b.next;
    value = b.value;
    b.next = nullptr;
    value = nullopt;
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
  vector<subnode_t*> *subNodes;
  bool is_leaf;

public:
  RTreeNode(type_t&& value): is_leaf(true) {
    subNodes = new vector<subnode_t*>;
    bg::envelope(value, this->box);
    subNodes->push_back(new subnode_t(move(value)));
  }

  RTreeNode(): is_leaf(true), subNodes(new vector<subnode_t*>) {}

  ~RTreeNode() {
    for(auto nd : *subNodes) {
      delete nd;
    }
    delete subNodes;
  }

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
      subNodes->push_back(new subnode_t(move(value)));
    } else {

    }
    if(subNodes->size() < limit) {
      return pair(Status::OK, nullptr);
    }
    return pair(Status::NEEDSBALANCE, this);
  }

  auto root_balance() -> void {
    subnode_t* s1 = new subnode_t();
    subnode_t* s2 = new subnode_t();
    s1->next = new node_t();
    s2->next = new node_t();
    double maxd = -1;
    int n1, n2;
    for(int i = 0; i < subNodes->size(); i++) {
      for(int j = (i+1); j < subNodes->size(); j++) {
        double dis = bg::distance(subNodes->at(i)->getPolygon(), subNodes->at(j)->getPolygon());
        if(dis > maxd) {
          maxd = dis;
          n1 = i;
          n2 = j;
        }
      }
    }

    int c = 0;
     
    for(int i = 0; i < subNodes->size(); i++) {
      if(i != n1 and i != n2) {
        if(c == 0) {
          double d1 = bg::distance(subNodes->at(n1)->getPolygon(), subNodes->at(i)->getPolygon());
          double d2 = bg::distance(subNodes->at(n1)->getPolygon(), subNodes->at(i)->getPolygon());
          if(d1 > d2) {
            s2->next->push_node(subNodes->at(i));
            c = -1;
          } else {
            s1->next->push_node(subNodes->at(i));
            c = 1;
          }
        } else if(c == 1) {
          s2->next->push_node(subNodes->at(i));
          c = 0;
        } else {
          s1->next->push_node(subNodes->at(i));
          c = 0;
        }
      }
    }

    s1->next->push_node(subNodes->at(n1));
    s2->next->push_node(subNodes->at(n2));

    delete subNodes;
    subNodes = new vector<subnode_t*>;

    push_node(s1);
    push_node(s2);
  }

  auto toString(string opt = "") -> string {
    stringstream t;
    t << bg::dsv(box);
    string value = opt + t.str();
    value += " {\n";
    for(auto sub : *subNodes) {
      if (sub->value.has_value()) {
        value += opt + "\t";
        stringstream s;
        s << bg::dsv(sub->value.value());
        value += s.str();
        value += "\n";
      } else {
        if(sub->next) {
          value += sub->next->toString(opt + "\t");
        }
      }
    }
    value += opt + "}\n";
    return value;
  }

  auto push_node(subnode_t* next) -> void {
    if(next == nullptr) {return;}
    box_t b;
    bg::envelope(next->getPolygon(), b);
    bg::set<bg::min_corner, 0>(box, min(bg::get<bg::min_corner, 0>(box), bg::get<bg::min_corner, 0>(b)));
    bg::set<bg::min_corner, 1>(box, min(bg::get<bg::min_corner, 1>(box), bg::get<bg::min_corner, 1>(b)));
    bg::set<bg::max_corner, 0>(box, max(bg::get<bg::max_corner, 0>(box), bg::get<bg::max_corner, 0>(b)));
    bg::set<bg::max_corner, 1>(box, max(bg::get<bg::max_corner, 1>(box), bg::get<bg::max_corner, 1>(b)));
    subNodes->push_back(next);
  }
};
