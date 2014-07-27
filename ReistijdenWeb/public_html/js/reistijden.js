var trajectenAPI = "http://localhost:8084/ReistijdenRESTService/trajecten";
var trajecten = [];
var map;

var app = angular.module('app', ['ui.bootstrap']);

app.controller("ReistijdenController", function($scope, $modal, $http)
{
    $scope.trajecten = trajecten;
    $scope.selectedLocation = undefined;
    $scope.selectedId = undefined;

    $http.get(trajectenAPI).then(function(res)
    {
        $scope.trajecten = res.data;

        map = L.map('map').setView([52.36, 4.89], 13);
        L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png',{attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'}).addTo(map);

        $scope.trajecten.forEach(function(traject)
        {
            var pointList = [];

            traject.coordinates.forEach(function(coordinate)
            {
                pointList.push(new L.LatLng(coordinate[0], coordinate[1]));
            });

            var polyline = new L.Polyline(pointList,{
                id: traject._id,
                color: 'green',
                weight: 5,
                opacity: 0.5,
                smoothFactor: 1,
                clickable: true
            });

            polyline.on('click', function(e)
            {
                $scope.open(traject);
            });
            polyline.addTo(map);
        });
    });

    this.selectedTab = 1;
    this.selectTab = function(setTab)
    {
        this.selectedTab = setTab;
    };

    this.isTabSelected = function(checkTab)
    {
        return this.selectedTab === checkTab;
    };

    this.search = function()
    {
        for(var i=0; i<$scope.trajecten.length; i++)
        {
            if($scope.trajecten[i].location == $scope.selectedLocation)
            {
                return $scope.trajecten[i];
            }
        }
    };

    $scope.open = function(traject)
    {
        var modalInstance = $modal.open({
            templateUrl: 'dialogContent.html',
            controller: ModalInstanceCtrl,
            resolve: {
                traject: function ()
                {
                    return traject;
                }
            }
        });
    };
});

var ModalInstanceCtrl = function($scope, $modalInstance, traject)
{
    $scope.traject = traject;
    $scope.ok = function (){
        $modalInstance.close();
    };
};

$("#search").click(function(ev)
{
    this.select();
});