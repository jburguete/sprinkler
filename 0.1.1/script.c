#include <stdio.h>

int main()
{
	FILE *filein, *fileout;
	char buffer[64];
	double x, d, v, a;
	filein = fopen("Nery20psi.csv", "r");
	fileout = fopen("Nery.xml", "w");
	fprintf(fileout, "<?xml version=\"1.0\"?>\n");
	fprintf(fileout, "<trajectory>\n");
	fprintf(fileout, "\t<air/>\n");
	while (fscanf(filein, "%s%s%s%s%s%s%s%lf%s%s%s%lf%s%lf%s%s%lf",
		buffer,
		buffer,
		buffer,
		buffer,
		buffer,
		buffer,
		buffer,
		&a,
		buffer,
		buffer,
		buffer,
		&d,
		buffer,
		&v,
		buffer,
		buffer,
		&x) == 17)
	{
		fprintf(fileout, "\t<drop x=\"%lg\" z=\"0\" velocity=\"%lg\" "
			"angle=\"%lg\" diameter=\"%lg\"/>\n",
			x,
			v,
			a,
			d);
	}
	fclose(filein);
	fprintf(fileout, "</trajectory>");
	fclose(fileout);
	return 0;
}
