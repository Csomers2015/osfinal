The zip file contains a few different files

read_single.cpp
- compile with "make single" and run with "./single"
- this will parse the full 249,000+ entries in the crimes.csv file
- the program terminates after generating a report stored in "report.txt" and generating a 
  heatmap for the portion of crimes reported within the beounds described by topL and botR

read_two.cpp and read_four.cpp
- compile with "make two" and "make four" and run with "./two" and "./four"
- these will parse the subset "subsub.csv" which contains a subset of 100 crimes
- these programs use two and four threads respectively

crimes.csv subset.csv subsub.csv
- crimes.csv is the full list of over 249,000 crimes
- subset.csv is a subset of 1000 crimes
- subsub.csv is a subset of 100 crimes

Bmore.bmp
- an image used as a base for the heatmap

CImg.h
- a library used to open and interact with image files

report.txt
- contains various stats derived from the data read

heatmap.bmp
- a heatmap generated by the executable "single"
- uses Bmore.bmp as the base and overlays a heatmap showing crime density with areas of 0 crime 
  density showing just the Bmore.bmp image pixels
- the scale goes from yellow being the highest areas of crime to green being the lowest areas
  of crime

