var app = angular.module('app', ['ui.bootstrap']);

app.controller("PanelController", function()
{
    this.tab = 1;
    this.selectTab = function(setTab)
    {
        this.tab = setTab;
    };

    this.isSelected = function(checkTab)
    {
        return this.tab === checkTab;
    };
});

app.controller("ModalController", function($scope, $modal, $log)
{
    $scope.open = function(size)
    {
        $scope.selectedTraject = selectedTraject;
        var modalInstance = $modal.open({
            templateUrl: 'dialogContent.html',
            controller: ModalInstanceCtrl,
            size: size,
            resolve:
            {
                selectedTraject: function()
                {
                    return $scope.selectedTraject;
                }
            }
        });
    };
});

var ModalInstanceCtrl = function ($scope, $modalInstance, selectedTraject)
{
    $scope.selectedTraject = selectedTraject;

    $scope.ok = function () {
        $modalInstance.close();
    };

    $scope.cancel = function () {
        $modalInstance.dismiss('cancel');
    };
};