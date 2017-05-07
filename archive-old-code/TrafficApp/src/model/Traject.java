package model;

import java.util.List;
import org.json.simple.JSONArray;

public class Traject {

    private String trajectId;
    private String location;
    private String type;
    private JSONArray rdCoordinates;
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

    public JSONArray getRDCoordinates() {
        return rdCoordinates;
    }

    public void setRDCoordinates(JSONArray coordinates) {
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
        return "Traject [trajectId=" + trajectId + ", type=" + type
                + ", coordinates=" + rdCoordinates + "]";
    }
}