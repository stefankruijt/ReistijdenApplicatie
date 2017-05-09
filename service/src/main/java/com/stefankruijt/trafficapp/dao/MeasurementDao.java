package com.stefankruijt.trafficapp.dao;

import com.stefankruijt.trafficapp.model.Measurement;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface MeasurementDao extends MongoRepository<Measurement, String> {

}
