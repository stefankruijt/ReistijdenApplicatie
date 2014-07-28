/*
**--------------------------------------------------------------
**    RDNAPTRANS(TM)2008
**
**    Authors: Jochem Lesparre, Joop van Buren, Marc Crombaghs, Frank Dentz, Arnoud Pol, Sander Oude Elberink
**             http://www.rdnap.nl
**    Based on RDNAPTRANS2004
**    Main changes:
**    - 7 similarity transformation parameters
**    - 0.0088 offset in the transformation between ellipsoidal height (h) and orthometric heights (NAP)
**    - coordinates are computed also outside the validity regions of the grid files x2c.grd, y2c.grd and nlgeo04.grd
**--------------------------------------------------------------
*/

/*
**--------------------------------------------------------------
**    Include statements and namespace
**--------------------------------------------------------------
*/
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;

/*
**--------------------------------------------------------------
**    Static data declarations
**    Mathematical constant pi = 3.14...
**--------------------------------------------------------------
*/
const double PI = 2.0 * asin(1.0);
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Geographic NL-Bessel coordinates of Amersfoort (pivot point and projection base point)
**        phi     latitude in decimal degrees
**        lambda  longitude in decimal degrees
**        h       ellipsoidal height in meters
**    Source of constants:
**        Hk.J. Heuvelink, "De stereografische kaartprojectie in hare toepassing bij de Rijksdriehoeksmeting". Delft: Rijkscommissie voor Graadmeting en Waterpassing, 1918.
**        HTW, "Handleiding voor de Technische Werkzaamheden van het Kadaster". Apeldoorn: Kadaster, 1996.
**--------------------------------------------------------------
*/
const double PHI_AMERSFOORT_BESSEL    = 52.0+ 9.0/60.0+22.178/3600.0;
const double LAMBDA_AMERSFOORT_BESSEL =  5.0+23.0/60.0+15.500/3600.0;
const double H_AMERSFOORT_BESSEL      =  0.0;
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Parameters of ellipsoids Bessel1841 and GRS80
**        a      half major axis in meters
**        inv_f  inverse flattening
**    Source of constants: HTW, "Handleiding voor de Technische Werkzaamheden van het Kadaster". Apeldoorn: Kadaster, 1996.
**--------------------------------------------------------------
*/
const double A_BESSEL     = 6377397.155;
const double INV_F_BESSEL = 299.1528128;
const double A_ETRS       = 6378137;
const double INV_F_ETRS   = 298.257222101;
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Transformation parameters relative to pivot point Amersfoort. Note: Do NOT confuse with parameters for the center of the ellipsoid!
**        tx     translation in direction of x axis in meters
**        ty     translation in direction of y axis in meters
**        tz     translation in direction of z axis in meters
**        alpha  rotation around x axis in radials
**        beta   rotation around y axis in radials
**        gamma  rotation around z axis in radials
**        delta  scale parameter (scale = 1 + delta)
**    Source of constants: A. de Bruijne, J. van Buren, A. Kösters and H. van der Marel, "De geodetische referentiestelsels van Nederland; Definitie en vastlegging van ETRS89, RD en NAP en hun onderlinge relatie". Delft: Nederlandse Commissie voor Geodesie (NCG), to be published in 2005.
**--------------------------------------------------------------
*/
const double TX_BESSEL_ETRS    =  593.0248;
const double TY_BESSEL_ETRS    =   25.9984;
const double TZ_BESSEL_ETRS    =  478.7459;
const double ALPHA_BESSEL_ETRS =    1.9342e-6;
const double BETA_BESSEL_ETRS  =   -1.6677e-6;
const double GAMMA_BESSEL_ETRS =    9.1019e-6;
const double DELTA_BESSEL_ETRS =    4.0725e-6;

const double TX_ETRS_BESSEL    = -593.0248;
const double TY_ETRS_BESSEL    =  -25.9984;
const double TZ_ETRS_BESSEL    = -478.7459;
const double ALPHA_ETRS_BESSEL =   -1.9342e-6;
const double BETA_ETRS_BESSEL  =    1.6677e-6;
const double GAMMA_ETRS_BESSEL =   -9.1019e-6;
const double DELTA_ETRS_BESSEL =   -4.0725e-6;
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Parameters of RD projection
**        scale         scale factor (k in some notations)
**                      this factor was first defined by Hk.J. Heuvelink as pow(10,-400e-7), nowadays we define it as exactly 0.9999079
**        x_amersfoort  false Easting
**        y_amersfoort  false Northing
**    Source of constants:
**        G. Bakker, J.C. de Munck and G.L. Strang van Hees, "Radio Positioning at Sea". Delft University of Technology, 1995.
**        G. Strang van Hees, "Globale en lokale geodetische systemen". Delft: Nederlandse Commissie voor Geodesie (NCG), 1997.
**--------------------------------------------------------------
*/
const double SCALE_RD = 0.9999079;
const double X_AMERSFOORT_RD = 155000;
const double Y_AMERSFOORT_RD = 463000;
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Names of grd files
**
**    Grd files are binary grid files in the format of the program Surfer(R)
**    The header contains information on the number of grid points, bounding box and extreme values.
**
**    RD-corrections in x and y
**
**          -8000 meters < RD Easting  (stepsize 1 km) < 301000 meters
**         288000 meters < RD Northing (stepsize 1 km) < 630000 meters
**
**    Geoid model NLGEO2004
**
**        50.525   degrees < ETRS89 latitude  (stepsize 0.050000 degrees) < 53.675 degrees
**         3.20833 degrees < ETRS89 longitude (stepsize 0.083333 degrees) <  7.45833 degrees
**
**        Alternative notation:
**        50° 31' 30" < ETRS89_latitude  (stepsize 0° 3' 0") < 53° 40' 30"
**         3° 12' 30" < ETRS89_longitude (stepsize 0° 5' 0") <  7° 27' 30"
**
**        The stepsizes correspond to about 5,5 km x 5,5 km in the Netherlands.
**--------------------------------------------------------------
*/
const string GRID_FILE_DX    = "x2c.grd";
const string GRID_FILE_DY    = "y2c.grd";
const string GRID_FILE_GEOID = "nlgeo04.grd";
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Precision parameters for iterations (respectively in meters and degrees)
**--------------------------------------------------------------
*/
const double PRECISION     = 0.0001;
const double DEG_PRECISION = PRECISION/40e6*360;
/*
**--------------------------------------------------------------
**    Continuation of static data declarations
**    Mean difference between NAP and ellipsoidal Bessel height. This is only used for getting from x, y in RD to phi, lambda in ETRS89.
**--------------------------------------------------------------
*/
const double MEAN_GEOID_HEIGHT_BESSEL = 0.0;

/*
**--------------------------------------------------------------
**    Functions
**--------------------------------------------------------------
*/

/*
**--------------------------------------------------------------
**    Function name: deg_sin
**    Description:   sine for angles in degrees
**
**    Parameter      Type        In/Out Req/Opt Default
**    alpha          double      in     req     none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    sin(alpha)
**--------------------------------------------------------------
*/
double deg_sin(double alpha)
{
    return sin(alpha/180.0*PI);
}

/*
**--------------------------------------------------------------
**    Function name: deg_cos
**    Description:   cosine for angles in degrees
**
**    Parameter      Type        In/Out Req/Opt Default
**    alpha          double      in     req     none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    cos(alpha)
**--------------------------------------------------------------
*/
double deg_cos(double alpha)
{
    return cos(alpha/180.0*PI);
}

/*
**--------------------------------------------------------------
**    Function name: deg_tan
**    Description:   tangent for angles in degrees
**
**    Parameter      Type        In/Out Req/Opt Default
**    alpha          double      in     req     none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    tan(alpha)
**--------------------------------------------------------------
*/
double deg_tan(double alpha)
{
    return tan(alpha/180.0*PI);
}

/*
**--------------------------------------------------------------
**    Function name: deg_asin
**    Description:   inverse sine for angles in degrees
**
**    Parameter      Type        In/Out Req/Opt Default
**    a              double      in     req     none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    asin(a)
**--------------------------------------------------------------
*/
double deg_asin(double a)
{
    return (asin(a)*180.0/PI);
}

/*
**--------------------------------------------------------------
**    Function name: deg_atan
**    Description:   inverse tangent for angles in degrees
**
**    Parameter      Type        In/Out Req/Opt Default
**    a              double in     req     none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    atan(a)
**--------------------------------------------------------------
*/
double deg_atan(double a)
{
    return (atan(a)*180.0/PI);
}

/*
**--------------------------------------------------------------
**    Function name: atanh
**    Description:   inverse hyperbolic tangent
**
**    Parameter      Type        In/Out Req/Opt Default
**    a              double      in     req     none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    atanh(a)
**--------------------------------------------------------------
*/
double atanh(double a)
{
    return (0.5*log((1.0+a)/(1.0-a)));
}

/*
**--------------------------------------------------------------
**    Function name: deg_min_sec2decimal
**    Description:   converts from degrees, minutes and seconds to decimal degrees
**
**    Parameter      Type        In/Out Req/Opt Default
**    deg            double      in     req     none
**    min            double      in     req     none
**    sec            double      in     req     none
**    dec_deg        double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    All parameters are doubles, so one can also enter decimal minutes or degrees.
**    Note: Nonsense input is accepted too.
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void deg_min_sec2decimal(double deg, double min, double sec, double& dec_deg)
{
    dec_deg = (deg+min/60.0+sec/3600.0);
}

/*
**--------------------------------------------------------------
**    Function name: decimal2deg_min_sec
**    Description:   converts from decimal degrees to degrees, minutes and seconds
**
**    Parameter      Type        In/Out Req/Opt Default
**    dec_deg        double      in     req     none
**    deg            int         out    -       none
**    min            int         out    -       none
**    sec            double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void decimal2deg_min_sec(double dec_deg, int& deg, int& min, double& sec)
{
    deg = int(dec_deg);
    min = int((dec_deg-deg)*60.0);
    sec = ((dec_deg-deg)*60.0-min)*60.0;
}

/*
**--------------------------------------------------------------
**    Function name: geographic2cartesian
**    Description:   from geographic coordinates to cartesian coordinates
**
**    Parameter      Type        In/Out Req/Opt Default
**    phi            double      in     req     none
**    lambda         double      in     req     none
**    h              double      in     req     none
**    a              double      in     req     none
**    inv_f          double      in     req     none
**    x              double      out    -       none
**    y              double      out    -       none
**    z              double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    phi      latitude in degrees
**    lambda   longitude in degrees
**    h        ellipsoidal height
**    a        half major axis of the ellisoid
**    inv_f    inverse flattening of the ellipsoid
**    x, y, z  output of cartesian coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void geographic2cartesian(double phi, double lambda, double h,
                          double a, double inv_f,
                          double& x, double& y, double& z )
{
    /*
    **--------------------------------------------------------------
    **    Source: G. Bakker, J.C. de Munck and G.L. Strang van Hees, "Radio Positioning at Sea". Delft University of Technology, 1995.
    **--------------------------------------------------------------
    */

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **        f    flattening of the ellipsoid
    **        ee   first eccentricity squared (e squared in some notations)
    **        n    second (East West) principal radius of curvature (N in some notations)
    **--------------------------------------------------------------
    */
    double f  = 1.0/inv_f;
    double ee = f*(2.0-f);
    double n  = a/sqrt(1.0-ee*pow(deg_sin(phi),2));
    x = (n+h)*deg_cos(phi)*deg_cos(lambda);
    y = (n+h)*deg_cos(phi)*deg_sin(lambda);
    z = (n*(1.0-ee)+h)*deg_sin(phi);
}

/*
**--------------------------------------------------------------
**    Function name: cartesian2geographic
**    Description:   from cartesian coordinates to geographic coordinates
**
**    Parameter      Type        In/Out Req/Opt Default
**    x              double      in     req     none
**    y              double      in     req     none
**    z              double      in     req     none
**    a              double      in     req     none
**    inv_f          double      in     req     none
**    phi            double      out    -       none
**    lambda         double      out    -       none
**    h              double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x, y, z  input of cartesian coordinates
**    a        half major axis of the ellisoid
**    inv_f    inverse flattening of the ellipsoid
**    phi      output latitude in degrees
**    lambda   output longitude in degrees
**    h        output ellipsoidal height
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void cartesian2geographic(double x, double y, double z,
                          double a, double inv_f,
                          double& phi, double& lambda, double& h )
{
    /*
    **--------------------------------------------------------------
    **    Source: G. Bakker, J.C. de Munck and G.L. Strang van Hees, "Radio Positioning at Sea". Delft University of Technology, 1995.
    **--------------------------------------------------------------
    */

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **        f    flattening of the ellipsoid
    **        ee   first eccentricity squared (e squared in some notations)
    **        rho  distance to minor axis
    **        n    second (East West) principal radius of curvature (N in some notations)
    **--------------------------------------------------------------
    */
    double f   = 1.0/inv_f;
    double ee  = f*(2.0-f);
    double rho = sqrt(x*x+y*y);
    double n;

    /*
    **--------------------------------------------------------------
    **    Iterative calculation of phi
    **--------------------------------------------------------------
    */
    phi=0;
    double previous;
    double diff=90;
    while (diff>DEG_PRECISION)
    {
        previous = phi;
        n        = a/sqrt(1.0-ee*pow(deg_sin(phi),2));
        phi      = deg_atan(z/rho+n*ee*deg_sin(phi)/rho);
        diff     = fabs(phi-previous);
    }

    /*
    **--------------------------------------------------------------
    **     Calculation of lambda and h
    **--------------------------------------------------------------
    */
    lambda = deg_atan(y/x);
    h      = rho*deg_cos(phi)+z*deg_sin(phi)-n*(1.0-ee*pow(deg_sin(phi),2));
}

/*
**--------------------------------------------------------------
**    Function name: sim_trans
**    Description:   3 dimensional similarity transformation (7 parameters) around another pivot point "a" than the origin
**
**    Parameter      Type        In/Out Req/Opt Default
**    x_in           double      in     req     none
**    y_in           double      in     req     none
**    z_in           double      in     req     none
**    tx             double      in     req     none
**    ty             double      in     req     none
**    tz             double      in     req     none
**    alpha          double      in     req     none
**    beta           double      in     req     none
**    gamma          double      in     req     none
**    delta          double      in     req     none
**    xa             double      in     req     none
**    ya             double      in     req     none
**    za             double      in     req     none
**    x_out          double      out    -       none
**    y_out          double      out    -       none
**    z_out          double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x_in, y_in, z_in     input coordinates
**    tx                   translation in direction of x axis
**    ty                   translation in direction of y axis
**    tz                   translation in direction of z axis
**    alpha                rotation around x axis in radials
**    beta                 rotation around y axis in radials
**    gamma                rotation around z axis in radials
**    delta                scale parameter (scale = 1 + delta)
**    xa, ya, za           coordinates of pivot point a (in case of rotation around the center of the ellipsoid these parameters are zero)
**    x_out, y_out, z_out  output coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void sim_trans(double x_in, double y_in, double z_in,
               double tx, double ty, double tz,
               double alpha, double beta, double gamma,
               double delta,
               double xa, double ya, double za,
               double& x_out, double& y_out, double& z_out)

{
    /*
    **--------------------------------------------------------------
    **    Source: HTW, "Handleiding voor de Technische Werkzaamheden van het Kadaster". Apeldoorn: Kadaster, 1996.
    **--------------------------------------------------------------
    */

    /*
    **--------------------------------------------------------------
    **    Calculate the elements of the rotation_matrix:
    **
    **    a b c
    **    d e f
    **    g h i
    **
    **--------------------------------------------------------------
    */
    double a = cos(gamma)*cos(beta);
    double b = cos(gamma)*sin(beta)*sin(alpha)+sin(gamma)*cos(alpha);
    double c = -cos(gamma)*sin(beta)*cos(alpha)+sin(gamma)*sin(alpha);
    double d = -sin(gamma)*cos(beta);
    double e = -sin(gamma)*sin(beta)*sin(alpha)+cos(gamma)*cos(alpha);
    double f = sin(gamma)*sin(beta)*cos(alpha)+cos(gamma)*sin(alpha);
    double g = sin(beta);
    double h = -cos(beta)*sin(alpha);
    double i = cos(beta)*cos(alpha);

    /*
    **--------------------------------------------------------------
    **    Calculate the elements of the vector input_point:
    **    point_2 = input_point - pivot_point
    **--------------------------------------------------------------
    */
    double x = x_in-xa;
    double y = y_in-ya;
    double z = z_in-za;

    /*
    **--------------------------------------------------------------
    **    Calculate the elements of the output vector:
    **    output_point = scale * rotation_matrix * point_2 + translation_vector + pivot_point
    **--------------------------------------------------------------
    */
    x_out = (1.0+delta)*(a*x+b*y+c*z)+tx+xa;
    y_out = (1.0+delta)*(d*x+e*y+f*z)+ty+ya;
    z_out = (1.0+delta)*(g*x+h*y+i*z)+tz+za;
}

/*
**--------------------------------------------------------------
**    Function name: rd_projection
**    Description:   stereographic double projection
**
**    Parameter      Type        In/Out Req/Opt Default
**    phi            double      in     req     none
**    lambda         double      in     req     none
**    x_rd           double      out    -       none
**    y_rd           double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    phi         input Bessel latitude in degrees
**    lambda      input Bessel longitude in degrees
**    x_rd, rd_y  output RD coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void rd_projection(double phi, double lambda,
                   double& x_rd, double& y_rd)
{
    /*
    **--------------------------------------------------------------
    **    Source: G. Bakker, J.C. de Munck and G.L. Strang van Hees, "Radio Positioning at Sea". Delft University of Technology, 1995.
    **            G. Strang van Hees, "Globale en lokale geodetische systemen". Delft: Nederlandse Commissie voor Geodesie (NCG), 1997.
    **--------------------------------------------------------------
    */

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of constants:
    **        f                         flattening of the ellipsoid
    **        ee                        first eccentricity squared (e squared in some notations)
    **        e                         first eccentricity
    **        eea                       second eccentricity squared (e' squared in some notations)
    **
    **        phi_amersfoort_sphere     latitude of projection base point Amersfoort on sphere in degrees
    **        lambda_amersfoort_sphere  longitude of projection base point Amersfoort on sphere in degrees
    **
    **        r1                        first (North South) principal radius of curvature in Amersfoort (M in some notations)
    **        r2                        second (East West) principal radius of curvature in Amersfoort (N in some notations)
    **        r_sphere                  radius of sphere
    **
    **        n                         constant of Gaussian projection n = 1.000475...
    **        q_amersfoort              isometric latitude of Amersfoort on ellipsiod
    **        w_amersfoort              isometric latitude of Amersfoort on sphere
    **        m                         constant of Gaussian projection m = 0.003773... (also named c in some notations)
    **--------------------------------------------------------------
    */
    const double f=1/INV_F_BESSEL;
    const double ee=f*(2-f);
    const double e=sqrt(ee);
    const double eea = ee/(1.0-ee);

    const double phi_amersfoort_sphere = deg_atan(deg_tan(PHI_AMERSFOORT_BESSEL)/sqrt(1+eea*pow(deg_cos(PHI_AMERSFOORT_BESSEL),2)));
    const double lambda_amersfoort_sphere = LAMBDA_AMERSFOORT_BESSEL;

    const double r1 = A_BESSEL*(1-ee)/pow(sqrt(1-ee*pow(deg_sin(PHI_AMERSFOORT_BESSEL),2)),3);
    const double r2 = A_BESSEL/sqrt(1.0-ee*pow(deg_sin(PHI_AMERSFOORT_BESSEL),2));
    const double r_sphere = sqrt(r1*r2);

    const double n = sqrt(1+eea*pow(deg_cos(PHI_AMERSFOORT_BESSEL),4));
    const double q_amersfoort = atanh(deg_sin(PHI_AMERSFOORT_BESSEL))-e*atanh(e*deg_sin(PHI_AMERSFOORT_BESSEL));
    const double w_amersfoort = log(deg_tan(45+0.5*phi_amersfoort_sphere));
    const double m = w_amersfoort-n*q_amersfoort;

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **        q                    isometric latitude on ellipsiod
    **        w                    isometric latitude on sphere
    **        phi_sphere           latitide on sphere in degrees
    **        delta_lambda_sphere  difference in longitude on sphere with Amersfoort in degrees
    **        psi                  distance angle from Amersfoort on sphere
    **        alpha                azimuth from Amersfoort
    **        r                    distance from Amersfoort in projection plane
    **--------------------------------------------------------------
    */
    double q                    = atanh(deg_sin(phi))-e*atanh(e*deg_sin(phi));
    double w                    = n*q+m;
    double phi_sphere           = 2*deg_atan(exp(w))-90;
    double delta_lambda_sphere  = n*(lambda-lambda_amersfoort_sphere);
    double sin_half_psi_squared = pow(deg_sin(0.5*(phi_sphere-phi_amersfoort_sphere)),2)+pow(deg_sin(0.5*delta_lambda_sphere),2)*deg_cos(phi_sphere)*deg_cos(phi_amersfoort_sphere);
    double sin_half_psi         = sqrt(sin_half_psi_squared);
    double cos_half_psi         = sqrt(1-sin_half_psi_squared);
    double tan_half_psi         = sin_half_psi/cos_half_psi;
    double sin_psi              = 2*sin_half_psi*cos_half_psi;
    double cos_psi              = 1-2*sin_half_psi_squared;
    double sin_alpha            = deg_sin(delta_lambda_sphere)*(deg_cos(phi_sphere)/sin_psi);
    double cos_alpha            = (deg_sin(phi_sphere)-deg_sin(phi_amersfoort_sphere)*cos_psi)/(deg_cos(phi_amersfoort_sphere)*sin_psi);
    double r                    = 2*SCALE_RD*r_sphere*tan_half_psi;

    x_rd = r*sin_alpha+X_AMERSFOORT_RD;
    y_rd = r*cos_alpha+Y_AMERSFOORT_RD;
}

/*
**--------------------------------------------------------------
**    Function name: inv_rd_projection
**    Description:   inverse stereographic double projection
**
**    Parameter      Type        In/Out Req/Opt Default
**    x_rd           double      in     req     none
**    y_rd           double      in     req     none
**    phi            double      out    -       none
**    lambda         double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x_rd, rd_y  input RD coordinates
**    phi         output Bessel latitude in degrees
**    lambda      output Bessel longitude in degrees
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
void inv_rd_projection(double x_rd, double y_rd,
                       double& phi, double& lambda)
{
    /*
    **--------------------------------------------------------------
    **    Source: G. Bakker, J.C. de Munck and G.L. Strang van Hees, "Radio Positioning at Sea". Delft University of Technology, 1995.
    **            G. Strang van Hees, "Globale en lokale geodetische systemen". Delft: Nederlandse Commissie voor Geodesie (NCG), 1997.
    **--------------------------------------------------------------
    */

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of constants:
    **        f                         flattening of the ellipsoid
    **        ee                        first eccentricity squared (e squared in some notations)
    **        e                         first eccentricity
    **        eea                       second eccentricity squared (e' squared in some notations)
    **
    **        phi_amersfoort_sphere     latitude of projection base point Amersfoort on sphere in degrees
    **
    **        r1                        first (North South) principal radius of curvature in Amersfoort (M in some notations)
    **        r2                        second (East West) principal radius of curvature in Amersfoort (N in some notations)
    **        r_sphere                  radius of sphere
    **
    **        n                         constant of Gaussian projection n = 1.000475...
    **        q_amersfoort              isometric latitude of Amersfoort on ellipsiod
    **        w_amersfoort              isometric latitude of Amersfoort on sphere
    **        m                         constant of Gaussian projection m = 0.003773... (also named c in some notations)
    **--------------------------------------------------------------
    */
    const double f = 1/INV_F_BESSEL;
    const double ee = f*(2-f);
    const double e = sqrt(ee);
    const double eea = ee/(1.0-ee);

    const double phi_amersfoort_sphere = deg_atan(deg_tan(PHI_AMERSFOORT_BESSEL)/sqrt(1+eea*pow(deg_cos(PHI_AMERSFOORT_BESSEL),2)));

    const double r1 = A_BESSEL*(1-ee)/pow(sqrt(1-ee*pow(deg_sin(PHI_AMERSFOORT_BESSEL),2)),3);
    const double r2 = A_BESSEL/sqrt(1.0-ee*pow(deg_sin(PHI_AMERSFOORT_BESSEL),2));
    const double r_sphere = sqrt(r1*r2);

    const double n = sqrt(1+eea*pow(deg_cos(PHI_AMERSFOORT_BESSEL),4));
    const double q_amersfoort = atanh(deg_sin(PHI_AMERSFOORT_BESSEL))-e*atanh(e*deg_sin(PHI_AMERSFOORT_BESSEL));
    const double w_amersfoort = log(deg_tan(45+0.5*phi_amersfoort_sphere));
    const double m = w_amersfoort-n*q_amersfoort;

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **        r                    distance from Amersfoort in projection plane
    **        alpha                azimuth from Amersfoort
    **        psi                  distance angle from Amersfoort on sphere in degrees
    **        phi_sphere           latitide on sphere in degrees
    **        delta_lambda_sphere  difference in longitude on sphere with Amersfoort in degrees
    **        w                    isometric latitude on sphere
    **        q                    isometric latitude on ellipsiod
    **--------------------------------------------------------------
    */
    double r                   = sqrt(pow(x_rd-X_AMERSFOORT_RD,2)+pow(y_rd-Y_AMERSFOORT_RD,2));
    double sin_alpha           = (x_rd-X_AMERSFOORT_RD)/r;
    if (r<PRECISION) sin_alpha = 0;
    double cos_alpha           = (y_rd-Y_AMERSFOORT_RD)/r;
    if (r<PRECISION) cos_alpha = 1;
    double psi                 = 2*deg_atan(r/(2*SCALE_RD*r_sphere));
    double phi_sphere          = deg_asin(cos_alpha*deg_cos(phi_amersfoort_sphere)*deg_sin(psi)+deg_sin(phi_amersfoort_sphere)*deg_cos(psi));
    double delta_lambda_sphere = deg_asin((sin_alpha*deg_sin(psi))/deg_cos(phi_sphere));

    lambda = delta_lambda_sphere/n+LAMBDA_AMERSFOORT_BESSEL;

    double w = atanh(deg_sin(phi_sphere));
    double q = (w-m)/n;

    /*
    **--------------------------------------------------------------
    **    Iterative calculation of phi
    **--------------------------------------------------------------
    */
    phi=0;
    double previous;
    double diff=90;
    while (diff>DEG_PRECISION)
    {
        previous = phi;
        phi      = 2*deg_atan(exp(q+0.5*e*log((1+e*deg_sin(phi))/(1-e*deg_sin(phi)))))-90;
        diff     = fabs(phi-previous);
    }
}

/*
**--------------------------------------------------------------
**    Function name: read_grd_file_header
**    Description:   reads the header of a grd file
**
**    Parameter      Type        In/Out Req/Opt Default
**    filename       string      in     req     none
**    size_x         short int   out    -       none
**    size_y         short int   out    -       none
**    min_x          double      out    -       none
**    max_x          double      out    -       none
**    min_y          double      out    -       none
**    max_y          double      out    -       none
**    min_value      double      out    -       none
**    max_value      double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    filename   name of the to be read binary file
**    size_x     number of grid values in x direction (row)
**    size_y     number of grid values in y direction (col)
**    min_x      minimum of x
**    max_x      maximum of x
**    min_y      minimum of y
**    max_y      maximum of x
**    min_value  minimum value in grid (besides the error values)
**    max_value  maximum value in grid (besides the error values)
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int read_grd_file_header(string filename,
                         short int& size_x, short int& size_y,
                         double& min_x, double& max_x,
                         double& min_y, double& max_y,
                         double& min_value, double& max_value)
{
    /*
    **--------------------------------------------------------------
    **    Grd files are binary grid files in the format of the program Surfer(R)
    **--------------------------------------------------------------
    */

    fstream file(filename.c_str(), ios::in | ios::binary);

    /*
    **--------------------------------------------------------------
    **    Read file id
    **--------------------------------------------------------------
    */
    char id[5];
    for (int i=0; i<4; i=i+1)
    {
        file.seekg(i, ios::beg);
        file.read((char *)(&id[i]), 1);
    }
    id[4] = '\0';
    string id_string = string(id);

    /*
    **--------------------------------------------------------------
    **    Checks
    **--------------------------------------------------------------
    */
    if (!file)
    {
        cerr<<filename<<" does not exist"<<endl;
        return -1;
    }

    if (id_string!="DSBB")
    {
        cerr<<filename<<" is not a valid grd file"<<endl;
        return -1;
    }

    if (sizeof(short int)!=2)
    {
        cerr<<"Error: The number of bytes of a short integer in your compiler is "<<sizeof(short int)<<" and not 2 as in the file "<<filename<<endl;
        return -1;
    }

    if (sizeof(double)!=8)
    {
        cerr<<"Error: The number of bytes of a double in your compiler is "<<sizeof(double)<<" and not 8 as in the file "<<filename<<endl;
        return -1;
    }

    /*
    **--------------------------------------------------------------
    **    Read output parameters
    **--------------------------------------------------------------
    */
    file.seekg(4, ios::beg);
    file.read((char *)(&size_x), 2);

    file.seekg(6, ios::beg);
    file.read((char *)(&size_y), 2);


    file.seekg(8, ios::beg);
    file.read((char *)(&min_x), 8);

    file.seekg(16, ios::beg);
    file.read((char *)(&max_x), 8);

    file.seekg(24, ios::beg);
    file.read((char *)(&min_y), 8);

    file.seekg(32, ios::beg);
    file.read((char *)(&max_y), 8);

    file.seekg(40, ios::beg);
    file.read((char *)(&min_value), 8);

    file.seekg(48, ios::beg);
    file.read((char *)(&max_value), 8);

    return 0;
}

/*
**--------------------------------------------------------------
**    Function name: read_grd_file_body
**    Description:   reads a value from a grd file
**
**    Parameter      Type        In/Out Req/Opt Default
**    filename       string      in     req     none
**    number         long int    in     req     none
**    value          float       out    -       none
**
**    Additional explanation of the meaning of parameters
**    filename       name of the grd file to be read
**    record_number  number defining the position in the file
**    record_value   output of the read value
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int read_grd_file_body(string filename, long int record_number, float& record_value)
{
    const int record_length  =  4;
    const int header_length  = 56;

    /*
    **--------------------------------------------------------------
    **    Read
    **    Grd files are binary grid files in the format of the program Surfer(R)
    **    The first "header_length" bytes are the header of the file
    **    The body of the file consists of records of "record_length" bytes
    **    The records have a "record_number", starting with 0,1,2,...
    **--------------------------------------------------------------
    */
    fstream file(filename.c_str(), ios::in | ios::binary);
    file.seekg(header_length+record_number*record_length, ios::beg);
    file.read((char *)(&record_value), record_length);

    /*
    **--------------------------------------------------------------
    **    Checks
    **--------------------------------------------------------------
    */
    if (!file)
    {
        cerr<<filename<<" does not exist"<<endl;
        return -1;
    }

    if (sizeof(record_value)!=record_length)
    {
        cerr<<"Error: The number of bytes of a float in your compiler is "<<sizeof(record_value)<<" and not "<<record_length<<" as in the file "<<filename<<endl;
        return -1;
    }
    return 0;
}

/*
**--------------------------------------------------------------
**    Function name: grid_interpolation
**    Description:   grid interpolation using Overhauser splines
**
**    Parameter      Type        In/Out Req/Opt Default
**    x              double      in     req     none
**    y              double      in     req     none
**    grd_file       string      in     req     none
**    value          double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x, y           coordinates of the point for which a interpolated value is desired
**    grd_file       name of the grd file to be read
**    record_value   output of the interpolated value
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int grid_interpolation(double x, double y, string grd_file, double& value)
{
    int error;
    short int i, size_x, size_y;
    double min_x, max_x, min_y, max_y, min_value, max_value;
    long int record_number[16];
    float record_value[16];
    double ddx, ddy;
    double f[4], g[4];
    double gfac[16];
    double step_size_x, step_size_y;

    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **    size_x     number of grid values in x direction (row)
    **    size_y     number of grid values in y direction (col)
    **    min_x      minimum of x
    **    max_x      maximum of x
    **    min_y      minimum of y
    **    max_y      maximum of x
    **    min_value  minimum value in grid (besides the error values)
    **    max_value  maximum value in grid (besides the error values)
    **--------------------------------------------------------------
    */
    error = read_grd_file_header(grd_file, size_x, size_y, min_x, max_x, min_y, max_y, min_value, max_value);
    if (error!=0) return -1;

    step_size_x = (max_x-min_x)/(size_x-1);
    step_size_y = (max_y-min_y)/(size_y-1);

    /*
    **--------------------------------------------------------------
    **    Check for location safely inside the bounding box of grid
    **--------------------------------------------------------------
    */
    if (x<=(min_x+step_size_x) || x>=(max_x-step_size_x) ||
        y<=(min_y+step_size_y) || y>=(max_y-step_size_y))
    {
        cerr<<"Outside bounding box of "<<grd_file<<endl;
        if (grd_file=="x2c.grd") {error=1; value=0.0;}
        if (grd_file=="y2c.grd") {error=2; value=0.0;}
        if (grd_file=="nlgeo04.grd") error=3;
        return error;
    }

    /*
    **--------------------------------------------------------------
    **    The selected grid points are situated around point X like this:
    **
    **        12  13  14  15
    **
    **         8   9  10  11
    **               X
    **         4   5   6   7
    **
    **         0   1   2   3
    **
    **    ddx and ddy (in parts of the grid interval) are defined relative to grid point 9, respectively to the right and down.
    **--------------------------------------------------------------
    */
    ddx =    (x-min_x)/step_size_x-floor((x-min_x)/step_size_x);
    ddy = 1-((y-min_y)/step_size_y-floor((y-min_y)/step_size_y));

    /*
    **--------------------------------------------------------------
    **    Calculate the record numbers of the selected grid points
    **    The records are numbered from lower left corner to the uper right corner starting with 0:
    **
    **    size_x*(size_y-1) . . size_x*size_y-1
    **                   .                    .
    **                   .                    .
    **                   0 . . . . . . size_x-1
    **--------------------------------------------------------------
    */
    record_number[5]  = int((x-min_x)/step_size_x + floor((y-min_y)/step_size_y)*size_x);
    record_number[0]  = record_number[5]-size_x-1;
    record_number[1]  = record_number[5]-size_x   ;
    record_number[2]  = record_number[5]-size_x+1;
    record_number[3]  = record_number[5]-size_x+2;
    record_number[4]  = record_number[5]-1;
    record_number[6]  = record_number[5]+1;
    record_number[7]  = record_number[5]+2;
    record_number[8]  = record_number[5]+size_x-1;
    record_number[9]  = record_number[5]+size_x;
    record_number[10] = record_number[5]+size_x+1;
    record_number[11] = record_number[5]+size_x+2;
    record_number[12] = record_number[5]+2*size_x-1;
    record_number[13] = record_number[5]+2*size_x;
    record_number[14] = record_number[5]+2*size_x+1;
    record_number[15] = record_number[5]+2*size_x+2;

    /*
    **--------------------------------------------------------------
    **    Read the record values of the selected grid point
    **    Outside the validity area the records have a very large value (circa 1.7e38).
    **--------------------------------------------------------------
    */
    for (int i=0; i<16; i=i+1)
    {
        error = read_grd_file_body(grd_file, record_number[i], record_value[i]);
        if (error!=0) return -1;
        if (record_value[i]>max_value+PRECISION || record_value[i]<min_value-PRECISION)
        {
            cerr<<"Outside validity area of "<<grd_file<<endl;
            if (grd_file=="x2c.grd") {error=1; value=0.0;}
            if (grd_file=="y2c.grd") {error=2; value=0.0;}
            if (grd_file=="nlgeo04.grd") error=3;
            return error;
        }
    }

    /*
    **--------------------------------------------------------------
    **    Calculation of the multiplication factors
    **--------------------------------------------------------------
    */
    f[0] = -0.5*ddx+ddx*ddx-0.5*ddx*ddx*ddx;
    f[1] = 1.0-2.5*ddx*ddx+1.5*ddx*ddx*ddx;
    f[2] = 0.5*ddx+2.0*ddx*ddx-1.5*ddx*ddx*ddx;
    f[3] = -0.5*ddx*ddx+0.5*ddx*ddx*ddx;
    g[0] = -0.5*ddy+ddy*ddy-0.5*ddy*ddy*ddy;
    g[1] = 1.0-2.5*ddy*ddy+1.5*ddy*ddy*ddy;
    g[2] = 0.5*ddy+2.0*ddy*ddy-1.5*ddy*ddy*ddy;
    g[3] = -0.5*ddy*ddy+0.5*ddy*ddy*ddy;

    gfac[12] = f[0]*g[0];
    gfac[8]  = f[0]*g[1];
    gfac[4]  = f[0]*g[2];
    gfac[0]  = f[0]*g[3];
    gfac[13] = f[1]*g[0];
    gfac[9]  = f[1]*g[1];
    gfac[5]  = f[1]*g[2];
    gfac[1]  = f[1]*g[3];
    gfac[14] = f[2]*g[0];
    gfac[10] = f[2]*g[1];
    gfac[6]  = f[2]*g[2];
    gfac[2]  = f[2]*g[3];
    gfac[15] = f[3]*g[0];
    gfac[11] = f[3]*g[1];
    gfac[7]  = f[3]*g[2];
    gfac[3]  = f[3]*g[3];

    /*
    **--------------------------------------------------------------
    **    Calculation of the interpolated value
    **    Applying the multiplication factors on the selected grid values
    **--------------------------------------------------------------
    */
    value=0.0;
    for (i=0; i<16; i=i+1)
    {
        value=value+gfac[i]*record_value[i];
    }

    return 0;
}

/*
**--------------------------------------------------------------
**    Function name: rd_correction
**    Description:   apply the modelled distortions in the RD coordinate system
**
**    Parameter      Type        In/Out Req/Opt Default
**    x_pseudo_rd    double      in     req     none
**    y_pseudo_rd    double      in     req     none
**    x_rd           double      out    -       none
**    y_rd           double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x_pseudo_rd, y_pseudo_rd  input coordinates in undistorted pseudo RD
**    x_rd, y_rd                output coordinates in real RD
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int rd_correction(double x_pseudo_rd, double y_pseudo_rd,
                  double& x_rd, double& y_rd)
{
    int error;
    double dx, dy;

    error = grid_interpolation(x_pseudo_rd, y_pseudo_rd, GRID_FILE_DX, dx);
    error = grid_interpolation(x_pseudo_rd, y_pseudo_rd, GRID_FILE_DY, dy);
    x_rd=x_pseudo_rd-dx;
    y_rd=y_pseudo_rd-dy;
    return error;
}

/*
**--------------------------------------------------------------
**    Function name: inv_rd_correction
**    Description:   remove the modelled distortions in the RD coordinate system
**
**    Parameter      Type        In/Out Req/Opt Default
**    x_rd           double      in     req     none
**    y_rd           double      in     req     none
**    x_pseudo_rd    double      out    -       none
**    x_pseudo_rd    double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x_rd, y_rd                input coordinates in real RD
**    x_pseudo_rd, y_pseudo_rd  output coordinates in undistorted pseudo RD
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int inv_rd_correction(double x_rd, double y_rd,
                      double& x_pseudo_rd, double& y_pseudo_rd)
{
    int error;
    double dx, dy;

    /*
    **--------------------------------------------------------------
    **    The grid values are formally in pseudo RD. For the interpolation below the RD values are used. The intoduced error is certainly smaller than 0.0001 m for the X2c.grd and Y2c.grd.
    **--------------------------------------------------------------
    */
    x_pseudo_rd=x_rd;
    y_pseudo_rd=y_rd;
    error = grid_interpolation(x_pseudo_rd, y_pseudo_rd, GRID_FILE_DX, dx);
    error = grid_interpolation(x_pseudo_rd, y_pseudo_rd, GRID_FILE_DY, dy);
    x_pseudo_rd=x_rd+dx;
    y_pseudo_rd=y_rd+dy;
    return error;
}

/*
**--------------------------------------------------------------
**    Function name: etrs2rd
**    Description:   convert ETRS89 coordinates to RD coordinates
**
**    Parameter      Type        In/Out Req/Opt Default
**    phi_etrs       double      in     req     none
**    lambda_etrs    double      in     req     none
**    h_etrs         double      in     req     none
**    x_rd           double      out    -       none
**    y_rd           double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    phi_etrs, lambda_etrs, h_etrs  input ETRS89 coordinates
**    x_rd, y_rd                     output RD coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int etrs2rd(double phi_etrs, double lambda_etrs, double h_etrs,
            double& x_rd, double& y_rd, double& h_bessel)
{
    int error;
    double x_etrs, y_etrs, z_etrs;
    double x_bessel, y_bessel, z_bessel;
    double phi_bessel, lambda_bessel;
    double x_pseudo_rd, y_pseudo_rd;

    double x_amersfoort_bessel;
    double y_amersfoort_bessel;
    double z_amersfoort_bessel;
    double x_amersfoort_etrs;
    double y_amersfoort_etrs;
    double z_amersfoort_etrs;

    /*
    **--------------------------------------------------------------
    **    Calculate the cartesian ETRS89 coordinates of the pivot point Amersfoort
    **--------------------------------------------------------------
    */
    geographic2cartesian(PHI_AMERSFOORT_BESSEL, LAMBDA_AMERSFOORT_BESSEL, H_AMERSFOORT_BESSEL,
                         A_BESSEL, INV_F_BESSEL,
                         x_amersfoort_bessel, y_amersfoort_bessel, z_amersfoort_bessel);
    x_amersfoort_etrs = x_amersfoort_bessel+TX_BESSEL_ETRS;
    y_amersfoort_etrs = y_amersfoort_bessel+TY_BESSEL_ETRS;
    z_amersfoort_etrs = z_amersfoort_bessel+TZ_BESSEL_ETRS;

    /*
    **--------------------------------------------------------------
    **    Convert ETRS89 coordinates to RD coordinates
    **    (To convert from degrees, minutes and seconds use the function deg_min_sec2decimal() here)
    **--------------------------------------------------------------
    */
    geographic2cartesian(phi_etrs, lambda_etrs, h_etrs,
                         A_ETRS, INV_F_ETRS,
                         x_etrs, y_etrs, z_etrs);
    sim_trans(x_etrs, y_etrs, z_etrs,
              TX_ETRS_BESSEL, TY_ETRS_BESSEL, TZ_ETRS_BESSEL,
              ALPHA_ETRS_BESSEL, BETA_ETRS_BESSEL, GAMMA_ETRS_BESSEL,
              DELTA_ETRS_BESSEL,
              x_amersfoort_etrs, y_amersfoort_etrs, z_amersfoort_etrs,
              x_bessel, y_bessel, z_bessel);
    cartesian2geographic(x_bessel, y_bessel, z_bessel,
                         A_BESSEL, INV_F_BESSEL,
                         phi_bessel, lambda_bessel, h_bessel);
    rd_projection(phi_bessel, lambda_bessel, x_pseudo_rd, y_pseudo_rd);
    error = rd_correction(x_pseudo_rd, y_pseudo_rd, x_rd, y_rd);
    return error;
}

/*
**--------------------------------------------------------------
**    Function name: rd2etrs
**    Description:   convert RD coordinates to ETRS89 coordinates
**
**    Parameter      Type        In/Out Req/Opt Default
**    x_rd           double      in     req     none
**    y_rd           double      in     req     none
**    nap            double      in     req     none
**    phi_etrs       double      out    -       none
**    lambda_etrs    double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x_rd, y_rd, nap        input RD and NAP coordinates
**    phi_etrs, lambda_etrs  output ETRS89 coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int rd2etrs(double x_rd, double y_rd, double nap,
            double& phi_etrs, double& lambda_etrs, double& h_etrs)
{
    int error;
    double x_pseudo_rd, y_pseudo_rd;
    double phi_bessel, lambda_bessel, h_bessel;
    double x_bessel, y_bessel, z_bessel;
    double x_etrs, y_etrs, z_etrs;
    double x_amersfoort_bessel;
    double y_amersfoort_bessel;
    double z_amersfoort_bessel;

    /*
    **--------------------------------------------------------------
    **    Calculate the cartesian Bessel coordinates of the pivot point Amersfoort
    **--------------------------------------------------------------
    */
    geographic2cartesian(PHI_AMERSFOORT_BESSEL, LAMBDA_AMERSFOORT_BESSEL, H_AMERSFOORT_BESSEL,
                         A_BESSEL, INV_F_BESSEL,
                         x_amersfoort_bessel, y_amersfoort_bessel, z_amersfoort_bessel);

    /*
    **--------------------------------------------------------------
    **    Calculate appoximated value of ellipsoidal Bessel height
    **    The error made by using a constant for de Bessel geoid height is max. circa 1 meter in the ellipsoidal height (for the NLGEO2004 geoid model). This intoduces an error in the phi, lambda position too, this error is nevertheless certainly smaller than 0.0001 m.
    **--------------------------------------------------------------
    */
    h_bessel = nap + MEAN_GEOID_HEIGHT_BESSEL;

    /*
    **--------------------------------------------------------------
    **    Convert RD coordinates to ETRS89 coordinates
    **--------------------------------------------------------------
    */
    error = inv_rd_correction(x_rd, y_rd,
                              x_pseudo_rd, y_pseudo_rd);
    inv_rd_projection(x_pseudo_rd, y_pseudo_rd,
                      phi_bessel, lambda_bessel);
    geographic2cartesian(phi_bessel, lambda_bessel, h_bessel,
                         A_BESSEL, INV_F_BESSEL,
                         x_bessel, y_bessel, z_bessel);
    sim_trans(x_bessel, y_bessel, z_bessel,
              TX_BESSEL_ETRS, TY_BESSEL_ETRS, TZ_BESSEL_ETRS,
              ALPHA_BESSEL_ETRS, BETA_BESSEL_ETRS, GAMMA_BESSEL_ETRS,
              DELTA_BESSEL_ETRS,
              x_amersfoort_bessel, y_amersfoort_bessel, z_amersfoort_bessel,
              x_etrs, y_etrs, z_etrs);
    cartesian2geographic(x_etrs, y_etrs, z_etrs,
                         A_ETRS, INV_F_ETRS,
                         phi_etrs, lambda_etrs, h_etrs);
    /*
    **--------------------------------------------------------------
    **    To convert to degrees, minutes and seconds use the function decimal2deg_min_sec() here
    **--------------------------------------------------------------
    */
    return error;
}

/*
**--------------------------------------------------------------
**    Function name: etrs2nap
**    Description:   convert ellipsoidal ETRS89 height to NAP height
**
**    Parameter      Type        In/Out Req/Opt Default
**    phi            double      in     req     none
**    lambda         double      in     req     none
**    h              double      in     req     none
**    nap            double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    phi, lambda, h  input ETRS89 coordinates
**    nap             output NAP height
**
**    Return value: (besides the standard return values) none
**    on error (outside geoid grid) nap is not compted here
**    instead in etrs2rdnap nap=h_bessel
**--------------------------------------------------------------
*/
int etrs2nap(double phi, double lambda, double h,
             double& nap)
{
    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **        n  geoid height
    **    on error (outside geoid grid) nap is not compted
    **    instead in etrs2rdnap nap=h_bessel
    **--------------------------------------------------------------
    */
    int error;
    double n;
    error = grid_interpolation(lambda, phi, GRID_FILE_GEOID, n);
    if (error!=0) return error;
    nap = h-n+0.0088;
    return 0;
}

/*
**--------------------------------------------------------------
**    Function name: nap2etrs
**    Description:   convert NAP height to ellipsoidal ETRS89 height
**
**    Parameter      Type        In/Out Req/Opt Default
**    phi            double      in     req     none
**    lambda         double      in     req     none
**    nap            double      in     req     none
**    h              double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    phi, lambda  input ETRS89 position
**    nap          input NAP height at position phi, lambda
**    h            output ellipsoidal ETRS89 height
**
**    Return value: (besides the standard return values)
**    none
**    on error (outside geoid grid) h is not compted here
**    instead in rdnap2etrs h=h_etrs_sim (from similarity transformation)
**--------------------------------------------------------------
*/
int nap2etrs(double phi, double lambda, double nap,
             double& h)
{
    /*
    **--------------------------------------------------------------
    **    Explanation of the meaning of variables:
    **        n  geoid height
    **--------------------------------------------------------------
    */
    int error;
    double n;
    error = grid_interpolation(lambda, phi, GRID_FILE_GEOID, n);
    if (error!=0) return error;
    h = nap+n-0.0088;
    return 0;
}

/*
**--------------------------------------------------------------
**    Function name: etrs2rdnap
**    Description:   convert ETRS89 coordinates to RD and NAP coordinates
**
**    Parameter      Type        In/Out Req/Opt Default
**    phi            double      in     req     none
**    lambda         double      in     req     none
**    h              double      in     req     none
**    x_rd           double      out    -       none
**    y_rd           double      out    -       none
**    nap            double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    phi, lambda, h   input ETRS89 coordinates
**    x_rd, y_rd, nap  output RD and NAP coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int etrs2rdnap(double phi, double lambda, double h,
               double& x_rd, double& y_rd, double& nap)
{
    int error;
    double h_bessel, h_geoid;
    error = etrs2rd( phi, lambda, h, x_rd, y_rd, h_bessel);
    error = etrs2nap(phi, lambda, h, h_geoid);
    if (error==3) nap=h_bessel;
    else nap=h_geoid;
    return error;
}

/*
**--------------------------------------------------------------
**    Function name: rdnap2etrs
**    Description:   convert RD and NAP coordinates to ETRS89 coordinates
**
**    Parameter      Type        In/Out Req/Opt Default
**    x_rd           double      in     req     none
**    y_rd           double      in     req     none
**    nap            double      in     req     none
**    phi            double      out    -       none
**    lambda         double      out    -       none
**    h              double      out    -       none
**
**    Additional explanation of the meaning of parameters
**    x_rd, y_rd, nap  input RD and NAP coordinates
**    phi, lambda, h   output ETRS89 coordinates
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int rdnap2etrs(double x_rd, double y_rd, double nap,
                double& phi, double& lambda, double& h)
{
    int error;
    double h_etrs_sim, h_etrs_geoid;
    error = rd2etrs( x_rd, y_rd,   nap, phi, lambda, h_etrs_sim);
    error = nap2etrs(phi,  lambda, nap, h_etrs_geoid);
    if (error==3) h=h_etrs_sim;
    else h=h_etrs_geoid;
    return error;
}

/*
**--------------------------------------------------------------
**    Function name: main
**    Description:   very simple interface
**
**    Parameter      Type        In/Out Req/Opt Default
**    none
**
**    Additional explanation of the meaning of parameters
**    none
**
**    Return value: (besides the standard return values)
**    none
**--------------------------------------------------------------
*/
int main(int argc, char* argv[])
{
    double x_rd, y_rd, nap, phi, lambda, h;
	x_rd = std::stod(argv[1]);
    y_rd = std::stod(argv[2]);
	nap = std::stod(argv[3]);
	
    /*
    **--------------------------------------------------------------
    **    Calculation RD and NAP to ETRS89
    **--------------------------------------------------------------
    */
    rdnap2etrs(x_rd, y_rd, nap, phi, lambda, h);
	{
        cout<<fixed<<setprecision(9);
        cout<<phi<<endl;
        cout<<lambda<<endl;
    }
}

/*
**--------------------------------------------------------------
**    End of RDNAPTRANS(TM)2008
**--------------------------------------------------------------
*/
int original_main()
{
    int choice=9;
    int error;
    double x_rd, y_rd, nap, phi, lambda, h;

    cout<<endl;
    cout<<"RDNAPTRANS(TM)2008"<<endl;

    while (choice!=0)
    {
        error=0;
        cout<<endl;
        cout<<"Make your choice:"<<endl;
        cout<<"[1] ETRS89 to RD and NAP"<<endl;
        cout<<"[2] RD and NAP to ETRS89"<<endl;
        cout<<"[0] Close program"<<endl<<" ";
        cin>>choice;
        if (choice==1)
        {
            cout<<endl;
            cout<<"Enter latitude (degrees): ";
            cin>>phi;
            cout<<"Enter longitude (degrees): ";
            cin>>lambda;
            cout<<"Enter ellipsoidal height (meters, enter 43 if unknown): ";
            cin>>h;
            cout<<endl;

            /*
            **--------------------------------------------------------------
            **    Calculation ETRS89 to RD and NAP
            **--------------------------------------------------------------
            */
            error = etrs2rdnap(phi, lambda, h, x_rd, y_rd, nap);
            {
                cout<<fixed<<setprecision(4);
                cout<<"RD x = "<<x_rd<<endl;
                cout<<"   y = "<<y_rd<<endl;
                cout<<"NAP  = "<<nap<<endl;
            }
        }
        else if (choice==2)
        {
            cout<<endl;
            cout<<"Enter RD x (meters): ";
            cin>>x_rd;
            cout<<"Enter RD y (meters): ";
            cin>>y_rd;
            cout<<"Enter NAP  (meters): ";
            cin>>nap;
            cout<<endl;

            /*
            **--------------------------------------------------------------
            **    Calculation RD and NAP to ETRS89
            **--------------------------------------------------------------
            */
            error = rdnap2etrs(x_rd, y_rd, nap, phi, lambda, h);
            {
                cout<<fixed<<setprecision(9);
                cout<<"phi                 "<<phi<<endl;
                cout<<"lambda             = "<<lambda<<endl;
                cout<<fixed<<setprecision(4);
                cout<<"ellipsoidal height = "<<h<<endl;
            }
        }
        else if (choice==0)
        {
            cout<<endl;
            cout<<"Regular end of program"<<endl;
            cout<<endl;
        }
    }
	return 1;
}

/*
**--------------------------------------------------------------
**    End of RDNAPTRANS(TM)2008
**--------------------------------------------------------------
*/
