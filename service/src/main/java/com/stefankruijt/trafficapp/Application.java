package com.stefankruijt.trafficapp;

import com.stefankruijt.trafficapp.dao.MeasurementDao;
import com.stefankruijt.trafficapp.dao.TrajectDao;
import com.stefankruijt.trafficapp.model.Measurement;

import com.stefankruijt.trafficapp.model.Traject;
import com.stefankruijt.trafficapp.service.NewYorkTrafficService;
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
import java.net.URL;
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
     * On startup update all trajects
     */
    public void refreshTrajectData() {
        trajectDao.deleteAll();

        URL url = Thread.currentThread().getContextClassLoader().getResource("testdata/new-york-test-data.txt");
        File file = new File(url.getPath());
        // http://207.251.86.229/nyc-links-cams/LinkSpeedQuery.txt
        List<Traject> trajects = NewYorkTrafficService.NewYorkTrafficService(file);

        for(Traject traject : trajects) {
            Traject t = trajectDao.findOne(traject.getId());

            if(t == null) {
                t = new Traject(traject.getId(), traject.getLinkId(), traject.getLinkPoints(), traject.getEncodedPolyLine(),
                        traject.getEncodedPolyLineLvls(), traject.getOwner());
            }
            else {
                if(t.getHashCode() != traject.getHashCode()) {
                    t.setEncodedPolyLine(traject.getEncodedPolyLine());
                    t.setEncodedPolyLineLvls(traject.getEncodedPolyLineLvls());
                    t.setLinkId(traject.getLinkId());
                    t.setLinkPoints(traject.getLinkPoints());
                    t.setOwner(traject.getOwner());
                    t.setHashCode(traject.getHashCode());
                }
            }

            trajectDao.save(t);
        }
    }
}