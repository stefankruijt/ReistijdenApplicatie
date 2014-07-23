
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
        String result = JSON.serialize(dbtraject);
        return result;
    }
    
    public String getReistijden(String location)
    {
        // TODO  Deze methode is niet af.
        
        DBCollection collection = db.getCollection("measurements");
        BasicDBObject query = new BasicDBObject();
        query.put("location", location);
        DBCursor measurements = collection.find(query);
        
        
        String result = JSON.serialize(measurements);
        return result;
    }
}