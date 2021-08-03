#pragma once

#include <boost/geometry/geometries/geometries.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/geometry.hpp>                   
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <fstream> 
#include <limits>
#include <vector>
#include <string>   
#include <map>

using namespace std;
namespace bg = boost::geometry;

enum class Status {
  OK,
  NEEDSBALANCE
};
