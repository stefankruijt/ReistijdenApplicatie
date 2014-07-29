var host = "http://localhost:8084/ReistijdenRESTService";
var actueleReistijden = {};

var app = angular.module('app', ['ui.bootstrap', 'angularCharts']);

app.controller("ReistijdenController", function($scope, $modal, $http) {

    $http.get(host + "/trajecten").then(function(res)
    {
        $scope.trajecten = res.data;

        var map = L.map('map').setView([52.36, 4.89], 13);
        L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png',{attribution: '&copy; <a href="http://osm.org/copyright">OpenStreetMap</a> contributors'}).addTo(map);

        $http.get(host + "/actueleReistijden").then(function(res) {
            $scope.actueleReistijden = res.data;
            $scope.trajecten.forEach(function(traject) {
                var pointList = [];
                var trajectFF = traject.traveltimeFF;

                $scope.actueleReistijden.forEach(function(reistijd) {
                    actueleReistijden[reistijd._id] = reistijd.traveltime;
                });

                traject.coordinates.forEach(function(coordinate) {
                    pointList.push(new L.LatLng(coordinate[0], coordinate[1]))
                });

                var polyline = new L.Polyline(pointList, {
                    id: traject._id,
                    color: getTrajectColor(actueleReistijden[traject._id], trajectFF),
                    weight: 5,
                    opacity: 0.65,
                    smoothFactor: 1,
                    clickable: true
                });

                polyline.on('click', function(e) {
                    $scope.open(traject);
                });
                polyline.addTo(map);
            });
        });
    });

    this.selectedTab = 1;
    this.selectTab = function(setTab) {
        this.selectedTab = setTab;
    };

    this.isTabSelected = function(checkTab) {
        return this.selectedTab === checkTab;
    };

    this.search = function() {
        for(var i=0; i<$scope.trajecten.length; i++) {
            if($scope.trajecten[i].location == $scope.selectedLocation)
            {
                return $scope.trajecten[i];
            }
        }
    };

    $scope.open = function(traject) {
        var modalInstance = $modal.open({
            templateUrl: 'modalContent.html',
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

var ModalInstanceCtrl = function($scope, $modalInstance, traject, $http) {
    $scope.traject = traject;
    $scope.chartType = 'line';
    $scope.data = undefined;
    $scope.config = {
        labels: false,
        title : $scope.traject._id,
        legend : {
            display:true,
            position:'left'
        },
        innerRadius: 0
    };

    $http.get(host + "/reistijden?location="+traject._id).then(function(res) {
        $scope.measurements = res.data.measurements;
        $scope.actueleReistijd = actueleReistijden[traject._id];

        var series = ['snelheid', 'reistijd'];
        var datalist = [];

        for(var i=$scope.measurements.length-1; i>0; i--)
        {
            datalist.push({x: 60 - i*3, y: [$scope.measurements[i].velocity, $scope.measurements[i].traveltime]});
        }

        $scope.data = {series: series, data: datalist};
    });

    $scope.ok = function () {
        $modalInstance.close();
    };
};

$("#search").click(function(ev) {
    this.select();
});

function getTrajectColor(traveltime, traveltimeFF) {
    if (traveltime<=0){
        return 'white';
    }
    else if((traveltime/traveltimeFF)<=1.05) {
        return 'green';
    }
    else if ((traveltime/traveltimeFF)>=1.35) {
        return 'red';
    }
    else {
        return'yellow';
    }
};