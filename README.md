# DSP HW5 
<center> <font size=2> Jason [2021/01] </font> </center>

original hackmd : https://hackmd.io/@jasonho610/SJFfOcTaD

## Objective
In this assignment, we are going to convert a .wav from sampling rate 44.1k to 8k.

**Input signal $x[n]$**: input.wav, a stereo music file (44.1kHz, 16bits)
**Output signal $\tilde{x_d} [n]$**: output.wav, a stereo music file (8kHz, 16bits)
![](https://i.imgur.com/xwoE7B9.png)
where, $T = 1/44100$ sec for input signal, $TM/L = 1/8000$ sec for output signal.

In short,
![](https://i.imgur.com/uIBlVe3.png =500x)

The following table shows the use of the two sampling rates:
| Sampling<br>Rate | Use |
| :---: | :--- |
| 8000 Hz | Telephone and encrypted walkie-talkie, wireless intercom and wireless microphone transmission; adequate for human speech but without sibilance. |
| 44100 Hz | Audio CD, also most commonly used with MPEG-1 audio (VCD, SVCD, MP3). Originally chosen by Sony because it could be recorded on modified video equipment running at either 25 frames per second (PAL) or 30 frame/s (using an NTSC monochrome video recorder) and cover the 20 kHz bandwidth thought necessary to match professional analog recording equipment of the time. A PCM adaptor would fit digital audio samples into the analog video channel of, for example, PAL video tapes using 3 samples per line, 588 lines per frame, 25 frames per second. |

### Input Wav Info
![](https://i.imgur.com/2zx8tpk.png)

![](https://i.imgur.com/9OC6rZq.png)

|  | Hex | Dec | 
| --- | :---: | --- |
| ChunkSize | 01 31 4F 44 | <font color = #F28C06>20008772 bytes</font> |
| Subchunk1Size | 00 00 00 10 | 16 bytes |
| AudioFormat | 00 01 | 1 -> PCM (i.e. Linear quantization) |
| NumChannels | 00 02 | 2 -> Stereo |
| SampleRate | 00 00 44 AC | <font color = #03CF1F>44100 Hz </font>|
| ByteRate | 00 02 B1 10 | 176400 |
| BlockAlign | 00 04 | 4 |
| BitsPerSample | 00 10 | 16 bits |
| Subchunk2Size | 01 31 4F 20 | <font color = #9803CF>20008736 bytes</font> |

Total file size: <font color = "red"> 20008780 bytes </font>
= <font color = #F28C06>20008772 bytes</font> + 8 (bytes of "RIFF") 
= <font color = #9803CF>20008736 bytes</font> + 44 (bytes of Wavheader)


## Code Demonstration
The final code includes three parts:
1. main.cpp
2. Wav.h
3. Vector_Operation.h

### Stereo Wav Structure
Defined in Wav.h
```cpp
typedef struct WaveHeader
{
    // riff wave header
    char ChunkID[4] = {'R','I','F','F'};
    unsigned ChunkSize;        // 0 ~ FFFF,FFFF
    char Format[4] = {'W','A','V','E'};
    
    // fmt subchunk
    char SubChunk1ID[4] = {'f','m','t',' '};
    unsigned SubChunk1Size;    // 0 ~ FFFF,FFFF
    unsigned short AudioFormat;    // 0 ~ FFFF
    unsigned short NumChannels;    // 0 ~ FFFF
    unsigned SampleRate;       // 0 ~ FFFF,FFFF
    unsigned ByteRate;         // 0 ~ FFFF,FFFF
    unsigned short BlockAlign;     // 0 ~ FFFF
    unsigned short BitsPerSample;  // 0 ~ FFFF
    
    // data subchunk
    char SubChunk2ID[4] = {'d','a','t','a'};
    unsigned SubChunk2Size;    // 0 ~ FFFF,FFFF
} WaveHeader;

typedef struct Stereo_Wav
{
    WaveHeader header;
    vector<short> left_data;
    vector<short> right_data;
} Stereo_Wav;

WaveHeader make_WaveHeader(unsigned short const NumChannels, unsigned const SampleRate, unsigned short const BitsPerSample, long NoS)
{
    WaveHeader myWH;

    myWH.AudioFormat = 1;                  // 1 for PCM...
    myWH.SampleRate = SampleRate;
    myWH.NumChannels = NumChannels;        // 1 for Mono, 2 for Stereo
    myWH.BitsPerSample = BitsPerSample;
    myWH.ByteRate = (myWH.SampleRate * myWH.NumChannels * myWH.BitsPerSample)/8;
    myWH.BlockAlign = (myWH.NumChannels * myWH.BitsPerSample)/8;
    myWH.SubChunk2Size = (NoS * myWH.NumChannels * myWH.BitsPerSample)/8;
    myWH.SubChunk1Size = 16;               // 16 for PCM
    myWH.ChunkSize = 4+(8+myWH.SubChunk1Size)+(8+myWH.SubChunk2Size);

    return myWH;
}
```
### Wavfile Operation
Defined in Wav.h
```cpp
Stereo_Wav WaveRead(string filename)
{
    ifstream infile;
    WaveHeader header;
    vector<short> data;
    Stereo_Wav wavein;
    
    infile.open(filename, ofstream::binary|ios::in);
    if (!infile.is_open())
    {
        cerr << "Could not open the file" << endl;
        system("pause");
    }
    
    infile.read((char *) &header, sizeof(header));
    //cout << header.SampleRate << endl;
    wavein.header = header;

    while(!infile.eof())
    {
        short temp;        // data can't be greater than FFFF(65535).
        infile.read((char *) &temp, sizeof(temp));
        data.push_back(temp);
    }
    infile.close();

    /*------------------------------------*/
    /* Change data length here for testing*/
    for(unsigned i=0;i<data.size();i=i+2)
    {
        wavein.left_data.push_back(data[i]);
        wavein.right_data.push_back(data[i+1]);
    }
    
    return wavein;
}

void WaveWrite(string filename, Stereo_Wav waveout)
{
    ofstream outfile;
    outfile.open(filename, ofstream::binary|ofstream::out);
    if (!outfile.is_open())
    {
        cerr << "Could not open the file" << endl;
        system("pause");
    }
    
    outfile.write((char*)&waveout.header, sizeof(waveout.header));
    
    for(unsigned i=0;i<waveout.left_data.size();i++)
    {
        outfile.write((char*)&waveout.left_data[i], sizeof(waveout.left_data[i]));
        outfile.write((char*)&waveout.right_data[i], sizeof(waveout.right_data[i]));
    }
}
```
In order to check the correctness of readin file, the debug session shows:
```
data_left.size() : 5002184, datatype = <short>
```
Whereas, a short is 4 btyes : 5002184 x 4 = 20008736 bytes in total, so 
20008780 - 20008736 = 44 bytes = Wavheader byte size

### Resample
Defined in Vector_Operation.h
```cpp
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
```
### Convolution
Defined in Vector_Operation.h
```cpp
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
```
### Low-pass filter
Defined in main.cpp
```cpp
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
```
### Main Procedure
```cpp
int main()
{
    string infname = "input.wav";
    string outfname = "output.wav";
    Stereo_Wav wavein = WaveRead(infname);
    Stereo_Wav waveout;
    
    int M = 441, L = 80;
    vector<short> x_e, x_i;
    vector<double> h_n;
    
    time_t start = time(NULL);
    /*-----------Left Data-----------*/
    cout << "--------Left Data---------" << endl;
    x_e = resample(wavein.left_data, 5, "up");
    h_n = lpfilter(L, pi/M);
    x_i = conv(x_e, h_n, "same"); x_e.clear();
    waveout.left_data = resample(x_i, 7, "down"); x_i.clear()
    
    
    /*------------Right Data-------------*/
    cout << "--------Right Data---------" << endl;
    x_e = resample(wavein.right_data, 5, "up");
    h_n = lpfilter(L, pi/M);
    x_i = conv(x_e, h_n, "same"); x_e.clear();
    waveout.right_data = resample(x_i, 7, "down"); x_i.clear();
    
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
```
## Results
### Output Wav Info
![](https://i.imgur.com/abOb3si.png)

![](https://i.imgur.com/x65KH9D.png)
|  | Hex | Dec | 
| --- | :---: | --- |
| ChunkSize | 00 37 62 AC | <font color = #F28C06>3629740 bytes</font> |
| Subchunk1Size | 00 00 00 10 | 16 bytes |
| AudioFormat | 00 01 | 1 -> PCM (i.e. Linear quantization) |
| NumChannels | 00 02 | 2 -> Stereo |
| SampleRate | 00 00 1F 40 | <font color = #03CF1F>8000 Hz</font> |
| ByteRate | 00 00 7D 00 | 32000 |
| BlockAlign | 00 04 | 4 |
| BitsPerSample | 00 10 | 16 bits |
| Subchunk2Size | 00 37 62 88 | <font color = #9803CF>3629704 bytes</font> |

Total file size: <font color = "red"> 20008780 bytes </font>
= <font color = #F28C06>20008772 bytes</font> + 8 (bytes of "RIFF") 
= <font color = #9803CF>20008736 bytes</font> + 44 (bytes of Wavheader)

However, using convolution method with directly upsampling 80 and down downsampling 441 is an inefficient choice.
It took 33900(s) = 9.41(hr) to complete the task.


## Discussion
### Optimization by Multistage

Since all system are linear, we can shift the filter or resampling with random order, and then reconstruct them to other form.

![](https://i.imgur.com/i0RpLdd.png)

After each upsampling, however, the energy has been distributed. Thus, it is neccesary to give gain in the filter.

```cpp
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
```

The efficiency has improved by a lot.
It took 9554(s) = 2.65(hr) to complete the task.