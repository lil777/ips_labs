#include <stdio.h>
#include <ctime>
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <chrono>

using namespace std::chrono;
duration<double> duration_s;
duration<double> duration_p;

// Число разбиений и пределы
const int N = 10000000;
const double a = 0;
const double b = 1;

// Подинтегральная функция
double fun(double x)
{
	return 4/(1 + x*x);
}

// Последовательная реализация метода прямоугольников
double serialFindIntegral(int n)
{
	double h = (b - a)/(n - 1);
	double S = 0.0;
	for (size_t i = 0; i < n; i++) {
		S += fun(i*h) + fun((i + 1)*h);
	}		
	return S*h/2.0;
}

// Параллельная реализация метода прямоугольников
double parallelFindIntegral(int n)
{
	double h = (b - a)/n;
	cilk::reducer_opadd<double> S(0.0);
	cilk_for(size_t i = 0; i < n; i++) {
		S += fun(i*h) + fun((i + 1)*h);
	}	
	return S->get_value()*h/2.0;
}

// Зависимость времени выполнения от числа итераций N
int timeCompare()
{
	double ans;
	high_resolution_clock::time_point ts1, ts2, tp1, tp2;
	printf("___Serial method___|___Parallel method___ \n\n");
	for (size_t i = 100; i < N*1000; i *= 10)
	{
		ts1 = high_resolution_clock::now();
		ans = serialFindIntegral(N);
		ts2 = high_resolution_clock::now();
		duration_s = (ts2 - ts1);

		tp1 = high_resolution_clock::now();
		ans = serialFindIntegral(N);
		tp2 = high_resolution_clock::now();
		duration_p = (tp2 - tp1);
		printf("      %lf     |     %lf \n", duration_s.count(), duration_p.count());
	}
	return 0;
}

int main()
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	double ans_s = serialFindIntegral(N);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration_s = (t2 - t1);
	printf("___Serial method___ \n");
	printf("Integral = %lf \n", ans_s);
	printf("Duration is: %lf  seconds \n \n", duration_s.count());

	t1 = high_resolution_clock::now();
	double ans_p = parallelFindIntegral(N);
	t2 = high_resolution_clock::now();
	duration_p = (t2 - t1);
	printf("___Parallel method___ \n");
	printf("Integral = %lf \n", ans_p);
	printf("Duration is: %lf  seconds \n \n", duration_p.count());

	printf("Boost = %lf\n", duration_s.count() / duration_p.count());

	timeCompare();
	return 0;
}