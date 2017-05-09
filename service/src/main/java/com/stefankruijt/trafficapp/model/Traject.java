package com.stefankruijt.trafficapp.model;

import org.springframework.data.annotation.Id;

import java.util.Arrays;
import java.util.List;

public class Traject {

    @Id
    private String trajectId;

    private String location;
    private String type;
    private String[] rdCoordinates;
    private List<long[]> etrs89Coordinates;

    public String getTrajectId() {
        return trajectId;
    }

    public void setTrajectId(String trajectId) {
        this.trajectId = trajectId;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public String getLocation() {
        return location;
    }

    public void setLocation(String location) {
        this.location = location;
    }

    public String[] getRDCoordinates() {
        return rdCoordinates;
    }

    public void setRDCoordinates(String[] coordinates) {
        this.rdCoordinates = coordinates;
    }

    public List<long[]> getEtrs89Coordinates() {
        return etrs89Coordinates;
    }

    public void setEtrs89Coordinates(List<long[]> etrs89Coordinates) {
        this.etrs89Coordinates = etrs89Coordinates;
    }

    @Override
    public String toString() {
        return "Traject{" +
                "trajectId='" + trajectId + '\'' +
                ", location='" + location + '\'' +
                ", type='" + type + '\'' +
                ", rdCoordinates=" + Arrays.toString(rdCoordinates) +
                ", etrs89Coordinates=" + etrs89Coordinates +
                '}';
    }
}