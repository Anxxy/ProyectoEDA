#pragma once
#include "./macros.hpp"
#define DELBYV(s, c) for (auto itr = s.begin(); itr != s.end(); itr++){ if(*itr == c){s.erase(itr); break;} }

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
      subnode_t* next;
      double mind = numeric_limits<double>::max();
      for(auto nd : *subNodes) {
        double d = bg::distance(nd->getPolygon(), value);
        if(d < mind) {
          mind = d;
          next = nd;
        }
      }
      pair<Status, node_t*> res = next->next->add(move(value));
      switch(res.first) {
        case Status::OK:
          break;
        case Status::NEEDSBALANCE:
          this->balance(next);
          break;
      }
    }
    if(subNodes->size() < limit) {
      return pair(Status::OK, nullptr);
    }
    return pair(Status::NEEDSBALANCE, this);
  }

  auto root_balance() -> void {
    is_leaf = false;
    subnode_t* s1 = new subnode_t();
    subnode_t* s2 = new subnode_t();
    s1->next = new node_t();
    s2->next = new node_t();

    map<string, double> mem;
    
    function<double(vector<subnode_t*>, set<int>, set<int>, int)> partition;

    partition = [&mem, &partition](vector<subnode_t*> nodes, set<int> P1, set<int> P2, int i) -> double {
      if(P1.size() >= (limit/2) and P2.size() >= (limit/2)) {
        double a1 = bg::area(setSubnodesToBox(nodes, P1));
        // cout << "A1: " << setToString(P1) << " - " << a1  << endl;
        double a2 = bg::area(setSubnodesToBox(nodes, P2));
        // cout << "A2: " << setToString(P2) << " - " << a2  << endl;
        return max(a1, a2);
      }
      
      if(P1.size() >= (limit/2)) {
        P2.insert(i);
        return partition(nodes, P1, P2, i+1);
      }

      if(P2.size() >= (limit/2)) {
        P1.insert(i);
        return partition(nodes, P1, P2, i+1);
      }

      double v1 = 0, v2 = 0;
      
      P1.insert(i);

      string key1 = setToString(P1);

      if(mem.find(key1) != mem.end()) {
        v1 = mem[key1];
      } else {
        v1 = partition(nodes, P1, P2, i+1);
        mem[key1] = v1;
      }

      DELBYV(P1, i)

      P2.insert(i);

      string key2 = setToString(P2);

      if(mem.find(key2) != mem.end()) {
        v2 = mem[key2];
      } else {
        v2 = partition(nodes, P1, P2, i+1);
        mem[key2] = v2;
      }
      
      return min(v1, v2);
    };
    
    double minArea = partition(*(this->subNodes), set<int>(), set<int>(), 0);
    
    // cout << "minArea: " << minArea << endl;
    
    // cout << "BOX: " << bg::wkt(setSubnodesToBox(*(this->subNodes), set<int>{0, 3})) << endl;
    // cout << "BOX: " << bg::area(setSubnodesToBox(*(this->subNodes), set<int>{0, 3})) << endl;
    
    string ndstring = "";

    for(auto p : mem) {
      if(p.second == minArea) {
        if(ndstring == "") {
          ndstring = p.first;
        } else {
          if(ndstring.length() < p.first.length()) {
            ndstring = p.first;
          }
        }
      }
    }
    
    vector<string> str_indexes;
    boost::split(str_indexes, ndstring, [](char c) { return c=='|';});

    vector<int> indexes;
    for(auto str : str_indexes) {
      indexes.push_back(stoi(str));
    }
    
    int a1 = 0;
    for(int i = 0; i < indexes.size(); i++) {
      int a2 = indexes[i];
      if(a1 == a2) {
        s2->next->push_node(subNodes->at(a2));
      } else {
        s1->next->push_node(subNodes->at(a1));
        i--;
      }
      a1++;
    }
    
    for(int i = a1; i < subNodes->size(); i++) {
        s1->next->push_node(subNodes->at(i));
    }
    
    subNodes->clear();

    this->push_node(s1);
    this->push_node(s2);
  }
  
  auto balance(subnode_t* s1) -> void {
    is_leaf = false;
    subnode_t* s2 = new subnode_t();
    s2->next = new node_t();

    vector<subnode_t*>* nodes = s1->next->subNodes;
    s1->next->subNodes = new vector<subnode_t*>;

    map<string, double> mem;

    function<double(vector<subnode_t*>, set<int>, set<int>, int)> partition;

    partition = [&mem, &partition](vector<subnode_t*> nodes, set<int> P1, set<int> P2, int i) -> double {
      if(P1.size() >= (limit/2) and P2.size() >= (limit/2)) {
        double a1 = bg::area(setSubnodesToBox(nodes, P1));
        // cout << "A1: " << setToString(P1) << " - " << a1  << endl;
        double a2 = bg::area(setSubnodesToBox(nodes, P2));
        // cout << "A2: " << setToString(P2) << " - " << a2  << endl;
        return max(a1, a2);
      }
      
      if(P1.size() >= (limit/2)) {
        P2.insert(i);
        return partition(nodes, P1, P2, i+1);
      }

      if(P2.size() >= (limit/2)) {
        P1.insert(i);
        return partition(nodes, P1, P2, i+1);
      }

      double v1 = 0, v2 = 0;
      
      P1.insert(i);

      string key1 = setToString(P1);

      if(mem.find(key1) != mem.end()) {
        v1 = mem[key1];
      } else {
        v1 = partition(nodes, P1, P2, i+1);
        mem[key1] = v1;
      }

      DELBYV(P1, i)

      P2.insert(i);

      string key2 = setToString(P2);

      if(mem.find(key2) != mem.end()) {
        v2 = mem[key2];
      } else {
        v2 = partition(nodes, P1, P2, i+1);
        mem[key2] = v2;
      }
      
      return min(v1, v2);
    };

    double minArea = partition(*nodes, set<int>(), set<int>(), 0);

    string ndstring = "";

    for(auto p : mem) {
      if(p.second == minArea) {
        if(ndstring == "") {
          ndstring = p.first;
        } else {
          if(ndstring.length() < p.first.length()) {
            ndstring = p.first;
          }
        }
      }
    }

    vector<string> str_indexes;
    boost::split(str_indexes, ndstring, [](char c) { return c=='|';});

    vector<int> indexes;
    for(auto str : str_indexes) {
      indexes.push_back(stoi(str));
    }

    int a1 = 0;
    for(int i = 0; i < indexes.size(); i++) {
      int a2 = indexes[i];
      if(a1 == a2) {
        s2->next->push_node(nodes->at(a2));
      } else {
        s1->next->push_node(nodes->at(a1));
        i--;
      }
      a1++;
    }
    
    for(int i = a1; i < nodes->size(); i++) {
        s1->next->push_node(nodes->at(i));
    }

    this->push_node(s2);
    delete nodes;
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
    if(subNodes->size() == 0) {
      bg::envelope(next->getPolygon(), box);
      subNodes->push_back(next);
      return;
    }
    box_t b;
    bg::envelope(next->getPolygon(), b);
    bg::set<bg::min_corner, 0>(box, min(bg::get<bg::min_corner, 0>(box), bg::get<bg::min_corner, 0>(b)));
    bg::set<bg::min_corner, 1>(box, min(bg::get<bg::min_corner, 1>(box), bg::get<bg::min_corner, 1>(b)));
    bg::set<bg::max_corner, 0>(box, max(bg::get<bg::max_corner, 0>(box), bg::get<bg::max_corner, 0>(b)));
    bg::set<bg::max_corner, 1>(box, max(bg::get<bg::max_corner, 1>(box), bg::get<bg::max_corner, 1>(b)));
    subNodes->push_back(next);
  }

  static auto setToString(set<int> v) -> string {
    string result = "";
    set<int>::iterator itr;
    for (itr = v.begin(); itr != v.end(); itr++) {
      if(itr != v.begin()) {
        result += "|";
      }
      result += to_string(*itr);
    }
    return result;
  }

  static auto setSubnodesToBox(vector<subnode_t*> v, set<int> s) -> box_t {
    box_t result;
    set<int>::iterator itr = s.begin();

    bg::envelope(v[*itr]->getPolygon(), result);
    itr++;
    for (; itr != s.end(); itr++) {
      int i = *itr;
      box_t b;
      bg::envelope(v[i]->getPolygon(), b);
      
      bg::set<bg::min_corner, 0>(result, min(bg::get<bg::min_corner, 0>(result), bg::get<bg::min_corner, 0>(b)));
      bg::set<bg::min_corner, 1>(result, min(bg::get<bg::min_corner, 1>(result), bg::get<bg::min_corner, 1>(b)));
      bg::set<bg::max_corner, 0>(result, max(bg::get<bg::max_corner, 0>(result), bg::get<bg::max_corner, 0>(b)));
      bg::set<bg::max_corner, 1>(result, max(bg::get<bg::max_corner, 1>(result), bg::get<bg::max_corner, 1>(b)));
    }
    return result;
  }
};

#undef DELBYV