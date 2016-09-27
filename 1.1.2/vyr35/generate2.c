#include <stdio.h>

int
main ()
{
  unsigned int i;
  FILE *file;
  file = fopen ("plot1", "w");
  fprintf (file, "set term pos eps enh 12\n");
  fprintf (file, "set size 0.47,0.47\n");
  fprintf (file, "set key left\n");
  fprintf (file, "set xlabel 'x (m)'\n");
  fprintf (file, "set ylabel 'z (m)'\n");
  fprintf (file, "f(x)=0.4403*x+6.447e-3*x**2-3.969e-3*x**3+8.113e-5*x**4\n");
  fprintf (file, "set title 'D_{eq}<1 mm'\n");
  fprintf (file, "set out 'vyr35-o0-1.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file, "\t'vyr35-o%u' u ($9<0.001?$2:1/0):4 ev 10 not w d,\\\n", i + 1);
  fprintf (file, "\t'vyr35-o%u' u ($9<0.001?$2:1/0):4 ev 10 not w d\n", i + 1);
  fprintf (file, "set title '1 mm<D_{eq}<2 mm'\n");
  fprintf (file, "set out 'vyr35-o1-2.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-o%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-o%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '2 mm<D_{eq}<3 mm'\n");
  fprintf (file, "set out 'vyr35-o2-3.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-o%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-o%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '3 mm<D_{eq}<4 mm'\n");
  fprintf (file, "set out 'vyr35-o3-4.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-o%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-o%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '4 mm<D_{eq}<5 mm'\n");
  fprintf (file, "set out 'vyr35-o4-5.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-o%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-o%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '5 mm<D_{eq}'\n");
  fprintf (file, "set out 'vyr35-o5-.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file, "\t'vyr35-o%u' u ($9<0.005?1/0:$2):4 ev 10 not w d,\\\n", i + 1);
  fprintf (file, "\t'vyr35-o%u' u ($9<0.005?1/0:$2):4 ev 10 not w d\n", i + 1);
  fprintf (file, "set title 'D_{eq}<1 mm'\n");
  fprintf (file, "set out 'vyr35-s0-1.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file, "\t'vyr35-s%u' u ($9<0.001?$2:1/0):4 ev 10 not w d,\\\n", i + 1);
  fprintf (file, "\t'vyr35-s%u' u ($9<0.001?$2:1/0):4 ev 10 not w d\n", i + 1);
  fprintf (file, "set title '1 mm<D_{eq}<2 mm'\n");
  fprintf (file, "set out 'vyr35-s1-2.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-s%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-s%u' u ($9<0.001?1/0:$9<0.002?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '2 mm<D_{eq}<3 mm'\n");
  fprintf (file, "set out 'vyr35-s2-3.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-s%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-s%u' u ($9<0.002?1/0:$9<0.003?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '3 mm<D_{eq}<4 mm'\n");
  fprintf (file, "set out 'vyr35-s3-4.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-s%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-s%u' u ($9<0.003?1/0:$9<0.004?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '4 mm<D_{eq}<5 mm'\n");
  fprintf (file, "set out 'vyr35-s4-5.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file,
             "\t'vyr35-s%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 ev 10 not w d,\\\n",
             i + 1);
  fprintf (file, "\t'vyr35-s%u' u ($9<0.004?1/0:$9<0.005?$2:1/0):4 ev 10 not w d\n",
           i + 1);
  fprintf (file, "set title '5 mm<D_{eq}'\n");
  fprintf (file, "set out 'vyr35-s5-.eps'\n");
  fprintf (file, "plot [0:14][-1.35:2.5] f(x) t'Main jet' w l,\\\n");
  for (i = 0; i < 2457; ++i)
    fprintf (file, "\t'vyr35-s%u' u ($9<0.005?1/0:$2):4 ev 10 not w d,\\\n", i + 1);
  fprintf (file, "\t'vyr35-s%u' u ($9<0.005?1/0:$2):4 ev 10 not w d\n", i + 1);
  fclose (file);
}
