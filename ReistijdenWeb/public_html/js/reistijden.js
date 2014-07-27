var trajectenAPI = "http://localhost:8084/ReistijdenRESTService/trajecten";
var trajecten = [];
var polys = [];
var map;
var selectedTraject = "TS_HugoDeVrieslaan_S113_S100";

var app = angular.module('app', ['ui.bootstrap']);

app.controller("NavbarController", function($scope)
{
    $scope.trajecten = trajecten;
    $scope.selectedLocation = undefined;
    $scope.selectedId = undefined;

    this.tab = 1;
    this.selectTab = function(setTab)
    {
        this.tab = setTab;
    };

    this.isSelected = function(checkTab)
    {
        return this.tab === checkTab;
    };

    this.search = function()
    {
        for(var i=0; i<trajecten.length; i++)
        {
            if(trajecten[i].location == $scope.selectedLocation )
            {
                return trajecten[i].id;
            }
        }
    };
});

app.controller("ModalController", function($scope, $modal)
{
    $scope.open = function(size)
    {
        if(size!=undefined)
        {
            selectedTraject = size;
        }
        var modalInstance = $modal.open({
            templateUrl: 'dialogContent.html',
            controller: ModalInstanceCtrl,
        });
    };
});

var ModalInstanceCtrl = function ($scope, $modalInstance)
{
    $scope.selectedTraject = selectedTraject;

    $scope.ok = function () {
        $modalInstance.close();
    };

    $scope.cancel = function () {
        $modalInstance.dismiss('cancel');
    };
};

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
        for(var i = 0; i < trajecten.length; i++)
        {
            $("#routeList").append('<li class="list-group-item">'+trajecten[i].location+'</li>');
        }
        
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
};

$("#search").click(function(ev)
{
    this.select();
});