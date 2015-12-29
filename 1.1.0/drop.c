/*
Sprinkler: a software to calculate drop trajectories in sprinkler irrigation.

AUTHORS: Javier Burguete.

Copyright 2012-2015, AUTHORS.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY AUTHORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

/**
 * \file drop.c
 * \brief Source file with the basic functions to calculate the trajectory of a
 *   drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <libxml/parser.h>
#include <glib.h>
#include <libintl.h>
#include <gsl/gsl_rng.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "utils.h"
#include "air.h"
#include "drop.h"

#define DEBUG_DROP 0            ///< macro to debug drop functions.

double drop_diameter;           ///< drop diameter.
unsigned int drop_detach_model; ///< drop jet detach model.
unsigned int drop_drag_model;   ///< drop drag resistance model.
double drop_velocity;           ///< drop initial velocity.
double drop_horizontal_angle;
  ///< horizontal angle of the drop initial velocity.
double drop_vertical_angle;
  ///< vertical angle of the drop initial velocity.
double drop_drag_coefficient;
  ///< drop drag resistance coefficient for the constant model.
double (*drop_axis_ratio) (Drop * d, Air * a, double v);
  ///< pointer to the function to calculate the drop axis ratio.
double (*drop_drag) (Drop * d, Air * a, double v);
  ///< pointer to the function to calculate the drop drag resistance
  ///< coefficient.

/**
 * \fn double water_compressibility (double t)
 * \brief function to calculate the water compressibility according to F. E.
 *   Jones and G. L. Harris (1992), ITS-90 density of water formulation for
 *   volumetric standards calibration. Journal of Researc of the National
 *   Institute of Standards and Technology 97(3), 335-340.
 * \param t
 * \brief temperature in Celsius.
 * \return isothermal compressibility of air-free water.
 */
double
water_compressibility (double t)
{
  return 5.083101e-10
    + t * (-3.682930e-12 + t * (7.263725e-14
                                + t * (-6.597702e-16 + t * 2.877670e-18)));
}

/**
 * \fn double water_density (Air * a)
 * \brief function to calculate the water density according to F. E.
 *   Jones and G. L. Harris (1992), ITS-90 density of water formulation for
 *   volumetric standards calibration. Journal of Research of the National
 *   Institute of Standards and Technology 97(3), 335-340.
 * \param a
 * \brief Air struct.
 * \return density of air-saturated water.
 */
double
water_density (Air * a)
{
  double t, compressibility;
  t = a->temperature;
  compressibility = water_compressibility (t);
  return (999.84847
          + t * (6.337563e-2
                 + t * (-8.523829e-3
                        + t * (6.943248e-5
                               - t * 2.821216e-7))))
    * (1. + compressibility * (a->pressure - 101325));
}

/**
 
* \fn double water_surface_tension (double kelvin)
 * \brief function to calculate the water surface tension according to N. B.
 *   Vargaftik, B. N. Volkov and L. D. Voljak (1983), international tables of
 *   the surface tension of water. J. Phys. Chem. Ref. Data 12(3), 817-820.
 * \param kelvin
 * \brief temperature in Kelvin.
 * \return surface tension of pure water.
 */
double
water_surface_tension (double kelvin)
{
  double x;
  x = 1. - kelvin / 647.15;
  return 2.358e-1 * pow (x, 1.256) * (1. - 0.625 * x);
}

/**
 * \fn void drop_print_ballistic (Drop * d)
 * \brief function to print some drop variables obtained with a ballistic model.
 * \param d
 * \brief Drop struct.
 */
void
drop_print_ballistic (Drop * d)
{
  printf ("Balistic model:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n",
          vector_module (d->v[0], d->v[1], d->v[2]),
          180. / M_PI * atan (d->v[2] / d->v[0]), d->r[0], d->r[1]);
}

/**
 * \fn void drop_print_parabolic (Drop * d)
 * \brief function to print some drop variables obtained with a parabolic model.
 * \param d
 * \brief Drop struct.
 */
void
drop_print_parabolic (Drop * d)
{
  double t, vz;
  t = -d->r[0] / d->v[0];
  vz = d->v[2] - G * t;
  printf ("Parabolic model:\n\tvelocity=%lg\n\tangle=%lg\n\tx=%lg\n\tz=%lg\n",
          vector_module (d->v[0], 0., vz),
          180. / M_PI * atan (vz / d->v[0]),
          d->r[0] + t * d->v[0], d->r[2] + t * (d->v[2] - 0.5 * G * t));
}

/**
 * \fn void drop_init (Drop * d, Air * a, gsl_rng *rng)
 * \brief function to init the drop variables.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param rng
 * \brief GSL pseudo-random numbers generator struct.
 */
void
drop_init (Drop * d, Air * a, gsl_rng * rng)
{
#if DEBUG_DROP
  fprintf (stderr, "drop_init: start\n");
#endif
  d->diameter = drop_diameter;
  d->density = water_density (a);
  d->surface_tension = water_surface_tension (a->kelvin);
  if (drop_detach_model == DROP_DETACH_MODEL_RANDOM)
    d->jet_time *= gsl_rng_uniform (rng);
  switch (drop_drag_model)
    {
    case DROP_DRAG_MODEL_CONSTANT:
      drop_drag = drop_drag_constant;
      break;
    case DROP_DRAG_MODEL_SPHERE:
      drop_drag = drop_drag_sphere;
      break;
    default:
      drop_axis_ratio = drop_axis_ratio_Burguete;
      drop_drag = drop_drag_ovoid;
    }
#if DEBUG_DROP
  fprintf (stderr, "drop_init: drag_model=%u\n", drop_drag_model);
  fprintf (stderr, "drop_init: density=%lg\n", d->density);
  fprintf (stderr, "drop_init: surface tension=%lg\n", d->surface_tension);
  fprintf (stderr, "drop_init: end\n");
#endif
}

/**
 * \fn void drop_error (char *message)
 * \brief function to show an error message opening a Drop struct.
 * \param message
 * \brief error message.
 */
void
drop_error (char *message)
{
  error_message = g_strconcat (gettext ("Drop file"), ": ", message, NULL);
}

/**
 * \fn void drop_open_console (Drop * d, Air * a)
 * \brief function to input a Drop struct in console.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 */
void
drop_open_console (Drop * d, Air * a)
{
#if DEBUG_DROP
  fprintf (stderr, "drop_open_console: start\n");
#endif
  printf ("Drop diameter: ");
  scanf ("%lf", &drop_diameter);
  printf ("Drop jet model (0: total, 1: random): ");
  scanf ("%u", &drop_detach_model);
#if DEBUG_DROP
  fprintf (stderr, "drop_open_console: end\n");
#endif
}

/**
 * \fn int drop_open_xml (Drop * d, Air * a, xmlNode *node)
 * \brief function to input a Drop struct on a XML node.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
drop_open_xml (Drop * d, Air * a, xmlNode * node)
{
  xmlChar *buffer;
  double sh, ch, sv, cv;
  int k;
#if DEBUG_DROP
  fprintf (stderr, "drop_open_xml: start\n");
#endif
  buffer = NULL;
  if (xmlStrcmp (node->name, XML_DROP))
    {
      drop_error (gettext ("bad label"));
      goto exit_on_error;
    }
  drop_diameter = xml_node_get_float (node, XML_DIAMETER, &k);
  if (!k)
    {
      drop_error (gettext ("bad diameter"));
      goto exit_on_error;
    }
  d->r[0] = xml_node_get_float_with_default (node, XML_X, 0., &k);
  if (!k)
    {
      drop_error (gettext ("bad x"));
      goto exit_on_error;
    }
  d->r[1] = xml_node_get_float_with_default (node, XML_Y, 0., &k);
  if (!k)
    {
      drop_error (gettext ("bad y"));
      goto exit_on_error;
    }
  d->r[2] = xml_node_get_float_with_default (node, XML_Z, 0., &k);
  if (!k)
    {
      drop_error (gettext ("bad z"));
      goto exit_on_error;
    }
  drop_velocity = xml_node_get_float_with_default (node, XML_VELOCITY, 0., &k);
  if (!k)
    {
      drop_error (gettext ("bad velocity"));
      goto exit_on_error;
    }
  drop_horizontal_angle
    = xml_node_get_float_with_default (node, XML_HORIZONTAL_ANGLE, 0., &k);
  if (!k)
    {
      drop_error (gettext ("bad horizontal angle"));
      goto exit_on_error;
    }
  drop_vertical_angle
    = xml_node_get_float_with_default (node, XML_VERTICAL_ANGLE, 0., &k);
  if (!k)
    {
      drop_error (gettext ("bad vertical angle"));
      goto exit_on_error;
    }
  buffer = xmlGetProp (node, XML_DRAG_MODEL);
  if (!buffer)
    {
      drop_error (gettext ("no drag model"));
      goto exit_on_error;
    }
  if (!xmlStrcmp (buffer, XML_CONSTANT))
    {
      drop_drag_model = DROP_DRAG_MODEL_CONSTANT;
      drop_drag_coefficient
        = xml_node_get_float_with_default (node, XML_DRAG, 0., &k);
      if (!k)
        {
          drop_error (gettext ("bad drag value"));
          goto exit_on_error;
        }
    }
  else if (!xmlStrcmp (buffer, XML_SPHERE))
    drop_drag_model = DROP_DRAG_MODEL_SPHERE;
  else if (!xmlStrcmp (buffer, XML_OVOID))
    drop_drag_model = DROP_DRAG_MODEL_OVOID;
  else
    {
      drop_error (gettext ("unknown drag resistance model"));
      goto exit_on_error;
    }
  xmlFree (buffer);
  buffer = xmlGetProp (node, XML_DETACH_MODEL);
  if (!buffer || !xmlStrcmp (buffer, XML_TOTAL))
    drop_detach_model = DROP_DETACH_MODEL_TOTAL;
  else if (!xmlStrcmp (buffer, XML_RANDOM))
    drop_detach_model = DROP_DETACH_MODEL_RANDOM;
  else
    {
      drop_error (gettext ("unknown jet model"));
      goto exit_on_error;
    }
  sincos (M_PI / 180. * drop_horizontal_angle, &sh, &ch);
  sincos (M_PI / 180. * drop_vertical_angle, &sv, &cv);
  d->v[0] = drop_velocity * cv * ch;
  d->v[1] = drop_velocity * cv * sh;
  d->v[2] = drop_velocity * sv;
#if DEBUG_DROP
  fprintf (stderr, "drop_open_xml: end\n");
#endif
  return 1;

exit_on_error:
  xmlFree (buffer);
#if DEBUG_DROP
  fprintf (stderr, "drop_open_xml: end\n");
#endif
  return 0;
}

/**
 * \fn double drop_axis_ratio_Burguete (Drop * d, Air * a, double v)
 * \brief function to calculate the axis ratio of a drop according to Burguete
 *   et al. (2016).
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param v
 * \brief drop velocity.
 * \return axis ratio.
 */
double
drop_axis_ratio_Burguete (Drop * d, Air * a, double v)
{
  double Weber, ratio;
#if DEBUG_DROP
  fprintf (stderr, "drop_axis_ratio_Burguete: air density=%lg\n", a->density);
  fprintf (stderr, "drop_axis_ratio_Burguete: drop velocity=%lg\n", v);
  fprintf (stderr, "drop_axis_ratio_Burguete: drop diameter=%lg\n",
           d->diameter);
  fprintf (stderr, "drop_axis_ratio_Burguete: drop surface tension=%lg\n",
           d->surface_tension);
#endif
  Weber = 0.25 * a->density * v * v * d->diameter / d->surface_tension;
  ratio = fmax (1. - 0.1742 * Weber, 0.642);
#if DEBUG_DROP
  fprintf (stderr, "drop_axis_ratio_Burguete: Weber=%lg axis_ratio=%lg\n",
           Weber, ratio);
#endif
  return ratio;
}

/**
 * \fn drop_drag_constant (Drop * d, Air * a, double v)
 * \brief function to get a fixed drop drag resistance coefficient.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param v
 * \brief drop velocity.
 * \return fixed drop drag resistance coefficient.
 */
double
drop_drag_constant (Drop * d, Air * a, double v)
{
  return drop_drag_coefficient;
}

/**
 * \fn drop_drag_sphere (Drop * d, Air * a, double v)
 * \brief function to calculate the drop drag resistance coefficient according
 *   to Fukui et al. (1980) or Morsi & Alexander (1972).
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param v
 * \brief drop velocity.
 * \return drop drag resistance coefficient of a solid smooth sphere.
 */
double
drop_drag_sphere (Drop * d, Air * a, double v)
{
  double Re, cd;
  Re = v * d->diameter / a->kinematic_viscosity;
#if DRAG_SPHERE == DRAG_SPHERE_FUKUI
  if (Re >= 1440.)
    cd = 0.45;
  else if (Re >= 128.)
    cd = 72.2 / Re - 0.0000556 * Re + 0.46;
  else if (Re > 0.)
    cd = 33.3 / Re - 0.0033 * Re + 1.2;
  else
    cd = 0.;
#elif DRAG_SPHERE == DRAG_SPHERE_MORSI_ALEXANDER
  if (Re > 10000.)
    cd = 0.5191 - 1662.5 / Re + 5416700. / (Re * Re);
  else if (Re > 5000.)
    cd = 0.46 - 490.546 / Re + 578700. / (Re * Re);
  else if (Re > 1000.)
    cd = 0.357 + 148.62 / Re - 47500. / (Re * Re);
  else if (Re > 100.)
    cd = 0.3644 + 98.33 / Re - 2778. / (Re * Re);
  else if (Re > 10.)
    cd = 0.6167 + 46.5 / Re - 116.67 / (Re * Re);
  else if (Re > 1.)
    cd = 1.222 + 29.1667 / Re - 3.8889 / (Re * Re);
  else if (Re > 0.1)
    cd = 3.69 + 22.73 / Re + 0.0903 / (Re * Re);
  else if (Re > 0.)
    cd = 24. / Re;
  else
    cd = 0.;
#endif
#if DEBUG_DROP
  fprintf (stderr, "drop_drag_sphere: R=%lg cd=%lg\n", R, cd);
#endif
  return cd;
}

/**
 * \fn drop_drag_ovoid (Drop * d, Air * a, double v)
 * \brief function to calculate the drop drag resistance coefficient according
 *   to Burguete et al. (2016).
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param v
 * \brief drop velocity.
 * \return drop drag resistance coefficient of an ovoid drop.
 */
double
drop_drag_ovoid (Drop * d, Air * a, double v)
{
  double x;
  d->axis_ratio = drop_axis_ratio (d, a, v);
  x = d->axis_ratio - 1.;
  return (1. + 2.322 * x * x) * drop_drag_sphere (d, a, v)
    / pow (d->axis_ratio, 2. / 3.);
}

/**
 * \fn double drop_move (Drop * d, Air * a, double factor)
 * \brief function to calculate drag resistance factor and the acceleration
 *   vector of a drop.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param factor
 * \brief drag resistance factor.
 * \return drag resistance factor.
 */
double
drop_move (Drop * d, Air * a, double factor)
{
  double vrx, vry, v;
#if DEBUG_DROP
  fprintf (stderr, "drop_move: start\n");
#endif
  vrx = d->v[0] - a->u;
  vry = d->v[1] - a->v;
  v = vector_module (vrx, vry, d->v[2]);
#if DEBUG_DROP
  fprintf (stderr, "drop_move: vr=(%lg,%lg,%lg) v=%lg\n", vrx, vry, d->v[2], v);
  fprintf (stderr, "drop_move: cd=%lg\n", drop_drag (d, a, v));
#endif
  d->drag = -0.75 * factor * v * drop_drag (d, a, v) * a->density
    / (d->density * d->diameter);
  d->a[0] = d->drag * vrx;
  d->a[1] = d->drag * vry;
  d->a[2] = -(1. - a->density / d->density) * G + d->drag * d->v[2];
#if DEBUG_DROP
  fprintf (stderr, "drop_move: a=(%lg,%lg,%lg)\n", d->a[0], d->a[1], d->a[2]);
  fprintf (stderr, "drop_move: drag=%lg\n", -d->drag);
  fprintf (stderr, "drop_move: end\n");
#endif
  return -d->drag;
}

#if HAVE_GTK

/**
 * \fn void dialog_drop_new (Drop * d, Air * a)
 * \brief function to set the drop data in a GtkDialog.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 */
void
dialog_drop_new (Drop * d, Air * a)
{
  DialogDrop dlg[1];

  dlg->label_diameter = (GtkLabel *) gtk_label_new ("Diámetro de la gota");
  dlg->spin_diameter = (GtkSpinButton *)
    gtk_spin_button_new_with_range (0.00050, 0.00700, 0.00001);
  gtk_spin_button_set_value (dlg->spin_diameter, drop_diameter);

  dlg->grid = (GtkGrid *) gtk_grid_new ();
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->label_diameter, 0, 0, 1, 1);
  gtk_grid_attach (dlg->grid, (GtkWidget *) dlg->spin_diameter, 1, 0, 1, 1);

  dlg->window =
    (GtkDialog *) gtk_dialog_new_with_buttons ("Datos de la gota",
                                               window_parent, GTK_DIALOG_MODAL,
                                               GTK_STOCK_OK, GTK_RESPONSE_OK,
                                               GTK_STOCK_CANCEL,
                                               GTK_RESPONSE_CANCEL, NULL);
  gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (dlg->window)),
                     GTK_WIDGET (dlg->grid));
  gtk_widget_show_all (GTK_WIDGET (dlg->window));

  if (gtk_dialog_run (dlg->window) == GTK_RESPONSE_OK)
    {
      drop_diameter = gtk_spin_button_get_value (dlg->spin_diameter);
      drop_init (d, a);
    }
  gtk_widget_destroy ((GtkWidget *) dlg->window);
}

#endif
