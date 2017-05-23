package com.stefankruijt.trafficapp.oldcode;

/*import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import com.mongodb.util.JSON;
import java.io.IOException;
import java.io.PrintWriter;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import reistijdenapplicatie.DatabaseConnector;

public class ReistijdenServlet extends HttpServlet 
{
    DatabaseConnector database;

    public void init(ServletConfig config) throws ServletException
    {
        ServletContext context = config.getServletContext();
        String databaseURI = context.getInitParameter("databaseUri");  
        database =  new DatabaseConnector(databaseURI);
    }

     * Processes requests for both HTTP <code>GET</code> and <code>POST</code>
     * methods.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs

    protected void processRequest(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        response.addHeader("Access-Control-Allow-Origin", "*");
        response.addHeader("Allow", "GET");       
        
        String location = request.getParameter("location");
        DBCursor json = database.getReistijden(location, 20);
        JSONObject object = new JSONObject();
        String id = "";
        boolean locationFound = false;
        JSONArray list = new JSONArray();
        
        while(json.hasNext())
        {
            locationFound = true;
            DBObject measurement = json.next();
            id = (String) measurement.get("location");
            
            JSONObject values = new JSONObject();
            values.put("timestamp", measurement.get("timestamp"));
            values.put("traveltime", measurement.get("traveltime"));
            values.put("velocity", measurement.get("velocity"));
            
            list.add(values);
        }
         
        object.put("id", id);        
        object.put("measurements", list);
        
        response.setContentType("application/json");
        try (PrintWriter out = response.getWriter()) 
        {
            if(locationFound)
                out.print(JSON.serialize(object));
            else
                out.print("Location not found");
            out.flush();
        }
    }

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">

     * Handles the HTTP <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        processRequest(request, response);
    }


     * Handles the HTTP <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        processRequest(request, response);
    }
}*/