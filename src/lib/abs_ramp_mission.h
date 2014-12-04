/**
*
*  @file abs_ramp_mission.h
*
*  @brief all autonomous movements resulting from starting on the ramp
*
*  @param none N/A
*
*  @return Returns nothing
*
*  @copyright Copyright 2014, got robot? FTC Team 5037
*
*/

#ifndef ABS_RAMP_MISSION_H
#define ABS_RAMP_MISSION_H

#include "abs_drive.h"
#include "abs_turn.h"
#include "abs_second_objective.h"
#include "abs_IR_center_read.h"

void abs_ramp_mission()
{
	switch(g_input_array[FIRST_OBJECTIVE])
	{
	case 0:

		wait1Msec(STARTING_DELAY*DELAY_MULTIPLICATION_FACTOR);
		abs_second_objective(SECOND_STOP);
		break; //STOP
	case 1:

		if(g_start_heading_forward==true)
		{
			StartTask(abs_IR_center_read);
			abs_drive(FORWARD, E_ANGLE, 480, 40, true, GYRO);
			abs_turn(CLOCKWISE, POINT, TURN, 180, 60);
			servo[goal_claw] = g_goal_claw_down;
		}
		else
		{
			abs_drive(BACKWARD, E_ANGLE, 500, 40, true, GYRO);
			servo[goal_claw] = g_goal_claw_down;
		}
		wait1Msec(STARTING_DELAY*DELAY_MULTIPLICATION_FACTOR);
		abs_second_objective(SECOND_ROLLGOAL1);
		break; //ROLLING GOAL 1
	case 2:

		wait1Msec(STARTING_DELAY*DELAY_MULTIPLICATION_FACTOR);
		abs_second_objective(SECOND_CENTER);
		break; //CENTER GOAL
	case 3:

		if(g_start_heading_forward==true)
		{
		}
		else
		{
		}
		wait1Msec(STARTING_DELAY*DELAY_MULTIPLICATION_FACTOR);
		abs_second_objective(SECOND_ROLLGOAL2);
		break; //ROLLING GOAL 2
	}
}

#endif /* !ABS_RAMP_MISSION_H */
