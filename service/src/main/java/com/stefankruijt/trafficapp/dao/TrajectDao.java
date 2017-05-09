package com.stefankruijt.trafficapp.dao;

import com.stefankruijt.trafficapp.model.Traject;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface TrajectDao extends MongoRepository<Traject, String> {

}
