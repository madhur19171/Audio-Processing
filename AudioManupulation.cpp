#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include "WavIO.h"
using namespace std;
class Complex
{
public:
    double real, imag, mag, phase, pi;
    Complex()
    {
        pi = 2 * acos(0.0);
        real = 0;
        imag = 0;
        mag = real == 0 ? 0 : sqrt(real * real + imag * imag);
        phase = real == 0 ? 0 : atan(imag / real);
    }
    Complex(double r, double i)
    {
        pi = 2 * acos(0.0);
        real = r;
        imag = i;
        mag = real == 0 ? 0 : sqrt(real * real + imag * imag);
        phase = real == 0 ? 0 : atan(real / imag);
    }
    void polar(double r, double p)
    {
        real = r * cos(p);
        imag = r * sin(p);
        mag = r;
        phase = p;
    }

    Complex conj()
    {
        return Complex(real, -1 * imag);
    }

    static Complex add(Complex a, Complex b)
    {
        return Complex(a.real + b.real, a.imag + b.imag);
    }
    static Complex sub(Complex a, Complex b)
    {
        return Complex(a.real - b.real, a.imag - b.imag);
    }
    static Complex mul(Complex a, Complex b)
    {
        return Complex((a.real * b.real - a.imag * b.imag), (a.real * b.imag + a.imag + b.real));
    }
    static Complex div(Complex a, Complex b)
    {
        Complex num = mul(a, (Complex(b.real, -1 * b.imag)));
        double den = b.mag * b.mag;
        return Complex(num.real / den, num.imag / den);
    }
    static Complex exp(Complex a, int x)
    {
        Complex temp = Complex();
        temp.polar(pow(a.mag, x), a.phase * x);
        return temp;
    }
    static double dot(Complex a, Complex b)
    {
        return a.real * b.real + a.imag * b.imag;
    }
    static Complex dot(vector<Complex> a, vector<Complex> b)
    {
        Complex sum = Complex(0, 0);
        for (int i = 0; i < a.size(); i++)
        {
            sum = Complex::add(sum, Complex::mul(b[i], a[i].conj()));
        }
        return sum;
    }
};

class Wave
{
public:
    int N;
    Complex W;
    double pi = 2 * acos(0.0);
    vector<vector<Complex>> basis;
    vector<Complex> wave;
    vector<Complex> RDFT;
    vector<double> RiDFT;
    Wave(int x)
    {
        N = x;
        W = Complex();
        W.polar(1, 2 * pi / N);
        for (int i = 0; i < N; i++)
        {
            vector<Complex> base;
            for (int j = 0; j < N; j++)
            {
                base.push_back(Complex::exp(W, i * j));
            }
            basis.push_back(base);
        }
    }
    vector<vector<Complex>> getBasis()
    {
        return basis;
    }
    void setWave(vector<double> wa)
    {
        wave.clear();
        for (int i = 0; i < wa.size(); i++)
        {
            wave.push_back(Complex(wa[i], 0.0));
        }
    }
    void setWave(vector<short> wa)
    {
        wave.clear();
        for (int i = 0; i < wa.size(); i++)
        {
            wave.push_back(Complex((double)wa[i], 0.0));
        }
    }
    void computeRDFT()
    {
        RDFT.clear();
        for (int i = 0; i <= N / 2; i++)
        {
            RDFT.push_back(Complex::dot(basis[i], wave));
        }
    }
    void computeRiDFT()
    {
        RiDFT.clear();
        Complex sum;
        Complex x;
        if (N % 2 == 0)
        {
            for (int i = 0; i < N; i++)
            {
                sum = Complex(0.0, 0.0);
                for (int j = 0; j < N / 2; j++)
                {
                    x = Complex::mul(basis[j][i], RDFT[j]);
                    sum = Complex::add(sum, x);
                }
                for (int j = N / 2; j < N; j++)
                {
                    x = Complex::mul(basis[j][i], RDFT[N - j].conj());
                    sum = Complex::add(sum, x);
                }
                RiDFT.push_back(sum.real / N);
            }
        }
        else
        {
            /* code */
            for (int i = 0; i < N; i++)
            {
                sum = Complex(0.0, 0.0);
                for (int j = 0; j <= N / 2; j++)
                {
                    x = Complex::mul(basis[j][i], RDFT[j]);
                    sum = Complex::add(sum, x);
                }
                for (int j = N / 2; j < N; j++)
                {
                    x = Complex::mul(basis[j][i], RDFT[N - j].conj());
                    sum = Complex::add(sum, x);
                }
                RiDFT.push_back(sum.real / N);
            }
        }
    }
    vector<short> shortiDFT()
    {
        vector<short> ret;
        for (int i = 0; i < RiDFT.size(); i++)
        {
            ret.push_back((short)RiDFT[i]);
        }
        return ret;
    }
    void printDFT()
    {
        printf("Real:\t%d\t", RDFT.size());
        for (int i = 0; i <= N / 2; i++)
        {
            printf("%f, ", RDFT[i].real);
        }
        printf("\nImaginary\t");
        for (int i = 0; i <= N / 2; i++)
        {
            printf("%f, ", RDFT[i].imag);
        }
    }
    void printiDFT()
    {
        for (int i = 0; i < N; i++)
        {
            printf("%f, ", RiDFT[i]);
        }
    }
    void rightShiftDFT(int x)
    {
        Complex co = Complex(0, 0);
        for (int i = RDFT.size() - 1 - x; i >= 0; i--)
        {
            RDFT[i + x] = RDFT[i];
        }
        for (int i = 0; i < x; i++)
        {
            RDFT[i] = co;
        }
    }
};

int main()
{
    /*double rate = 500;
    double frequency = 100;
    double length = 1;
    double pi = 2 * acos(0.0);

    vector<double> sine;
    for (int j = 1; j < length + 1; j++)
        for (int i = 0; i < rate; i++)
            sine.push_back((10000 * sin(2.0 * pi * frequency * (i / rate))));

    for (int i = 0; i < sine.size(); i++)
    {
        printf("%f, ", sine[i]);
    }
    printf("\n\n");

    Wave obj(rate * length);

    //printf("\n\n");
    obj.setWave(sine);
    obj.computeRDFT();
    obj.printDFT();
    obj.rightShiftDFT(100);
    printf("\n\n");

    obj.computeRiDFT();
    obj.printiDFT();
    printf("%d \n", obj.RiDFT.size());
    char name[] = "changedfreq2.wav";
    WaveIO out(name, 1, rate, 16, obj.shortiDFT());
    */

    char name[] = "recording1.wav";
    WaveIO obj(name);
    obj.readWav();
    int fft = 200;
    int length = (((obj.DataLength / 2) / obj.SamplingRate) * obj.SamplingRate) / fft;
    printf("%d", length);
    Wave audio(fft);
    vector<short> recorded;
    for (int i = 0; i < length; i++)
    {
        vector<short> lis;
        for(int j = i * fft; j < i * fft + fft; j++)
        {
            lis.push_back(obj.data[j]);
        }
        audio.setWave(lis);
        audio.computeRDFT();
        audio.rightShiftDFT(5);
        audio.computeRiDFT();
        vector<short> out = audio.shortiDFT();
        for (int i = 0; i < out.size(); i++)
        {
            recorded.push_back(out[i]);
        }
        printf("%d ", recorded.size());
    }
    printf("%d", recorded.size());
    char name3[] = "recorded1.wav";
    WaveIO obj3(name3, 1, obj.SamplingRate, obj.BitsPerSample, recorded);
    
}