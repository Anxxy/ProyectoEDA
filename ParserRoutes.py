import csv

with open('green_tripdata_2015-01.csv') as file:
  routes = csv.DictReader(file)

  f = open("green_tripdata_2015-01-1000.txt", "w")
  i = 0
  for route in routes:
      print(route['Pickup_longitude'], route['Pickup_latitude'], route['Dropoff_longitude'], route['Dropoff_latitude'], file=f)
      if i > 1000:
          break
      i+=1
  f.close()
  print("finish")
