package reistijdenapplicatie;

import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

import model.Measurement;
import model.Traject;

import org.json.simple.JSONArray;
import util.CoordinateConvertor;

import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
import com.mongodb.DuplicateKeyException;
import com.mongodb.MongoClient;

public class DatabaseConnector 
{
    private String host;
    private int port;
    private DB db;

    public DatabaseConnector(String host, int port) 
    {
        this.host = host;
        this.port = port;

        try 
        {
            this.db = connectAndGetTrajectDataDatabase();
        } 
        catch (UnknownHostException e) 
        {
            e.printStackTrace();
        }
    }

    public DB connectAndGetTrajectDataDatabase() throws UnknownHostException 
    {
        MongoClient mongo = new MongoClient(host, port);
        db = mongo.getDB("TrajectData");
        return db;
    }

    public void insertMeasurementsToDatabase(List<Measurement> measurements) 
    {
        DBCollection table = db.getCollection("measurements");
        for (Measurement measurement : measurements) 
        {
            BasicDBObject document = new BasicDBObject();
            document.put("_id", measurement.getTrajectId() + measurement.getTimestamp());
            document.put("timestamp", measurement.getTimestamp());
            document.put("traveltime", measurement.getTravelTime());
            document.put("location", measurement.getTrajectId());
            document.put("velocity", measurement.getVelocity());
            
            try 
            {
                table.insert(document);
            }
            catch (DuplicateKeyException ex) 
            {
                System.out.println("Measurement is already in database");
            }
        }
    }

    public void updateOrInsertTrajects(List<Traject> trajects) 
    {
        DBCollection table = db.getCollection("trajects");
        for (Traject traject : trajects) 
        {
            BasicDBObject query = new BasicDBObject();
            query.put("_id", traject.getTrajectId());
            DBObject dbtraject = table.findOne(query);
            if (dbtraject == null) 
            {
                // Traject isn't found in database, so convert the coordinates to ETR89 coordinates and add them to the database.
                InsertNewTraject(table, traject);
            }
            else 
            {
                if (dbtraject.get("coordinatesHashCode").equals(traject.getRDCoordinates().hashCode())) 
                {
                    // Traject is found in database, but coordinates from json file are exactly the same (hashcode is similar)
                    // System.out.println("Traject: " + traject.getTrajectId() + " is already in database, and unchanged");
                }
                else 
                {
                    // Traject is found in database, but coordinates from json file are different than the ones in the database.
                    System.out.println("Traject: " + traject.getTrajectId() + " needs to be updated.");
                    // Remove traject from database
                    RemoveTraject(table, traject);
                    // And add it again (with the new coordinate values				
                    InsertNewTraject(table, traject);
                }
            }
        }
    }

    private List<double[]> ConvertRDCoordinatesToETRS89(JSONArray rdCoordinates) 
    {
        List<double[]> result = new ArrayList<>(rdCoordinates.size());

        for (Object rdCoordinate : rdCoordinates) 
        {
            JSONArray array = (JSONArray) rdCoordinate;
            long val1 = (long) array.get(0);
            long val2 = (long) array.get(1);
            double[] coordinates = CoordinateConvertor.ConvertRDCoordinateToETRS89(val1, val2);
            result.add(coordinates);
        }
        return result;
    }

    private void InsertNewTraject(DBCollection table, Traject traject) 
    {
        // Convert coordinates to a more usefull coordinate system
        List<double[]> coordinates = ConvertRDCoordinatesToETRS89(traject.getRDCoordinates());

        BasicDBObject document = new BasicDBObject();
        document.put("_id", traject.getTrajectId());
        document.put("name", traject.getName());
        document.put("coordinatesHashCode", traject.getRDCoordinates().hashCode());
        document.put("coordinates", coordinates);
        table.insert(document);
    }

    private void RemoveTraject(DBCollection table, Traject traject) 
    {
        BasicDBObject document = new BasicDBObject();
        document.put("_id", traject.getTrajectId());
        table.remove(document);
    }
}