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
 * \file drop.h
 * \brief Header file with the basic functions to calculate the trajectory of a
 *   drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef DROP__H
#define DROP__H 1

/**
 * \enum DropDetachModel
 * \brief enum to define the drop jet detach models.
 */
enum DropDetachModel
{
  DROP_DETACH_MODEL_TOTAL = 0,  ///< total time into the jet.
  DROP_DETACH_MODEL_RANDOM = 1  ///< random time to detach the jet.
};

/**
 * \enum DropDragModel
 * \brief enum to define the drop jet drag models.
 */
enum DropDragModel
{
  DROP_DRAG_MODEL_CONSTANT = 0, ///< constant drag resistance coefficient.
  DROP_DRAG_MODEL_SPHERE = 1,
  ///< drag resistance coefficient as a solid smooth sphere.
  DROP_DRAG_MODEL_OVOID = 2
    ///< drag resistance coefficient as a solid smooth ovoid.
};

/**
 * \struct Drop
 * \brief struct to define a drop.
 */
typedef struct
{
  double r[3];                  ///< position.
  double v[3];                  ///< velocity.
  double a[3];                  ///< acceleration.
  double diameter;              ///< diameter.
  double density;               ///< density.
  double surface_tension;       ///< surface tension.
  double axis_ratio;            ///< axis ratio.
  double drag;                  ///< drag resistance factor.
  double jet_time;              ///< time into the jet.
} Drop;

extern double drop_diameter;
extern unsigned int drop_detach_model;
extern unsigned int drop_drag_model;
extern double drop_velocity;
extern double drop_horizontal_angle;
extern double drop_vertical_angle;
extern double drop_maximum_diameter;
extern double drop_drag_coefficient;
extern double (*drag_axis_ratio) (Drop * d, Air * a, double v);
extern double (*drop_drag) (Drop * d, Air * a, double v);

double water_compressibility (double t);
double water_density (Air * a);
double water_surface_tension (double kelvin);
void drop_print_ballistic (Drop * d);
void drop_print_parabolic (Drop * d);
void drop_init (Drop * d, Air * a, gsl_rng * rng);
void drop_error (char *message);
void drop_open_console (Drop * d, Air * a);
int drop_open_xml (Drop * d, Air * a, xmlNode * node);
double drop_axis_ratio_Burguete (Drop * d, Air * a, double v);
double drop_drag_constant (Drop * d, Air * a, double v);
double drop_drag_sphere (Drop * d, Air * a, double v);
double drop_drag_ovoid (Drop * d, Air * a, double v);
double drop_move (Drop * d, Air * a, double factor);

#if HAVE_GTK

/**
 * \struct DialogDrop
 * \brief dialog to set the drop data.
 */
typedef struct
{
  GtkLabel *label_diameter;     ///< diameter GtkLabel.
  GtkSpinButton *spin_diameter; ///< diameter GtkSpinButton.
  GtkGrid *grid;                ///< GtkGrid to pack the widgets.
  GtkDialog *window;            ///< GtkDialog to show the widgets.
} DialogDrop;

void dialog_drop_new (Drop * d, Air * a);

#endif

#endif
