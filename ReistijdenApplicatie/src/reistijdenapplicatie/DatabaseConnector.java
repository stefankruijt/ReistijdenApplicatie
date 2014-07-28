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
import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import com.mongodb.DuplicateKeyException;
import com.mongodb.MongoClient;
import com.mongodb.util.JSON;

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
                // Check whether hashcode of traject is similar, if not remove traject and convert all RD coordinates to ETRS89
                if (!dbtraject.get("coordinatesHashCode").equals(traject.getRDCoordinates().hashCode())) 
                {
                    // Traject is found in database, but coordinates from json file are different than the ones in the database.
                    System.out.println("Traject: " + traject.getTrajectId() + " needs to be updated.");
                    // Remove traject from database
                    removeTraject(table, traject);
                    // And add it again (with the new coordinate values				
                    InsertNewTraject(table, traject);
                }
            }
        }
    }

    private List<double[]> ConvertRDCoordinatesToETRS89(JSONArray rdCoordinates) 
    {
        List<double[]> convertedCoordinates = new ArrayList<>(rdCoordinates.size());

        for (Object rdCoordinate : rdCoordinates) 
        {
            JSONArray array = (JSONArray) rdCoordinate;
            long val1 = (long) array.get(0);
            long val2 = (long) array.get(1);
            double[] coordinates = CoordinateConvertor.ConvertRDCoordinateToETRS89(val1, val2);
            convertedCoordinates.add(coordinates);
        }
        return convertedCoordinates;
    }

    private void InsertNewTraject(DBCollection table, Traject traject) 
    {
        // Convert coordinates to a more usefull coordinate system (RTRS89) 
        List<double[]> coordinates = ConvertRDCoordinatesToETRS89(traject.getRDCoordinates());

        BasicDBObject document = new BasicDBObject();
        document.put("_id", traject.getTrajectId());
        document.put("location", traject.getLocation());
        document.put("coordinatesHashCode", traject.getRDCoordinates().hashCode());
        document.put("coordinates", coordinates);
        table.insert(document);
    }

    private void removeTraject(DBCollection table, Traject traject) 
    {
        BasicDBObject document = new BasicDBObject();
        document.put("_id", traject.getTrajectId());
        table.remove(document);
    }

    public String getTrajectData(String location) 
    {
        DBCollection collection = db.getCollection("trajects");
        BasicDBObject query = new BasicDBObject();        
        
        query.put("_id", location);
        DBObject dbtraject = collection.findOne(query);

        if (dbtraject == null) 
        {
            return "Location not found";
        }

        String result = JSON.serialize(dbtraject);
        return result;
    }

    public DBCursor getAllTrajects() 
    {
        DBCollection collection = db.getCollection("trajects");
        DBCursor dbtraject = collection.find();
        return dbtraject;
    }

    public String getAllTrajectsString() 
    {
        return JSON.serialize(getAllTrajects());
    }

    public DBCursor getReistijden(String location, int limit) 
    {
        DBCollection collection = db.getCollection("measurements");
        BasicDBObject query = new BasicDBObject();
        query.put("location", location);
        DBCursor measurements = collection.find(query);
        measurements.sort(new BasicDBObject("timestamp", -1));
        measurements.limit(limit);
        return measurements;
    }

    // TraveltimeFF onbreekt in JSON file van trafficlink-online.nl Voor deze waarde wordt nu de gemiddelde reistijd per 24 uur genomen.
    public void updateTraveltimeFF(String id, int averageTraveltime) 
    {
        DBCollection collection = db.getCollection("trajects");
        BasicDBObject query = new BasicDBObject();
        query.put("_id", id);

        BasicDBObject update = new BasicDBObject();
        update.append("$set", new BasicDBObject().append("traveltimeFF", averageTraveltime));
        collection.update(query, update);
    }

    public void updateActueleReistijden(ArrayList reistijdenOverzicht) 
    {
        DBCollection collection = db.getCollection("actueleReistijden");

        for (Object reistijdenOverzicht1 : reistijdenOverzicht) 
        {
            ArrayList list = (ArrayList) reistijdenOverzicht1;
            if (collection.find(new BasicDBObject("_id", list.get(0))).length() > 0) 
            {
                BasicDBObject query = new BasicDBObject();
                query.put("_id", list.get(0));

                BasicDBObject update = new BasicDBObject();
                update.append("$set", new BasicDBObject().append("traveltime", list.get(1)).append("timestamp", list.get(2)));
                collection.update(query, update);
            }
            else 
            {
                BasicDBObject query = new BasicDBObject();
                query.put("_id", list.get(0));
                query.put("traveltime", list.get(1));
                query.put("timestamp", list.get(2));
                collection.insert(query);
            }
        }
    }

    public DBCursor getActueleReistijden() 
    {
        DBCollection collection = db.getCollection("actueleReistijden");
        DBCursor dbtraject = collection.find();
        return dbtraject;
    }
}