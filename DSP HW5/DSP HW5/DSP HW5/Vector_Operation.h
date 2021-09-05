//
//  Vector_Operation.h
//  DSP HW5
//
//  Created by 何冠勳 on 2021/1/29.
//
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <time.h>
#include "Wav.h"
using namespace std;

#ifndef Vector_Operation_h
#define Vector_Operation_h
vector<short> resample(vector<short> b, int times, string mode)
{
    vector<short> a;
    if(mode == "up")
    {
        for(unsigned i=0;i<b.size();i++)
        {
            a.push_back(b[i]);
            for(int j=0; j<times-1; j++)
                a.push_back(0);
        }
    }
    else if (mode == "down")
    {
        for(unsigned i=0;i<b.size();i+=times)
            a.push_back(b[i]);
    }
    return a;
}

vector<short> conv(vector<short> signal, vector<double> imp_resp, string opt)
{
    // this function is to pattern on python model numpy.convolve()
    unsigned M = (unsigned)signal.size();
    unsigned L = (unsigned)imp_resp.size();
    unsigned N = M+L-1;
    short count = 0;
    
    cout << "Calculating convolution, please wait" << endl;
    vector<short> con;
    for(int n=0;n<N;n++)
    {
        double temp = 0;
        for(int k=0;k<L;k++)
        {
            double input = 0.0;
            if((n-k>=0)&&(n-k<M))
                input = signal[n-k];
            temp = temp+(input*imp_resp[k]);
        }
        con.push_back(temp);
        if((n%10000)==0)
        {
            cout << ".";
            count++;
            if((count%50)==0)
                cout << endl;
        }
    }
    cout << endl << "Convolution done!" << endl;

    if(opt=="same")
    {
        unsigned max_len = max(M, L);
        vector<short> con_cut(con.begin()+(N-max_len)/2, con.begin()+(N-max_len)/2+max_len);
        return con_cut;
    }
    else return con;
}

#endif /* Vector_Operation_h */
