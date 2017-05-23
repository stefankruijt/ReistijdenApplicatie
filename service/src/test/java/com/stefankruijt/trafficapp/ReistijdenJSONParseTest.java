/*package com.stefankruijt.trafficapp;

import java.io.FileNotFoundException;
import java.util.List;

import model.Measurement;
import model.Traject;
import org.json.simple.parser.ParseException;
import org.junit.Assert;
import org.junit.Test;
import reistijdenapplicatie.ReistijdenJSONConnector;
import reistijdenapplicatie.ReistijdenJSONParser;

public class ReistijdenJSONParseTest 
{ 
    @Test
    public void JSONTrajectenParseTest() throws FileNotFoundException, ParseException
    {
        List<Traject> trajecten = null;

        ReistijdenJSONConnector json = new ReistijdenJSONConnector("TrajectSensorsNH.GeoJSON.txt");
        String data = json.GetJSONData();
        ReistijdenJSONParser parser = new ReistijdenJSONParser(data);
        trajecten = parser.ParseTrajects();            
           
        Assert.assertEquals("TS_S116_S100_S114", trajecten.get(0).getLocation().toString());
        Assert.assertEquals("TrajectSensor_Route059", trajecten.get(0).getTrajectId());
            
        Assert.assertEquals("TS_S114_S100_S116", trajecten.get(1).getLocation().toString());
        Assert.assertEquals("TrajectSensor_Route058", trajecten.get(1).getTrajectId());
            
        Assert.assertEquals("TS_A10_S108S106_S100", trajecten.get(2).getLocation().toString());
        Assert.assertEquals("TrajectSensor_Route057", trajecten.get(2).getTrajectId());              
    }
    
    @Test
    public void JSONMeasurementsParseTest() throws FileNotFoundException, ParseException
    {
        List<Measurement> measurements = null;
        ReistijdenJSONConnector json = new ReistijdenJSONConnector("TrajectSensorsNH.GeoJSON.txt");
        String data = json.GetJSONData();
        ReistijdenJSONParser parser = new ReistijdenJSONParser(data);
            
        measurements = parser.ParseMeasurements();
        
        Assert.assertEquals(102, measurements.get(0).getTravelTime());
        Assert.assertEquals("TrajectSensor_Route059", measurements.get(0).getTrajectId());
        Assert.assertEquals(33, measurements.get(0).getVelocity());

        Assert.assertEquals(92, measurements.get(1).getTravelTime());
        Assert.assertEquals("TrajectSensor_Route058", measurements.get(1).getTrajectId());
        Assert.assertEquals(37, measurements.get(1).getVelocity());
        
        Assert.assertEquals(683, measurements.get(2).getTravelTime());
        Assert.assertEquals("TrajectSensor_Route057", measurements.get(2).getTrajectId());
        Assert.assertEquals(20, measurements.get(2).getVelocity());
        
        Assert.assertEquals(0, measurements.get(3).getTravelTime());
        Assert.assertEquals("TrajectSensor_Route056", measurements.get(3).getTrajectId());
        Assert.assertEquals(-1, measurements.get(3).getVelocity());
    }    
}*/