package com.stefankruijt.trafficapp.job;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.scheduling.annotation.Scheduled;

import java.util.Date;

public class ImportSensorDataJob {

    private static final Logger log = LoggerFactory.getLogger(ImportSensorDataJob.class);

    @Scheduled(fixedRate = 5000)
    public void reportCurrentTime() {
        log.info("The time is now {}", new Date());
    }
}
