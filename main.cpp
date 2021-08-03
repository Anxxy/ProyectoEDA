#include "./tree.hpp"
#include "./rtreenode.hpp"
#include "./parser.hpp"

auto interface(Tree<RTreeNode<>> t) -> void {
  using point = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon = bg::model::polygon<point>;
  using box = bg::model::box<point>;

  while(true){
    int value;
    string id;
    cout << "1. Consulta 1" << endl;
    cout << "2. Consulta 2" << endl;
    cout << "3. Consulta 3" << endl;
    cout << "4. Consulta 5" << endl;
    cout << "5. Exit" << endl;
    
    cin >> value;

    if(value == 1){
      auto result = t.routesInTheNeighborhood<RTreeNodeRoutes<>>();

      for(auto i : result){
        cout << i.first << endl;
        cout << i.second->getRoot()->toString() << endl;
      }
    }

    else if(value == 2){
      auto result_2 = t.topNeighborhoodWithMoreRoutes();

      for(auto i : result_2){
        cout << i.first << endl;
        cout << i.second << endl;
      }
    }

    else if(value == 3){
      double point1_x;
      double point1_y;
      double point2_x;
      double point2_y;

      cout << "Point 1 X:" << endl;
      cin >> point1_x;

      cout << "Point 1 Y:" << endl;
      cin >> point1_y;

      cout << "Point 2 X:" << endl;
      cin >> point2_x;

      cout << "Point 2 Y:" << endl;
      cin >> point2_y;

      box box_1(point(point1_x, point1_y), point(point2_x, point2_y));

      auto result_3 = t.rangeRoutes(box_1);

      cout << result_3 << endl;
    }
    
    else if(value == 4){
      double point_x;
      double point_y;
      double radio;

      cout << "Point X:" << endl;
      cin >> point_x;

      cout << "Point Y:" << endl;
      cin >> point_y;

      cout << "Radio:" << endl;
      cin >> radio;

      auto result_4 = t.rangeRoutesCircle(point_x, point_y, radio);

      cout << result_4 << endl;
    }

    else {
      return;
    }
  }
}

auto main() -> int {
  using point = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon = bg::model::polygon<point>;

  using box = bg::model::box<point>;
/*
  polygon poly1;
  bg::append(poly1.outer(), point(0.0, 0.0));
  bg::append(poly1.outer(), point(5.0, 0.0));
  bg::append(poly1.outer(), point(5.0, 5.0));
  bg::append(poly1.outer(), point(0.0, 5.0));
  bg::append(poly1.outer(), point(0.0, 0.0));

  polygon poly2;
  bg::append(poly2.outer(), point(1.5, 1.0));
  bg::append(poly2.outer(), point(0.0, 5.0));
  bg::append(poly2.outer(), point(3.0, 3.0));
  bg::append(poly2.outer(), point(4.0, 0.0));
  bg::append(poly2.outer(), point(-1.0, 0.0));

  polygon line1;
  bg::append(line1.outer(), point(1.0, 1.0));
  bg::append(line1.outer(), point(2.0, 2.0));
  
  polygon line2;
  bg::append(line2.outer(), point(6.0, 1.0));
  bg::append(line2.outer(), point(2.0, 2.0));

  polygon line3;
  bg::append(line3.outer(), point(-1.0, 0.0));
  bg::append(line3.outer(), point(5.0, 0.0));*/

  // box b(point(10, 10), point(15, 15));

  //polygon a[8];

  // bg::read_wkt("POLYGON((0 0, 0 2, 2 2, 2 0, 0 0))", a[0]);
  // bg::read_wkt("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))", a[1]);
  // bg::read_wkt("POLYGON((1 1, 1 2, 2 2, 2 1, 1 1))", a[2]);
  // bg::read_wkt("POLYGON((1 1, 1 2))", a[3]);
  // bg::read_wkt("POLYGON((0 2, 1 2, 1 3, 0 3, 0 2))", a[4]);

  //bg::read_wkt("POLYGON((6 5))", a[0]);
  //bg::read_wkt("POLYGON((4 8))", a[1]);
  //bg::read_wkt("POLYGON((20 10))", a[2]);
  //bg::read_wkt("POLYGON((4 7))", a[3]);
  //bg::read_wkt("POLYGON((20 9))", a[4]);
  //bg::read_wkt("POLYGON((5 4))", a[5]);
  //bg::read_wkt("POLYGON((22 8))", a[6]);
  //bg::read_wkt("POLYGON((21 7))", a[7]);

  // cout << bg::wkt(a[0]) << endl;
  // cout << bg::wkt(a[1]) << endl;
  // cout << bg::wkt(a[2]) << endl;

  // cout << bg::distance(a[0], b) << endl;

  /*bg::correct(poly1);
  bg::correct(poly2);
  bg::correct(line1);
  bg::correct(line2);
  bg::correct(line3);*/

  Tree<RTreeNode<>> t;
  //Tree<RTreeNodeRoutes<>> t2;
  /*t.insert("1",poly1);
  t.insert("2",poly2);
  t.insert_route("1",line1);
  t.insert_route("2",line2);
  t.insert_route("3",line3);*/

  /*t.insert(a[1]);
  t.insert(a[2]);
  t.insert(a[3]);
  t.insert(a[4]);
  t.insert(a[0]);
  t.insert(a[5]);
  t.insert(a[6]);
  t.insert(a[7]);*/

  //cout << "Good Tree" << endl;

  parserNeighborhood("neighborhoods.txt", t);
  parserRoutes("green_tripdata_2015-01-1000.txt", t);

  interface(t);
  
  //cout << t.getRoot()->toString() << endl;
  /*stringstream b;
  b << bg::dsv(t.getRoot()->box);
  string value = "" + b.str();
  cout << value << endl;
  cout << t.getRoot()->routes->getRoot()->numberOfRoutes() << endl;*/

  /*auto result = t.routesInTheNeighborhood<RTreeNodeRoutes<>>();

  for(auto i : result){
    cout << i.first << endl;
    cout << i.second->getRoot()->toString() << endl;
  }*/

  /*auto result_2 = t.topNeighborhoodWithMoreRoutes();

  for(auto i : result_2){
    cout << i.first << endl;
    cout << i.second << endl;
  }*/
  /*box box_1(point(-75.0, -74.0), point(-73.0, 41.0));
  box box_2(point(-73.0, 41.0), point(-75.0, 40.0));
  box box_3(point(-73.92, 40.75), point(-73.91, 41.76));
  box box_4(point(-73.92, 41.76), point(-73.91, 40.75));

  auto result_3 = t.rangeRoutes(box_1);
  auto result_4 = t.rangeRoutes(box_2);
  auto result_5 = t.rangeRoutes(box_3);
  auto result_6 = t.rangeRoutes(box_4);

  cout << result_3 << endl;
  cout << result_4 << endl;
  cout << result_5 << endl;
  cout << result_6 << endl;*/

  //auto result_7 = t.rangeRoutesCircle(-73.92, 41.76, 2);
  //auto result_8 = t.rangeRoutesCircle(-73.92, 40.76, 0.05);
  
  //cout << result_7 << endl;
  //cout << result_8 << endl;
  
  /*cout << t.getRoot()->routes->getRoot()->toString() << endl;
  for(auto sub : *t.getRoot()->subNodes){
    if(sub->routes) cout << sub->routes->getRoot()->toString() << endl;
  }*/


  return 0;
}
