package com.stefankruijt.trafficapp;

import com.stefankruijt.trafficapp.dao.MeasurementDao;
import com.stefankruijt.trafficapp.dao.TrajectDao;
import com.stefankruijt.trafficapp.model.Measurement;

import com.stefankruijt.trafficapp.model.Traject;
import com.stefankruijt.trafficapp.util.CoordinateConvertor;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.core.io.ClassPathResource;
import org.springframework.core.io.Resource;
import org.springframework.scheduling.annotation.EnableScheduling;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

@SpringBootApplication
@EnableScheduling
public class Application implements CommandLineRunner{

    @Autowired
    private MeasurementDao measurementDao;

    @Autowired
    private TrajectDao trajectDao;

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }

    @Override
    public void run(String... args) throws Exception {
        measurementDao.deleteAll();
        measurementDao.save(new Measurement(new Date(), 12, 12));
        measurementDao.save(new Measurement(new Date(), 15, 18));
        measurementDao.save(new Measurement(new Date(), 14, 13));

        refreshTrajectData();
    }

    /***
     * On startup read traject sensor file and update database.
     */
    public void refreshTrajectData() {
        trajectDao.deleteAll();
        JSONObject sensors = readTrajectSensorsFromFile("TrajectSensorsNH.json");

        JSONArray features = (JSONArray) sensors.get("features");
        for(Object element : features) {
            JSONObject a = (JSONObject) element;
            Traject t = trajectDao.findOne(a.get("Id").toString());

            if(t == null) {
                t = new Traject(a.get("Id").toString());
                JSONObject geometry = (JSONObject) a.get("geometry");
                JSONArray coordinates = (JSONArray) geometry.get("coordinates");

                //t.setEtrs89Coordinates(ConvertRDCoordinatesToETRS89(a.get("Id")));
            }

            trajectDao.save(t);
        }
    }

    private JSONObject readTrajectSensorsFromFile(String fileName) {
        JSONParser parser = new JSONParser();
        JSONObject result = new JSONObject();

        try {
            Resource resource = new ClassPathResource(fileName);
            result = (JSONObject) parser.parse(readFile(resource.getFile().getPath()));
        } catch (IOException e) {
            e.printStackTrace();
        } catch (ParseException e) {
            e.printStackTrace();
        }

        return result;
    }

    private List<double[]> ConvertRDCoordinatesToETRS89(String[] rdCoordinates)
    {
        List<double[]> convertedCoordinates = new ArrayList(rdCoordinates.length);

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

    static String readFile(String path) throws IOException
    {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, StandardCharsets.UTF_8);
    }
}