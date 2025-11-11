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
 * pid.c
 *
 *  Created on: Nov 11, 2025
 *      Author: fil
 */
#include "main.h"
#include "../../kernel/A.h"
#include "../../kernel/A_exported_functions.h"

#include "pid.h"
#include <math.h> // For fabs() if needed for anti-windup

PIDController_TypeDef	*pid_drv_ptr;

// Set a new setpoint
void PID_SetSetpoint(PIDController_TypeDef *pid, float setpoint)
{
    pid->setpoint = setpoint;
}

// Update PID tuning parameters (gains)
void PID_SetTunings(PIDController_TypeDef *pid, float kp, float ki, float kd)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
}

// Compute the PID output based on the current input and time step dt
float PID_Compute(PIDController_TypeDef *pid, float input)
{
    // Avoid division by zero or negative time step
    if (pid->dt <= 0.0f)
        return pid->output;

    pid->input = input;
    pid->error = pid->setpoint - pid->input;

    // --- Proportional Term ---
    float proportional = pid->Kp * pid->error;

    // --- Integral Term ---
    pid->integral += pid->error * pid->dt;
    // Anti-windup: Limit integral term if output is saturated
    // Check if the output *would* be saturated based on P and I terms alone
    float potential_output = proportional + pid->Ki * pid->integral;
    if (potential_output > pid->output_max && pid->error > 0) {
        pid->integral -= pid->error * pid->dt; // Don't add to integral if already saturated high
    } else if (potential_output < pid->output_min && pid->error < 0) {
        pid->integral -= pid->error * pid->dt; // Don't subtract from integral if already saturated low
    }
    float integral_term = pid->Ki * pid->integral;

    // --- Derivative Term ---
    // Use derivative of the *input* (negative derivative of error) to avoid "derivative kick"
    if (pid->first_run) {
        pid->derivative = 0.0f; // Avoid large derivative on first run
        pid->first_run = 0;
    } else {
        pid->derivative = -(pid->input - pid->last_input) / pid->dt; // d(input)/dt
    }
    float derivative_term = pid->Kd * pid->derivative;

    // Calculate the total output
    pid->output = proportional + integral_term + derivative_term;

    // --- Output Saturation ---
    if (pid->output > pid->output_max) {
        pid->output = pid->output_max;
    } else if (pid->output < pid->output_min) {
        pid->output = pid->output_min;
    }

    // Store current input for next derivative calculation
    pid->last_input = pid->input;

    return pid->output;
}

// Reset the PID controller state (integral, derivative, error history)
void PID_Reset(PIDController_TypeDef *pid)
{
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->last_input = 0.0f;
    pid->output = 0.0f;
    pid->first_run = 1;
}

// Initialize the PID controller structure
ITCM_AREA_CODE void PID_Set(PIDController_TypeDef *pid, float kp, float ki, float kd, float min_out, float max_out)
{
	/* user params */
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->setpoint = 0.0f;
    pid->input = 0.0f;
    pid->output = 0.0f;
    pid->output_min = min_out;
    pid->output_max = max_out;
}

ITCM_AREA_CODE uint32_t	PID_register(PIDController_TypeDef *pid)
{
PIDController_TypeDef *eptr, *pre_eptr;

	if ( pid->periodic_timer == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( pid->User_Callback == NULL)
		return DRIVER_REQUEST_FAILED;
	if ( pid_drv_ptr == NULL)
	{
		pid_drv_ptr = pid;
		pid->next_pid = NULL;
	}
	else
	{
		eptr = pre_eptr = pid_drv_ptr;
		while(eptr->next_pid != NULL)
		{
			pre_eptr = eptr;
			eptr = (PIDController_TypeDef *)eptr->next_pid;
		}
		pre_eptr->next_pid = (uint32_t *)pid;
		pid->next_pid = NULL;
	}
	pid->process = get_current_process();
	pid->periodic_timer->User_Callback = pid->User_Callback;
	periodic_timer_register(pid->periodic_timer);
	periodic_timer_start(pid->periodic_timer);
    /* internal variables */
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->last_input = 0.0f; // Initialize last_input for derivative calculation
    pid->first_run = 1; // Set flag for first run
	return 0;
}
