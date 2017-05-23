package com.stefankruijt.trafficapp.service;

import com.stefankruijt.trafficapp.model.Traject;
import com.stefankruijt.trafficapp.util.TsvReader;

import java.io.*;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;

public class NewYorkTrafficService {

    public static List<Traject> NewYorkTrafficService(URL url)
    {
        try {
            InputStream stream = url.openStream();
            return TsvReader.readTrajects(stream);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new ArrayList<>();
    }

    public static List<Traject> NewYorkTrafficService(File file)
    {
        try {
            InputStream stream = new FileInputStream(file);
            return TsvReader.readTrajects(file);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return new ArrayList<>();
    }
}