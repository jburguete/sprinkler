#include <stdio.h>

int main()
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
	  fprintf (file2, "\t<trajectory bed_level=\"-1.5\" dt=\"0.001\" "
			   "cfl=\"0.1\" file=\"%u\">\n", ++i);
	  fprintf (file2, "\t\t<drop diameter=\"%.14le\" velocity=\"%.14le\" "
			   "vertical_angle=\"%lg\" x=\"%lg\" z=\"%lg\" "
			   "drag_model=\"ovoid\"/>\n",
			   x[3] / 1000., x[4], -x[2], x[1], x[0]);
	  fprintf (file2, "\t</trajectory>\n");
	}
  fprintf (file2, "</invert>\n");
  fclose (file2);
  fclose (file1);
}
