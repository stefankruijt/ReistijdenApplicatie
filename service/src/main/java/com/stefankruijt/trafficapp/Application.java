package com.stefankruijt.trafficapp;

import com.stefankruijt.trafficapp.dao.MeasurementDao;
import com.stefankruijt.trafficapp.model.Measurement;

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
import java.util.Date;

@SpringBootApplication
@EnableScheduling
public class Application implements CommandLineRunner{

    @Autowired
    private MeasurementDao repository;

    public static void main(String[] args) {
        SpringApplication.run(Application.class, args);
    }

    @Override
    public void run(String... args) throws Exception {
        repository.deleteAll();
        repository.save(new Measurement(new Date(), 12, 12));
        repository.save(new Measurement(new Date(), 15, 18));
        repository.save(new Measurement(new Date(), 14, 13));

        refreshTrajectData();
    }

    /***
     * On startup read traject sensor file and update database.
     */
    public void refreshTrajectData() {
        JSONObject sensors = readTrajectSensorsFromFile("TrajectSensorsNH.GeoJSON.txt");

        // TODO Update mongodb
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

    static String readFile(String path) throws IOException
    {
        byte[] encoded = Files.readAllBytes(Paths.get(path));
        return new String(encoded, StandardCharsets.UTF_8);
    }
}
