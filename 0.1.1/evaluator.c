#include <stdio.h>
#include <stdlib.h>

double
interpolate (double x, double x1, double x2, double z1, double z2)
{
  return z1 + (x - x1) * (z2 - z1) / (x2 - x1);
}

int
main (int argn, char **argc)
{
  char buffer[512];
  FILE *file;
  double *x, *z, t[9], e, k;
  unsigned int i1, i2, i3, n;
  snprintf (buffer, 512, "%s-vyr", argc[1]);
  file = fopen (buffer, "r");
  n = 0;
  x = z = NULL;
  while (fscanf (file, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", t, t + 1, t + 2, t + 3,
			     t + 4, t + 5, t + 6, t + 7, t + 8) == 9)
	{
	  x = realloc (x, (n + 1) * sizeof (double));
	  z = realloc (z, (n + 1) * sizeof (double));
	  x[n] = t[1];
	  z[n] = t[3];
	  ++n;
	}
  fclose (file);
printf ("n=%u\n", n);
  file = fopen (argc[2], "r");
  for (e = 0.; fscanf (file, "%lf%lf", t, t + 1) == 2;)
	{
	  t[0] *= 0.01;
	  t[1] *= 0.01;
	  if (t[0] < x[0]) k = z[0];
	  else if (t[0] > x[n - 1]) k = z[n - 1];
	  else
		{
          for (i1 = 0, i3 = n - 1; i3 - i1 > 1;)
			{
			  i2 = (i3 + i1) / 2;
			  if (t[0] < x[i2])
				i3 = i2;
			  else
				i1 = i2;
			}
		  k = interpolate (t[0], x[i1], x[i3], z[i1], z[i3]);
		}
	  k -= t[1];
	  e += k * k;
	}
  fclose (file);
  file = fopen (argc[3], "w");
  fprintf (file, "%.14le", e);
  fclose (file);
  free (z);
  free (x);
}
