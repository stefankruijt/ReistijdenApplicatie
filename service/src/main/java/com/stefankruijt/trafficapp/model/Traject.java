package com.stefankruijt.trafficapp.model;

import org.springframework.data.annotation.Id;

public class Traject {

    @Id
    private String id;
    private int hashCode;

    private String linkId;
    private String linkPoints;
    private String EncodedPolyLine;
    private String EncodedPolyLineLvls;
    private String Owner;

    public Traject(String id, String linkId, String linkPoints, String encodedPolyLine, String encodedPolyLineLvls, String owner) {
        this.id = id;
        this.linkId = linkId;
        this.linkPoints = linkPoints;
        EncodedPolyLine = encodedPolyLine;
        EncodedPolyLineLvls = encodedPolyLineLvls;
        Owner = owner;

        hashCode = this.hashCode();
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public int getHashCode() {
        return hashCode;
    }

    public void setHashCode(int hashCode) {
        this.hashCode = hashCode;
    }

    public String getLinkId() {
        return linkId;
    }

    public void setLinkId(String linkId) {
        this.linkId = linkId;
    }

    public String getLinkPoints() {
        return linkPoints;
    }

    public void setLinkPoints(String linkPoints) {
        this.linkPoints = linkPoints;
    }

    public String getEncodedPolyLine() {
        return EncodedPolyLine;
    }

    public void setEncodedPolyLine(String encodedPolyLine) {
        EncodedPolyLine = encodedPolyLine;
    }

    public String getEncodedPolyLineLvls() {
        return EncodedPolyLineLvls;
    }

    public void setEncodedPolyLineLvls(String encodedPolyLineLvls) {
        EncodedPolyLineLvls = encodedPolyLineLvls;
    }

    public String getOwner() {
        return Owner;
    }

    public void setOwner(String owner) {
        Owner = owner;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        Traject traject = (Traject) o;

        if (hashCode != traject.hashCode) return false;
        if (id != null ? !id.equals(traject.id) : traject.id != null) return false;
        if (!linkId.equals(traject.linkId)) return false;
        if (!linkPoints.equals(traject.linkPoints)) return false;
        if (!EncodedPolyLine.equals(traject.EncodedPolyLine)) return false;
        if (!EncodedPolyLineLvls.equals(traject.EncodedPolyLineLvls)) return false;
        return Owner.equals(traject.Owner);
    }

    @Override
    public int hashCode() {
        int result = id != null ? id.hashCode() : 0;
        result = 31 * result + hashCode;
        result = 31 * result + linkId.hashCode();
        result = 31 * result + linkPoints.hashCode();
        result = 31 * result + EncodedPolyLine.hashCode();
        result = 31 * result + EncodedPolyLineLvls.hashCode();
        result = 31 * result + Owner.hashCode();
        return result;
    }
}