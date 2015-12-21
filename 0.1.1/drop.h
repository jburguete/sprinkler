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
 * \file drop.h
 * \brief Header file with the basic functions to calculate the trajectory of a
 *   drop with the ballistic model.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef DROP__H
#define DROP__H 1

/**
 * \struct Drop
 * \brief struct to define a drop.
 */
typedef struct
{
  double diameter;              ///< diameter.
  double density;               ///< density.
  double surface_tension;       ///< surface tension.
  double axis_ratio;            ///< axis ratio.
  double drag;                  ///< drag resistance factor.
  double r[3];                  ///< position.
  double v[3];                  ///< velocity.
  double a[3];                  ///< acceleration.
} Drop;

extern double drop_diameter;
extern double drop_velocity;
extern double drop_horizontal_angle;
extern double drop_vertical_angle;
extern double drop_drag_coefficient;
extern double (*drag_axis_ratio) (Drop *d, Air *a, double v);
extern double (*drop_drag) (Drop *d, Air *a, double v);

double water_compressibility (double t);
double water_density (Air * a);
double water_surface_tension (double kelvin);
void drop_init (Drop * d, Air * a);
int drop_open_file (Drop * d, Air * a, FILE * file);
void drop_open_console (Drop * d, Air * a);
int drop_open_xml (Drop *d, Air *a, xmlNode *node);
double drag_axis_ratio_Burguete (Drop *d, Air *a, double v);
double drop_drag_constant (Drop *d, Air *a, double v);
double drop_drag_sphere (Drop *d, Air *a, double v);
double drop_drag_ovoid (Drop *d, Air *a, double v);
double drop_move (Drop * d, Air *a);

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
