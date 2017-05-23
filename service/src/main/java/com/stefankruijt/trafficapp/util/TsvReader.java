package com.stefankruijt.trafficapp.util;

import com.stefankruijt.trafficapp.model.Traject;
import org.apache.commons.io.FileUtils;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class TsvReader {

    public static List<Traject> readTrajects(File file) throws IOException{
        String fileText = FileUtils.readFileToString(file, "UTF-8");
        return readTrajects(fileText);
    }

    public static List<Traject> readTrajects(InputStream stream) throws IOException{
        StringBuilder data = new StringBuilder();

        BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
        String line;

        while ((line = reader.readLine()) != null) {
            data.append(line);
        }
        reader.close();

        return readTrajects(data.toString());
    }

    private static List<Traject> readTrajects(String fileText) {
        List<Traject> result = new ArrayList<>();

        Scanner scanner = new Scanner(fileText);
        int i = 0;

        String[] fieldNames = new String[0];
        while(scanner.hasNextLine()) {
            String line = scanner.nextLine();
            if(!line.endsWith("\"")) {
                String nextLine = scanner.nextLine();
                line = line.concat(nextLine);
            }

            String[] splitted = line.split("\t");

            // First line contains field names
            if(i==0) {
                fieldNames = new String[splitted.length];
                for(int j=0; j<splitted.length; j++) {
                    fieldNames[j] = splitted[j];
                }
            }
            else {
                if(splitted.length != fieldNames.length) {
                    System.out.println("Skip line + " + i + " since it has an incorrect amount of fields => " + line);
                    continue;
                }
                result.add(new Traject(splitted[0].replace("\"", ""),
                        splitted[5].replace("\"", ""),
                        splitted[6].replace("\"", ""),
                        splitted[7].replace("\"", ""),
                        splitted[8].replace("\"", ""),
                        splitted[9].replace("\"", "")));
            }
            i++;
        }

        return result;
    }
}