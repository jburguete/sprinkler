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
 * \file config.h
 * \brief Configuration header file.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2012-2015 Javier Burguete Tolosa, all rights reserved.
 */
#ifndef CONFIG__H
#define CONFIG__H 1

// Some constants
#define AIR_MOLECULAR_MASS 0.028964     ///< dry air molecular mass.
#define G 9.81                  ///< gravitational acceleration constant.
#define KELVIN_TEMPERATURE 273.15       ///< Kelvin to Celsius degree.
#define R 8.314                 ///< thermodinamic constant.
#define WATER_MOLECULAR_MASS 0.018016   ///< water molecular mass.

// Default values
#define AIR_HUMIDITY 100.       ///< saturated relative humidity.
#define AIR_PRESSURE 101325.    ///< atmospheric pressure.
#define AIR_TEMPERATURE 20.     ///< normal atmospheric temperature in Celsius.
#define MAXIMUM_DROP_DIAMETER 0.0080    ///< maximum diameter of stable drops.
#define MINIMUM_DROP_DIAMETER 0.0004    ///< minimum diameter of emitted drops.
#define RANDOM_SEED 7007        ///< pseudo-random numbers generator seed.
#define WIND_ANGLE 0.           ///< wind azimuth angle.
#define WIND_HEIGHT 2.          ///< reference height to measure the wind.
#define WIND_UNCERTAINTY 0.     ///< wind uncertainty.
#define WIND_VELOCITY 0.        ///< wind velocity.

// Models
#define DRAG_SPHERE_FUKUI 1     ///< Fukui et al. (1980) model of a sphere drag.
#define DRAG_SPHERE_MORSI_ALEXANDER 2
  ///< Morsi & Alexander (1972) model of a sphere drag.
#define DRAG_SPHERE DRAG_SPHERE_FUKUI   ///< model of a sphere drag.

// XML labels
#define XML_A0                    (const xmlChar*)"a0"
  ///< a0 XML label.
#define XML_A1                    (const xmlChar*)"a1"
  ///< a1 XML label.
#define XML_A2                    (const xmlChar*)"a2"
  ///< a2 XML label.
#define XML_A3                    (const xmlChar*)"a3"
  ///< a3 XML label.
#define XML_A4                    (const xmlChar*)"a4"
  ///< a4 XML label.
#define XML_AIR                   (const xmlChar*)"air"
  ///< air XML label.
#define XML_ANGLE                 (const xmlChar*)"angle"
  ///< angle XML label.
#define XML_BED_LEVEL             (const xmlChar*)"bed_level"
  ///< bed_level XML label.
#define XML_BIG_DROPS             (const xmlChar*)"big_drops"
  ///< big_drops XML label.
#define XML_CFL                   (const xmlChar*)"cfl"
  ///< cfl XML label.
#define XML_CONSTANT              (const xmlChar*)"constant"
  ///< constant XML label.
#define XML_DETACH_MODEL          (const xmlChar*)"detach_model"
  ///< detach_model XML label.
#define XML_DIAMETER              (const xmlChar*)"diameter"
  ///< diameter XML label.
#define XML_DRAG                  (const xmlChar*)"drag"
  ///< drag XML label.
#define XML_DRAG_MODEL            (const xmlChar*)"drag_model"
  ///< drag_model XML label.
#define XML_DROP                  (const xmlChar*)"drop"
  ///< drop XML label.
#define XML_DROPS_NUMBER          (const xmlChar*)"drops_number"
  ///< drops_number XML label.
#define XML_DT                    (const xmlChar*)"dt"
  ///< dt XML label.
#define XML_DX                    (const xmlChar*)"dx"
  ///< dx XML label.
#define XML_DY                    (const xmlChar*)"dy"
  ///< dy XML label.
#define XML_FILE                  (const xmlChar*)"file"
  ///< file XML label.
#define XML_HEIGHT                (const xmlChar*)"height"
  ///< height XML label.
#define XML_HORIZONTAL_ANGLE      (const xmlChar*)"horizontal_angle"
  ///< horizontal angle XML label.
#define XML_HUMIDITY              (const xmlChar*)"humidity"
  ///< humidity XML label.
#define XML_INVERT                (const xmlChar*)"invert"
  ///< invert XML label.
#define XML_JET                   (const xmlChar*)"jet"
  ///< jet XML label.
#define XML_JET_MODEL             (const xmlChar*)"jet_model"
  ///< jet_model XML label.
#define XML_JET_TIME              (const xmlChar*)"jet_time"
  ///< jet_time XML label.
#define XML_MEASUREMENT           (const xmlChar*)"measurement"
  ///< measurement XML label.
#define XML_MAXIMUM_ANGLE         (const xmlChar*)"maximum_angle"
  ///< maximum angle XML label.
#define XML_MAXIMUM_DROP_DIAMETER (const xmlChar*)"maximum_drop_diameter"
  ///< maximum drop_diameter XML label.
#define XML_MINIMUM_ANGLE         (const xmlChar*)"minimum_angle"
  ///< minimum angle XML label.
#define XML_MINIMUM_DROP_DIAMETER (const xmlChar*)"minimum_drop_diameter"
  ///< minimum drop_diameter XML label.
#define XML_NULL_DRAG             (const xmlChar*)"null_drag"
  ///< null_drag XML label.
#define XML_OVOID                 (const xmlChar*)"ovoid"
  ///< ovoid XML label.
#define XML_PRESSURE              (const xmlChar*)"pressure"
  ///< pressure XML label.
#define XML_PROGRESSIVE           (const xmlChar*)"progressive"
  ///< progressive XML label.
#define XML_RANDOM                (const xmlChar*)"random"
  ///< random XML label.
#define XML_SPHERE                (const xmlChar*)"sphere"
  ///< sphere XML label.
#define XML_SPRINKLER             (const xmlChar*)"sprinkler"
  ///< sprinkler XML label.
#define XML_TEMPERATURE           (const xmlChar*)"temperature"
  ///< temperature XML label.
#define XML_TOTAL                 (const xmlChar*)"total"
  ///< total XML label.
#define XML_TRAJECTORY            (const xmlChar*)"trajectory"
  ///< trajectory XML label.
#define XML_UNCERTAINTY           (const xmlChar*)"uncertainty"
  ///< uncertainty XML label.
#define XML_VELOCITY              (const xmlChar*)"velocity"
  ///< velocity XML label.
#define XML_VERTICAL_ANGLE        (const xmlChar*)"vertical_angle"
  ///< horizontal angle XML label.
#define XML_X                     (const xmlChar*)"x"
  ///< x XML label.
#define XML_Y                     (const xmlChar*)"y"
  ///< y XML label.
#define XML_Z                     (const xmlChar*)"z"
  ///< z XML label.

#endif
