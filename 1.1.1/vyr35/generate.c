#include <stdio.h>
#include <math.h>

#define DENSITY 1.17796
#define TENSION 0.07273984

/*
double f(double Deq, double Dmax, double v)
{
  double D3;
  D3 = Dmax * Dmax * Dmax;
  return (Deq * Deq + 0.1742 / 4. * DENSITY / TENSION * v * v * D3) * Deq - D3;
}

double Deq (double Dmax, double v)
{
  double d1, d2, d3;
  unsigned int i;
  d1 = 0.;
  d3 = Dmax;
  for (i = 0; i < 64; ++i)
  {
	  d2 = 0.5 * (d1 + d3);
	  if (f(d2, Dmax, v) < 0.) d1 = d2; else d3 = d2;
  }
  return d2;
}
*/

double
Deq (double Dmax, double v)
{
  double kWe;
  kWe = 0.1767 / 6. * DENSITY / TENSION * v * v * Dmax;
  kWe = sqrt (1. + kWe * kWe * kWe);
  return Dmax * (pow (0.5 * (1. + kWe), 1. / 3.)
                 - pow (0.5 * (kWe - 1), 1. / 3.));
}

int
main ()
{
  unsigned int i;
  double x[5];
  FILE *file1, *file2;
  file1 = fopen ("DatosGotas.txt", "r");
  file2 = fopen ("invert.xml", "w");
  fprintf (file2, "<?xml version=\"1.0\"?>\n");
  fprintf (file2, "<invert>\n");
  fprintf (file2, "\t<air/>\n");
  fprintf (file2, "\t<jet a1=\"0.4403\" a2=\"6.447e-3\" a3=\"-3.969e-3\" "
           "a4=\"8.113e-5\"/>\n");
  i = 0;
  while (fscanf (file1, "%lf%lf%lf%lf%lf", x, x + 1, x + 2, x + 3, x + 4) == 5)
    {
      x[3] = Deq (x[3] / 1000., x[4]);
      fprintf (file2, "\t<trajectory bed_level=\"-1.5\" dt=\"0.001\" "
               "cfl=\"0.1\" file=\"o%u\">\n", ++i);
      fprintf (file2, "\t\t<drop diameter=\"%.14le\" velocity=\"%.14le\" "
               "vertical_angle=\"%lg\" x=\"%lg\" z=\"%lg\" "
               "drag_model=\"ovoid\"/>\n", x[3], x[4], -x[2], x[1], x[0]);
      fprintf (file2, "\t</trajectory>\n");
      fprintf (file2, "\t<trajectory bed_level=\"-1.5\" dt=\"0.001\" "
               "cfl=\"0.1\" file=\"s%u\">\n", i);
      fprintf (file2, "\t\t<drop diameter=\"%.14le\" velocity=\"%.14le\" "
               "vertical_angle=\"%lg\" x=\"%lg\" z=\"%lg\" "
               "drag_model=\"sphere\"/>\n", x[3], x[4], -x[2], x[1], x[0]);
      fprintf (file2, "\t</trajectory>\n");
    }
  fprintf (file2, "</invert>\n");
  fclose (file2);
  fclose (file1);
}
