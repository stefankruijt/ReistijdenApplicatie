import com.mongodb.DBCursor;
import com.mongodb.DBObject;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Date;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

@WebServlet("/reistijden")
public class ReistijdenServlet extends HttpServlet 
{
    DatabaseConnector database = new DatabaseConnector("localhost", 27017);

    /**
     * Processes requests for both HTTP <code>GET</code> and <code>POST</code>
     * methods.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        String location = request.getParameter("location");
        DBCursor json = database.getReistijden(location);
        JSONObject object = new JSONObject();
        String id = "";
        boolean locationFound = false;
        JSONArray list = new JSONArray();
        
        while(json.hasNext())
        {
            locationFound = true;
            DBObject measurement = json.next();
            id = (String) measurement.get("location");
            
            JSONArray values = new JSONArray();
            Date timestamp = (Date) measurement.get("timestamp");
            values.add(timestamp);
            int traveltime = (int) measurement.get("traveltime");
            values.add(traveltime);
            int velocity = (int) measurement.get("velocity");        
            values.add(velocity);
            
            list.add(values);
        }
         
        object.put("id", id);        
        object.put("measurements", list);
        
        response.setContentType("application/json");
        try (PrintWriter out = response.getWriter()) 
        {
            if(locationFound)
                out.print(object);
            else
                out.print("Location not found");
            out.flush();
        }
    }

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /**
     * Handles the HTTP <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        processRequest(request, response);
    }

    /**
     * Handles the HTTP <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException 
    {
        processRequest(request, response);
    }

    /**
     * Returns a short description of the servlet.
     *
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() 
    {
        return "Short description";
    }// </editor-fold>
}