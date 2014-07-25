package reistijdenapplicatie;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import model.Measurement;
import model.Traject;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

public class ReistijdenJSONParser 
{
    private JSONArray features;

    public ReistijdenJSONParser(String jsonString) throws ParseException 
    {
        JSONParser jsonParser = new JSONParser();
        JSONObject jsonObject = (JSONObject) jsonParser.parse(jsonString);
        features = (JSONArray) jsonObject.get("features");
    }

    public List<Measurement> ParseMeasurements() 
    {
        List<Measurement> measurements = new ArrayList<Measurement>();

        for (int i = 0; i < features.size(); i++) 
        {
            Measurement measurement = new Measurement();

            JSONObject route = (JSONObject) features.get(i);
            JSONObject properties = (JSONObject) route.get("properties");

            String id = route.get("Id").toString();
            measurement.setTrajectId(id);

            // Read traject sensor properties
            long velocity = (long) properties.get("Velocity");
            long travelTime = (long) properties.get("Traveltime");
            
            // Ignore tractsensors that don't have any usefull information
            if(velocity == -1 || travelTime == 0)
                continue;

            Date convertedDate;
            try 
            {
                SimpleDateFormat dateFormat = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss");
                convertedDate = dateFormat.parse(properties.get("Timestamp").toString());
            } 
            catch (java.text.ParseException e) 
            {
                e.printStackTrace();
                // In case of an error with parsing the timestamp ignore this sensor and continue since a timestamp is required.
                continue;
            }

            measurement.setVelocity((int) velocity);
            measurement.setTimestamp(convertedDate);
            measurement.setTravelTime((int) travelTime);
            measurements.add(measurement);
        }
        return measurements;
    }

    public List<Traject> ParseTrajects() 
    {
        List<Traject> trajects = new ArrayList<Traject>();
        for (int i = 0; i < features.size(); i++) 
        {
            Traject traject = new Traject();

            JSONObject route = (JSONObject) features.get(i);

            String id = route.get("Id").toString();
            traject.setTrajectId(id);
            
            JSONObject properties = (JSONObject) route.get("properties");
            traject.setLocation(properties.get("Name").toString());

            JSONObject geometry = (JSONObject) route.get("geometry");
            String type = (String) geometry.get("type");
            JSONArray coordinates = (JSONArray) geometry.get("coordinates");

            traject.setRDCoordinates(coordinates);
            traject.setType(type);
            trajects.add(traject);
        }
        return trajects;
    }
}