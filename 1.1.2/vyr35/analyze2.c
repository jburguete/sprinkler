#include <stdio.h>
#include <math.h>

#define NX 8
#define NY 4
#define ND 5
#define NT NX * NY * ND
#define FACTOR 0.25

void
show_error (char *message)
{
  printf ("ERROR! %s\n", message);
}

int
main (int argn, char **argc)
{
  double xm[NX] = { 1.5, 3., 4.5, 6., 7.5, 9., 10.5, 12.5 };
  double ym[NY] = { -1.35, 0., 0.9, 1.4 };
  double dm[ND] = { 0., 0.001, 0.002, 0.003, 0.004 };
  double vx[NT], vy[NT];
  double x[7];
  unsigned int i, j, k, l, n[NT];
  char *filename[ND] = {
    "Gotas0.txt",
    "Gotas1.txt",
    "Gotas2.txt",
    "Gotas3.txt",
    "Gotas4.txt"
  };
  char msg[512];
  FILE *filein, *fileout[ND];
  for (i = 0; i < NT; ++i)
    {
      vx[i] = vy[i] = 0.;
      n[i] = 0;
    }
  filein = fopen (argc[1], "r");
  if (!filein)
    {
      show_error ("Unable to open the DatosGotas2.txt file\n");
      return 1;
    }
  while (fscanf (filein, "%lf%lf%lf%lf%lf%lf%lf",
                 x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6) == 7)
    {
      for (i = 0; i < NX; ++i)
        if (x[0] == xm[i])
          break;
      if (i == NX)
        {
          snprintf (msg, 512, "Bad x=%lg\n", x[0]);
          show_error (msg);
          return 1;
        }
      for (j = 0; j < NY; ++j)
        if (x[2] == ym[j])
          break;
      if (j == NY)
        {
          snprintf (msg, 512, "Bad y=%lg\n", x[2]);
          show_error (msg);
          return 1;
        }
      for (k = ND; --k > 0;)
        if (x[3] > dm[k])
          break;
      i += NX * (j + NY * k);
      vx[i] += x[4];
      vy[i] += x[6];
      ++n[i];
    }
  for (i = 0; i < NT; ++i)
    {
      vx[i] *= FACTOR / n[i];
      vy[i] *= FACTOR / n[i];
    }
  for (l = k = 0; k < ND; ++k)
    {
	  snprintf (msg, 512, "%s-%s", argc[1], filename[k]);
      fileout[k] = fopen (msg, "w");
      for (j = 0; j < NY; ++j)
        for (i = 0; i < NX; ++i, ++l)
          if (n[l])
            fprintf (fileout[k], "%lg %lg %lg %lg\n",
                     xm[i], ym[j], vx[l], vy[l]);
      fclose (fileout[k]);
    }
  return 0;
}
