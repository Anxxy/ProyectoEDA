import json

with open('neighborhoods.json') as file:
  neighborhoods = json.load(file)

  f = open("neighborhoods.txt", "w")

  for feature in neighborhoods['features']:
      print('Neighborhood:', feature['properties']['neighborhood'], file=f)
      print('Geometry:', file=f)
      for coordinate in feature['geometry']['coordinates'][0]:
        print(coordinate[0],' ',coordinate[1], file=f)
      print('', file=f)
  print("finish")