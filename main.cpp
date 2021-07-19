#include "./tree.hpp"
#include "./rtreenode.hpp"

auto main() -> int {
  using point = model::point<double, 2, cs::cartesian>;
  using polygon = model::polygon<point>;

  polygon a[3];

  read_wkt("POLYGON((0 0, 0 2, 2 2, 2 0, 0 0))", a[0]);
  read_wkt("POLYGON((0 0, 0 1, 1 1, 1 0, 0 0))", a[1]);
  read_wkt("POLYGON((1 1, 1 2, 2 2, 2 1, 1 1))", a[2]);

  cout << wkt(a[0]) << endl;
  cout << wkt(a[1]) << endl;
  cout << wkt(a[2]) << endl;

  cout << distance(a[0], a[2]) << endl;

  Tree<RTreeNode<>> t;
  return 0;
}
