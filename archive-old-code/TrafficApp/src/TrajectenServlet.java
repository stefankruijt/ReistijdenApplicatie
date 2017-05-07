import java.io.IOException;
import java.io.PrintWriter;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import reistijdenapplicatie.DatabaseConnector;

public class TrajectenServlet extends HttpServlet 
{
    DatabaseConnector database;
    
    public void init(ServletConfig config) throws ServletException
    {
        ServletContext context = config.getServletContext();
        String databaseURI = context.getInitParameter("databaseUri");  
        database =  new DatabaseConnector(databaseURI);
    }
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
        response.addHeader("Access-Control-Allow-Origin", "*");
        response.addHeader("Allow", "GET");
        
        String location = request.getParameter("location");   
        String json;
        
        if(location == null)
            json = database.getAllTrajectsString();
        else
            json = database.getTrajectData(location);
        
        response.setContentType("application/json");
        try (PrintWriter out = response.getWriter()) 
        {
            out.print(json);
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
}