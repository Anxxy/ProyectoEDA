#include "./tree.hpp"
#include "./rtreenode.hpp"

auto main() -> int {
  using point = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon = bg::model::polygon<point>;

  using box = bg::model::box<point>;

  // box b(point(10, 10), point(15, 15));

  polygon a[4];

  bg::read_wkt("POLYGON((0 0, 0 2, 2 2, 2 0, 0 0))", a[0]);
  bg::read_wkt("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))", a[1]);
  bg::read_wkt("POLYGON((1 1, 1 2, 2 2, 2 1, 1 1))", a[2]);
  bg::read_wkt("POLYGON((1 1, 1 2))", a[3]);

  // cout << wkt(a[0]) << endl;
  // cout << wkt(a[1]) << endl;
  // cout << wkt(a[2]) << endl;

  // cout << distance(a[0], b) << endl;

  Tree<RTreeNode<>> t;
  t.insert(a[1]);
  t.insert(a[2]);
  t.insert(a[3]);

  cout << t.getRoot()->toString() << endl;
  return 0;
}
