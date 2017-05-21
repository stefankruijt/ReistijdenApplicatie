package com.stefankruijt.trafficapp.model;

import org.springframework.data.annotation.Id;

import java.util.Arrays;
import java.util.List;

public class Traject {

    @Id
    private String id;

    private int hashCode;
    private String name;
    private String location;
    private String type;
    private String[] rdCoordinates;
    private List<long[]> etrs89Coordinates;

    public Traject(String name) {
        this.name = name;
        this.hashCode = hashCode();
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
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
                "id='" + id + '\'' +
                ", hashCode='" + hashCode + '\'' +
                ", location='" + location + '\'' +
                ", type='" + type + '\'' +
                ", rdCoordinates=" + Arrays.toString(rdCoordinates) +
                ", etrs89Coordinates=" + etrs89Coordinates +
                '}';
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Traject traject = (Traject) o;

        if (hashCode != traject.hashCode) return false;
        if (id != null ? !id.equals(traject.id) : traject.id != null) return false;
        if (name != null ? !name.equals(traject.name) : traject.name != null) return false;
        if (location != null ? !location.equals(traject.location) : traject.location != null) return false;
        if (type != null ? !type.equals(traject.type) : traject.type != null) return false;
        // Probably incorrect - comparing Object[] arrays with Arrays.equals
        if (!Arrays.equals(rdCoordinates, traject.rdCoordinates)) return false;
        return etrs89Coordinates != null ? etrs89Coordinates.equals(traject.etrs89Coordinates) : traject.etrs89Coordinates == null;
    }

    @Override
    public int hashCode() {
        int result = id != null ? id.hashCode() : 0;
        result = 31 * result + hashCode;
        result = 31 * result + (name != null ? name.hashCode() : 0);
        result = 31 * result + (location != null ? location.hashCode() : 0);
        result = 31 * result + (type != null ? type.hashCode() : 0);
        result = 31 * result + Arrays.hashCode(rdCoordinates);
        result = 31 * result + (etrs89Coordinates != null ? etrs89Coordinates.hashCode() : 0);
        return result;
    }
}