/**
 * \file drops.c
 * \brief File to calculate the trajectory of a random drops set with the
 *   ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * \def DROP_MAX
 * \brief Macro to define the maximum drop diameter size.
 * \def DROP_MIN
 * \brief Macro to define the minimum drop diameter size.
 * \def RANDOM_SEED
 * \brief Macro to define the default pseudo-random numbers generator seed.
 * \def DT
 * \brief Macro to define the default time step size of the numerical method.
 */
#define DROP_MAX 0.007
#define DROP_MIN 0.0005
#define RANDOM_SEED 7
#define DT 0.001

/**
 * \struct Data
 * \brief Struct to define the drops set data.
 */
typedef struct
{
    /**
	 * \var sprinkler_height
	 * \brief Sprinkler height.
	 * \var sprinkler_pressure
	 * \brief Sprinkler pressure.
	 * \var sprinkler_angle
	 * \brief Angle of the the sprinkler nozzle with respect to the ground.
	 * \var wind_velocity
	 * \brief Wind velocity.
	 * \var wind_angle
	 * \brief Horizontal wind angle.
	 * \var jet_length
	 * \brief Jet length.
	 * \var air_temperature
	 * \brief Air temperature in Celsius degrees.
	 * \var air_humidity
	 * \brief Relative air humidity.
	 * \var air_pressure
	 * \brief Air pressure.
	 * \var drops_number
	 * \brief Number of drops.
	 */
  double sprinkler_height, sprinkler_pressure, sprinkler_angle, wind_velocity,
    wind_angle, jet_length, air_temperature, air_humidity, air_pressure;
  unsigned int drops_number;
} Data;

/**
 * \fn int data_read (Data *data, char *file_name)
 * \brief Function to read the drops set data in a file.
 * \param data
 * \brief Drops set data.
 * \param file_name
 * \brief Input file name.
 * \return 1 on success, 0 on error.
 */
int
data_read (Data * data, char *file_name)
{
  FILE *file;
  file = fopen (file_name, "r");
  if (!file)
    return 0;
  if (fscanf (file, "%le%le%le%le%le%le%le%le%le%u",
              &data->sprinkler_height,
              &data->sprinkler_pressure,
              &data->sprinkler_angle,
              &data->jet_length,
              &data->wind_velocity,
              &data->wind_angle,
              &data->air_temperature,
              &data->air_humidity,
              &data->air_pressure, &data->drops_number) != 10)
    return 0;
  fclose (file);
  return 1;
}

int
main (int argn, char **argc)
{
  int i;
  double drop_diameter, jet_length, x[8], y[8];
  FILE *file, *file_output;
  Data data[1];
  srand (RANDOM_SEED);
  if (!data_read (data, argc[1]))
    return 1;
  file_output = fopen (argc[2], "w");
  for (i = 0; i < data->drops_number; ++i)
    {
      file = fopen ("input", "w");
      drop_diameter = DROP_MIN + (DROP_MAX - DROP_MIN) * rand ()
        / ((double) RAND_MAX);
      jet_length = data->jet_length * rand () / ((double) RAND_MAX);
      fprintf (file, "0 0 %le %le %le 0 %le 0 %le %le %le %le %le %le %le",
               data->sprinkler_height,
               data->sprinkler_pressure,
               data->sprinkler_angle,
               jet_length,
               data->wind_velocity,
               data->wind_angle,
               data->air_temperature,
               data->air_humidity, data->air_pressure, drop_diameter, DT);
      fclose (file);
      system ("./trajectory input output");
      file = fopen ("output", "r");
      while (fscanf (file, "%le%le%le%le%le%le%le%le",
                     x, x + 1, x + 2, x + 3, x + 4, x + 5, x + 6, x + 7) == 8)
        memcpy (y, x, 8 * sizeof (double));
      fclose (file);
      fprintf (file_output, "%le %le %le %le %le\n",
               jet_length, drop_diameter, y[1], y[4], y[6]);

    }
  fclose (file_output);
#ifdef WIN32
  system ("del input output");
#else
  system ("rm input output");
#endif
  return 0;
}
