var trajectenAPI = "http://localhost:8084/ReistijdenRESTService/trajecten";    
var trajecten = [];
var polys = [];
var map;
var selectedTraject = "TS_HugoDeVrieslaan_S113_S100";

$(document).ready(function() 
{
    // Load Trajecten JSON from ReistijdenRESTService
    $.getJSON(trajectenAPI, function(json) 
    {   
        for (var i = 0; i < json.length; i++) 
        {
            var traject = new Object();
            traject.id = json[i]["_id"];
            traject.location = json[i]["location"];
            traject.coordinates = json[i]["coordinates"]; 
            trajecten.push(traject);
        }
    }).done(function()
    {
        var searchLocations = [];        
        for(var i = 0; i < trajecten.length; i++)
        {
            $("#routeList").append('<li class="list-group-item">'+trajecten[i].location+'</li>');
            searchLocations.push(trajecten[i].location);
        }
       
        $('#search').typeahead({source: searchLocations});
        
        map = L.map('map').setView([52.36, 4.89], 13);
        L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {
        attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'
        }).addTo(map);   
        
        drawRoutesOnMap();    
        console.log(polys);
    });   
});

function drawRoutesOnMap()
{
    for(var q=0; q<trajecten.length; q++)
    {
        var route = trajecten[q];
        var pointList = [];
    
        for(var i = 0; i < route.coordinates.length; i++)
        {
            var point = new L.LatLng(route.coordinates[i][0], route.coordinates[i][1]);
            pointList.push(point);                
        }
    
        var polyline = new L.Polyline(pointList, 
        {       
            id: route.id,
            color: 'green',
            weight: 5,
            opacity: 0.5,
            smoothFactor: 1,
            clickable: true,
        });

        polyline.on('click', function(e)
        {
            selectedTraject = e.target.options.id;
            var $scope = angular.element('.modalC').scope();
            $scope.open();
    });
        polyline.addTo(map);        
    }
}