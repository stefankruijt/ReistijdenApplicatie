package reistijdenapplicatie;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ReistijdenJSONConnector 
{
    private InputStream stream;

    public ReistijdenJSONConnector(URL url) throws IOException 
    {
        stream = url.openStream();
    }

    public ReistijdenJSONConnector(String file) throws FileNotFoundException 
    {
        stream = new FileInputStream(file);
    }

    public String GetJSONData() 
    {
        StringBuilder jsonString = new StringBuilder();
        try 
        {
            BufferedReader reader = new BufferedReader(new InputStreamReader(stream));
            String line;

            while ((line = reader.readLine()) != null) 
            {
                jsonString.append(line);
            }
            reader.close();
        } 
        catch (IOException e) 
        {
            e.printStackTrace();
        }
        return jsonString.toString();
    }
    
    public void closeStream()
    {
        try 
        {
            stream.close();
        } 
        catch (IOException ex) 
        {
            Logger.getLogger(ReistijdenJSONConnector.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}