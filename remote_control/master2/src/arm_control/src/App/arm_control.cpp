#include "App/arm_control.h"

extern OD_Motor_Msg rv_motor_msg[10];

float arx_arm::ramp(float goal, float current, float ramp_k)
{
    float retval = 0.0f;
    float delta = 0.0f;
    delta = goal - current;
    if (delta > 0)
    {
        if (delta > ramp_k)
        {  
                current += ramp_k;
        }   
        else
        {
                current += delta;
        }
    }
    else
    {
        if (delta < -ramp_k)
        {
                current += -ramp_k;
        }
        else
        {
                current += delta;
        }
    }	
    retval = current;
    return retval;
}

arx_arm::arx_arm(int CONTROL_MODE)
{
    control_mode=CONTROL_MODE;

    arx5_cmd.reset = false;
    arx5_cmd.x = 0.0;
    arx5_cmd.y = 0.0;
    arx5_cmd.z = 0.01; 
    arx5_cmd.base_yaw = 0;
    arx5_cmd.gripper = 0;
    arx5_cmd.gripper_roll = 0;
    arx5_cmd.waist_pitch = 0;
    arx5_cmd.waist_yaw = 0;
    arx5_cmd.mode = FORWARD;

    solve.solve_init();
}

void arx_arm::get_curr_pos()
{
    solve.get_joint(current_pos,current_torque,rv_motor_msg,current_normal);
}

void arx_arm::set_loop_rate(const unsigned int rate)
{
    loop_rate = rate;
    return;
}

command arx_arm::get_cmd()
{

    if (play.is_playing)
    {
        return arx5_cmd;
    }
    else
    {
        arm_torque_mode();
        arm_reset_mode();
        arm_get_pos();
                                    
    }
    return arx5_cmd;
}

void arx_arm::update_real(command cmd)
{
    solve.mk1(arx5_cmd,current_pos,target_pos,is_teach_mode);
    if(is_starting)
    {
        motor_control_cmd=false;
        init_step();
    }
    else
    {
        if(control_mode == 0  ||  control_mode == 1)
        {
                solve.mk(arx5_cmd,current_pos,target_pos,is_teach_mode);

                if(teach2pos_returning ){
                    is_starting=1;
                }else{
                    is_starting=0;
                    temp_init=0;
                    solve.control(target_pos,motor_control_cmd,is_teach_mode,current_normal);
                }
        }
    }
    return;
}

void arx_arm::init_step()
{
   temp_init++;
   if(temp_init>2)
   {
            for (int i = 0; i < 7; i++)
            {
                target_pos[i] = ramp(0, target_pos[i], 0.003);
            }
            bool all_positions_within_threshold = true;
            for (int i = 0; i < 7; i++) {
                if (std::fabs(current_pos[i]) >= 0.1) {
                    all_positions_within_threshold = false;
                    calc_init=0;
                    break;
                }
            }
            if (all_positions_within_threshold) {
                calc_init++;
                if(calc_init>300){
                    is_starting=0;
                    is_arrived=1;
                }

            } else {
                is_arrived =0;
                is_init=2;
                teach2pos_returning = false ;
            }

            if(init_kp<150)
            init_kp+= 1.0f; //1.0f
            if(init_kd< 12) 
            init_kd+=0.2f;  //0.2
            // std::cout << motor_control_cmd << is_teach_mode << current_normal << std::endl; //001;
            solve.control2(target_pos,motor_control_cmd,is_teach_mode,current_normal);


   }
    else
    {
        for (int i = 0; i < 7; i++)
        {
            target_pos[i] = current_pos[i];
        }
    }
    cmd_init();
    ROS_WARN(">>>is_init>>>");

      
}


int arx_arm::rosGetch()
{ 
    static struct termios oldTermios, newTermios;
    tcgetattr( STDIN_FILENO, &oldTermios);          
    newTermios = oldTermios; 
    newTermios.c_lflag &= ~(ICANON);                      
    newTermios.c_cc[VMIN] = 0; 
    newTermios.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSANOW, &newTermios);  

    int keyValue = getchar(); 

    tcsetattr( STDIN_FILENO, TCSANOW, &oldTermios);  
    return keyValue;
}


void arx_arm::getKey(char key_t) {
   int wait_key=100;

    if(key_t == 'w')
    arx5_cmd.key_x = arx5_cmd.key_x_t=1;
    else if(key_t == 's')
    arx5_cmd.key_x = arx5_cmd.key_x_t=-1;
    else arx5_cmd.key_x_t++;
    if(arx5_cmd.key_x_t>wait_key)
    arx5_cmd.key_x = 0;

    if(key_t == 'a')
    arx5_cmd.key_y =arx5_cmd.key_y_t= 1;
    else if(key_t == 'd')
    arx5_cmd.key_y =arx5_cmd.key_y_t= -1;
    else if(key_t == 'R')
    arx5_cmd.key_y =arx5_cmd.key_y_t= -1;
    else if(key_t == 'L')
    arx5_cmd.key_y =arx5_cmd.key_y_t= 1;
    else arx5_cmd.key_y_t++;
    if(arx5_cmd.key_y_t>wait_key)
    arx5_cmd.key_y = 0;   

    if(key_t == 'U')
    arx5_cmd.key_z =arx5_cmd.key_z_t= 1;
    else if(key_t == 'D')
    arx5_cmd.key_z =arx5_cmd.key_z_t= -1;
    else arx5_cmd.key_z_t++;
    if(arx5_cmd.key_z_t>wait_key)
    arx5_cmd.key_z = 0;

    if(key_t == 'q')
    arx5_cmd.key_base_yaw =arx5_cmd.key_base_yaw_t= 1;
    else if(key_t == 'e')
    arx5_cmd.key_base_yaw =arx5_cmd.key_base_yaw_t= -1;
    else arx5_cmd.key_base_yaw_t++;
    if(arx5_cmd.key_base_yaw_t>wait_key)
    arx5_cmd.key_base_yaw = 0;

    if(key_t == 'r')
    arx5_cmd.key_reset =arx5_cmd.key_reset_t=1;
    else arx5_cmd.key_reset_t++;
    if(arx5_cmd.key_reset_t>wait_key)
    arx5_cmd.key_reset =0;

    if(key_t == 'i')
    arx5_cmd.key_i =arx5_cmd.key_i_t=1;
    else arx5_cmd.key_i_t++;
    if(arx5_cmd.key_i_t>wait_key)
    arx5_cmd.key_i =0;

    if(key_t == 'p')
    arx5_cmd.key_p =arx5_cmd.key_p_t=1;
    else arx5_cmd.key_p_t++;
    if(arx5_cmd.key_p_t>wait_key)
    arx5_cmd.key_p =0;

    if(key_t == 'o')
    arx5_cmd.key_o =arx5_cmd.key_o_t=1;
    else arx5_cmd.key_o_t++;
    if(arx5_cmd.key_o_t>wait_key)
    arx5_cmd.key_o =0;

    if(key_t == 'c')
    arx5_cmd.key_c =arx5_cmd.key_c_t=1;
    else arx5_cmd.key_c_t++;
    if(arx5_cmd.key_c_t>wait_key)
    arx5_cmd.key_c =0;

    if(key_t == 't')
    arx5_cmd.key_t =arx5_cmd.key_t_t=1;
    else arx5_cmd.key_t_t++;
    if(arx5_cmd.key_t_t>wait_key)
    arx5_cmd.key_t =0;

    if(key_t == 'g')
    arx5_cmd.key_g =arx5_cmd.key_g_t=1;
    else arx5_cmd.key_g_t++;
    if(arx5_cmd.key_g_t>wait_key)
    arx5_cmd.key_g =0; 

    if(key_t == 'm')
    arx5_cmd.key_m =arx5_cmd.key_m_t=1;
    else arx5_cmd.key_m_t++;
    if(arx5_cmd.key_m_t>wait_key)
    arx5_cmd.key_m =0;
// r p y
    if(key_t == 'n')
    arx5_cmd.key_roll =arx5_cmd.key_roll_t= 1;
    else if(key_t == 'm')
    arx5_cmd.key_roll =arx5_cmd.key_roll_t= -1;
    else arx5_cmd.key_roll_t++;
    if(arx5_cmd.key_roll_t>wait_key)
    arx5_cmd.key_roll = 0;  

    if(key_t == 'l')
    arx5_cmd.key_pitch =arx5_cmd.key_pitch_t= 1;
    else if(key_t == '.')
    arx5_cmd.key_pitch =arx5_cmd.key_pitch_t= -1;
    else arx5_cmd.key_pitch_t++;
    if(arx5_cmd.key_pitch_t>wait_key)
    arx5_cmd.key_pitch = 0;  

    if(key_t == ',')
    arx5_cmd.key_yaw =arx5_cmd.key_yaw_t= 1;
    else if(key_t == '/')
    arx5_cmd.key_yaw =arx5_cmd.key_yaw_t= -1;
    else arx5_cmd.key_yaw_t++;
    if(arx5_cmd.key_yaw_t>wait_key)
    arx5_cmd.key_yaw = 0;  

    if(key_t == 'u')
    arx5_cmd.key_u =arx5_cmd.key_u_t=1;
    else arx5_cmd.key_u_t++;
    if(arx5_cmd.key_u_t>wait_key)
    arx5_cmd.key_u =0;

    if(key_t == 'j')
    arx5_cmd.key_j =arx5_cmd.key_j_t=1;
    else arx5_cmd.key_j_t++;
    if(arx5_cmd.key_j_t>wait_key)
    arx5_cmd.key_j =0;

    if(key_t == 'h')
    arx5_cmd.key_h =arx5_cmd.key_h_t=1;
    else arx5_cmd.key_h_t++;
    if(arx5_cmd.key_h_t>wait_key)
    arx5_cmd.key_h =0;

    if(key_t == 'k')
    arx5_cmd.key_k =arx5_cmd.key_k_t=1;
    else arx5_cmd.key_k_t++;
    if(arx5_cmd.key_k_t>wait_key)
    arx5_cmd.key_k =0;

    if(key_t == 'v')
    arx5_cmd.key_v =arx5_cmd.key_v_t=1;
    else arx5_cmd.key_v_t++;
    if(arx5_cmd.key_v_t>wait_key)
    arx5_cmd.key_v =0;

    if(key_t == 'b')
    arx5_cmd.key_b =arx5_cmd.key_b_t=1;
    else arx5_cmd.key_b_t++;
    if(arx5_cmd.key_b_t>wait_key)
    arx5_cmd.key_b =0;
    
    return ;
}

void arx_arm::arm_torque_mode()
{
        if( (Teleop_Use()->buttons_[5] == 1 && !button5_pressed)  || ( arx5_cmd.key_i ==1 &&  !button5_pressed) )
        {
            button5_pressed = true;
            if (!is_torque_control) // teleoperation
            {
                init_kp=10,init_kp_4=20,init_kd=init_kd_4=init_kd_6=init_kp_4=0; 
                is_teach_mode = true;
                is_torque_control = true;
                for (int i = 0; i < 6; i++)
                {
                    prev_target_pos[i] = target_pos[i];
                }
            }else
            {   
                is_teach_mode = false;
                is_torque_control = false; // switch between teleoperation and torque control
                teach2pos_returning = true; // gradually back to initial pose
                for (int i = 0; i < 6; i++)
                {
                    target_pos[i] = current_pos[i];
                }
                
            }
        }
           
 
        if (button5_pressed && (!Teleop_Use()->buttons_[5] && !arx5_cmd.key_i ))
            button5_pressed = false; 

}

void arx_arm::arm_replay_mode()
{
 // Replay 
    if ((Teleop_Use()->buttons_[0] == 1 && !button0_pressed) ||  (  arx5_cmd.key_p ==1  && !button0_pressed) )
    {
            if(!play.is_playing)
            {
            if(play_flag==0 ){
                    is_starting=1;
                    init_kp=0,init_kp_4=0,init_kd=init_kd_4=init_kd_6=init_kp_4=0; 
                    play_flag=2;

                arx5_cmd.reset = true;
                arx5_cmd.waist_pitch  = arx5_cmd.waist_pitch_t  =arx5_cmd.control_pit   = joy_pitch_t  =joy_pitch      =0      ;
                arx5_cmd.x            = arx5_cmd.x_t            =arx5_cmd.control_x     = joy_x_t      =joy_x          =0      ;
                arx5_cmd.y            = arx5_cmd.y_t            =arx5_cmd.control_y     = joy_y_t      =joy_y          =0      ;
                arx5_cmd.z            = arx5_cmd.z_t            =arx5_cmd.control_z     = joy_z_t      =joy_z          =0      ;      
                arx5_cmd.base_yaw     = arx5_cmd.base_yaw_t     =      0 ;
                arx5_cmd.gripper_roll = arx5_cmd.gripper_roll_t =arx5_cmd.control_roll  = joy_roll_t   =joy_roll       =0      ;
                arx5_cmd.waist_yaw    = arx5_cmd.waist_yaw_t    =arx5_cmd.control_yaw   = joy_yaw_t    =joy_yaw        =0      ;
                arx5_cmd.mode = FORWARD;                

                }else{//执行动作
                    play_file_list=play.getFilesList(ros::package::getPath("arm_control") + "/saved_record");
                    play.play_start_all(target_pos,current_pos,play_file_list);
                    play.repeat_stop_flag = false;
        
                }  
            }
            else
            {
                play.repeat_stop_flag = true;
            }
        button0_pressed = true;
        ROS_ERROR("button0_pressed\n");
    }
    if (button0_pressed && (!Teleop_Use()->buttons_[0] && !arx5_cmd.key_p))
    {
        // play_flag=0;
        button0_pressed = false;
        ROS_ERROR("button0_pressed false\n");
    }

}

void arx_arm::arm_reset_mode(){
///////////////////////////////////////////////////////////
        if((Teleop_Use()->buttons_[1] == 1)|| (arx5_cmd.key_reset==1)) // reset
        {      
            arx5_cmd.base_yaw_t=ramp(0.0, arx5_cmd.base_yaw_t, 0.01);    
            arx5_cmd.gripper_t=0;      
            arx5_cmd.control_roll=ramp(0.0, arx5_cmd.control_roll, 0.1); 
            arx5_cmd.control_pit=ramp(0.0, arx5_cmd.control_pit, 0.1);
            arx5_cmd.control_yaw=ramp(0.0, arx5_cmd.control_yaw, 0.1);   

            arx5_cmd.control_x=ramp(0.0, arx5_cmd.control_x, 0.0006); 

            if(arx5_cmd.x <0.01){
            arx5_cmd.control_y=ramp(0.0, arx5_cmd.control_y, 0.0006);
            arx5_cmd.control_z=ramp(0.0, arx5_cmd.control_z, 0.0006);
            }
            joy_yaw=joy_pitch=joy_roll=0;
        }
    
            arx5_cmd.reset = false;

}

void arx_arm::arm_get_pos(){

            arx5_cmd.base_yaw_t += (Teleop_Use()->axes_[0]/100.0f + arx5_cmd.key_base_yaw/100.0f);
            // ROS_INFO("arx5_cmd.base_yaw_t>%f,Teleop_Use()->axes_[0]>%f,arx5_cmd.key_base_yaw>%f",arx5_cmd.base_yaw_t,Teleop_Use()->axes_[0],arx5_cmd.key_base_yaw);
            // 手柄通道+键盘通道+ROS通道  
            if(abs(arx5_ros_cmd.x)<0.1)
                ros_move_k_x=500;
            else ros_move_k_x=100;

            if(abs(arx5_ros_cmd.y)<0.1)
                ros_move_k_y=500;
            else ros_move_k_y=100;

            if(abs(arx5_ros_cmd.z)<0.1)
                ros_move_k_z=500;
            else ros_move_k_z=100;

            arx5_cmd.control_x +=(arx5_cmd.key_x/2000.0f  +arx5_ros_cmd.x/ros_move_k_x);
            arx5_cmd.control_y +=(arx5_cmd.key_y/2000.0f  +arx5_ros_cmd.y/ros_move_k_y);
            arx5_cmd.control_z +=(arx5_cmd.key_z/2000.0f  +arx5_ros_cmd.z/ros_move_k_z);

            if (Teleop_Use()->buttons_[2] == 1){ //手柄控制 - 键位组合
            arx5_cmd.control_pit -= (Teleop_Use()->axes_[7]/100.0f+arx5_cmd.key_pitch/1000.0f);
            arx5_cmd.control_yaw   += (Teleop_Use()->axes_[6]/100.0f+arx5_cmd.key_yaw/1000.0f);
            }else
            { //arx5_ros_cmd
            arx5_cmd.control_pit += (arx5_ros_cmd.pitch/1000.0f -arx5_cmd.key_pitch/100.0f);
            arx5_cmd.control_yaw   += (arx5_ros_cmd.yaw/1000.0f   +arx5_cmd.key_yaw  /100.0f);
            }
            arx5_cmd.control_roll  += (-Teleop_Use()->axes_[6]/100.0f-arx5_cmd.key_roll/100.0f + arx5_ros_cmd.roll/1000.0f);
            
            joy_x_t = arx5_cmd.control_x      + magic_pos[0];
            joy_y_t = arx5_cmd.control_y      + magic_pos[1];
            joy_z_t = arx5_cmd.control_z      + magic_pos[2];
            joy_pitch_t=arx5_cmd.control_pit  + magic_angle[0];
            joy_yaw_t  =arx5_cmd.control_yaw  + magic_angle[1];
            joy_roll_t =arx5_cmd.control_roll + magic_angle[2];
            
            //限位
            limit_pos();

            arx5_cmd.reset = true;
            float reset_temp_k=0.001;

                arx5_cmd.x            = ramp(joy_x_t, arx5_cmd.x, reset_temp_k);  
                arx5_cmd.y            = ramp(joy_y_t, arx5_cmd.y, reset_temp_k);
                arx5_cmd.z            = ramp(joy_z_t, arx5_cmd.z, reset_temp_k);
                arx5_cmd.base_yaw     = ramp(arx5_cmd.base_yaw_t, arx5_cmd.base_yaw, 0.009);
                arx5_cmd.gripper_roll = ramp(joy_roll_t, arx5_cmd.gripper_roll, 0.01);
                arx5_cmd.waist_pitch  = ramp(joy_pitch_t, arx5_cmd.waist_pitch, 0.01);
                arx5_cmd.waist_yaw    = ramp(joy_yaw_t, arx5_cmd.waist_yaw, 0.01);
                arx5_cmd.mode = FORWARD;
   

}

void arx_arm::arm_teach_mode(){

// Record settings
            if (arx5_cmd.key_t == 1 && !button_teach)
            {
                if(!is_recording){
                    is_recording  = true;
                    is_teach_mode = true;
                }
                else
                {
                    is_recording  = false;
                    is_teach_mode = false;
                    out_teach_path = ros::package::getPath("arm_control") + "/teach_record/out.txt";
                    play.end_record(out_teach_path);
                    // current_normal = false;
                }
                button_teach = true;
            }
            if (button_teach && !arx5_cmd.key_t)
                button_teach = false;

            // Replay 
            if ((arx5_cmd.key_g == 1) && (!is_recording) && (!button_teach))
            {
                std::vector<std::string> arx;
                    play.play_start(ros::package::getPath("arm_control") + "/teach_record/out.txt",target_pos,current_pos,arx);
                button_replay = true;
                // ROS_ERROR("debug");
            }
            if (button_replay && !arx5_cmd.key_g)
                button_replay = false;
 
}

void arx_arm::limit_pos()
{
        joy_x_t = limit<float>(joy_x_t, lower_bound_waist[0], upper_bound_waist[0]);
        joy_y_t = limit<float>(joy_y_t, lower_bound_waist[1], upper_bound_waist[1]);
        joy_z_t = limit<float>(joy_z_t, lower_bound_waist[2], upper_bound_waist[2]);
        joy_pitch_t = limit<float>(joy_pitch_t, lower_bound_pitch, upper_bound_pitch);
        joy_yaw_t   = limit<float>(joy_yaw_t, lower_bound_yaw, upper_bound_yaw);
        joy_roll_t  = limit<float>(joy_roll_t, lower_bound_sim[ROLL], upper_bound_sim[ROLL]);

        arx5_cmd.control_x = limit<float>(arx5_cmd.control_x, lower_bound_waist[0], upper_bound_waist[0]);
        arx5_cmd.control_y = limit<float>(arx5_cmd.control_y, lower_bound_waist[1], upper_bound_waist[1]);
        arx5_cmd.control_z = limit<float>(arx5_cmd.control_z, lower_bound_waist[2], upper_bound_waist[2]);
        arx5_cmd.control_pit   = limit<float>(arx5_cmd.control_pit, lower_bound_pitch, upper_bound_pitch);
        arx5_cmd.control_yaw   = limit<float>(arx5_cmd.control_yaw, lower_bound_yaw, upper_bound_yaw);
        arx5_cmd.control_roll  = limit<float>(arx5_cmd.control_roll, lower_bound_sim[ROLL], upper_bound_sim[ROLL]);

        magic_pos[0] = limit<float>(magic_pos[0], lower_bound_waist[0], upper_bound_waist[0]);
        magic_pos[1] = limit<float>(magic_pos[1], lower_bound_waist[1], upper_bound_waist[1]);
        magic_pos[2] = limit<float>(magic_pos[2], lower_bound_waist[2], upper_bound_waist[2]);
        magic_angle[0] = limit<float>(magic_angle[0], lower_bound_pitch, upper_bound_pitch);
        magic_angle[1] = limit<float>(magic_angle[1], lower_bound_yaw, upper_bound_yaw);
        magic_angle[2] = limit<float>(magic_angle[2], lower_bound_sim[ROLL], upper_bound_sim[ROLL]);        

}


void arx_arm::cmd_init()
{
    arx5_cmd.waist_pitch  = arx5_cmd.waist_pitch_t  =arx5_cmd.control_pit   = joy_pitch_t  =joy_pitch      =0      ;
    arx5_cmd.x            = arx5_cmd.x_t            =arx5_cmd.control_x   = joy_x_t      =joy_x            =0   ;
    arx5_cmd.y            = arx5_cmd.y_t            =arx5_cmd.control_y  = joy_y_t      =joy_y             =0   ;
    arx5_cmd.z            = arx5_cmd.z_t            =arx5_cmd.control_z   = joy_z_t      =joy_z            =0   ;      
    arx5_cmd.control_pit     =      0 ;
    arx5_cmd.control_yaw = joy_roll_t        =0      ;
    arx5_cmd.control_roll   =arx5_cmd.control_yaw   = joy_yaw_t           =0      ;
    arx5_cmd.mode = FORWARD;

}