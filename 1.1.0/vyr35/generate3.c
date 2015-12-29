#include <stdio.h>
#include <math.h>

#define AIR_MOLECULAR_MASS 0.028964     ///< dry air molecular mass.
#define KELVIN_TEMPERATURE 273.15       ///< Kelvin to Celsius degree.
#define R 8.314                 ///< thermodinamic constant.
#define WATER_MOLECULAR_MASS 0.018016   ///< water molecular mass.

double
saturation_pressure (double T)
{
  return exp (23.7836 - 3782.89 / (T - 42.850));
}

double
density (double T, double P, double H)
{
  return (AIR_MOLECULAR_MASS * P
          + (WATER_MOLECULAR_MASS - AIR_MOLECULAR_MASS)
		  * saturation_pressure (T) * 0.01 * H) / (R * T);
}

double
surface_tension (double T)
{
  double x;
  x = 1. - T / 647.15;
  return 2.358e-1 * pow (x, 1.256) * (1. - 0.625 * x);
}

double
Deq (double Dmax, double v)
{
  double f, g, T;
  T = 20. + KELVIN_TEMPERATURE;
  g = 0.1742 / 6. * density (T, 101325., 100.) * v * v / surface_tension (T)
    * Dmax;
  f = sqrt (1. + g * g * g);
  return Dmax * (pow(0.5 * (f + 1.), 1. / 3.) - pow (0.5 * (f - 1.), 1. / 3.));
}

int main()
{
  unsigned int i;
  double x[6];
  FILE *file1, *file2;
  file1 = fopen ("DatosGotas.txt", "r");
  file2 = fopen ("DatosGotas2.txt", "w");
  while (fscanf (file1, "%lf%lf%lf%lf%lf", x, x + 1, x + 2, x + 3, x + 4) == 5)
	{
	  x[5] = Deq (x[3] / 1000., x[4]);
	  fprintf (file2, "%lg %lg %lg %lg %lg %lg\n",
			   x[0], x[1], x[2], x[3] / 1000., x[4], x[5]);
	}
  fclose (file2);
  fclose (file1);
}
