#include <iostream>
#include <cmath>
#include <vector>
const int GROUPNUMBER = 3;
const int MYNUMBER = 2;
const float a = 0;
const float b = 1.3;
const float yFirst = M_E;
using namespace std;
vector<pair<float, float>> firstTwentyH;
vector<pair<float, float>> firstTwentyHalfH;
vector<pair<float, float>> lastTwentyH;
vector<pair<float, float>> lastTwentyHalfH;
float u(float x)
{
    return expf(cosf(powf(x, GROUPNUMBER+0.05*MYNUMBER)));
}
float f(float x, float u)
{
    return -(GROUPNUMBER + 0.05*MYNUMBER)*powf(x, GROUPNUMBER-1+0.05*
                                               MYNUMBER)*u*sinf(powf(x, GROUPNUMBER+0.05*MYNUMBER));
}
float getY(float x, float y, float h)
{
    float k1 = h*f(x,y);
    float k2 = h*f(x+h/2, y+k1/2);
    float k3 = h*f(x+h/2, y+k2/2);
    float k4 = h*f(x+h, y+k3);
    return y + (k1 + 2*k2 + 2*k3 + k4) / 6.;
}
void approximateByRungeKutta(vector<pair<float, float>>&
                             approximation, float h, float eps)
{
    firstTwentyH.clear();
    firstTwentyHalfH.clear();
    lastTwentyH.clear();
    lastTwentyHalfH.clear();
    float x = a;
    while (x <= b && h != 0)
    {
        int iter = 0;
        while (iter < 5)
        {
            ++iter;
            float y1, y2, delta;
            y1 = getY(approximation.back().first, approximation.back().
                      second, h);
            float tempY = getY(approximation.back().first, approximation.
                               back().second, h/2);
            y2 = getY(approximation.back().first + h/2, tempY, h/2);
            float err = fabsf(y1-y2)/15.;
            delta = powf(eps/fabsf(err), 0.2);
            if (delta >= 1)
            {
                approximation.push_back(make_pair(x + h, y1));
                if (approximation.size() <= 21)
                {
                    firstTwentyH.push_back(make_pair(x + h, y1));
                    firstTwentyHalfH.push_back(make_pair(x + h, y2));
                }
                else if (approximation.size() >= 2583)
                {
                    lastTwentyH.push_back(make_pair(x + h, y1));
                    lastTwentyHalfH.push_back(make_pair(x + h, y2));
                }
                x += h;
                iter = 5;
            }
            else
            {
                h = 0.9*delta*h/2;
            }
            if(x + h > b)
            {
                h = b - x;
            }
        }
    }
}
void approximateByAdams(vector<pair<float, float>>& approximation,
                        float h)
{
    float x = approximation.back().first;
    int N = 3;
    while (x <= b)
    {
        float A = 1901*f(approximation[N].first, approximation[N].second);
        float B = -2774*f(approximation[N-1].first, approximation[N-1].
                          second);
        float C = 2616*f(approximation[N-2].first, approximation[N-2].second
                         );
        float D = -1274*f(approximation[N-3].first, approximation[N-3].
                          second);
        float E = 251*f(approximation[N-4].first, approximation[N-4].second)
        ;
        float nextY = approximation[N].second + h*(A + B + C + D + E)/720.0;
        approximation.push_back(make_pair(x + h, nextY));
        ++N;
        x += h;
    }
}
int main(int argc, const char * argv[])
{
    vector<pair<float, float>> approximation;
    approximation.push_back(make_pair(a, yFirst));
    float h = 0.2;
    float eps = 0.001;
    approximateByRungeKutta(approximation, h, eps);
    cout << "Approximation by Runge-Kutta:\n";
    for(auto it:approximation)
        cout << it.first << " " << it.second << " " << u(it.first) <<
        endl;
    cout << endl;
    cout << "For Runge-Kutta graphic -lg(errMax), lg(N), N:\n";
    float maxErr = 0;
    for(int i = 1; i <= 5; ++i)
    {
        approximation.clear();
        approximation.push_back(make_pair(a, yFirst));
        h = powf(0.5, i/5.)*0.001;
        //cout << h << "IM HEE";
        approximateByRungeKutta(approximation, h, eps);
        maxErr = 0;
        for(auto it:approximation)
        {
            float tempErr = fabsf(it.second - u(it.first));
            if (tempErr > maxErr)
            {
                maxErr = tempErr;
            }
        }
        cout << maxErr << endl;
        cout/* << "-lg(errMax): " */<< -logf(maxErr) << ", ";
        cout/* << " lg(N): " */<< logf((approximation.size()-1)*4) <<
        ", ";
        cout/* << "N: " << approximation.size()-1 */<< endl;
    }
    cout << endl;
    // h = 0.2;
    // approximation.clear();
    // approximateByRungeKutta(approximation, h, eps);
     cout << "For h = " << h << " N = " << approximation.size() - 1 <<
    " maxErr = " << maxErr << endl;
     cout << "h" << " " << "|u(x)-y(xn)|" << " " << "|(u(x) - y(xn))/u(x)|" << endl;
    // 1 & 1 & 1 & 1 \\ \hline

     for(int i = 1; i <= 20; ++i)
     {
     cout << i << "&" << approximation[i].first - approximation[i-1].first <<
    "&";
     cout << fabsf(approximation[i].second -
    u(approximation[i].first)) << "&";
     cout << fabsf((approximation[i].second -
    u(approximation[i].first))/u(approximation[i].first)) << "\\\\ \\hline";
     cout << endl;
     }




     int N = (int)approximation.size();
     for(int i = 20; i > 0; --i)
     {
     cout << "N-" << i-1 << "&" << approximation[N-i].first - approximation[N-i-1].first
    << "&";
     cout << fabsf(approximation[N-i].second - u(approximation[N-i].first))
    << "&";
     cout << fabsf((approximation[N-i].second - u(approximation[N-i].first))/u(approximation[N-i].first))
    << "\\\\ \\hline";;
     cout << endl;
     }

    for(auto it: firstTwentyH) {
        cout << it.second << endl;
    }

    for(int i = 0; i < 20; ++i)
    {
        cout << i + 1 << "&"<< firstTwentyH[i].second << "&" << firstTwentyHalfH[i].second << "&" << fabsf(firstTwentyH[i].second - firstTwentyHalfH[i].second)/15 << "\\\\ \\hline" << endl;
    }


    for(int i = 0; i < 20; ++i)
    {
        cout << "N-" << 19 - i << "&"<< lastTwentyH[i].second << "&" << lastTwentyHalfH[i].second << "&" << fabsf(lastTwentyH[i].second - lastTwentyHalfH[i].second)/15 << "\\\\ \\hline" << endl;
    }

//     for(auto it:lastTwentyHalfH)
//     {
//     cout << it.second << endl;
//     }
//     for(int i = 0; i < 20; ++i)
//     {
//     cout << fabsf(lastTwentyH[i].second - lastTwentyHalfH[i].second)/15 << endl;
//     }
    h = 0.0005;
    approximateByRungeKutta(approximation, h, eps);
    vector<pair<float, float>> approximationByAdams;
    for(int i = 0; i < 4; ++i)
    {
        approximationByAdams.push_back(approximation[i]);
    }
    approximateByAdams(approximationByAdams, h);




    cout << "For Adams graphic -lg(errMax), lg(N), N:\n";
    for(int i = 1; i <= 5; ++i)
    {
        h = powf(0.5, i/5.)*0.001;
        approximationByAdams.clear();
        approximateByRungeKutta(approximation, h, eps);
        for(int i = 0; i < 4; ++i)
        {
            approximationByAdams.push_back(approximation[i]);
        }
        approximateByAdams(approximationByAdams, h);
        maxErr = 0;
        for(auto it:approximationByAdams)
        {
            float tempErr = fabsf(it.second - u(it.first));
            if (tempErr > maxErr)
            {
                maxErr = tempErr;
            }
        }
        cout/* << "-lg(errMax): " */<< -logf(maxErr) << ", ";
        cout/* << " lg(N): " */<< logf((approximationByAdams.size()-1)*
                                            4) << ", ";
        cout/* << "N: " << approximationByAdams.size()-1 */<< endl
        ;
    }
    cout << endl;
    for(int i = 0; i < 20; ++i)
    {
        cout << i+1 << "&" << approximationByAdams[i].second << "\\\\ \\hline" << endl;
    }

         for(int i = 20; i > 0; --i)
         {
             cout << "N-" << i-1 << "&"  << approximationByAdams[N-i].second << "\\\\ \\hline" << endl;

         }
//     for(auto it:approximationByAdams)
//     {
//     cout << it.first << " " << it.second << " " << u(it.first) <<
//    endl;
//     }
//     int N = approximationByAdams.size() - 1;
//     for(int i = 20; i > 0; --i)
//     {
//     cout << approximationByAdams[N-i].second << endl;
//     }
    cout << "N for Adams: " << approximationByAdams.size() - 1 <<
    endl;
    cout << "maxErr for Adams: " << maxErr << endl;
    return 0;
}
