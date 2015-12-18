/**
 * \file trajectory.c
 * \brief File to calculate the trajectory of a drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#include <stdio.h>
#include <math.h>
#include "drop.h"

/**
 * \struct Sprinkler
 * \brief struct to define a sprinkler.
 */
typedef struct
{
    /**
	 * \var x
	 * \brief position x component.
	 * \var y
	 * \brief position y component.
	 * \var z
	 * \brief position z component.
	 * \var pressure
	 * \brief water pressure.
	 * \var vertical_angle
	 * \brief vertical angle.
	 * \var horizontal_angle
	 * \brief horizontal angle.
	 * \var jet_length
	 * \brief length of the exiting jet.
	 * \var bed_level
	 * \brief bed level.
	 */
  double x, y, z, pressure, vertical_angle, horizontal_angle, jet_length,
    bed_level;
} Sprinkler;

/**
 * \struct Drop
 * \brief struct to define a drop.
 */
typedef struct
{
    /**
	 * \var diameter
	 * \brief drop diameter.
	 * \var density
	 * \brief drop density.
	 */
  double diameter, density;
} Drop;

/**
 * \struct Trajectory
 * \brief struct to define a drop trajectory.
 */
typedef struct
{
    /**
	 * \var x
	 * \brief position x component.
	 * \var y
	 * \brief position y component.
	 * \var z
	 * \brief position z component.
	 * \var t
	 * \brief time.
	 * \var vx
	 * \brief velocity x component.
	 * \var vy
	 * \brief velocity y component.
	 * \var vz
	 * \brief velocity z component.
	 * \var ax
	 * \brief acceleration x component.
	 * \var ay
	 * \brief acceleration y component.
	 * \var az
	 * \brief acceleration z component.
	 * \var dt
	 * \brief numerical time step size.
	 * \var drag
	 * \brief drag resistence factor.
	 */
  double x, y, z, t, vx, vy, vz, ax, ay, az, dt, drag;
} Trajectory;

/**
 * \fn void sprinkler_open(Sprinkler *s, FILE *file)
 * \brief function to open a sprinkler struct in a file.
 * \param s
 * \brief sprinkler struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
sprinkler_open (Sprinkler * s, FILE * file)
{
  return fscanf (file, "%le%le%le%le%le%le%le%le",
                 &(s->x),
                 &(s->y),
                 &(s->z),
                 &(s->pressure),
                 &(s->vertical_angle),
                 &(s->horizontal_angle),
                 &(s->jet_length), &(s->bed_level)) == 8;
}

/**
 * \fn void sprinkler_init(Sprinkler *s)
 * \brief function to input a sprinkler struct.
 * \param s
 * \brief sprinkler struct.
 */
void
sprinkler_init (Sprinkler * s)
{
  printf ("Sprinkler x: ");
  scanf ("%le", &(s->x));
  printf ("Sprinkler y: ");
  scanf ("%le", &(s->y));
  printf ("Sprinkler z: ");
  scanf ("%le", &(s->z));
  printf ("Sprinkler pressure: ");
  scanf ("%le", &(s->pressure));
  printf ("Jet vertical angle: ");
  scanf ("%le", &(s->vertical_angle));
  printf ("Jet horizontal angle: ");
  scanf ("%le", &(s->horizontal_angle));
  printf ("Jet length: ");
  scanf ("%le", &(s->jet_length));
  printf ("Bed level: ");
  scanf ("%le", &(s->bed_level));
}

/**
 * \fn double air_viscosity (Air *a, double kelvin)
 * \brief function to calculate the air dynamic viscosity with the Sutherland
 *   equation.
 * \param a
 * \brief air struct.
 * \param kelvin
 * \brief Kelvin temperature.
 * \return air dynamic viscosity.
 */
double
air_viscosity (Air * a, double kelvin)
{
  return 1.458e-6 * kelvin * sqrt (kelvin) / (kelvin + 110.4);
}

/**
 * \fn double air_saturation_pressure (Air *a, double kelvin)
 * \brief function to calculate the water saturation pressure in air with the
 *   Antoine equation.
 * \param a
 * \brief air struct.
 * \param kelvin
 * \brief Kelvin temperature.
 * \return water saturation pressure in air.
 */
double
air_saturation_pressure (Air * a, double kelvin)
{
  return exp (23.7836 - 37.8289 / (kelvin - 42.850));
}

/**
 * \fn void air_open(Air *a, FILE *file)
 * \brief function to open an air struct in a file.
 * \param a
 * \brief air struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
air_open (Air * a, FILE * file)
{
  double k1, k2, kelvin;
  if (fscanf (file, "%le%le%le%le%le", &k1, &k2, &(a->temperature),
              &(a->humidity), &(a->pressure)) != 5)
    return 0;
  k1 *= M_PI / 180.;
  k2 *= M_PI / 180.;
  a->vx = k1 * cos (k2);
  a->vy = k1 * sin (k2);
  kelvin = a->temperature + KELVIN;
  a->viscosity = air_viscosity (a, kelvin);
  a->saturation_pressure = air_saturation_pressure (a, kelvin);
  a->vapour_pressure = a->saturation_pressure * 0.01 * a->humidity;
  a->density = (AIR_MOLECULAR_MASS * a->pressure
                + (WATER_MOLECULAR_MASS -
                   AIR_MOLECULAR_MASS) * a->vapour_pressure) / (R * kelvin);
  printf ("Air density = %le\n", a->density);
  printf ("Air viscosity = %le\n", a->viscosity);
  return 1;
}

/**
 * \fn void air_init(Air *a)
 * \brief function to input an air struct.
 * \param a
 * \brief air struct.
 */
void
air_init (Air * a)
{
  double k1, k2, kelvin;
  printf ("Wind velocity: ");
  scanf ("%le", &k1);
  printf ("Wind angle: ");
  scanf ("%le", &k2);
  k1 *= M_PI / 180.;
  k2 *= M_PI / 180.;
  a->vx = k1 * cos (k2);
  a->vy = k1 * sin (k2);
  printf ("Air temperature (Celsius): ");
  scanf ("%le", &(a->temperature));
  printf ("Air relative humidity: ");
  scanf ("%le", &(a->humidity));
  printf ("Air pressure: ");
  scanf ("%le", &(a->pressure));
  kelvin = a->temperature + KELVIN;
  a->viscosity = air_viscosity (a, kelvin);
  a->saturation_pressure = air_saturation_pressure (a, kelvin);
  a->vapour_pressure = a->saturation_pressure * 0.01 * a->humidity;
  a->density = (AIR_MOLECULAR_MASS * a->pressure
                + (WATER_MOLECULAR_MASS -
                   AIR_MOLECULAR_MASS) * a->vapour_pressure) / (R * kelvin);
  printf ("Air density = %le\n", a->density);
  printf ("Air viscosity = %le\n", a->viscosity);
}

/**
 * \fn void drop_open (Drop * d, Air * a, FILE * file)
 * \brief function to open a drop struct in a file.
 * \param d
 * \brief drop struct.
 * \param a
 * \brief air struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
drop_open (Drop * d, Air * a, FILE * file)
{
  double t;
  if (fscanf (file, "%le", &(d->diameter)) != 1)
    return 0;
  t = a->temperature - 4.;
  d->density = 999.985064 + t * (-0.0037845 + t * (-0.0070759 + t * 0.0000333));
  printf ("Drop density = %le\n", d->density);
  return 1;
}

/**
 * \fn void drop_init (Drop * d, Air * a)
 * \brief function to input a drop struct.
 * \param d
 * \brief drop struct.
 * \param a
 * \brief air struct.
 */
void
drop_init (Drop * d, Air * a)
{
  double t;
  printf ("Drop diameter: ");
  scanf ("%le", &(d->diameter));
  t = a->temperature - 4.;
  d->density = 999.985064 + t * (-0.0037845 + t * (-0.0070759 + t * 0.0000333));
  printf ("Drop density = %le\n", d->density);
}

/**
 * \fn double drop_drag (double Reynolds)
 * \brief function to calculate the drop drag resistance coefficient according
 *   to Fukui et al. (1980).
 * \param Reynolds
 * \brief Reynolds number.
 * \return drop drag resistance coefficient.
 */
double
drop_drag (double Reynolds)
{
  if (Reynolds == 0.)
    return 0.;
  if (Reynolds >= 1440.)
    return 0.45;
  if (Reynolds >= 128.)
    return 72.2 / Reynolds - 0.0000556 * Reynolds + 0.46;
  return 33.3 / Reynolds - 0.0033 * Reynolds + 1.2;
}

/**
 * \fn double drop_move (Drop * d, Air * a, Trajectory * t)
 * \brief function to calculate drag resistance and acceleration vector.
 * \param d
 * \brief drop struct.
 * \param a
 * \brief air struct.
 * \param t
 * \brief trajectory struct.
 * \return drag resistance parameter.
 */
double
drop_move (Drop * d, Air * a, Trajectory * t)
{
  double vrx, vry, v;
  vrx = t->vx - a->vx;
  vry = t->vy - a->vy;
  v = vector_module (vrx, vry, t->vz);
  t->drag = -0.75 * v * drop_drag (a->density * v * d->diameter / a->viscosity)
    * a->density / (d->density * d->diameter);
  t->ax = t->drag * vrx;
  t->ay = t->drag * vry;
  t->az = -G + t->drag * t->vz;
  return -t->drag;
}

/**
 * \fn int trajectory_open (Trajectory * t, Sprinkler * s, Drop * d, \
 *   FILE * file)
 * \brief function to open a trajectory struct in a file.
 * \param t
 * \brief trajectory struct.
 * \param s
 * \brief sprinkler struct.
 * \param d
 * \brief drop struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
trajectory_open (Trajectory * t, Sprinkler * s, Drop * d, FILE * file)
{
  double v, k2, k3;
  if (fscanf (file, "%le", &(t->dt)) != 1)
    return 0;
  printf ("Time step size: %le\n", t->dt);
  t->t = 0.;
  t->x = s->x;
  t->y = s->y;
  t->z = s->z;
  v = sqrt (2. * s->pressure / d->density);
  printf ("Water pressure: %le\nDrop density: %le\n", s->pressure, d->density);
  k2 = s->horizontal_angle * M_PI / 180.;
  k3 = s->vertical_angle * M_PI / 180.;
  t->vx = v * cos (k3) * cos (k2);
  t->vy = v * cos (k3) * sin (k2);
  t->vz = v * sin (k3);
  printf ("Drop velocity: (%le,%le,%le)\n", t->vx, t->vy, t->vz);
  return 1;
}

/**
 * \fn void trajectory_init (Trajectory * t, Sprinkler * s, Drop * d)
 * \brief function to input a trajectory struct.
 * \param t
 * \brief trajectory struct.
 * \param s
 * \brief sprinkler struct.
 * \param d
 * \brief drop struct.
 */
void
trajectory_init (Trajectory * t, Sprinkler * s, Drop * d)
{
  double v, k2, k3;
  printf ("Time step size: ");
  scanf ("%le", &(t->dt));
  t->t = 0.;
  t->x = s->x;
  t->y = s->y;
  t->z = s->z;
  v = sqrt (2. * s->pressure / d->density);
  printf ("Water pressure: %le\nDrop density: %le\n", s->pressure, d->density);
  k2 = s->horizontal_angle * M_PI / 180.;
  k3 = s->vertical_angle * M_PI / 180.;
  t->vx = v * cos (k3) * cos (k2);
  t->vy = v * cos (k3) * sin (k2);
  t->vz = v * sin (k3);
  printf ("Drop velocity: (%le,%le,%le)\n", t->vx, t->vy, t->vz);
  t->drag = 0.;
}

/**
 * \fn void trajectory_jet (Trajectory * t, Sprinkler * s)
 * \brief function to calculate the trajectory of the jet assuming negligible
 *   drag resistance.
 * \param t
 * \brief trajectory struct.
 * \param s
 * \brief sprinkler struct.
 */
void
trajectory_jet (Trajectory * t, Sprinkler * s)
{
  if (s->jet_length == 0.)
    return;
  t->t = s->jet_length / sqrt (t->vx * t->vx + t->vy * t->vy);
  t->x += t->t * t->vx;
  t->y += t->t * t->vy;
  t->z += t->t * (t->vz - 0.5 * G * t->t);
  t->vz -= G * t->t;
}

/**
 * \fn void trajectory_runge_kutta_4 (Trajectory * t, Drop * d, Air * a)
 * \brief function implementing a 4th order Runge-Kutta method to calculate a
 *   drop trajectory.
 * \param t
 * \brief trajectory struct.
 * \param d
 * \brief drop struct.
 * \param a
 * \brief air struct.
 */
void
trajectory_runge_kutta_4 (Trajectory * t, Drop * d, Air * a)
{
  double dt2, dt6;
  Trajectory t2[1], t3[1], t4[1];
  dt2 = 0.5 * t->dt;
  t2->x = t->x + dt2 * t->vx;
  t2->y = t->y + dt2 * t->vy;
  t2->z = t->z + dt2 * t->vz;
  t2->vx = t->vx + dt2 * t->ax;
  t2->vy = t->vy + dt2 * t->ay;
  t2->vz = t->vz + dt2 * t->az;
  drop_move (d, a, t2);
  t3->x = t->x + dt2 * t2->vx;
  t3->y = t->y + dt2 * t2->vy;
  t3->z = t->z + dt2 * t2->vz;
  t3->vx = t->vx + dt2 * t2->ax;
  t3->vy = t->vy + dt2 * t2->ay;
  t3->vz = t->vz + dt2 * t2->az;
  drop_move (d, a, t3);
  t4->x = t->x + t->dt * t3->vx;
  t4->y = t->y + t->dt * t3->vy;
  t4->z = t->z + t->dt * t3->vz;
  t4->vx = t->vx + t->dt * t3->ax;
  t4->vy = t->vy + t->dt * t3->ay;
  t4->vz = t->vz + t->dt * t3->az;
  drop_move (d, a, t4);
  dt6 = 1. / 6. * t->dt;
  t->x += dt6 * (t->vx + t4->vx + 2.0 * (t2->vx + t3->vx));
  t->y += dt6 * (t->vy + t4->vy + 2.0 * (t2->vy + t3->vy));
  t->z += dt6 * (t->vz + t4->vz + 2.0 * (t2->vz + t3->vz));
  t->vx += dt6 * (t->ax + t4->ax + 2.0 * (t2->ax + t3->ax));
  t->vy += dt6 * (t->ay + t4->ay + 2.0 * (t2->ay + t3->ay));
  t->vz += dt6 * (t->az + t4->az + 2.0 * (t2->az + t3->az));
  t->t += t->dt;
}

/**
 * \fn void trajectory_impact_correction (Trajectory * t, Drop * d, Air * a, \
 *   Sprinkler * s)
 * \brief function to correct the trajectory at the soil impact.
 * \param t
 * \brief trajectory struct.
 * \param d
 * \brief drop struct.
 * \param a
 * \brief air struct.
 * \param s
 * \brief sprinkler struct.
 */
void
trajectory_impact_correction (Trajectory * t, Drop * d, Air * a, Sprinkler * s)
{
  double dt, h;
  drop_move (d, a, t);
  h = s->bed_level - t->z;
  dt = (-sqrt (t->vz * t->vz - 2. * h * t->az) - t->vz) / t->az;
  t->x -= dt * (t->vx - 0.5 * dt * t->ax);
  t->y -= dt * (t->vy - 0.5 * dt * t->ay);
  t->z -= dt * (t->vz - 0.5 * dt * t->az);
  t->vx -= dt * t->ax;
  t->vy -= dt * t->ay;
  t->vz -= dt * t->az;
  t->t -= dt;
}

/**
 * \fn void trajectory_write (Trajectory * t, FILE * file)
 * \brief function to write a trajectory in a file.
 * \param t
 * \brief trajectory struct.
 * \param file
 * \brief file.
 */
void
trajectory_write (Trajectory * t, FILE * file)
{
  fprintf (file, "%lg %lg %lg %lg %lg %lg %lg %lg\n",
           t->t, t->x, t->y, t->z, t->vx, t->vy, t->vz, -t->drag);
}

/**
 * \fn int open(char *name, Sprinkler * sprinkler, Air * air, Drop * drop, \
 *   Trajectory * trajectory)
 * \brief function to open all data in a file.
 * \param name
 * \brief file name.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief air struct.
 * \param drop
 * \brief drop struct.
 * \param trajectory.
 * \brief trajectory struct.
 * \return 1 on success, 0 on error.
 */
int
open (char *name, Sprinkler * sprinkler, Air * air, Drop * drop,
      Trajectory * trajectory)
{
  FILE *file;
  file = fopen (name, "r");
  if (!file)
    return 0;
  if (!sprinkler_open (sprinkler, file)
      || !air_open (air, file) || !drop_open (drop, air, file))
    {
      fclose (file);
      return 0;
    }
  trajectory_open (trajectory, sprinkler, drop, file);
  fclose (file);
  return 1;
}

/**
 * \fn void init (Sprinkler * sprinkler, Air * air, Drop * drop, \
 *   Trajectory * trajectory)
 * \brief function to input all data.
 * \param sprinkler
 * \brief sprinkler struct.
 * \param air
 * \brief air struct.
 * \param drop
 * \brief drop struct.
 * \param trajectory.
 * \brief trajectory struct.
 */
void
init (Sprinkler * sprinkler, Air * air, Drop * drop, Trajectory * trajectory)
{
  sprinkler_init (sprinkler);
  air_init (air);
  drop_init (drop, air);
  trajectory_init (trajectory, sprinkler, drop);
}

/**
 * \fn int main (int argn, char **argc)
 * \brief main function.
 * \param argn
 * \brief arguments number.
 * \param argc
 * \brief argument strings.
 * \return code error.
 */
int
main (int argn, char **argc)
{
  double dt, dtmax;
  FILE *file;
  Sprinkler sprinkler[1];
  Air air[1];
  Drop drop[1];
  Trajectory trajectory[1];
  if (argn == 2)
    {
      init (sprinkler, air, drop, trajectory);
      file = fopen (argc[1], "w");
    }
  else if (argn == 3)
    {
      if (!open (argc[1], sprinkler, air, drop, trajectory))
        {
          printf ("Unable to open the data file\n");
          return 2;
        }
      file = fopen (argc[2], "w");
    }
  else
    {
      printf
        ("Usage of this program is:\n\ttrajectory file_data file_output\n");
      return 1;
    }
  if (!file)
    {
      printf ("Unable to open the output file\n");
      return 3;
    }
  trajectory_write (trajectory, file);
  trajectory_jet (trajectory, sprinkler);
  for (dt = trajectory->dt; trajectory->z > sprinkler->bed_level;)
    {
      trajectory_write (trajectory, file);
      dtmax = 1. / drop_move (drop, air, trajectory);
      if (dt > dtmax)
        trajectory->dt = dtmax;
      else
        trajectory->dt = dt;
      trajectory_runge_kutta_4 (trajectory, drop, air);
    }
  trajectory_impact_correction (trajectory, drop, air, sprinkler);
  trajectory_write (trajectory, file);
  return 0;
}
