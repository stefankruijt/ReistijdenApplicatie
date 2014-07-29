Readme.txt for RDNAPTRANS2008

Release of RDNAPTRANS2008

After 24 March 2009 the RD database will contain new values for ETRS89, based on a re-adjustment of the AGRS.NL stations. 
From that moment on the use of RDNAPTRANS2008 for these new ETRS89 coordinates is more precise than the use of the existing RDNAPTRANS2004. 
However the differences do not exceed 1 cm in horizontal and vertical position and there is a transition period till 1-4-2010.

RDNAPTRANS2008 is presented to you in the form of the zip-file RDNAPTRANS2008.zip containing the files:

- rdnaptrans2008.cpp  source code in C++, this code is just an example to show the formulas used.

- rdnaptrans2008.xxx  Windows executable with extension .xxx to let it pass firewalls etc. The program transforms only one point per run. 

- x2c.grd    binary file with the x-RD correction grid (identical in RDNAPTRANS2004)

- y2c.grd    binary file with the y-RD correction grid (identical in RDNAPTRANS2004)

- nlgeo04.grd   binary file with the NLGEO2004 geoid grid (identical in RDNAPTRANS2004)

- Use of RDNAPTRANS2008.doc  Procedure for using the name RDNAPTRANS2008 and test coordinates 

- Transpar_2008.txt  ASCII file with the RDNAPTRANS2008 transformation parameters

- Readme.txt    this explanation

If coordinates are computed for points outside the region where interpolation in between grid points of x2c.grd, y2c.grd or nlgeo04.grd is possible, the output should be accompanied by a warning. 
Outside these grids RD or NAP are not well defined. On the border of the grids the resulting coordinates show discontinuities up to 25 cm for position or height. 

The minimum changes to update a program from RDNAPTRANS2004 to RDNAPTRANS2008 are the 7 transformation parameters and the height offset of 8.8 mm as given in Transpar_2008.txt.


For information
Joop van Buren
rd@kadaster.nl
