package com.stefankruijt.trafficapp.util;

import com.stefankruijt.trafficapp.model.Traject;
import org.junit.Test;

import java.io.File;
import java.net.URL;
import java.util.List;

import static org.hamcrest.core.Is.is;
import static org.hamcrest.core.IsEqual.equalTo;
import static org.junit.Assert.assertThat;

public class TsvReaderTest {
    @Test
    public void readTrajects() throws Exception {
        URL url = Thread.currentThread().getContextClassLoader().getResource("new-york-test-data.txt");
        File file = new File(url.getPath());

        List<Traject> trajects = TsvReader.readTrajects(file);

        assertThat(trajects.isEmpty(), is(false));
        assertThat(trajects.size(), is(21));
        assertThat(trajects.get(0).getLinkId(), equalTo("4616337"));
        assertThat(trajects.get(0).getId(), equalTo("1"));
        assertThat(trajects.get(0).getOwner(), equalTo("NYC_DOT_LIC"));
    }
}