/*
Sprinkler: a software to calculate drop trajectories in sprinkler irrigation.

AUTHORS: Javier Burguete.

Copyright 2012-2014, AUTHORS.

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
#include <gsl/gsl_rng.h>
#if HAVE_GTK
#include <gtk/gtk.h>
#endif
#include "config.h"
#include "utils.h"
#include "air.h"
#include "drop.h"

double drop_diameter;           ///< drop diameter.
double drop_velocity;           ///< drop initial velocity.
double drop_horizontal_angle;
  ///< horizontal angle of the drop initial velocity.
double drop_vertical_angle;
  ///< vertical angle of the drop initial velocity.
double drop_drag_coefficient;
  ///< drop drag resistance coefficient for the constant model.
double (*drag_axis_ratio) (Drop *d, Air *a, double v);
  ///< pointer to the function to calculate the drop axis ratio.
double (*drop_drag) (Drop *d, Air *a, double v);
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
 * \fn double water_density (Air *a)
 * \brief function to calculate the water compressibility according to F. E.
 *   Jones and G. L. Harris (1992), ITS-90 density of water formulation for
 *   volumetric standards calibration. Journal of Research of the National
 *   Institute of Standards and Technology 97(3), 335-340.
 * \param a
 * \brief air struct.
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
 * \fn void drop_init (Drop * d, Air * a)
 * \brief function to init the drop variables.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief air struct.
 */
void
drop_init (Drop * d, Air * a)
{
  d->diameter = drop_diameter;
  d->density = water_density (a);
  d->surface_tension = water_surface_tension (a->kelvin);
  printf ("Drop density = %le\n", d->density);
  printf ("Drop surface tension = %le\n", d->surface_tension);
}

/**
 * \fn void drop_open_file (Drop * d, Air * a, FILE * file)
 * \brief function to open a Drop struct in a file.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief air struct.
 * \param file
 * \brief file.
 * \return 1 on success, 0 on error.
 */
int
drop_open_file (Drop * d, Air * a, FILE * file)
{
  if (fscanf (file, "%lf", &drop_diameter) != 1)
	return 0;
  drop_init (d, a);
  return 1;
}

/**
 * \fn void drop_open_console (Drop * d, Air * a)
 * \brief function to input a Drop struct in console.
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief air struct.
 */
void
drop_open_console (Drop * d, Air * a)
{
  printf ("Drop diameter: ");
  scanf ("%lf", &drop_diameter);
  drop_init (d, a);
}

/**
 * \fn int drop_open_xml (Drop *d, Air *a, xmlNode *node)
 * \brief function to input a Drop struct on a XML node.
 * \param d
 * \brief Drop struct.
 * \param node
 * \brief XML node.
 * \return 1 on success, 0 on error.
 */
int
drop_open_xml (Drop *d, Air *a, xmlNode *node)
{
  int k;
  double sh, ch, sv, cv;
  if (xmlStrcmp (node->name, XML_DROP))
    return 0;
  drop_diameter = xml_node_get_float (node, XML_DIAMETER, &k);
  if (!k)
    return 0;
  d->r[0] = xml_node_get_float_with_default (node, XML_X, 0., &k);
  if (!k)
    return 0;
  d->r[1] = xml_node_get_float_with_default (node, XML_Y, 0., &k);
  if (!k)
    return 0;
  d->r[2] = xml_node_get_float_with_default (node, XML_Z, 0., &k);
  if (!k)
    return 0;
  drop_velocity = xml_node_get_float_with_default (node, XML_VELOCITY, 0., &k);
  if (!k)
    return 0;
  drop_horizontal_angle
	= xml_node_get_float_with_default (node, XML_HORIZONTAL_ANGLE, 0., &k);
  if (!k)
    return 0;
  drop_vertical_angle
	= xml_node_get_float_with_default (node, XML_VERTICAL_ANGLE, 0., &k);
  if (!k)
    return 0;
  sincos (M_PI / 180. * drop_horizontal_angle, &sh, &ch);
  sincos (M_PI / 180. * drop_vertical_angle, &sv, &cv);
  d->v[0] = drop_velocity * cv * ch;
  d->v[1] = drop_velocity * cv * sh;
  d->v[2] = drop_velocity * sv;
  return 1;
}

/**
 * \fn double drag_axis_ratio_Burguete (Drop *d, Air *a, double v)
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
drag_axis_ratio_Burguete (Drop *d, Air *a, double v)
{
  double Weber;
  Weber = 0.25 * a->density * v * v * d->diameter / d->surface_tension;
  return 1. - 0.1742 * Weber;
}

/**
 * \fn drop_drag_constant (Drop *d, Air *a, double v)
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
drop_drag_constant (Drop *d, Air *a, double v)
{
  return drop_drag_coefficient;
}

/**
 * \fn drop_drag_sphere (Drop *d, Air *a, double v)
 * \brief function to calculate the drop drag resistance coefficient according
 *   to Fukui et al. (1980).
 * \param d
 * \brief Drop struct.
 * \param a
 * \brief Air struct.
 * \param v
 * \brief drop velocity.
 * \return drop drag resistance coefficient of a solid smooth sphere.
 */
double
drop_drag_sphere (Drop *d, Air *a, double v)
{
  double Reynolds;
  Reynolds = v * d->diameter / a->kinematic_viscosity;
  if (Reynolds == 0.)
    return 0.;
  if (Reynolds >= 1440.)
    return 0.45;
  if (Reynolds >= 128.)
    return 72.2 / Reynolds - 0.0000556 * Reynolds + 0.46;
  return 33.3 / Reynolds - 0.0033 * Reynolds + 1.2;
}

/**
 * \fn drop_drag_ovoid (Drop *d, Air *a, double v)
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
drop_drag_ovoid (Drop *d, Air *a, double v)
{
  drop_axis_ratio (d, a, v);
  return drop_drag_sphere (d, a, v) * (3.709 + d->axis_ratio * (- 5.519
	+ 2.731 * d->axis_ratio));
}

/**
 * \fn double drop_move (Drop * d, Air *a)
 * \brief function to calculate drag resistance factor and the acceleration
 *   vector of a drop.
 * \param d
 * \brief drop struct.
 * \param a
 * \brief air struct.
 * \return drag resistance factor.
 */
double
drop_move (Drop * d, Air *a)
{
  double vrx, vry, v;
  vrx = d->v[0] - a->u;
  vry = d->v[1] - a->v;
  v = vector_module (vrx, vry, d->v[2]);
  d->drag =
	-0.75 * v * drop_drag (d, a, v) * a->density / (d->density * d->diameter);
  d->a[0] = d->drag * vrx;
  d->a[1] = d->drag * vry;
  d->a[2] = - (1. - a->density / d->density) * G + d->drag * d->v[2];
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
