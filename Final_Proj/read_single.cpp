#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#define cimg_display 0
#include "CImg.h"
using namespace std;

class color {
public:
    unsigned char r, g, b;
};
class results {
public:
    int jan14, feb14, mar14, apr14, may14, jun14, jul14, aug14, sep14, oct14, nov14, dec14,
        jan15, feb15, mar15, apr15, may15, jun15, jul15, aug15, sep15, oct15, nov15, dec15,
        jan16, feb16, mar16, apr16, may16, jun16, jul16, aug16, sep16, oct16, nov16, dec16,
        jan17, feb17, mar17, apr17, may17, jun17, jul17, aug17, sep17, oct17, nov17, dec17,
        jan18, feb18, mar18, apr18, may18, jun18, jul18, aug18, sep18, oct18, nov18, dec18,
        jan19, feb19, mar19, apr19, may19, jun19, jul19, aug19, sep19, oct19, nov19; //counts the instances of a month per year
    int spring14, spring15, spring16, spring17, spring18, spring19;
    int summer14, summer15, summer16, summer17, summer18, summer19;
    int fall14, fall15, fall16, fall17, fall18, fall19;
    int winter14, winter15, winter16, winter17, winter18, winter19;
    bool jan, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec; //used to store the month before moving on to check year per line
    bool finished;
    int tm0, tm1, tm2, tm3, tm4, tm5, tm6, tm7, tm8, tm9, tm10, tm11, tm12, tm13, tm14, tm15, tm16, tm17, tm18, tm19, tm20, tm21, tm22, tm23; //number of crimes for hour tmx
    int homicide, rape, robbery, assault, burglary, larceny, auto_theft, arson, shooting; //number of instances of each crime type
    int inside, outside; //crimes committed indoors or outdoors
    int NE, C, NW, W, SW, N, E, S, SE;
    results();
    void combine();
    void write_report();
    void draw_map();
    pair<double,double> topL, botR;
    int heat[1086][858];
    int inArea[1086][858];
};
class thread_state {
public:
    bool ready; //true if thread is waiting to read the next line
    bool activated; //when activated, the thred has exclusive access to the crimes stream
    results res;
};
//global vars
results report;
ifstream crimes;
thread_state t1;
int processed = 0;
//population data from opendatanetwork.com/entity/0500000US24510/Baltimore_city_MD/demographics.population.change?year=2017
//pop18 is projected value
int pop14 = 622271;
int pop15 = 622454;
int pop16 = 621000;
int pop17 = 619796;
int pop18 = 617353;
//variables used to get array coords for heat map
int WIDTH = 1086; //WIDTH
int HEIGHT = 858; //HEIGHT
double longrange = -76.436355 - -76.734748;
double latrange = 39.384347 - 39.201415;
double longstep = longrange / WIDTH;
double latstep = latrange / HEIGHT;
int maxCrimes = 0;
int heatRecorded = 0;

//default result variable has false data bools
results::results() {
    jan14 = jan15 = jan16 = jan17 = jan18 = jan19 = 0;
    feb14 = feb15 = feb16 = feb17 = feb18 = feb19 = 0;
    mar14 = mar15 = mar16 = mar17 = mar18 = mar19 = 0;
    apr14 = apr15 = apr16 = apr17 = apr18 = apr19 = 0;
    may14 = may15 = may16 = may17 = may18 = may19 = 0;
    jun14 = jun15 = jun16 = jun17 = jun18 = jun19 = 0; 
    jul14 = jul15 = jul16 = jul17 = jul18 = jul19 = 0;
    aug14 = aug15 = aug16 = aug17 = aug18 = aug19 = 0; 
    sep14 = sep15 = sep16 = sep17 = sep18 = sep19 = 0;
    oct14 = oct15 = oct16 = oct17 = oct18 = oct19 = 0;
    nov14 = nov15 = nov16 = nov17 = nov18 = nov19 = 0;
    dec14 = dec15 = dec16 = dec17 = dec18 = 0;
    spring14 = spring15 = spring16 = spring17 = spring18 = spring19 = 0;
    summer14 = summer15 = summer16 = summer17 = summer18 = summer19 = 0;
    fall14 = fall15 = fall16 = fall17 = fall18 = fall19 = 0;
    winter14 = winter15 = winter16 = winter17 = winter18 = winter19 = 0;
    jan = feb = mar = apr = may = jun = jul = aug = sep = oct = nov = dec = false;
    tm0 = tm1 = tm2 = tm3 = tm4 = tm5 = tm6 = tm7 = tm8 = tm9 = tm10 = tm11 = tm12 = tm13 = tm14 = tm15 = tm16 = tm17 = tm18 = tm19 = tm20 = tm21 = tm22 = tm23 = 0;
    homicide = rape = robbery = assault = burglary = larceny = auto_theft = arson = shooting = 0;
    inside = outside = 0;
    NE = C = NW = W = SW = N = E = S = SE = 0;
    finished = false; 
    topL.first = -76.734748;
    topL.second = 39.384347;
    botR.first = -76.436355;
    botR.second = 39.201415;
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            heat[i][j] = 0;
        }
    }
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            inArea[i][j] = 0;
        }
    }
}
//combines the thread's results into one report
void results::combine() {
    //2014 months
    jan14 = t1.res.jan14;
    feb14 = t1.res.feb14;
    mar14 = t1.res.mar14;
    apr14 = t1.res.apr14;
    may14 = t1.res.may14;
    jun14 = t1.res.jun14;
    jul14 = t1.res.jul14;
    aug14 = t1.res.aug14;
    sep14 = t1.res.sep14;
    oct14 = t1.res.oct14;
    nov14 = t1.res.nov14;
    dec14 = t1.res.dec14;
    //2015 months
    jan15 = t1.res.jan15;
    feb15 = t1.res.feb15;
    mar15 = t1.res.mar15;
    apr15 = t1.res.apr15;
    may15 = t1.res.may15;
    jun15 = t1.res.jun15;
    jul15 = t1.res.jul15;
    aug15 = t1.res.aug15;
    sep15 = t1.res.sep15;
    oct15 = t1.res.oct15;
    nov15 = t1.res.nov15;
    dec15 = t1.res.dec15;
    //2016 months
    jan16 = t1.res.jan16;
    feb16 = t1.res.feb16;
    mar16 = t1.res.mar16;
    apr16 = t1.res.apr16;
    may16 = t1.res.may16;
    jun16 = t1.res.jun16;
    jul16 = t1.res.jul16;
    aug16 = t1.res.aug16;
    sep16 = t1.res.sep16;
    oct16 = t1.res.oct16;
    nov16 = t1.res.nov16;
    dec16 = t1.res.dec16;
    //2017 months
    jan17 = t1.res.jan17;
    feb17 = t1.res.feb17;
    mar17 = t1.res.mar17;
    apr17 = t1.res.apr17;
    may17 = t1.res.may17;
    jun17 = t1.res.jun17;
    jul17 = t1.res.jul17;
    aug17 = t1.res.aug17;
    sep17 = t1.res.sep17;
    oct17 = t1.res.oct17;
    nov17 = t1.res.nov17;
    dec17 = t1.res.dec17;
    //2018 months
    jan18 = t1.res.jan18;
    feb18 = t1.res.feb18;
    mar18 = t1.res.mar18;
    apr18 = t1.res.apr18;
    may18 = t1.res.may18;
    jun18 = t1.res.jun18;
    jul18 = t1.res.jul18;
    aug18 = t1.res.aug18;
    sep18 = t1.res.sep18;
    oct18 = t1.res.oct18;
    nov18 = t1.res.nov18;
    dec18 = t1.res.dec18;
    //2019
    jan19 = t1.res.jan19;
    feb19 = t1.res.feb19;
    mar19 = t1.res.mar19;
    apr19 = t1.res.apr19;
    may19 = t1.res.may19;
    jun19 = t1.res.jun19;
    jul19 = t1.res.jul19;
    aug19 = t1.res.aug19;
    sep19 = t1.res.sep19;
    oct19 = t1.res.oct19;
    nov19 = t1.res.nov19;
    //seasons
    //2014
    spring14 = t1.res.spring14;
    summer14 = t1.res.summer14;
    fall14 = t1.res.fall14;
    winter14 = t1.res.winter14;
    //2015
    spring15 = t1.res.spring15;
    summer15 = t1.res.summer15;
    fall15 = t1.res.fall15;
    winter15 = t1.res.winter15;
    //2016
    spring16 = t1.res.spring16;
    summer16 = t1.res.summer16;
    fall16 = t1.res.fall16;
    winter16 = t1.res.winter16;
    //2017
    spring17 = t1.res.spring17;
    summer17 = t1.res.summer17;
    fall17 = t1.res.fall17;
    winter17 = t1.res.winter17;
    //2018
    spring18 = t1.res.spring18;
    summer18 = t1.res.summer18;
    fall18 = t1.res.fall18;
    winter18 = t1.res.winter18;
    //2019 
    spring19 = t1.res.spring19;
    summer19 = t1.res.summer19;
    fall19 = t1.res.fall19;
    winter19 = t1.res.winter19;
    //crime times
    tm0 = t1.res.tm0;
    tm1 = t1.res.tm1;
    tm2 = t1.res.tm2;
    tm3 = t1.res.tm3;
    tm4 = t1.res.tm4;
    tm5 = t1.res.tm5;
    tm6 = t1.res.tm6;
    tm7 = t1.res.tm7;
    tm8 = t1.res.tm8;
    tm9 = t1.res.tm9;
    tm10 = t1.res.tm10;
    tm11 = t1.res.tm11;
    tm12 = t1.res.tm12;
    tm13 = t1.res.tm13;
    tm14 = t1.res.tm14;
    tm15 = t1.res.tm15;
    tm16 = t1.res.tm16;
    tm17 = t1.res.tm17;
    tm18 = t1.res.tm18;
    tm19 = t1.res.tm19;
    tm20 = t1.res.tm20;
    tm21 = t1.res.tm21;
    tm22 = t1.res.tm22;
    tm23 = t1.res.tm23;
    //crime code
    homicide = t1.res.homicide;
    rape = t1.res.rape;
    robbery = t1.res.robbery;
    assault = t1.res.assault;
    burglary = t1.res.burglary;
    larceny = t1.res.larceny;
    auto_theft = t1.res.auto_theft;
    arson = t1.res.arson;
    shooting = t1.res.shooting;
    //in/out
    inside = t1.res.inside;
    outside = t1.res.outside;
    //district
    N = t1.res.N;
    C = t1.res.C;
    NE = t1.res.NE;
    NW = t1.res.NW;
    S = t1.res.S;
    SE = t1.res.SE;
    SW = t1.res.SW;
    E = t1.res.E;
    W = t1.res.W;
    //heat
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            heat[i][j] = t1.res.heat[i][j];
        }
    }
    //maxCrimes is the max number of crimes within a 2 pixel radius of any given pixel
    int total = 0;
    //for each pixel
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            //for each pixel within a radius of 5 pixels
            for(int x = i - 7; x < i + 8; x++) {
                for(int y = j - 7; y < j + 8; y++) {
                    //if in the bounds of the image, add to total
                    if(x >= 0 && x <= WIDTH - 1 && y >= 0 && y <= HEIGHT - 1) {
                        total += heat[x][y];
                    }
                }
            }
            //store inArea
            inArea[i][j] = total;
            //update maxCrimes
            if(total > maxCrimes) {
                maxCrimes = total;
            }
            total = 0;
        }
    }
}
//writes a report to the output file "report.txt"
void results::write_report() {
    ofstream output;
    output.open("report.txt");
    //total crimes
    output << "total crimes: " << processed << endl;
    //2014 dates
    output << "2014 crimes: " << jan14 + feb14 + mar14 + apr14 + may14 + jun14 + jul14 + aug14 + sep14 + oct14 + nov14 + dec14 << 
    " " << (jan14 + feb14 + mar14 + apr14 + may14 + jun14 + jul14 + aug14 + sep14 + oct14 + nov14 + dec14) / (double)pop14 << endl;
    output << "spring: " << spring14 << endl;
    output << "summer: " << summer14 << endl;
    output << "fall: " << fall14 << endl;
    output << "winter: " << winter14 << endl;
    output << "jan14: " << jan14 << endl;
    output << "feb14: " << feb14 << endl;
    output << "mar14: " << mar14 << endl;
    output << "apr14: " << apr14 << endl;
    output << "may14: " << may14 << endl;
    output << "jun14: " << jun14 << endl;
    output << "jul14: " << jul14 << endl;
    output << "aug14: " << aug14 << endl;
    output << "sep14: " << sep14 << endl;
    output << "oct14: " << oct14 << endl;
    output << "nov14: " << nov14 << endl;
    output << "dec14: " << dec14 << endl;
    //2015 dates
    output << "2015 crimes: " << jan15 + feb15+ mar15 + apr15 + may15 + jun15 + jul15 + aug15 + sep15 + oct15 + nov15 + dec15 << 
    " " << (jan15 + feb15 + mar15 + apr15 + may15 + jun15 + jul15 + aug15 + sep15 + oct15 + nov15 + dec15) / (double)pop15 << endl;
    output << "spring: " << spring15 << endl;
    output << "summer: " << summer15 << endl;
    output << "fall: " << fall15 << endl;
    output << "winter: " << winter15 << endl;
    output << "jan15: " << jan15 << endl;
    output << "feb15: " << feb15 << endl;
    output << "mar15: " << mar15 << endl;
    output << "apr15: " << apr15 << endl;
    output << "may15: " << may15 << endl;
    output << "jun15: " << jun15 << endl;
    output << "jul15: " << jul15 << endl;
    output << "aug15: " << aug15 << endl;
    output << "sep15: " << sep15 << endl;
    output << "oct15: " << oct15 << endl;
    output << "nov15: " << nov15 << endl;
    output << "dec15: " << dec15 << endl;
    //2016 crimes
    output << "2016 crimes: " << jan16 + feb16 + mar16 + apr16 + may16 + jun16 + jul16 + aug16 + sep16 + oct16 + nov16 + dec16 << 
    " " << (jan16 + feb16 + mar16 + apr16 + may16 + jun16 + jul16 + aug16 + sep16 + oct16 + nov16 + dec16) / (double)pop16 << endl;
    output << "spring: " << spring16 << endl;
    output << "summer: " << summer16 << endl;
    output << "fall: " << fall16 << endl;
    output << "winter: " << winter16 << endl;
    output << "jan16: " << jan16 << endl;
    output << "feb16: " << feb16 << endl;
    output << "mar16: " << mar16 << endl;
    output << "apr16: " << apr16 << endl;
    output << "may16: " << may16 << endl;
    output << "jun16: " << jun16 << endl;
    output << "jul16: " << jul16 << endl;
    output << "aug16: " << aug16 << endl;
    output << "sep16: " << sep16 << endl;
    output << "oct16: " << oct16 << endl;
    output << "nov16: " << nov16 << endl;
    output << "dec16: " << dec16 << endl;
    //2017 crimes
    output << "2017 crimes: " << jan17 + feb17 + mar17 + apr17 + may17 + jun17 + jul17 + aug17 + sep17 + oct17 + nov17 + dec17 << 
    " " << (jan17 + feb17 + mar17 + apr17 + may17 + jun17 + jul17 + aug17 + sep17 + oct17 + nov17 + dec17) / (double)pop17 << endl;
    output << "spring: " << spring17 << endl;
    output << "summer: " << summer17 << endl;
    output << "fall: " << fall17 << endl;
    output << "winter: " << winter17 << endl;
    output << "jan17: " << jan17 << endl;
    output << "feb17: " << feb17 << endl;
    output << "mar17: " << mar17 << endl;
    output << "apr17: " << apr17 << endl;
    output << "may17: " << may17 << endl;
    output << "jun17: " << jun17 << endl;
    output << "jul17: " << jul17 << endl;
    output << "aug17: " << aug17 << endl;
    output << "sep17: " << sep17 << endl;
    output << "oct17: " << oct17 << endl;
    output << "nov17: " << nov17 << endl;
    output << "dec17: " << dec17 << endl;
    //2018 crimes
    output << "2018 crimes: " << jan18 + feb18 + mar18 + apr18 + may18 + jun18 + jul18 + aug18 + sep18 + oct18 + nov18 + dec18 << 
    " " << (jan18 + feb18 + mar18 + apr18 + may18 + jun18 + jul18 + aug18 + sep18 + oct18 + nov18 + dec18) / (double)pop18 << endl;
    output << "spring: " << spring18 << endl;
    output << "summer: " << summer18 << endl;
    output << "fall: " << fall18 << endl;
    output << "winter: " << winter18 << endl;
    output << "jan18: " << jan18 << endl;
    output << "feb18: " << feb18 << endl;
    output << "mar18: " << mar18 << endl;
    output << "apr18: " << apr18 << endl;
    output << "may18: " << may18 << endl;
    output << "jun18: " << jun18 << endl;
    output << "jul18: " << jul18 << endl;
    output << "aug18: " << aug18 << endl;
    output << "sep18: " << sep18 << endl;
    output << "oct18: " << oct18 << endl;
    output << "nov18: " << nov18 << endl;
    output << "dec18: " << dec18 << endl;
    //2019 crimes
    output << "2019 crimes: " << jan19 + feb19 + mar19 + apr19 + may19 + jun19 + jul19 + aug19 + sep19 + oct19 + nov19 << endl;
    output << "spring: " << spring19 << endl;
    output << "summer: " << summer19 << endl;
    output << "fall: " << fall19 << endl;
    output << "winter: " << winter19 << endl;
    output << "jan19: " << jan19 << endl;
    output << "feb19: " << feb19 << endl;
    output << "mar19: " << mar19 << endl;
    output << "apr19: " << apr19 << endl;
    output << "may19: " << may19 << endl;
    output << "jun19: " << jun19 << endl;
    output << "jul19: " << jul19 << endl;
    output << "aug19: " << aug19 << endl;
    output << "sep19: " << sep19 << endl;
    output << "oct19: " << oct19 << endl;
    output << "nov19: " << nov19 << endl;
    //crime times
    output << "t0: " << tm0 << " " << tm0/(double)processed << endl;
    output << "t1: " << tm1 << " " << tm1/(double)processed << endl;
    output << "t2: " << tm2 << " " << tm2/(double)processed << endl;
    output << "t3: " << tm3 << " " << tm3/(double)processed << endl;
    output << "t4: " << tm4 << " " << tm4/(double)processed<< endl;
    output << "t5: " << tm5 << " " << tm5/(double)processed << endl;
    output << "t6: " << tm6 << " " << tm6/(double)processed << endl;
    output << "t7: " << tm7 << " " << tm7/(double)processed << endl;
    output << "t8: " << tm8 << " " << tm8/(double)processed << endl;
    output << "t9: " << tm9 << " " << tm9/(double)processed << endl;
    output << "t10: " << tm10 << " " << tm10/(double)processed << endl;
    output << "t11: " << tm11 << " " << tm11/(double)processed << endl;
    output << "t12: " << tm12 << " " << tm12/(double)processed << endl;
    output << "t13: " << tm13 << " " << tm13/(double)processed << endl;
    output << "t14: " << tm14 << " " << tm14/(double)processed << endl;
    output << "t15: " << tm15 << " " << tm15/(double)processed << endl;
    output << "t16: " << tm16 << " " << tm16/(double)processed << endl;
    output << "t17: " << tm17 << " " << tm17/(double)processed << endl;
    output << "t18: " << tm18 << " " << tm18/(double)processed << endl;
    output << "t19: " << tm19 << " " << tm19/(double)processed << endl;
    output << "t20: " << tm20 << " " << tm20/(double)processed << endl;
    output << "t21: " << tm21 << " " << tm21/(double)processed << endl;
    output << "t22: " << tm22 << " " << tm22/(double)processed << endl;
    output << "t23: " << tm23 << " " << tm23/(double)processed << endl;
    //crimes
    output << "homicide: " << homicide << " " << homicide/(double)processed << endl;
    output << "rape: " << rape << " " << rape/(double)processed << endl;
    output << "robbery: " << robbery << " " << robbery/(double)processed << endl;
    output << "assault: " << assault << " " << assault/(double)processed << endl;
    output << "burglary: " << burglary << " " << burglary/(double)processed << endl;
    output << "larceny: " << larceny << " " << larceny/(double)processed << endl;
    output << "auto theft: " << auto_theft << " " << auto_theft/(double)processed << endl;
    output << "arson: " << arson << " " << arson/(double)processed << endl;
    output << "shooting: " << shooting << " " << shooting/(double)processed << endl;
    //in/out
    output << "inside: " << inside << " " << inside/(double)processed << endl;
    output << "outside: " << outside << " " << outside/(double)processed << endl;
    //district
    output << "Northern: " << N << " " << N/(double)processed << endl;
    output << "Northeastern: " << NE << " " << NE/(double)processed << endl;
    output << "Northwestern: " << NW << " " << NW/(double)processed << endl;
    output << "Central: " << C << " " << C/(double)processed << endl;
    output << "Southern: " << S << " " << S/(double)processed << endl;
    output << "Southeastern: " << SE << " " << SE/(double)processed << endl;
    output << "Southwestern: " << SW << " " << SW/(double)processed << endl;
    output << "Eastern: " << E << " " << E/(double)processed << endl;
    output << "Western: " << W << " " << W/(double)processed << endl;

    output.close(); 
}
//given the array heat associated with the combined results, generate a heatmap and overlay it with map of baltimore city
void results::draw_map() {
    cout << "heat " << heatRecorded << endl;
    cout << "maxCrimes " << maxCrimes << endl;
    //open map image and output image
    cimg_library::CImg<double> map("Bmore.bmp");
    cimg_library::CImg<double> output(WIDTH, HEIGHT, 1, 3);
    //use maxCrimes to sclae color of each pixel with red indicatiing higher crime rates and blue indicating lower
    double crimeStep = 1.0 / maxCrimes; //crimeStep * number of crimes determines how red the image will be
    double red; //a double with value 0 to 1 used to determine how red a pixel will be
    color image[WIDTH][HEIGHT];
    //for each pixel, get its color and store it in image array
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            red = crimeStep * inArea[i][j];
            //red scales with crime, and blue becomes 1-red. as crimes increase up to maxCrimes, pixels become red and as they
            //move toward 0, they become blue
            image[i][j].r = red * 255;
            image[i][j].g = 125.0;
            image[i][j].b = 0;
        }
    }
    for(int i = 0; i < WIDTH; i++) {
        for(int j = 0; j < HEIGHT; j++) {
            //if the heatmap shows no crimes in an area, use the pixel from the baltimore map
            if(image[i][j].r == 0) {
                output(i,j,0) = map(i,j,0);
                output(i,j,1) = map(i,j,1);
                output(i,j,2) = map(i,j,2);
            }
            //if non-zero value for crimes within a 7 pixel radius, overlay with baltimore map with 75% influence given to heat map
            else {
                output(i,j,0) = (map(i,j,0) * .25) + (image[i][j].r * .75);
                output(i,j,1) = (map(i,j,1) * .25) + (image[i][j].g * .75);
                output(i,j,2) = (map(i,j,2) * .25) + (image[i][j].b * .75);
            }
        }
    }
    //save output image
    output.save_bmp("heatmap.bmp");
}
void *consume(void *status) {
    //buffer for holding temporary strings from ifstream and char pointer to interact as a string
    char buff[200];
    char *string;
    string = &buff[0];
    char temp_buff[200];
    char *position;
    position = string;
    int temp_index = 0;
    double longitude, latitude;
    int longcoord, latcoord;
    int day;
    pair<double,double> longlat;
    thread_state *state = (thread_state *) status;
    //tries to get and process a line until a thread causes it to reach eof
    while(crimes.eof() == false) {
        //if this thread has been chosen to activate, read and process next line of crimes
        if(state->activated == true) {
            crimes.getline(string, 199);
            //relinquish control over stream
            processed++;
            state->ready = false;
            state->activated = false;
            //////////
            //get date
            //////////
            //get month
            while(*position != '/') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator, increment position to point to next char and reset temp_index for next use
            temp_buff[temp_index] = '\0';
            position++;
            temp_index = 0;
            //if january
            if(strcmp(&temp_buff[0],"1") == 0) {
                state->res.jan = true;
            }
            //if february
            else if(strcmp(&temp_buff[0],"2") == 0) {
                state->res.feb = true;
            }
            //if march
            else if(strcmp(&temp_buff[0],"3") == 0) {
                state->res.mar = true;
            }
            //if april
            else if(strcmp(&temp_buff[0],"4") == 0) {
                state->res.apr = true;
            }
            //if may
            else if(strcmp(&temp_buff[0],"5") == 0) {
                state->res.may = true;
            }
            //if june
            else if(strcmp(&temp_buff[0],"6") == 0) {
                state->res.jun = true;
            }
            //if july
            else if(strcmp(&temp_buff[0],"7") == 0) {
                state->res.jul = true;
            }
            //if august
            else if(strcmp(&temp_buff[0],"8") == 0) {
                state->res.aug = true;
            }
            //if september
            else if(strcmp(&temp_buff[0],"9") == 0) {
                state->res.sep = true;
            }
            //if october
            else if(strcmp(&temp_buff[0],"10") == 0) {
                state->res.oct = true;
            }
            //if november
            else if(strcmp(&temp_buff[0],"11") == 0) {
                state->res.nov = true;
            }
            //if december
            else if(strcmp(&temp_buff[0],"12") == 0) {
                state->res.dec = true;
            }
            //get day
            while(*position != '/') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator, increment position to point to next char and reset temp_index for next use
            temp_buff[temp_index] = '\0';
            position++;
            temp_index = 0;
            day = atoi(&temp_buff[0]);
            //get year
            while(*position != ',') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator, increment position and reset temp_index
            temp_buff[temp_index] = '\0';
            position++;
            temp_index = 0;
            //get month/year combo. when found, lower month flag and increment month/year variable
            //if 2014
            if(strcmp(&temp_buff[0],"2014") == 0) {
                //if january
                if(state->res.jan == true) {
                    state->res.jan = false;
                    state->res.jan14++;
                    state->res.winter14++;
                }
                //if february
                if(state->res.feb == true) {
                    state->res.feb = false;
                    state->res.feb14++;
                    state->res.winter14++;
                }
                //if march
                if(state->res.mar == true) {
                    state->res.mar = false;
                    state->res.mar14++;
                    if(day < 21) {
                        state->res.winter14++;
                    }
                    else {
                        state->res.spring14++;
                    }
                }
                //if april
                if(state->res.apr == true) {
                    state->res.apr = false;
                    state->res.apr14++;
                    state->res.spring14++;
                }
                //if may
                if(state->res.may == true) {
                    state->res.may = false;
                    state->res.may14++;
                    state->res.spring14++;
                }
                //if june
                if(state->res.jun == true) {
                    state->res.jun = false;
                    state->res.jun14++;
                    if(day < 21) {
                        state->res.spring14++;
                    }
                    else {
                        state->res.summer14++;
                    }
                }
                //if july
                if(state->res.jul == true) {
                    state->res.jul = false;
                    state->res.jul14++;
                    state->res.summer14++;
                }
                //if august
                if(state->res.aug == true) {
                    state->res.aug = false;
                    state->res.aug14++;
                    state->res.summer14++;
                }
                //if september
                if(state->res.sep == true) {
                    state->res.sep = false;
                    state->res.sep14++;
                    if(day < 23) {
                        state->res.summer14++;
                    }
                    else {
                        state->res.fall14++;
                    }
                }
                //if october
                if(state->res.oct == true) {
                    state->res.oct = false;
                    state->res.oct14++;
                    state->res.fall14++;
                }
                //if november
                if(state->res.nov == true) {
                    state->res.nov = false;
                    state->res.nov14++;
                    state->res.fall14++;
                }
                //if december
                if(state->res.dec == true) {
                    state->res.dec = false;
                    state->res.dec14++;
                    if(day < 21) {
                        state->res.fall14++;
                    }
                    else {
                        state->res.winter14++;
                    }
                }
            }
            //if 2015
            else if(strcmp(&temp_buff[0],"2015") == 0) {
                //if january
                if(state->res.jan == true) {
                    state->res.jan = false;
                    state->res.jan15++;
                    state->res.winter15++;
                }
                //if february
                if(state->res.feb == true) {
                    state->res.feb = false;
                    state->res.feb15++;
                    state->res.winter15++;
                }
                //if march
                if(state->res.mar == true) {
                    state->res.mar = false;
                    state->res.mar15++;
                    if(day < 20) {
                        state->res.winter15++;
                    }
                    else {
                        state->res.spring15++;
                    }
                }
                //if april
                if(state->res.apr == true) {
                    state->res.apr = false;
                    state->res.apr15++;
                    state->res.spring15++;
                }
                //if may
                if(state->res.may == true) {
                    state->res.may = false;
                    state->res.may15++;
                    state->res.spring15++;
                }
                //if june
                if(state->res.jun == true) {
                    state->res.jun = false;
                    state->res.jun15++;
                    if(day < 21) {
                        state->res.spring15++;
                    }
                    else {
                        state->res.summer15++;
                    }
                }
                //if july
                if(state->res.jul == true) {
                    state->res.jul = false;
                    state->res.jul15++;
                    state->res.summer15++;
                }
                //if august
                if(state->res.aug == true) {
                    state->res.aug = false;
                    state->res.aug15++;
                    state->res.summer15++;
                }
                //if september
                if(state->res.sep == true) {
                    state->res.sep = false;
                    state->res.sep15++;
                    if(day < 23) {
                        state->res.summer15++;
                    }
                    else {
                        state->res.fall15++;
                    }
                }
                //if october
                if(state->res.oct == true) {
                    state->res.oct = false;
                    state->res.oct15++;
                    state->res.fall15++;
                }
                //if november
                if(state->res.nov == true) {
                    state->res.nov = false;
                    state->res.nov15++;
                    state->res.fall15++;
                }
                //if december
                if(state->res.dec == true) {
                    state->res.dec = false;
                    state->res.dec15++;
                    if(day < 21) {
                        state->res.fall15++;
                    }
                    else {
                        state->res.winter15++;
                    }
                }
            }
            //if 2016
            else if(strcmp(&temp_buff[0],"2016") == 0) {
                //if january
                if(state->res.jan == true) {
                    state->res.jan = false;
                    state->res.jan16++;
                    state->res.winter16++;
                }
                //if february
                if(state->res.feb == true) {
                    state->res.feb = false;
                    state->res.feb16++;
                    state->res.winter16++;
                }
                //if march
                if(state->res.mar == true) {
                    state->res.mar = false;
                    state->res.mar16++;
                    if(day < 20) {
                        state->res.winter16++;
                    }
                    else {
                        state->res.spring16++;
                    }
                }
                //if april
                if(state->res.apr == true) {
                    state->res.apr = false;
                    state->res.apr16++;
                    state->res.spring16++;
                }
                //if may
                if(state->res.may == true) {
                    state->res.may = false;
                    state->res.may16++;
                    state->res.spring16++;
                }
                //if june
                if(state->res.jun == true) {
                    state->res.jun = false;
                    state->res.jun16++;
                    if(day < 21) {
                        state->res.spring16++;
                    }
                    else {
                        state->res.summer16++;
                    }
                }
                //if july
                if(state->res.jul == true) {
                    state->res.jul = false;
                    state->res.jul16++;
                    state->res.summer16++;
                }
                //if august
                if(state->res.aug == true) {
                    state->res.aug = false;
                    state->res.aug16++;
                    state->res.summer16++;
                }
                //if september
                if(state->res.sep == true) {
                    state->res.sep = false;
                    state->res.sep16++;
                    if(day < 23) {
                        state->res.summer16++;
                    }
                    else {
                        state->res.fall16++;
                    }
                }
                //if october
                if(state->res.oct == true) {
                    state->res.oct = false;
                    state->res.oct16++;
                    state->res.fall16++;
                }
                //if november
                if(state->res.nov == true) {
                    state->res.nov = false;
                    state->res.nov16++;
                    state->res.fall16++;
                }
                //if december
                if(state->res.dec == true) {
                    state->res.dec = false;
                    state->res.dec16++;
                    if(day < 21) {
                        state->res.fall16++;
                    }
                    else {
                        state->res.winter16++;
                    }
                }
            }
            //if 2017
            else if(strcmp(&temp_buff[0],"2017") == 0) {
                //if january
                if(state->res.jan == true) {
                    state->res.jan = false;
                    state->res.jan17++;
                    state->res.winter17++;
                }
                //if february
                if(state->res.feb == true) {
                    state->res.feb = false;
                    state->res.feb17++;
                    state->res.winter17++;
                }
                //if march
                if(state->res.mar == true) {
                    state->res.mar = false;
                    state->res.mar17++;
                    if(day < 20) {
                        state->res.winter17++;
                    }
                    else {
                        state->res.spring17++;
                    }
                }
                //if april
                if(state->res.apr == true) {
                    state->res.apr = false;
                    state->res.apr17++;
                    state->res.spring17++;
                }
                //if may
                if(state->res.may == true) {
                    state->res.may = false;
                    state->res.may17++;
                    state->res.spring17++;
                }
                //if june
                if(state->res.jun == true) {
                    state->res.jun = false;
                    state->res.jun17++;
                    if(day < 21) {
                        state->res.spring17++;
                    }
                    else {
                        state->res.summer17++;
                    }
                }
                //if july
                if(state->res.jul == true) {
                    state->res.jul = false;
                    state->res.jul17++;
                    state->res.summer17++;
                }
                //if august
                if(state->res.aug == true) {
                    state->res.aug = false;
                    state->res.aug17++;
                    state->res.summer17++;
                }
                //if september
                if(state->res.sep == true) {
                    state->res.sep = false;
                    state->res.sep17++;
                    if(day < 23) {
                        state->res.summer17++;
                    }
                    else {
                        state->res.fall17++;
                    }
                }
                //if october
                if(state->res.oct == true) {
                    state->res.oct = false;
                    state->res.oct17++;
                    state->res.fall17++;
                }
                //if november
                if(state->res.nov == true) {
                    state->res.nov = false;
                    state->res.nov17++;
                    state->res.fall17++;
                }
                //if december
                if(state->res.dec == true) {
                    state->res.dec = false;
                    state->res.dec17++;
                    if(day < 21) {
                        state->res.fall17++;
                    }
                    else {
                        state->res.winter17++;
                    }
                }
            }
            //if 2018
            else if(strcmp(&temp_buff[0],"2018") == 0) {
                //if january
                if(state->res.jan == true) {
                    state->res.jan = false;
                    state->res.jan18++;
                    state->res.winter18++;
                }
                //if february
                if(state->res.feb == true) {
                    state->res.feb = false;
                    state->res.feb18++;
                    state->res.winter18++;
                }
                //if march
                if(state->res.mar == true) {
                    state->res.mar = false;
                    state->res.mar18++;
                    if(day < 20) {
                        state->res.winter18++;
                    }
                    else {
                        state->res.spring18++;
                    }
                }
                //if april
                if(state->res.apr == true) {
                    state->res.apr = false;
                    state->res.apr18++;
                    state->res.spring18++;
                }
                //if may
                if(state->res.may == true) {
                    state->res.may = false;
                    state->res.may18++;
                    state->res.spring18++;
                }
                //if june
                if(state->res.jun == true) {
                    state->res.jun = false;
                    state->res.jun18++;
                    if(day < 21) {
                        state->res.spring18++;
                    }
                    else {
                        state->res.summer18++;
                    }
                }
                //if july
                if(state->res.jul == true) {
                    state->res.jul = false;
                    state->res.jul18++;
                    state->res.summer18++;
                }
                //if august
                if(state->res.aug == true) {
                    state->res.aug = false;
                    state->res.aug18++;
                    state->res.summer18++;
                }
                //if september
                if(state->res.sep == true) {
                    state->res.sep = false;
                    state->res.sep18++;
                    if(day < 23) {
                        state->res.summer18++;
                    }
                    else {
                        state->res.fall18++;
                    }
                }
                //if october
                if(state->res.oct == true) {
                    state->res.oct = false;
                    state->res.oct18++;
                    state->res.fall18++;
                }
                //if november
                if(state->res.nov == true) {
                    state->res.nov = false;
                    state->res.nov18++;
                    state->res.fall18++;
                }
                //if december
                if(state->res.dec == true) {
                    state->res.dec = false;
                    state->res.dec18++;
                    if(day < 21) {
                        state->res.fall18++;
                    }
                    else {
                        state->res.winter18++;
                    }
                }
            }
            //if 2019
            else if(strcmp(&temp_buff[0],"2019") == 0) {
                //if january
                if(state->res.jan == true) {
                    state->res.jan = false;
                    state->res.jan19++;
                    state->res.winter19++;
                }
                //if february
                if(state->res.feb == true) {
                    state->res.feb = false;
                    state->res.feb19++;
                    state->res.winter19++;
                }
                //if march
                if(state->res.mar == true) {
                    state->res.mar = false;
                    state->res.mar19++;
                    if(day < 20) {
                        state->res.winter19++;
                    }
                    else {
                        state->res.spring19++;
                    }
                }
                //if april
                if(state->res.apr == true) {
                    state->res.apr = false;
                    state->res.apr19++;
                    state->res.spring19++;
                }
                //if may
                if(state->res.may == true) {
                    state->res.may = false;
                    state->res.may19++;
                    state->res.spring19++;
                }
                //if june
                if(state->res.jun == true) {
                    state->res.jun = false;
                    state->res.jun19++;
                    if(day < 21) {
                        state->res.spring19++;
                    }
                    else {
                        state->res.summer19++;
                    }
                }
                //if july
                if(state->res.jul == true) {
                    state->res.jul = false;
                    state->res.jul19++;
                    state->res.summer19++;
                }
                //if august
                if(state->res.aug == true) {
                    state->res.aug = false;
                    state->res.aug19++;
                    state->res.summer19++;
                }
                //if september
                if(state->res.sep == true) {
                    state->res.sep = false;
                    state->res.sep19++;
                    if(day < 23) {
                        state->res.summer19++;
                    }
                    else {
                        state->res.fall19++;
                    }
                }
                //if october
                if(state->res.oct == true) {
                    state->res.oct = false;
                    state->res.oct19++;
                    state->res.fall19++;
                }
                //if november
                if(state->res.nov == true) {
                    state->res.nov = false;
                    state->res.nov19++;
                    state->res.fall19++;
                }
            }
            ////////////////
            //get crime hour
            ////////////////
            while(*position != ':') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator and reset temp_index
            temp_buff[temp_index] = '\0';
            temp_index = 0;
            //skip position to crime code
            while(*position != ',') {
                position++;
            }
            position++;
            //increment position to point to the crime code
            //increment appropriate crime hour
            //if hour 0
            if(strcmp(&temp_buff[0],"0") == 0) {
                state->res.tm0++;
            }
            //if hour 1
            else if(strcmp(&temp_buff[0],"1") == 0) {
                state->res.tm1++;
            }
            //if hour 2
            else if(strcmp(&temp_buff[0],"2") == 0) {
                state->res.tm2++;
            }
            //if hour 3
            else if(strcmp(&temp_buff[0],"3") == 0) {
                state->res.tm3++;
            }
            //if hour 4
            else if(strcmp(&temp_buff[0],"4") == 0) {
                state->res.tm4++;
            }
            //if hour 5
            else if(strcmp(&temp_buff[0],"5") == 0) {
                state->res.tm5++;
            }
            //if hour 6
            else if(strcmp(&temp_buff[0],"6") == 0) {
                state->res.tm6++;
            }
            //if hour 7
            else if(strcmp(&temp_buff[0],"7") == 0) {
                state->res.tm7++;
            }
            //if hour 8
            else if(strcmp(&temp_buff[0],"8") == 0) {
                state->res.tm8++;
            }
            //if hour 9
            else if(strcmp(&temp_buff[0],"9") == 0) {
                state->res.tm9++;
            }
            //if hour 10
            else if(strcmp(&temp_buff[0],"10") == 0) {
                state->res.tm10++;
            }
            //if hour 11
            else if(strcmp(&temp_buff[0],"11") == 0) {
                state->res.tm11++;
            }
            //if hour 12
            else if(strcmp(&temp_buff[0],"12") == 0) {
                state->res.tm12++;
            }
            //if hour 13
            else if(strcmp(&temp_buff[0],"13") == 0) {
                state->res.tm13++;
            }
            //if hour 14
            else if(strcmp(&temp_buff[0],"14") == 0) {
                state->res.tm14++;
            }
            //if hour 15
            else if(strcmp(&temp_buff[0],"15") == 0) {
                state->res.tm15++;
            }
            //if hour 16
            else if(strcmp(&temp_buff[0],"16") == 0) {
                state->res.tm16++;
            }
            //if hour 17
            else if(strcmp(&temp_buff[0],"17") == 0) {
                state->res.tm17++;
            }
            //if hour 18
            else if(strcmp(&temp_buff[0],"18") == 0) {
                state->res.tm18++;
            }
            //if hour 19
            else if(strcmp(&temp_buff[0],"19") == 0) {
                state->res.tm19++;
            }
            //if hour 20
            else if(strcmp(&temp_buff[0],"20") == 0) {
                state->res.tm20++;
            }
            //if hour 21
            else if(strcmp(&temp_buff[0],"21") == 0) {
                state->res.tm21++;
            }
            //if hour 22
            else if(strcmp(&temp_buff[0],"22") == 0) {
                state->res.tm22++;
            }
            //if hour 23
            else if(strcmp(&temp_buff[0],"23") == 0) {
                state->res.tm23++;
            }
            ////////////////
            //get crime code
            ////////////////
            //first char of crime code is general crime and position is already pointing to it, so make comparison
            //if homicide
            if(*position == '1') {
                state->res.homicide++;
            }
            //if rape
            else if(*position == '2') {
                state->res.rape++;
            }
            //if robbery
            else if(*position == '3') {
                state->res.robbery++;
            }
            //if assault
            else if(*position == '4') {
                state->res.assault++;
            }
            //if burglary
            else if(*position == '5') {
                state->res.burglary++;
            }
            //if larceny
            else if(*position == '6') {
                state->res.larceny++;
            }
            //if auto theft
            else if(*position == '7') {
                state->res.auto_theft++;
            }
            //if arson
            else if(*position == '8') {
                state->res.arson++;
            }
            //if shooting
            else if(*position == '9') {
                state->res.shooting++;
            }
            //skip to location
            while(*position != ',') {
                position++;
            }
            position++;
            //skip to description
            while(*position != ',') {
                position++;
            }
            position++;
            //skip to inside/outside
            while(*position != ',') {
                position++;
            }
            position++;
            //if inside
            if(*position == 'I') {
                state->res.inside++;
            }
            //if outside
            else {
                state->res.outside++;
            }
            //skip to weapon
            position = position + 2;
            //skip to district
            while(*position != ',') {
                position++;
            }
            position++;
            //get district
            while(*position != ',') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator, increment position and reset temp_index
            temp_buff[temp_index] = '\0';
            temp_index = 0;
            position++;
            //if Northern
            if(strcmp(&temp_buff[0],"NORTHERN") == 0) {
                state->res.N++;
            }
            //if Northeastern
            else if(strcmp(&temp_buff[0],"NORTHEAST") == 0) {
                state->res.NE++;
            }
            //if Northwestern
            else if(strcmp(&temp_buff[0],"NORTHWEST") == 0) {
                state->res.NW++;
            }
            //if central
            else if(strcmp(&temp_buff[0],"CENTRAL") == 0) {
                state->res.C++;
            }
            //if Southern
            else if(strcmp(&temp_buff[0],"SOUTHERN") == 0) {
                state->res.S++;
            }
            //if Southeastern
            else if(strcmp(&temp_buff[0],"SOUTHEAST") == 0) {
                state->res.SE++;
            }
            //if Southwestern
            else if(strcmp(&temp_buff[0],"SOUTHWEST") == 0) {
                state->res.SW++;
            }
            //if Eastern
            else if(strcmp(&temp_buff[0],"EASTERN") == 0) {
                state->res.E++;
            }
            //if Western
            else if(strcmp(&temp_buff[0],"WESTERN") == 0) {
                state->res.W++;
            }
            //get longitude
            while(*position != ',') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator, increment position and reset temp_index
            temp_buff[temp_index] = '\0';
            temp_index = 0;
            position++;
            longitude = strtod(&temp_buff[0], NULL);
            //get latitude
            while(*position != '\0') {
                temp_buff[temp_index] = *position;
                position++;
                temp_index++;
            }
            //add null terminator, increment position and reset temp_index
            temp_buff[temp_index] = '\0';
            temp_index = 0;
            latitude = strtod(&temp_buff[0], NULL);
            //if valid data and in bounds
            if(longitude != 0 && latitude != 0 && longitude > state->res.topL.first && latitude < state->res.topL.second &&
                longitude < state->res.botR.first && latitude > state->res.botR.second) {
                //get array point corresponding to longitude latitude coords
                //longitude
                longitude = longitude - state->res.topL.first; //gets a value from 0 to longrange
                longcoord = floor(longitude / longstep); //get array coord
                latitude = latitude - state->res.botR.second; //gets a value from 0 to latrange
                latcoord = floor(latitude / latstep);
                //increment number of crimes at that position
                state->res.heat[longcoord][latcoord]++;
                heatRecorded++; //debugging var
                
            }
            //
            //update ready and position pointer
            position = string;
            state->ready = true;
        }
    }
    state->res.finished = true;
}

int main() {
    //initialize thread states
    t1.ready = true;
    t1.activated = false;
    //open crimes.csv
    crimes.open("crimes.csv");
    //create consumer threads
    pthread_t con1;
    int thread1;
    thread_state *thr1;
    thr1 = &t1;
    //use getline to skip columns string
    char *dummyString;
    crimes.getline(dummyString, 200);
    //begin thread processing 
    thread1 = pthread_create(&con1, NULL, consume, thr1);
    //initial .getline() to skip the column names
    //while there is a thread that has not finished its findings
    while(t1.res.finished == false) {
        //check for a ready thread
        //if t1 is ready and no thread is accessing the stream, activate t1
        if(t1.ready == true && t1.activated == false) {
            t1.activated = true;
        }
        //repeats until a thread causes crimes.eof() to be true
    }
    //join threads before closing
    pthread_join(con1, NULL);
    //collect data from reports
    report.combine();
    //write report
    report.write_report();
    //draw heatmap
    report.draw_map();
    //close crimes.csv
    crimes.close();
    cout << "processed " << processed << endl;
    return 0;
}