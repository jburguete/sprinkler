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
 * \file trajectory.h
 * \brief Header file to define drop trajectory data.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef TRAJECTORY__H
#define TRAJECTORY__H 1

/**
 * \enum TrajectoryJetModel
 * \brief enum to define the trajectory jet models.
 */
enum TrajectoryJetModel
{
  TRAJECTORY_JET_MODEL_NULL_DRAG = 0,   ///< null drag resistance.
  TRAJECTORY_JET_MODEL_PROGRESSIVE = 1, ///< progressively increasing.
  TRAJECTORY_JET_MODEL_BIG_DROPS = 2    ///< big drops movement.
};

/**
 * \struct Trajectory
 * \brief struct to define a drop trajectory.
 */
typedef struct
{
  Drop drop[1];                 ///< drop data.
  FILE *file;                   ///< results file.
  char *filename;               ///< results file name.
  double t;                     ///< time.
  double dt;                    ///< numerical time step size.
  double cfl;                   ///< stability number.
  double bed_level;             ///< bed level.
  double jet_time;              ///< time of the emitted jet.
  unsigned int jet_model;       ///< jet model type.
} Trajectory;

extern void (*trajectory_jet) (Trajectory * t, Air * a);

void trajectory_init (Trajectory * t, Air * a, gsl_rng * rng);
void trajectory_error (char *message);
void trajectory_open_console (Trajectory * t, Air * a, char *name);
int trajectory_open_xml (Trajectory * t, Air * a, xmlNode * node, char *name);
void trajectory_open_data (Trajectory * t, Air * a, gsl_rng * rng,
		                   double diameter, unsigned int jet_model,
						   unsigned int detach_model, unsigned int drag_model,
						   double maximum_diameter, double drag_coefficient);
void trajectory_runge_kutta_4 (Trajectory * t, Air * a, double factor);
void trajectory_jet_null_drag (Trajectory * t, Air * a);
void trajectory_jet_progressive (Trajectory * t, Air * a);
void trajectory_jet_big_drops (Trajectory * t, Air * a);
void trajectory_impact_correction (Trajectory * t, Air * a);
void trajectory_initial_correction (Trajectory * t, Air * a);
void trajectory_write (Trajectory * t);
void trajectory_calculate (Trajectory * t, Air * a, Measurement * m,
                           unsigned int n, FILE *file);
void trajectory_invert (Trajectory * t, Air * a);

#endif
