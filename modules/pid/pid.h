/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * pid.h
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */

#ifndef MODULES_PID_PID_H_
#define MODULES_PID_PID_H_

#include <stdint.h>

// Structure to hold PID controller parameters and state
typedef struct {
	uint8_t 			process;
	uint8_t				status;
	uint8_t				flags;
	uint32_t 			*next_pid;
	TIM_HandleTypeDef 	*timer;
	uint32_t 			timer_channel;
	float 				Kp;            // Proportional gain
    float 				Ki;            // Integral gain
    float 				Kd;            // Derivative gain
    float 				setpoint;      // Desired value (SP)
    float 				input;         // Current process variable (PV)
    float 				output;        // Calculated control output (u(t))
    float 				output_min;    // Minimum output limit
    float 				output_max;    // Maximum output limit

    // Internal state variables
    float 				error;         // Current error (SP - PV)
    float 				last_error;    // Error from the previous iteration
    float 				integral;      // Accumulated integral error
    float 				derivative;    // Calculated derivative term
    float 				last_input;    // Last process variable value (for derivative calculation)
    uint8_t first_run;      // Flag to handle first iteration for derivative
} PIDController_TypeDef;

// Function Prototypes
extern	void	PID_Init(PIDController_TypeDef *pid, float kp, float ki, float kd, float min_out, float max_out);
extern	void	PID_SetSetpoint(PIDController_TypeDef *pid, float setpoint);
extern	void	PID_SetTunings(PIDController_TypeDef *pid, float kp, float ki, float kd);
extern	float	PID_Compute(PIDController_TypeDef *pid, float input, float dt); // Calculate PID output
extern	void	PID_Reset(PIDController_TypeDef *pid); // Reset integral and derivative terms


#endif /* MODULES_PID_PID_H_ */
