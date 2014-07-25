package reistijdenapplicatie;

import java.io.IOException;
import java.net.URL;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;
import model.Measurement;
import model.Traject;

public class Main 
{
    DatabaseConnector database = new DatabaseConnector("localhost", 27017);
    
    private static final int MAX_TRY = 5;
    
    public static void main(String[] args) 
    {
        try
        {
            new Main();
        }
        catch(Exception ex)
        {
            ex.printStackTrace();
        }
    }

    public Main() 
    {        
          Timer timer = new Timer();
          timer.schedule(new RefreshTrafficDataTask(), 0, 180000);
    }
    
    private class RefreshTrafficDataTask extends TimerTask 
    {
        @Override
        public void run() 
        {
            for(int i = 0; i < MAX_TRY; i++)
            {    
                if(RefreshTrafficData())
                    break; // If RefreshTrafficData was succesfull stop the loop.
                else
                {
                    try 
                    {
                        Thread.sleep(20000); //Wait 20 seconds before trying again.
                        RefreshTrafficData();
                    } 
                    catch (InterruptedException ex) 
                    {
                        Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
            }
        }
        
        public boolean RefreshTrafficData()
        {
            try 
            {
                System.out.println("RefreshTrafficDataTask started at: " + new Date().toString());
                
                ReistijdenJSONConnector connector = new ReistijdenJSONConnector(new URL("http://www.trafficlinkonline.nl/trafficlinkdata/wegdata/TrajectSensorsNH.GeoJSON"));
                //ReistijdenJSONConnector connector = new ReistijdenJSONConnector("G:/TrajectSensorsNH.GeoJSON.txt");
                String data = connector.GetJSONData();

                ReistijdenJSONParser parser = new ReistijdenJSONParser(data);
                List<Measurement> measurements = parser.ParseMeasurements();
                List<Traject> trajects = parser.ParseTrajects();

                database.insertMeasurementsToDatabase(measurements);
                database.updateOrInsertTrajects(trajects);
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
            
            return true;
        }
    }    
}
