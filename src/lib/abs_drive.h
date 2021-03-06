/**
*
*  @file abs_drive.h
*
*  @brief it allows the robot to drive forword and backward
*
*  @param dir Tells the robot what direction to go
*
*  @param dist_method tells the robot how it should know when to stop
*
*  @param dist tells the robot how far to go
*
*  @param speed tells the robot how fast to go
*
*  @param stop_at_end tells the robot if it should stop when it gets to were it needs to go or not
*
*  @return returns nothing
*
*  @copyright Copyright 2013, Got Robot? FTC Team 5037
*
*/


#ifndef ABS_DRIVE_H
#define ABS_DRIVE_H

#include "abs_gyro_drive.h"
#include "abs_sonar_drive.h"
#include "abs_dlog.h"
#include "abs_reset_angle_sensor.h"
#include "abs_get_angle_sensor_val.h"
#include "abs_move_utils.h"
#include "abs_stall_detect.h"
#include "abs_reset_stall_detect.h"

void abs_drive(e_drive_direction dir, e_move_stopping_method dist_method, int dist, int speed, bool stop_at_end, e_drive_type drive_type, e_slow_down_at_end slowDown, E_STALL_ACTION stall_action)
{
	/** logging constants */
	const string speed_str = "speed";
	const string dist_str = "dist";
	const string rel_bpu_str = "rel TU";
	const string rel_asu_str = "rel ASU";
	const string bearing_ac2_str = "g_bearing_ac2";

	int last_heading = g_const_heading;

	//log the paramiters
	switch(dist_method)
	{
	case E_IR_DETECT:
		abs_dlog(__FILE__ , "IR enter", speed_str, speed, dist_str, dist, bearing_ac2_str, g_bearing_ac2, rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));
		break;
	case E_IR_DETECT2:
		abs_dlog(__FILE__ , "IR2 enter", speed_str, speed, dist_str, dist, bearing_ac2_str, g_bearing_ac2, rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));
		break;
	case E_ANGLE:
		abs_dlog(__FILE__ , "angle enter", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));
		break;
	case E_TIME:
		abs_dlog(__FILE__ , "time enter", speed_str, speed, dist_str, dist, "time", time1[T1], rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));
		break;
	case E_OPTICAL:
		abs_dlog(__FILE__ , "Optical enter", speed_str, speed, dist_str, dist, "g_calibrated_optical_threshold_val", g_calibrated_optical_threshold_val, rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));
		break;
	}
	int i = 0;
	nMotorEncoder(ENCODER_SIDE)= 0;
	if(g_gyro_inherit==true) g_gyro_inherit = false;
	else g_rel_heading = 0;

	//------------------------
	// time stopping method
	//------------------------
	if(dist_method == E_TIME)
	{
		ClearTimer(T1);
		//wait a specified time to stop
		while(time1[T1] < dist)
		{
			if(drive_type == GYRO)
			{
				abs_gyro_drive(speed,dir);
			}
			else if(drive_type == WALL_SONAR)
			{
				abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
			}
			/** No gyro correction*/
			else
			{
				if(dir == FORWARD)
				{
					motor[left_motor] = speed;
					motor[right_motor] = speed;
				}
				else
				{
					motor[left_motor] = -speed;
					motor[right_motor] = -speed;
				}
			}
		}
		abs_dlog(__FILE__ ,"time break", speed_str, speed, dist_str, dist, "time", time1[T1], rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));
	}
	//------------------------
	// encoder stopping method
	//------------------------
	else if(dist_method == E_DEGREES)
	{
		//keep going until we get an encoder value
		while(i<5)
		{
			if(abs(nMotorEncoder(ENCODER_SIDE)) > dist)//distance_to_encoder_derees(dist))
			{
				i++;
				PlayTone(200,20);
			}
			if(drive_type == GYRO)
			{
				abs_gyro_drive(speed,dir);
			}
			else if(drive_type == WALL_SONAR)
			{
				abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
			}
			/** No gyro correction*/
			else
			{
				if(dir == FORWARD)
				{
					motor[left_motor] = speed;
					motor[right_motor] = speed;
				}
				else
				{
					motor[left_motor] = -speed;
					motor[right_motor] = -speed;
				}
			}
		}
		abs_dlog(__FILE__ ,"degree break",speed_str, speed, dist_str, dist, "motor encoder", nMotorEncoder(right_motor), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));
	}
	//------------------------
	// IR stopping method
	//------------------------
	//drive until we see the ir
	else if(dist_method == E_IR_DETECT)
	{
		int total_dist = 150;
		int half_dist = 100;

		total_dist = 75;

		abs_reset_angle_sensor_val(SOFT_RESET);//was soft reset
		abs_dlog(__FILE__ ,"reset angle", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));

		if(dir == FORWARD)
		{
			//wait intil we get past the specified area or we detect the IR
			while(true)
			{
				if(abs_get_angle_sensor_val(RELATIVE_BPU) > total_dist)
				{
					abs_dlog(__FILE__ ,"angle break", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));
					break;
				}
				if(abs_get_angle_sensor_val(RELATIVE_BPU) < half_dist)
				{
					if(!((g_bearing_ac2 >= dist - 1) || (g_bearing_ac2 == 0)))
					{
						abs_dlog(__FILE__ ,"IR break", speed_str, speed, dist_str, dist, bearing_ac2_str, g_bearing_ac2, rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));
						break;
					}
				}
				else
				{
					if(!((g_bearing_ac2 >= dist) || (g_bearing_ac2 == 0)))
					{
						abs_dlog(__FILE__,"IR break", speed_str, speed, dist_str, dist, bearing_ac2_str, g_bearing_ac2, rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));
						break;
					}
				}
				//drive
				if(drive_type == GYRO)
				{
					abs_gyro_drive(speed,dir);
				}
				else if(drive_type == WALL_SONAR)
				{
					abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
				}

				/** No gyro correction*/
				else
				{
					motor[left_motor] = speed;
					motor[right_motor] = speed;
				}
			}
			//g_screen_state = S_TIME_SHOW;
			g_debug_time_1 = nPgmTime;
		}
		abs_reset_angle_sensor_val(SOFT_RESET);//was soft reset
		abs_dlog(__FILE__ ,"reset angle", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));

		if(dir == FORWARD)
		{
			while(g_ir_bearing2 > dist)
			{
				if(drive_type == GYRO)
				{
					abs_gyro_drive(speed,dir);
				}
				else if(drive_type == WALL_SONAR)
				{
					abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
				}
				/** No gyro correction*/
				else
				{
					motor[left_motor] = speed;
					motor[right_motor] = speed;
				}
			}
		}
		else
		{
			while(g_ir_bearing2 < dist)
			{
				if(drive_type == GYRO)
				{
					abs_gyro_drive(speed,dir);
				}
				else if(drive_type == WALL_SONAR)
				{
					abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
				}
				/** No gyro correction*/
				else
				{
					motor[left_motor] = -speed;
					motor[right_motor] = -speed;
				}
			}
		}
	}
	//------------------------
	// angle sensor stopping method
	//------------------------
	//Tells the robot to stop based on the real distence it has driven determined by the angle sensor
	else if(dist_method == E_ANGLE)
	{
		int pre_dist = 0;
		if(g_reset_angle_record == false) pre_dist = abs_get_angle_sensor_val(RELATIVE_TU);
		abs_reset_angle_sensor_val(SOFT_RESET);//was soft reset
		g_reset_angle_record = true;

		abs_dlog(__FILE__ ,"reset angle", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));

		while(abs_get_angle_sensor_val(RELATIVE_TU) < (dist+pre_dist)/* && abs_stall_detect(abs_get_angle_sensor_val(RELATIVE_TU) != true*/)
		{
			if(drive_type == GYRO)
			{
				if(slowDown == SLOW_DOWN)
				{
					abs_gyro_drive(adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_TU)),dir);
					stall_detect(stall_action, RELATIVE_TU)
				}
				else
				{
					abs_gyro_drive(speed, dir);
				}
			}

			else if(drive_type == WALL_SONAR)
			{
				if(abs_get_angle_sensor_val(RELATIVE_TU)<((dist+pre_dist)-((dist+pre_dist)/5)))
				{
					abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
					stall_detect(stall_action, RELATIVE_TU)
				}
				else
				{
					abs_gyro_drive(speed, dir);
					stall_detect(stall_action, RELATIVE_TU)
				}
			}

			/** No gyro correction*/
			else
			{
				if(slowDown == SLOW_DOWN)
				{
					if(dir == FORWARD)
					{
						motor[left_motor] = adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_TU));
						motor[right_motor] = adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_TU));
					}
					else
					{
						motor[left_motor] = -adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_TU));
						motor[right_motor] = -adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_TU));
					}
				}
				else
				{
					if(dir == FORWARD)
					{
						motor[left_motor] = speed;
						motor[right_motor] = speed;
					}
					else
					{
						motor[left_motor] = -speed;
						motor[right_motor] = -speed;
					}

				}
			}
			/*if(abs_stall_detect(abs_get_angle_sensor_val(RELATIVE_TU)))
			{
			PlayTone(300, 50);
			wait10Msec(70);
			PlayTone(300, 50);
			wait10Msec(70);
			PlayTone(300, 50);
			wait10Msec(70);
			}*/
		}

		abs_dlog(__FILE__ ,"angle break", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_TU));
	}
	//================
	// OPTICAL
	//================
	else if(dist_method == E_OPTICAL)
	{
		bool optical_fail = false;
		abs_reset_angle_sensor_val(SOFT_RESET);//was soft reset

		abs_dlog(__FILE__ ,"reset angle", speed_str, speed, dist_str, dist, rel_asu_str, abs_get_angle_sensor_val(RELATIVE_ASU), rel_bpu_str, abs_get_angle_sensor_val(RELATIVE_BPU));

		int max_optical_detected = 0;
		while(true)
		{
			//finds out what the highest value of the optical sensor was
			max_optical_detected = max(max_optical_detected, g_optical_sensor);

			if(g_optical_sensor>g_calibrated_optical_threshold_val&&abs_get_angle_sensor_val(RELATIVE_ASU)<g_optical_move_min_dist)
			{
				abs_dlog(__FILE__ ,"Premature Optical detection", "Min BPU", g_optical_move_min_dist, "Actual BPU when detected", abs_get_angle_sensor_val(RELATIVE_BPU), "Optical Threshold", g_calibrated_optical_threshold_val, "Optical Value detected", max_optical_detected);
				optical_fail = true;
			}
			if(g_optical_sensor>g_calibrated_optical_threshold_val&&optical_fail==false)
			{
				abs_dlog(__FILE__ ,"optical break", speed_str, speed, dist_str, dist, "g_calibrated_optical_threshold", g_calibrated_optical_threshold_val, "g_optical_sensor", g_optical_sensor);
				break;
			}
			else if (abs_get_angle_sensor_val(RELATIVE_BPU) > dist)
			{
				abs_dlog(__FILE__ ,"angle break", "speed", speed, "max distance", dist, "Optical Threshold", g_calibrated_optical_threshold_val, "optical Value detected", max_optical_detected);
				break;
			}

			if(drive_type == GYRO)
			{
				abs_gyro_drive(adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_BPU)),dir);
			}

			else if(drive_type == WALL_SONAR)
			{
				abs_sonar_drive(speed, dir, g_sonar_wall_dist, g_sonar3);
			}
			/** No gyro correction*/
			else
			{
				if(dir == FORWARD)
				{
					motor[left_motor] = adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_BPU));
					motor[right_motor] = adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_BPU));
				}
				else
				{
					motor[left_motor] = -adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_BPU));
					motor[right_motor] = -adjusted_drive_speed(speed, dist, abs_get_angle_sensor_val(RELATIVE_BPU));
				}
			}
		}
	}
	else
	{
		motor[left_motor] = 0;
		motor[right_motor] = 0;
		while(true)
		{
			PlayTone(200,20);
			wait1Msec(500);
		}
	}

	//------------------------
	// Stop
	//------------------------
	//if the robot was set to stop at the end and not cost then stop
	if(stop_at_end)
	{
		motor[left_motor] = 0;
		motor[right_motor] = 0;
	}

	abs_reset_stall_detect();
	g_debug_time_2 = nPgmTime;

	int rel_asu = abs_get_angle_sensor_val(RELATIVE_ASU);
	int rel_bpu = abs_get_angle_sensor_val(RELATIVE_BPU);
	abs_dlog(__FILE__ ,"exit", speed_str, speed, dist_str, dist, rel_asu_str, rel_asu, rel_bpu_str, rel_bpu);

	g_const_heading = last_heading;
}

#endif /* !ABS_DRIVE_H */
