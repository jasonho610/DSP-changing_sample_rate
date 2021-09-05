//
//  main.cpp
//  DSP HW5
//
//  Created by 何冠勳 on 2020/12/30.
//
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "Wav.h"
#include "Vector_Operation.h"
#define order 1023
#define pi 3.14159265358979323846
using namespace std;

double sinc(double x)
{
    if(x == 0) return 1;
    else return sin(x)/x;
}

double impulse_response(int n, int M, double w_c, string window_opt)
{
    double w, centr = (double)(n - M);
    if(window_opt == "hanning") w = 0.5 + 0.5*cos(pi*centr/M);
    else if(window_opt == "hamming") w = 0.54 + 0.46*cos(pi*centr/M);
    else w = 1;
    
    return sinc(centr*w_c)*(w_c/pi)*w;
}

vector<double> lpfilter(int gain, double cutoff, int m = order)
{
    vector<double> h_n;
    for(int j=0;j<order+1;j++)
        h_n.push_back((double)gain*impulse_response(j, (order-1)/2, cutoff, "hanning"));
    return h_n;
}

int main()
{
    string infname = "input.wav";
    string outfname = "output.wav";
    Stereo_Wav wavein = WaveRead(infname);
    Stereo_Wav waveout;
    
    //int M = 441, L = 80;
    vector<short> x_e, x_i;
    vector<double> h_n;
    
    time_t start = time(NULL);
    /*-----------Left Data-----------*/
    cout << "--------Left Data---------" << endl;
    cout << "Stage 1 : Up 4" << endl;
    x_e = resample(wavein.left_data, 4, "up");
    h_n = lpfilter(4, pi/4);
    x_e = conv(x_e, h_n, "same");
    
    cout << "Stage 2 : Up 4" << endl;
    x_e = resample(x_e, 4, "up");
    x_e = conv(x_e, h_n, "same");
    
    cout << "Stage 3 : Up 5 and Down 7" << endl;
    x_e = resample(x_e, 5, "up");
    h_n = lpfilter(5, pi/7);
    x_i = conv(x_e, h_n, "same"); x_e.clear();
    x_i = resample(x_i, 7, "down");
    
    cout << "Stage 4 : Down 7" << endl;
    h_n = lpfilter(1, pi/7);
    x_i = conv(x_i, h_n, "same");
    x_i = resample(x_i, 7, "down");

    cout << "Stage 5 : Down 3" << endl;
    h_n = lpfilter(1, pi/3);
    x_i = conv(x_i, h_n, "same");
    x_i = resample(x_i, 3, "down");

    cout << "Stage 6 : Down 3" << endl;
    x_i = conv(x_i, h_n, "same");
    waveout.left_data = resample(x_i, 3, "down"); x_i.clear();
    
    
    /*------------Right Data-------------*/
    cout << "--------Right Data---------" << endl;
    cout << "Stage 1 : Up 4" << endl;
    x_e = resample(wavein.right_data, 4, "up");
    h_n = lpfilter(4, pi/4);
    x_e = conv(x_e, h_n, "same");
    
    cout << "Stage 2 : Up 4" << endl;
    x_e = resample(x_e, 4, "up");
    x_e = conv(x_e, h_n, "same");
    
    cout << "Stage 3 : Up 5 and Down 7" << endl;
    x_e = resample(x_e, 5, "up");
    h_n = lpfilter(5, pi/7);
    x_i = conv(x_e, h_n, "same"); x_e.clear();
    x_i = resample(x_i, 7, "down");
    
    cout << "Stage 4 : Down 7" << endl;
    h_n = lpfilter(1, pi/7);
    x_i = conv(x_i, h_n, "same");
    x_i = resample(x_i, 7, "down");

    cout << "Stage 5 : Down 3" << endl;
    h_n = lpfilter(1, pi/3);
    x_i = conv(x_i, h_n, "same");
    x_i = resample(x_i, 3, "down");

    cout << "Stage 6 : Down 3" << endl;
    x_i = conv(x_i, h_n, "same");
    waveout.right_data = resample(x_i, 3, "down"); x_i.clear();
    
    /*------------duration-------------*/
    time_t end = time(NULL);
    double diff = difftime(end, start);
    cout << endl << "Conversion Done, ";
    cout << "took " << diff << " sec." << endl;
    
    /*------------write wavfile--------------*/
    waveout.header = make_WaveHeader(2, 8000, 16, waveout.left_data.size());
    WaveWrite(outfname, waveout);
    
    return 0;
}

