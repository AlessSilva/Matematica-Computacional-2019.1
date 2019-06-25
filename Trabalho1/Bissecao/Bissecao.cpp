#include <bits/stdc++.h>
using namespace std;

double f( double x )
{
    double y = (pow(x,5)) - (3*pow(x,4)) - (3*pow(x,2)) + 2;
    return y;
}

double MetodoBissecaoRecursivo( double a, double b, double e )
{
    double fa = f(a);
    double fb = f(b);
    double c = (a+b) / 2;
    double fc = f(c);
    
    cout << " a : " << a << " b: " <<b <<" c: " << c << " fc: " << fc << " b-a: " << abs(b-a) << "\n";
    
    if( fc == 0.0 or ( abs(fc) < e and abs(a-b) < e ) )
    {
         return c;
    }
    if( fa*fc < 0 )
    {
        return MetodoBissecaoRecursivo(a,c,e);
    }

    return MetodoBissecaoRecursivo(c,b,e);
}

int main(){
	double e = pow(2,-5);
    double a = 1.0;
    double b = 4.0;

    cout.precision(10);
    cout << "Método da Bisseção" << "\n";
    cout << MetodoBissecaoRecursivo(a,b,e);
}
