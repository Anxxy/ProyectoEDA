#pragma once

//template<typename NODE>
auto parserNeighborhood(string name, Tree<RTreeNode<>> &t) -> void {
  using point = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon = bg::model::polygon<point>;
  //cout << "Good start" << endl;
  polygon poly;
  string cadena;
  string neighborhood;
  double x;
  double y;
  //cout << "Good varibles" << endl;

  ifstream fe(name);
  fe >> cadena;
  //cout << cadena << endl;
  if(cadena == "Neighborhood:"){
    while (!fe.eof()) {
      bg::clear(poly);
      neighborhood = "";
      fe >> cadena;
      
      while(cadena != "Geometry:") {
        neighborhood += cadena;
        neighborhood += " ";
        fe >> cadena;
      } 
      //cout << neighborhood << endl;

      while(!fe.eof()){
        fe >> cadena;
        if (cadena == "Neighborhood:") break;
        x = atof(cadena.c_str());
        fe >> cadena;
        y = atof(cadena.c_str());
        bg::append(poly.outer(), point(x, y));
      }

      bg::correct(poly);
      t.insert(neighborhood,poly);
    }
  }
  //cout << "Good load" << endl;

  fe.close();
  //cout << "Good ending" << endl;
}


auto parserRoutes(string name, Tree<RTreeNode<>> &t) -> void {
  using point = bg::model::point<double, 2, bg::cs::cartesian>;
  using polygon = bg::model::polygon<point>;
  string cadena;
  polygon poly;
  int id = 0;
  double x;
  double y;

  ifstream fe(name);
  while(!fe.eof()){
    bg::clear(poly);
    fe >> cadena;
    x = atof(cadena.c_str());
    fe >> cadena;
    y = atof(cadena.c_str());
    bg::append(poly.outer(), point(x, y));
    //cout << x << ' ' << y << ' ';
    fe >> cadena;
    x = atof(cadena.c_str());
    fe >> cadena;
    y = atof(cadena.c_str());
    bg::append(poly.outer(), point(x, y));
    //cout << x << ' ' << y << endl;
    
    bg::correct(poly);
    t.insert_route(to_string(id),poly);
    id++;
  }

  fe.close();
}
