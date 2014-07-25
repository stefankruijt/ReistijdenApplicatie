var trajectenAPI = "http://localhost:8084/ReistijdenRESTService/trajecten";    
var trajecten = [];

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
            console.log("Trajecten length:" + trajecten.length);
        }
    }).done(function()
    {
        var searchLocations = [];
        
        for(var i = 0; i < trajecten.length; i++)
        {
            $("#routeList").append('<li class="list-group-item">'+trajecten[i].id+'</li>');
            searchLocations.push(trajecten[i].location);
        }
       
        $('#search').typeahead({source: searchLocations});
    });   
});