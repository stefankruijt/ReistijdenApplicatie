/*
    
    private class CalculateTrajectFF extends TimerTask 
    {
        @Override
        public void run() 
        {
            try 
            {
                System.out.println("CalculateTrajectFF task for all trajects started at: " + new Date().toString());
                
                DBCursor trajects = database.getAllTrajects();
                while(trajects.hasNext())
                {                    
                    DBObject object = trajects.next();
                    String id = (String)object.get("_id");
                    DBCursor reistijden = database.getReistijden(id, 500);
                    int averageTraveltime = 0;
                    int N = 0;
                    while(reistijden.hasNext())
                    {
                        averageTraveltime += (int) reistijden.next().get("traveltime");
                        N++;
                    }
                    averageTraveltime = averageTraveltime / N;
                    database.updateTraveltimeFF(id, averageTraveltime);
                }
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
        }
    }
    
    private class CalculateActueleReistijden extends TimerTask 
    {
        @Override
        public void run() 
        {
            try 
            {
                System.out.println("CalculateActueleReistijden task for all trajects started at: " + new Date().toString());
                
                DBCursor trajecten = database.getAllTrajects();
                                
                ArrayList reistijdenOverzicht = new ArrayList();
                        
                while(trajecten.hasNext())
                {
                    ArrayList reistijd = new ArrayList();
                    DBObject traject = trajecten.next();     
                    
                    String id = (String) traject.get("_id");            
                    DBCursor cursor = database.getReistijden(id, 1);
                    DBObject obj = cursor.next();
                    reistijd.add(id);
                    reistijd.add(obj.get("traveltime"));
                    reistijd.add(obj.get("timestamp"));
                    
                    reistijdenOverzicht.add(reistijd);
                }
                database.updateActueleReistijden(reistijdenOverzicht);
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }
        }
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
                connector.closeStream();
                
                ReistijdenJSONParser parser = new ReistijdenJSONParser(data);
                List<Measurement> measurements = parser.ParseMeasurements();
                List<Traject> trajects = parser.ParseTrajects();

                database.insertMeasurementsToDatabase(measurements);
                database.updateOrInsertTrajects(trajects);
            }
            catch(Exception e)
            {
                e.printStackTrace();
                return false;
            }            
            return true;
        }
    }    
}*/