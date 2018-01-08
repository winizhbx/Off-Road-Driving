var infowindow
var elevator
var inputlat
var inputlng
var map

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

  // Add a listener for the click event. Display the elevation for the LatLng of
  // the click inside the infowindow.
  map.addListener('click', function(event) {
    displayLocation(event.latLng);
  });
}

function updateLat(){
  inputlat = Number(document.getElementById("lat").value);
  displayLocationByInput();
}

function updateLng(){
  inputlng = Number(document.getElementById("lon").value);
  displayLocationByInput();
}

function displayLocationByInput(){
  map.setCenter({lat: inputlat, lng: inputlng});
  var location = new google.maps.LatLng(inputlat, inputlng);
  displayLocation(location);
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
        infowindow.setContent('The elevation at this point <br>is ' + location + ' and ' +
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