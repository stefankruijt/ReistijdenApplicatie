package util;

import java.io.ByteArrayOutputStream;
import java.io.InputStream;

public class CoordinateConvertor 
{
    public static double[] ConvertRDCoordinateToETRS89(double x, double y) 
    {
        double[] result = new double[2];

        String filePath = "G:\\Software Development\\CoodinateConversion\\Debug\\CoodinateConversion.exe " + x + " " + y + " " + 0;
        System.out.println(filePath);
        try 
        {
            Process p = Runtime.getRuntime().exec(filePath);
            p.waitFor();
            InputStream in = p.getInputStream();
            ByteArrayOutputStream baos = new ByteArrayOutputStream();

            int c = -1;
            while ((c = in.read()) != -1) 
            {
                baos.write(c);
            }

            String response = new String(baos.toByteArray());
            String[] resultString = response.split("\n");
            result[0] = Double.parseDouble(resultString[0]);
            result[1] = Double.parseDouble(resultString[1]);
            return result;

        } 
        catch (Exception e) 
        {
            e.printStackTrace();
        }
        return new double[]{-1, -1};
    }
}