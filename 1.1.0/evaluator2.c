#include <stdio.h>

int
main (int argn, char **argc)
{
  char buffer[512];
  FILE *file;
  double t[9], e;
  snprintf (buffer, 512, "%s-vyr", argc[1]);
  file = fopen (buffer, "r");
  while (fscanf (file, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", t, t + 1, t + 2, t + 3,
                 t + 4, t + 5, t + 6, t + 7, t + 8) == 9);
  fclose (file);
  file = fopen (argc[3], "w");
  e = t[1] - 13.49;
  fprintf (file, "%.14le", e * e);
  fclose (file);
}

