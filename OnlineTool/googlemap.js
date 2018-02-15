var infowindow
var elevator
var inputlat
var inputlng
var map
var markerstart
var markerend


var CurrentMap
var ElevationMap

//AIzaSyAutYL5xVFjrUExFyNcxuHkwp1msikUH7A  Road API key

function initMap() {
  map = new google.maps.Map(document.getElementById('map'), {
    zoom: 8,
    center: {lat: 42.2808, lng: -83.7430},  // Ann Arbor.
    mapTypeId: 'terrain'
  });
  inputlat = 42.2808
  inputlng = -83.7430
  elevator = new google.maps.ElevationService;
  infowindow = new google.maps.InfoWindow({map: map});
  var lat = Number(document.getElementById("startlat").value);
  var lng = Number(document.getElementById("startlng").value);
  markerstart = new google.maps.Marker({
    position: {lat, lng},
    animation: google.maps.Animation.DROP,
    map: map,
    title: 'Start',
    label: 'A'
  });
  lat = Number(document.getElementById("endlat").value);
  lng = Number(document.getElementById("endlng").value);
  markerend = new google.maps.Marker({
    position: {lat, lng},
    animation: google.maps.Animation.DROP,
    map: map,
    title: 'End',
    label: 'B'
  });

  // Add a listener for the click event. Display the elevation for the LatLng of
  // the click inside the infowindow.
  map.addListener('click', function(event) {
    displayLocation(event.latLng);
  });
}

initMap()
displayStartLocation()
displayEndLocation()
// drawPath()
// GenerateMap()
// setTimeout(function () { Astar();}, 70000)
var temp = GetLocationsElevation(markerstart.getPosition());
console.log("temp = " + temp);

// setTimeout(function () { Astar();}, 0)

function updateLat(){
  inputlat = Number(document.getElementById("lat").value);
  displayLocationByInput();
}

function updateLng(){
  inputlng = Number(document.getElementById("lng").value);
  displayLocationByInput();
}

function displayLocationByInput(){
  map.setCenter({lat: inputlat, lng: inputlng});
  var location = new google.maps.LatLng(inputlat, inputlng);
  displayLocation(location);
}

function displayStartLocation(){
  var lat = Number(document.getElementById("startlat").value);
  var lng = Number(document.getElementById("startlng").value);
  markerstart.setPosition({lat, lng});

  if (markerend != null){
    var newlat = (markerend.position.lat() + lat) / 2;
    var newlng = (markerend.position.lng() + lng) / 2;
    map.setCenter({lat: newlat, lng: newlng});

    var bounds = new google.maps.LatLngBounds();
    bounds.extend(markerstart.getPosition()); 
    bounds.extend(markerend.getPosition()); 
    map.fitBounds(bounds);
  } else {
    map.setCenter({lat, lng});
  }
}

function displayEndLocation(){
  var lat = Number(document.getElementById("endlat").value);
  var lng = Number(document.getElementById("endlng").value);
  markerend.setPosition({lat, lng});
  if (markerend != null){
    var newlat = (markerstart.position.lat() + lat) / 2;
    var newlng = (markerstart.position.lng() + lng) / 2;
    map.setCenter({lat: newlat, lng: newlng});

    var bounds = new google.maps.LatLngBounds();
    bounds.extend(markerstart.getPosition()); 
    bounds.extend(markerend.getPosition()); 
    map.fitBounds(bounds);
  } else {
    map.setCenter({lat, lng});
  }
}


function displayLocation(location) {
  infowindow.setPosition(location);
  elevator.getElevationForLocations({
    'locations': [location]
  }, function(results, status) {
    infowindow.setPosition(location);
    if (status === 'OK') {
      // Retrieve the first result
      if (results[0]) {
        // Open the infowindow indicating the elevation at the clicked position.
        infowindow.setContent(location + " elevation is " +
            results[0].elevation + ' meters.');
        // infowindow2.setContent('The elevation at this point <br>is ' + myLatlng + ' and ' +
        //     results[1].elevation + ' meters.');
      } else {
        infowindow.setContent('No results found');
      }
    } else {
      infowindow.setContent('Elevation service failed due to: ' + status);
    }
  });
}

var error = 0;
var finish = 0;
var off = 0;

function GetPointElevation(location) {
  return elevator.getElevationForLocations({
    'locations': [location]
  }, function(results, status) {
    if (status === 'OK') {
      console.log("OK");
      return results[0].elevation;
    } else {
      console.log("ERROR");
      return 0;
    }
  });
}

function GetLocationsElevation(location) {
  return elevator.getElevationForLocations({
    'locations': location
  }, function(results, status) {
    if (status === 'OK') {
      error = 0;
      console.log("OK");
      var temp = []
      for (var i = 0; i < results.length; i++){
        temp.push(results[i].elevation);
      }
      ElevationMap.push(temp);
    } else {
      error = 1;
      off++;
      console.log("ERROR");
      infowindow.setContent('Elevation service failed due to: ' + status);
    }
  });
}

function drawPath(){
  var flightPlanCoordinates = [markerstart.position, 
                               markerend.position
                              ];
  var flightPath = new google.maps.Polyline({
    path: flightPlanCoordinates,
    geodesic: true,
    strokeColor: '#FF0000',
    strokeOpacity: 1.0,
    strokeWeight: 2
  });

  flightPath.setMap(map);
}

function GenerateMap(){
  var geod = GeographicLib.Geodesic.WGS84, r;

  // // Find the distance from Wellington, NZ (41.32S, 174.81E) to
  // // Salamanca, Spain (40.96N, 5.50W)...
  // r = geod.Inverse(-41.32, 174.81, 40.96, -5.50);
  // console.log("The distance is " + r.s12.toFixed(3) + " m.");
  // // This prints "The distance is 19959679.267 m."

  // Find the point 20000 km SW of Perth, Australia (32.06S, 115.74E)...
  r = geod.Direct(44.2062527, -119.5812443, 0, 5);
  console.log("The position is (" +
              r.lat2.toFixed(8) + ", " + r.lon2.toFixed(8) + ").");
  // This prints "The position is (32.11195529, -63.95925278)."

  var startlat = markerstart.position.lat();
  var startlng = markerstart.position.lng();
  var endlat = markerend.position.lat();
  var endlng = markerend.position.lng();

  r = geod.Inverse(startlat, startlng, endlat, endlng);
  var distance = r.s12.toFixed(5);

  var location = new google.maps.LatLng(startlat, startlng);
  CurrentMap = [];
  var step = 10.0;//meters

  var gridsize = Math.floor(distance / step);
  console.log("gridsize is " + gridsize);


  var direction = [];
  if (endlat > startlat) direction.push(0);
  else direction.push(180);

  if (endlng > startlng) direction.push(90);
  else direction.push(270);

  for (var i = 0; i < gridsize; i++){
    r = geod.Direct(startlat, startlng, direction[0], step * i);
    var temp = [];

    for (var j = 0; j < gridsize; j++){
      d = geod.Direct(r.lat2, r.lon2, direction[1], step * j);
      location = new google.maps.LatLng(d.lat2, d.lon2);
      temp.push(location);
    }

    CurrentMap.push(temp);
  }

  // console.log(CurrentMap);

  ElevationMap = []
  // var thread = require('thread')

  var offset = 0;
  // getElevationDataTime(offset, gridsize - 1);
  // getElevationDataTime(offset, gridsize - 1);

  // setTimeout(function () {console.log(ElevationMap);}, 10000);
}


function getElevationDataTime(offset, num){
  for (var i = offset; i < offset + num; i++){
    setTimeout(function () { 
      GetLocationsElevation(CurrentMap[i]); 
      if (error == 1){
        i = i - 1;
        error = 0;
        console.log("MINUS");
      }
      console.log(ElevationMap.length);
    }, 800 * (i - 4));
    
    
    // GetLocationsElevation(CurrentMap[i]);
    // setTimeout(GetLocationsElevation(CurrentMap[i]), 1000 * i);
  }
  // sleep(1000);
  // console.log(ElevationMap)
}

function distance(start, end){
  // var geod = GeographicLib.Geodesic.WGS84, r;
  // r = geod.Inverse(start.lat(), start.lng(), end.lat(), end.lng());
  // var distance = r.s12.toFixed(5);
  // return distance;

  var R = 6371e3; // metres
  var φ1 = start.lat();
  var φ2 = end.lat();
  var Δφ = (end.lat()-start.lat());
  var Δλ = (end.lng()-start.lng());

  var a = Math.sin(Δφ/2) * Math.sin(Δφ/2) +
          Math.cos(φ1) * Math.cos(φ2) *
          Math.sin(Δλ/2) * Math.sin(Δλ/2);
  var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1-a));

  var d = R * c;
  return Number(d);
}

function slope(start, end){
  var slope = Math.abs(ElevationMap[start[0] ][start[1] ] - ElevationMap[end[0] ][end[1] ]);
  slope /= distance(CurrentMap[start[0] ][start[1] ], CurrentMap[end[0] ][end[1] ]);
  return slope;
}

var connect8 = [[0, 1], [1, 0], [1, 1], [0, -1], [-1, 0], [-1, -1], [1, -1], [-1, 1]];

function Astar(){
  var queue = new Heap(function(a, b) {
      return a[1] - b[1];
  });
  var endlocation = markerend.getPosition();
  queue.push([[0, 0], distance(markerstart.getPosition(), endlocation)]);


  var visited = {};
  var costs = {};
  var parents = {};
  var results = [];
  costs[[0, 0]] = 0;
  parents[[0, 0]] = [0, 0];
  visited[[0, 0]] = 1;


  while(!queue.empty() ){
    
    var current = queue.pop();
    currentlocation = current[0];
    currentcost = costs[currentlocation];
    console.log(currentlocation);
    console.log(current[1]);
    console.log("queue.size = ", queue.size());

    if (distance(CurrentMap[currentlocation[0]][currentlocation[1]], endlocation) < 2000){
      results.push(currentlocation);
      var parent = parents[currentlocation];
      while(parent != currentlocation){
        results.push(parent);
        currentlocation = parent;
        parent = parents[parent];
      }
      break;
    }

    for (var i = 0; i < connect8.length; i++){
      neighbor = [currentlocation[0]+connect8[i][0], currentlocation[1]+connect8[i][1]];
      if (neighbor[0] < 0 || neighbor[0] >= CurrentMap.length) continue;
      if (neighbor[1] < 0 || neighbor[1] >= CurrentMap.length) continue;
      if (neighbor in visited){
        continue;
      }

      visited[neighbor] = 1;
      h = distance(CurrentMap[neighbor[0]][neighbor[1]], endlocation);
      c = distance(CurrentMap[neighbor[0]][neighbor[1]], CurrentMap[currentlocation[0]][currentlocation[1]]) + 
          slope(currentlocation, neighbor) * 100000;
      console.log("h = ", h);
      console.log("c = ", slope(currentlocation, neighbor) * 10000);
      console.log(neighbor);

      queue.push([neighbor, Number(currentcost + c + h)]);
      costs[neighbor] = currentcost + c;
      parents[neighbor] = currentlocation;
    }
    // break;
  }

  console.log(results);
  console.log(queue.size());
  console.log(distance(markerstart.getPosition(), markerend.getPosition()));

  var flightPlanCoordinates = [];
  for (var i = 0; i < results.length; i++){
    flightPlanCoordinates.push(CurrentMap[results[i][0]][results[i][1]]);
  }

  var flightPath = new google.maps.Polyline({
    path: flightPlanCoordinates,
    geodesic: true,
    strokeColor: '#FF0000',
    strokeOpacity: 1.0,
    strokeWeight: 2
  });

  flightPath.setMap(map);
}

// // Snap a user-created polyline to roads and draw the snapped path
// function runSnapToRoad(path) {
//   var pathValues = [];
//   for (var i = 0; i < path.getLength(); i++) {
//     pathValues.push(path.getAt(i).toUrlValue());
//   }

//   $.get('https://roads.googleapis.com/v1/snapToRoads', {
//     interpolate: true,
//     key: apiKey,
//     path: pathValues.join('|')
//   }, function(data) {
//     processSnapToRoadResponse(data);
//     drawSnappedPolyline();
//     getAndDrawSpeedLimits();
//   });
// }


// // This function is called when the user clicks the UI button requesting
// // a reverse geocode.
// function geocodePlaceId(geocoder, map, infowindow) {
//   var placeId = document.getElementById('place-id').value;
//   geocoder.geocode({'placeId': placeId}, function(results, status) {
//     if (status === 'OK') {
//       if (results[0]) {
//         map.setZoom(11);
//         map.setCenter(results[0].geometry.location);
//         var marker = new google.maps.Marker({
//           map: map,
//           position: results[0].geometry.location
//         });
//         infowindow.setContent(results[0].formatted_address);
//         infowindow.open(map, marker);
//       } else {
//         window.alert('No results found');
//       }
//     } else {
//       window.alert('Geocoder failed due to: ' + status);
//     }
//   });
// }

