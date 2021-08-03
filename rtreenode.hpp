#pragma once
#include "./macros.hpp"
#include "./tree.hpp"
#include "./rtreenoderoutes.hpp"
#define DELBYV(s, c) for (auto itr = s.begin(); itr != s.end(); itr++){ if(*itr == c){s.erase(itr); break;} }


template <int limit> class RTreeNode;

template <int limit = 4> class RTreeSubNode {
  template <int> friend class RTreeNode;

  using node_t = RTreeNode<limit>;
  using subnode_t = RTreeSubNode<limit>;
  using type_t = typename node_t::type_t;

private:
  string id;
  optional<type_t> value;

public:
  node_t* next;
  Tree<RTreeNodeRoutes<>>* routes; 
  
  RTreeSubNode(): id(""), next(nullptr), value(nullopt), routes(nullptr) {}
  RTreeSubNode(string id, type_t&& value): id(id), next(nullptr), value(move(value)), routes(nullptr) {}

  ~RTreeSubNode() {
    if (next)
      delete next;
    if(routes)
      delete routes;
  }

  auto setId(string id) -> void {
    this->id = id;
  }

  auto setValue(type_t &&value) -> void { this->value = value; }

  auto getId() -> string {
    return this->id;
  }

  auto getPolygon() -> type_t {
    if (value.has_value()) {
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

template <int limit = 4> class RTreeNode {
  template <typename NODE> friend class Tree;

  template <int> friend class RTreeSubNode;

  using point_t = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon_t = bg::model::polygon<point_t>;
  using box_t = bg::model::box<point_t>;
  using type_t = polygon_t;
  using node_t = RTreeNode<limit>;
  using subnode_t = RTreeSubNode<limit>;

private:
  bool is_leaf;

public:
  box_t box;
  vector<subnode_t*> *subNodes;
  Tree<RTreeNodeRoutes<>>* routes = nullptr; 

  RTreeNode(string id, type_t&& value): is_leaf(true) {
    subNodes = new vector<subnode_t*>;
    bg::envelope(value, this->box);
    subNodes->push_back(new subnode_t(id, move(value)));
  }

  RTreeNode() : is_leaf(true), subNodes(new vector<subnode_t *>) {}

  ~RTreeNode() {
    for (auto nd : *subNodes) {
      delete nd;
    }
    delete subNodes;
    delete routes;
  }

  auto insert(string id, type_t&& value) -> void {
    Status status = this->add(id, move(value)).first;
    switch(status) {
      case Status::OK:
        return;
      case Status::NEEDSBALANCE:
        this->root_balance();
        break;
    }
  }

  auto insert_route(string id, type_t value) -> void {
    box_t b;
    bg::envelope(value, b);
    type_t _value = value;
    point_t p;
    if(bg::within(b, this->box)){
      if(!routes) routes = new Tree<RTreeNodeRoutes<>>;
      routes->insert(id, value);
      for(auto sub : *subNodes) {
        if (sub->value.has_value()){
          type_t _value_2 = _value;
          if(bg::covered_by(_value, sub->getPolygon())){
            if(!sub->routes) sub->routes = new Tree<RTreeNodeRoutes<>>;
            sub->routes->insert(id, _value);
            return;
          }
        } else {
          if(sub->next) {
            sub->next->insert_route(id, _value);
          }
        }
      }
    }
  }

  auto add(string id, type_t&& value) -> pair<Status, node_t*> {
    box_t b;
    bg::envelope(value, b);
    bg::set<bg::min_corner, 0>(box, min(bg::get<bg::min_corner, 0>(box), bg::get<bg::min_corner, 0>(b)));
    bg::set<bg::min_corner, 1>(box, min(bg::get<bg::min_corner, 1>(box), bg::get<bg::min_corner, 1>(b)));
    bg::set<bg::max_corner, 0>(box, max(bg::get<bg::max_corner, 0>(box), bg::get<bg::max_corner, 0>(b)));
    bg::set<bg::max_corner, 1>(box, max(bg::get<bg::max_corner, 1>(box), bg::get<bg::max_corner, 1>(b)));
    if(this->is_leaf) {
      subNodes->push_back(new subnode_t(id, move(value)));
    } else {
      subnode_t *next;
      double mind = numeric_limits<double>::max();
      for (auto nd : *subNodes) {
        double d = bg::distance(nd->getPolygon(), value);
        if (d < mind) {
          mind = d;
          next = nd;
        }
      }
      pair<Status, node_t*> res = next->next->add(id, move(value));
      switch(res.first) {
        case Status::OK:
          break;
        case Status::NEEDSBALANCE:
          this->balance(next);
          break;
      }
    }
    if (subNodes->size() < limit) {
      return pair(Status::OK, nullptr);
    }
    return pair(Status::NEEDSBALANCE, this);
  }

  auto root_balance() -> void {
    is_leaf = false;
    subnode_t *s1 = new subnode_t();
    subnode_t *s2 = new subnode_t();
    s1->next = new node_t();
    s2->next = new node_t();

    map<string, double> mem;

    function<double(vector<subnode_t *>, set<int>, set<int>, int)> partition;

    partition = [&mem, &partition](vector<subnode_t *> nodes, set<int> P1,
                                   set<int> P2, int i) -> double {
      if (P1.size() >= (limit / 2) and P2.size() >= (limit / 2)) {
        double a1 = bg::area(setSubnodesToBox(nodes, P1));
        // cout << "A1: " << setToString(P1) << " - " << a1  << endl;
        double a2 = bg::area(setSubnodesToBox(nodes, P2));
        // cout << "A2: " << setToString(P2) << " - " << a2  << endl;
        return max(a1, a2);
      }

      if (P1.size() >= (limit / 2)) {
        P2.insert(i);
        return partition(nodes, P1, P2, i + 1);
      }

      if (P2.size() >= (limit / 2)) {
        P1.insert(i);
        return partition(nodes, P1, P2, i + 1);
      }

      double v1 = 0, v2 = 0;

      P1.insert(i);

      string key1 = setToString(P1);

      if (mem.find(key1) != mem.end()) {
        v1 = mem[key1];
      } else {
        v1 = partition(nodes, P1, P2, i + 1);
        mem[key1] = v1;
      }

      DELBYV(P1, i)

      P2.insert(i);

      string key2 = setToString(P2);

      if (mem.find(key2) != mem.end()) {
        v2 = mem[key2];
      } else {
        v2 = partition(nodes, P1, P2, i + 1);
        mem[key2] = v2;
      }

      return min(v1, v2);
    };

    double minArea = partition(*(this->subNodes), set<int>(), set<int>(), 0);

    // cout << "minArea: " << minArea << endl;

    // cout << "BOX: " << bg::wkt(setSubnodesToBox(*(this->subNodes),
    // set<int>{0, 3})) << endl; cout << "BOX: " <<
    // bg::area(setSubnodesToBox(*(this->subNodes), set<int>{0, 3})) << endl;

    string ndstring = "";

    for (auto p : mem) {
      if (p.second == minArea) {
        if (ndstring == "") {
          ndstring = p.first;
        } else {
          if (ndstring.length() < p.first.length()) {
            ndstring = p.first;
          }
        }
      }
    }

    vector<string> str_indexes;
    boost::split(str_indexes, ndstring, [](char c) { return c == '|'; });

    vector<int> indexes;
    for (auto str : str_indexes) {
      indexes.push_back(stoi(str));
    }

    int a1 = 0;
    for (int i = 0; i < indexes.size(); i++) {
      int a2 = indexes[i];
      if (a1 == a2) {
        s2->next->push_node(subNodes->at(a2));
      } else {
        s1->next->push_node(subNodes->at(a1));
        i--;
      }
      a1++;
    }

    for (int i = a1; i < subNodes->size(); i++) {
      s1->next->push_node(subNodes->at(i));
    }

    subNodes->clear();

    this->push_node(s1);
    this->push_node(s2);
  }

  auto balance(subnode_t *s1) -> void {
    is_leaf = false;
    subnode_t *s2 = new subnode_t();
    s2->next = new node_t();

    vector<subnode_t *> *nodes = s1->next->subNodes;
    s1->next->subNodes = new vector<subnode_t *>;

    map<string, double> mem;

    function<double(vector<subnode_t *>, set<int>, set<int>, int)> partition;

    partition = [&mem, &partition](vector<subnode_t *> nodes, set<int> P1,
                                   set<int> P2, int i) -> double {
      if (P1.size() >= (limit / 2) and P2.size() >= (limit / 2)) {
        double a1 = bg::area(setSubnodesToBox(nodes, P1));
        // cout << "A1: " << setToString(P1) << " - " << a1  << endl;
        double a2 = bg::area(setSubnodesToBox(nodes, P2));
        // cout << "A2: " << setToString(P2) << " - " << a2  << endl;
        return max(a1, a2);
      }

      if (P1.size() >= (limit / 2)) {
        P2.insert(i);
        return partition(nodes, P1, P2, i + 1);
      }

      if (P2.size() >= (limit / 2)) {
        P1.insert(i);
        return partition(nodes, P1, P2, i + 1);
      }

      double v1 = 0, v2 = 0;

      P1.insert(i);

      string key1 = setToString(P1);

      if (mem.find(key1) != mem.end()) {
        v1 = mem[key1];
      } else {
        v1 = partition(nodes, P1, P2, i + 1);
        mem[key1] = v1;
      }

      DELBYV(P1, i)

      P2.insert(i);

      string key2 = setToString(P2);

      if (mem.find(key2) != mem.end()) {
        v2 = mem[key2];
      } else {
        v2 = partition(nodes, P1, P2, i + 1);
        mem[key2] = v2;
      }

      return min(v1, v2);
    };

    double minArea = partition(*nodes, set<int>(), set<int>(), 0);

    string ndstring = "";

    for (auto p : mem) {
      if (p.second == minArea) {
        if (ndstring == "") {
          ndstring = p.first;
        } else {
          if (ndstring.length() < p.first.length()) {
            ndstring = p.first;
          }
        }
      }
    }

    vector<string> str_indexes;
    boost::split(str_indexes, ndstring, [](char c) { return c == '|'; });

    vector<int> indexes;
    for (auto str : str_indexes) {
      indexes.push_back(stoi(str));
    }

    int a1 = 0;
    for (int i = 0; i < indexes.size(); i++) {
      int a2 = indexes[i];
      if (a1 == a2) {
        s2->next->push_node(nodes->at(a2));
      } else {
        s1->next->push_node(nodes->at(a1));
        i--;
      }
      a1++;
    }

    for (int i = a1; i < nodes->size(); i++) {
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
    for (auto sub : *subNodes) {
      if (sub->value.has_value()) {
        value += opt + "\t";
        stringstream s;
        //s << bg::dsv(sub->value.value());
        s << sub->id;
        value += s.str();
        value += "\n";
      } else {
        if (sub->next) {
          value += sub->next->toString(opt + "\t");
        }
      }
    }
    value += opt + "}\n";
    return value;
  }

  auto push_node(subnode_t *next) -> void {
    if (next == nullptr) {
      return;
    }
    if (subNodes->size() == 0) {
      bg::envelope(next->getPolygon(), box);
      subNodes->push_back(next);
      return;
    }
    box_t b;
    bg::envelope(next->getPolygon(), b);
    bg::set<bg::min_corner, 0>(box, min(bg::get<bg::min_corner, 0>(box),
                                        bg::get<bg::min_corner, 0>(b)));
    bg::set<bg::min_corner, 1>(box, min(bg::get<bg::min_corner, 1>(box),
                                        bg::get<bg::min_corner, 1>(b)));
    bg::set<bg::max_corner, 0>(box, max(bg::get<bg::max_corner, 0>(box),
                                        bg::get<bg::max_corner, 0>(b)));
    bg::set<bg::max_corner, 1>(box, max(bg::get<bg::max_corner, 1>(box),
                                        bg::get<bg::max_corner, 1>(b)));
    subNodes->push_back(next);
  }

  static auto setToString(set<int> v) -> string {
    string result = "";
    set<int>::iterator itr;
    for (itr = v.begin(); itr != v.end(); itr++) {
      if (itr != v.begin()) {
        result += "|";
      }
      result += to_string(*itr);
    }
    return result;
  }

  static auto setSubnodesToBox(vector<subnode_t *> v, set<int> s) -> box_t {
    box_t result;
    set<int>::iterator itr = s.begin();

    bg::envelope(v[*itr]->getPolygon(), result);
    itr++;
    for (; itr != s.end(); itr++) {
      int i = *itr;
      box_t b;
      bg::envelope(v[i]->getPolygon(), b);

      bg::set<bg::min_corner, 0>(result, min(bg::get<bg::min_corner, 0>(result),
                                             bg::get<bg::min_corner, 0>(b)));
      bg::set<bg::min_corner, 1>(result, min(bg::get<bg::min_corner, 1>(result),
                                             bg::get<bg::min_corner, 1>(b)));
      bg::set<bg::max_corner, 0>(result, max(bg::get<bg::max_corner, 0>(result),
                                             bg::get<bg::max_corner, 0>(b)));
      bg::set<bg::max_corner, 1>(result, max(bg::get<bg::max_corner, 1>(result),
                                             bg::get<bg::max_corner, 1>(b)));
    }
    return result;
  }

  auto routesInTheNeighborhood() -> vector<pair<string,Tree<RTreeNodeRoutes<>>*>> {
    vector<pair<string,Tree<RTreeNodeRoutes<>>*>> result;
    for(auto sub : *subNodes) {
      if (sub->value.has_value()){
        if(sub->routes) result.push_back({sub->getId(),sub->routes});
      } else {
        if(sub->next) {
          auto result_temp =  sub->next->routesInTheNeighborhood();
          result.insert(result.end(),std::begin(result_temp),std::end(result_temp));
        }
      }
    }
    return result;
  }

  auto topNeighborhoodWithMoreRoutes() -> vector<pair<string,int>> {    
    vector<pair<string,int>> result;
    for(auto sub : *subNodes) {
      if (sub->value.has_value()){
        if(sub->routes) result.push_back({sub->getId(),sub->routes->getRoot()->numberOfRoutes()});
      } else {
        if(sub->next) {
          auto result_temp =  sub->next->topNeighborhoodWithMoreRoutes();
          result.insert(result.end(),std::begin(result_temp),std::end(result_temp));
        }
      }
    }

    return result;
  }

  auto rangeRoutes(box_t b) -> map<string, type_t> {
    map<string, type_t> result;
    if(bg::covered_by(this->box, b)){
      if(routes) result = routes->getRoot()->getRoutes();
    } else if(bg::intersects(this->box, b)){
      for(auto sub : *subNodes) {
        if (sub->value.has_value()){
          box_t b2;
          bg::envelope(sub->getPolygon(), b2);
          if(bg::covered_by(b2, b)){
            if(sub->routes) result = sub->routes->getRoot()->getRoutes();
          } else if (bg::intersects(b2, b)){
            if(sub->routes) {
              auto temp = sub->routes->getRoot()->rangeRoutes(b);
              result.insert(temp.begin(),temp.end());
            }  
          }
        } else {
          if(sub->next) {
            auto temp = sub->next->rangeRoutes(b);
            result.insert(temp.begin(),temp.end());
          }
        }
      }
    }
    return result;
  }

  auto rangeRoutesCircle(double x, double y, double r) -> map<string, type_t> {
    box_t b(point_t(x-r, y-r), point_t(x+r, y+r));
    map<string, type_t> result;
    if(bg::covered_by(this->box, b)){
      //cout << "covered" << endl;
      if(routes) result = routes->getRoot()->getRoutesCircle(x, y, r);
    } else if(bg::intersects(b, this->box)){
      //cout << "intersect" << endl;
      for(auto sub : *subNodes) {
        if (sub->value.has_value()){
          box_t b2;
          bg::envelope(sub->getPolygon(), b2);
          if(bg::covered_by(b2, b)){
            if(sub->routes) {
              auto temp = sub->routes->getRoot()->getRoutesCircle(x, y, r);
              result.insert(temp.begin(),temp.end());
            }  
          } else if (bg::intersects(b2, b)){
            if(sub->routes) {
              auto temp = sub->routes->getRoot()->rangeRoutesCircle(x, y, r);
              result.insert(temp.begin(),temp.end());
            }  
          }
        } else {
          if(sub->next) {
            auto temp = sub->next->rangeRoutesCircle(x, y, r);
            result.insert(temp.begin(),temp.end());
          }
        }
      }
    }
    return result;
  }


};

#undef DELBYV
