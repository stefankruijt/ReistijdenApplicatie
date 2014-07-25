
import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import com.mongodb.MongoClient;
import com.mongodb.util.JSON;
import java.net.UnknownHostException;

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
    
    public String getTrajectData(String location)
    {
        DBCollection collection = db.getCollection("trajects");        
        BasicDBObject query = new BasicDBObject();
        query.put("_id", location);
        DBObject dbtraject = collection.findOne(query);
        
        if(dbtraject==null)
            return "Location not found";
        
        String result = JSON.serialize(dbtraject);
        return result;
    }
    
    public String getAllTrajects()
    {
        DBCollection collection = db.getCollection("trajects");        
        DBCursor dbtraject = collection.find();
        String result = JSON.serialize(dbtraject);
        return result;
    }
    
    public DBCursor getReistijden(String location)
    {
        DBCollection collection = db.getCollection("measurements");
        BasicDBObject query = new BasicDBObject();
        query.put("location", location);
        DBCursor measurements = collection.find(query);
        measurements.sort(new BasicDBObject("timestamp", -1));
        measurements.limit(20);
        return measurements;
    }
}