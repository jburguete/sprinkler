#include <stdio.h>

int main()
{
	unsigned int i;
	FILE *file;
	file = fopen ("plot1", "w");
	fprintf (file, "set term pos eps enh 12\n");
	fprintf (file, "set size 0.47,0.47\n");
	fprintf (file, "set xlabel 'x (m)'\n");
	fprintf (file, "set ylabel 'z (m)'\n");
	fprintf (file, "f(x)=0.4403*x+6.447e-3*x**2-3.969e-3*x**3+8.113e-5*x**4\n");
	fprintf (file, "set out 'o0-1.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t'o%u' u ($9<0.001?$2:1/0):4 not w d,\\\n", i + 1);
	fprintf (file, "\t'o%u' u ($9<0.001?$2:1/0):4 not w d\n", i + 1);
	fprintf (file, "set out 'o1-2.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t'o%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t'o%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 'o2-3.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t'o%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t'o%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 'o3-4.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t'o%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t'o%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 'o4-5.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t'o%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t'o%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 'o5-.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t'o%u' u ($9<0.005?1/0:$2):4 not w d,\\\n", i + 1);
	fprintf (file, "\t'o%u' u ($9<0.005?1/0:$2):4 not w d\n", i + 1);
	fprintf (file, "set out 's0-1.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t's%u' u ($9<0.001?$2:1/0):4 not w d,\\\n", i + 1);
	fprintf (file, "\t's%u' u ($9<0.001?$2:1/0):4 not w d\n", i + 1);
	fprintf (file, "set out 's1-2.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t's%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t's%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 's2-3.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t's%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t's%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 's3-4.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t's%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t's%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 's4-5.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t's%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 not w d,\\\n",
			   i + 1);
	fprintf (file, "\t's%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 not w d\n",
			   i + 1);
	fprintf (file, "set out 's5-.eps'\n");
	fprintf (file, "plot [0:][-1.35:3] f(x) t'Main jet' w l lt 2,\\\n");
	for (i = 0; i < 2457; ++i)
	  fprintf (file, "\t's%u' u ($9<0.005?1/0:$2):4 not w d,\\\n", i + 1);
	fprintf (file, "\t's%u' u ($9<0.005?1/0:$2):4 not w d\n", i + 1);
	fclose (file);	
}
