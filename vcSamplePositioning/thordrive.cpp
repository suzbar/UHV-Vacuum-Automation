/**Automates UHV chamber Sample positioning.
 * Copyright (C) <2018>  <Suzana Barreto>

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "thordrive.h"

#include <boost/concept_check.hpp>
#include <math.h>

#include <iomanip>
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

*********************************************************************************************************************************************
**************************************************** LOW LEVEL FUNCTIONS *******************************************************************
*********************************************************************************************************************************************
**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/


/******************************************************************************************************************************************
 ********************************************************GENERAL FUNCTIONS*****************************************************************
 ****************************************************************************************************************************************/
thordrive::thordrive(bool is_tdc)
{
  tdc = is_tdc;
  const char* usbportstring;
  if(is_tdc){
    usbportstring = "/dev/serial/by-id/usb-Thorlabs_APT_DC_Motor_Controller_83861422-if00-port0";
  }
  else{
    usbportstring = "/dev/serial/by-id/usb-Thorlabs_APT_Stepper_Motor_Controller_70863162-if00-port0";
  }
  openConnector(usbportstring);
  //initialise the drive/s
  init();
}


void thordrive::setTDC(bool is_tdc)
{
  tdc = is_tdc;
}

void thordrive::setActiveDrive(unsigned char drive)
{
  if(drive == 0x11){
    activeDrive = "z";
  }
  else if(drive == 0x21){
    activeDrive = "x";
  }
  else if(drive == 0x22){
    activeDrive = "y";
  }
  //std::cout<<"active drive is\t"<<activeDrive<<std::endl;
  isActive = true;
}


void thordrive::releaseBufferMemory()
{
  delete buf;
}
void thordrive::releasesignedBufferMemory()
{
  delete signed_buf;
}

/******************************************************************************************************************************************
 ********************************************************COMMS FUNCTIONS*****************************************************************
 ****************************************************************************************************************************************/
/*
 * initialises terminal settings for comms
 */
void thordrive::openConnector(const char* usbport)
{
    FD_ZERO(&readSet);

    //open the port
    USB = open( usbport, O_RDWR | O_SYNC/*, S_IRUSR | S_IWUSR*/ /*O_RDWR| O_NOCTTY*/ );

    memset (&tty, 0, sizeof(tty));

    /* Make raw - set the terminal attributes*/
    cfmakeraw(&tty);
    cfsetspeed(&tty,B115200);
    tty.c_cc[VTIME] = 2;    // in tenths of a second
    tty.c_cc[VMIN] = 95;

    if(tcsetattr(USB, TCSAFLUSH, &tty) < 0 )
    {
      perror("openConnector()");
      exit(1);
    }

    FD_SET(USB,&readSet);
}

/**
 * writes to the serial port
 */
void thordrive::sendByteCommand(unsigned char* cmd,int len){
  //tcflush( USB, TCOFLUSH );
  //tcflush( USB, TCIFLUSH ); //flush anything not read by us
  //std::cout<<"SEND BYTE COMMAND: SENDING  " << std::dec << len <<" BYTES" <<std::endl;
  int n_written = 0;
  if(USB > 0){
      n_written = write( USB, cmd, len);
      if (n_written == -1){
        perror("sendByteCommand() (write())");
      }
      else if (n_written != len){
        std::cout << "Only written " << std::dec << n_written << " when " << len << " was exptected\n";
        std::cout<<"   number of written bytes is " << std::dec << n_written<<std::endl;
      }

  }

}

/**
 * writes signed data to the serial port
 */
void thordrive::sendSignedByteCommand(signed char* cmd,int len){
  //tcflush( USB, TCOFLUSH );
  //tcflush( USB, TCIFLUSH ); //flush anything not read by us
  std::cout<<"SEND BYTE COMMAND: SENDING  " << std::dec << len <<" BYTES" <<std::endl;
  int n_written = 0;
  if(USB > 0){
      n_written = write( USB, cmd, len);
      if (n_written == -1){
        perror("sendByteCommand() (write())");
      }
      else if (n_written != len){
        std::cout << "Only written " << std::dec << n_written << " when " << len << " was exptected\n";
        std::cout<<"   number of written bytes is " << std::dec << n_written<<std::endl;
      }
  }

}
/*
 * reads data from the port for status messages when moving - position values will be negative
 */
void thordrive::receiveSignedData(time_t timeout){

    tcflush( USB, TCIOFLUSH );
    //tcflush( USB, TCIFLUSH );
    //std::cout<<" In Receive signed data and buff size is "<< buffSize <<std::endl;
    int n = 0;//number of bytes read from read()
    // b = the number of bytes to read in one go from the stream
    int b = buffSize, bytesRead = 0;
    //char datbuf = '\0';
    int b2read = buffSize;//header has 6 bytes
    //allocate some memory for buffer based on previously set command and thus buffer size
    //not sure if this is necessary
    signed_buf = new signed char[buffSize];
    struct timeval tv;

    /* Whole response*/
    memset(signed_buf, 0, buffSize);
    //read one byte at a time, could also read full buffer in one go? ask fred which is better
    // check if data is available to read
     // timeout struct
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    //std::cout <<" about to use select method and bytes to read are "<<b2read<<std::endl;
    while(b2read >0){
      int result = select(USB + 1, &readSet,NULL,NULL,&tv);
      //std::cout <<" select returned "<<result<<std::endl;
      if(result == -1){
               perror("receiveData() (select())");
           exit(1);
      }
      else if(result > 0 && FD_ISSET(USB,&readSet)){
    n = read( USB, &(signed_buf[bytesRead]), b2read );
    if (n == -1) {
      perror("receiveData() (read())");
    }
    bytesRead +=n;
    b2read = buffSize - bytesRead;
      }
      else{
    // in this case the select has timmed output
    std::cout<<"READ OPERATION COULD NOT BE PERFORMED BECAUSE THE OPERATION TIMED OUT "<<std::endl;
    break;
      }
    }
    //std::cout << "Raw data: ";
    /*for (int i= 0 ;i<buffSize; i++){
      std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(signed_buf[i]);
    }
    std::cout<<std::endl;*/

}

/* reads data from the port
 *
 */
void thordrive::receiveData(unsigned char expected_command[],time_t timeout){
    tcflush( USB, TCIOFLUSH );
    //tcflush( USB, TCIFLUSH );
    //std::cout<<" In Receive data and buff size is "<< buffSize <<std::endl;
    int n = 0;//number of bytes read from read()
    //index = 0;
    // b = the number of bytes to read in one go from the stream
    int bytesRead = 0;
    //char datbuf = '\0';
    int b2read = buffSize;//header has 6 bytes
    //allocate some memory for buffer based on previously set command and thus buffer size
    //not sure if this is necessary
    buf = new unsigned char[buffSize];
    struct timeval tv;
    bool cont = false;
    /* Whole response*/
    memset(buf, 0, buffSize);
    //read one byte at a time, could also read full buffer in one go? ask fred which is better
    // check if data is available to read
     // timeout struct
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    //std::cout <<" about to use select method and bytes to read are "<<b2read<<std::endl;
    while(b2read >0){
      int result = select(USB + 1, &readSet,NULL,NULL,&tv);
      //std::cout <<" select returned "<<result<<std::endl;
      if(result == -1){
               perror("receiveData() (select())");
           exit(1);
      }
      else if(result > 0 && FD_ISSET(USB,&readSet)){
    n = read( USB, &(buf[bytesRead]), b2read );
    //std::cout<<"The read got this amoutn of bytes : "<<n<<"\n";
    if (n == -1) {
      perror("receiveData() (read())");
    }
    else if((n >= 2) & (!cont)){
      if((buf[0]== expected_command[0] ) & (buf[1] == expected_command[1])){
        cont = true;
        bytesRead +=n;
        b2read = buffSize - bytesRead;
       // std::cout <<" 1 bytes to read are now  "<<b2read<<std::endl;
      }
      else{
        /*for (int i= 0 ;i<buffSize; i++){
          std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(buf[i]);
        }
        std::cout<<std::endl;*/
        //how many bytes have been read
        int size = bytesRead + n; //this should be the size of the
        if(size == 2){
          //simple case - not the first two bytes of expected command so just reset the bytesRead variable back to 0
          //this will overwrite the first two values with new data
          bytesRead = 0;
          b2read = buffSize;
        }
        else{
          int start_pos, num_toshift;
          bool matched = false;
          //this is more complicated because any of the following bytes could be the start of the command so need to identify which are and
          //how far in or how much to shift by.
          //start at 1 because we have already checked that the first two bytes are not the expected command
          for(int i = 0; i < size; i++){
        //std::cout <<"in loop "<<i<<std::endl;
        //std::cout <<"buf element is "<<static_cast<int>(buf[i])<<std::endl;
        //std::cout <<"command element is "<<static_cast<int>(expected_command[0])<<static_cast<int>(expected_command[1])<<std::endl;
        if(buf[i] == expected_command[0]){
          if((i +1)< size ){ //check not out of bounds
            if(buf[i+1] == expected_command[1]){
              //both match the expected command
              start_pos = i;
              num_toshift = buffSize - start_pos;
              matched = true;
              break;
            }// array in bounds but no match just continue
          }// out of bounds of array - only the first element of a possible match exists just continue -
          //read once more and return to evaluation
        }// i does not match first byte, just continue onto next element
          }
          if(matched){ //shift the read bytes to front od array;
        for(int i = 0; i < num_toshift; i++){
          buf[i] = buf[start_pos+i];
          bytesRead = i+1;
        }

        b2read = buffSize - bytesRead;
        cont = true;
          }
        }
      }
    }
    else{
      bytesRead +=n;
      b2read = buffSize - bytesRead;
    }
      }
      else{
    // in this case the select has timmed output
    std::cout<<"READ OPERATION COULD NOT BE PERFORMED BECAUSE THE OPERATION TIMED OUT "<<std::endl;
    break;
      }
    }
    /**std::cout << "Raw data: ";
    for (int i= 0 ;i<buffSize; i++){
      std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(buf[i]);
    }
    std::cout<<std::endl;*/

}

/*************************************************************************************************************************************
*********************************************** ADD PARAMS TO COMMAND FUNCTIONS ******************************************************
**************************************************************************************************************************************/
/*
 * sets the trapezoid velocity of TDC actuator (rotary)
 */
void thordrive::setTDCVelocityParams(unsigned char* cmd,double vel, double acc)
{
  // these values are for stage PRM1-Z8 only
  double enccnt = 1919.64; //scaling factor for gears
  double time = 0.000341333; //2048 / (6  * pow(10,6)); //sampling interval this is 2048/6000000 = 0.000341333
  int c = 65536;//constant value
  //to slow down the motors then the vel should be smaller than 1 mm per sec 0.5, 0.25 etc
  uint32_t velocity_apt = static_cast<int32_t>(enccnt * time * c * vel); //because actuator is geared
  uint32_t acceleration = static_cast<int32_t>(enccnt * pow(time,2) * c * acc);
  // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
  cmd[6] = 0x01;
  cmd[7] = 0x00;
  //min velocity next four bytes
  cmd[8] = 0x00;
  cmd[9] = 0x00;
  cmd[10] = 0x00;
  cmd[11] = 0x00;
  //acceleration next four bytes
  cmd[12] = acceleration & 0xFF;
  cmd[13] = (acceleration >> 8) & 0xFF;
  cmd[14] = (acceleration >> 16) & 0xFF;
  cmd[15] = (acceleration >> 24) & 0xFF;
  //max velocity
  cmd[16] = velocity_apt & 0xFF;
  cmd[17] = (velocity_apt >> 8) & 0xFF;
  cmd[18] = (velocity_apt >> 16) & 0xFF;
  cmd[19] = (velocity_apt >> 24) & 0xFF;

}

/* not implemented yet as not sure if needed
 * sets the velocity of BSC203 actuators (linear slide drives)
 */
void thordrive::setBSCVelocityParams(unsigned char* cmd,double mm,double t_secs, unsigned char chan)
{
  //std::cout<<"PURPLE -- mm ="<<mm<<"\n";
  // these values are for stage DRV013 only
  uint32_t position = static_cast<uint32_t>(409600 * mm); //absolute number of microsteps
  //std::cout<<"PURPLE -- pos ="<<position<<"\n";
  //uint32_t pos_acc = 204800; // windows example
  uint32_t timeSecs = static_cast<uint32_t>(t_secs);
  //std::cout<<"PURPLE -- timesecs ="<<timeSecs<<"\n";
  uint32_t pos_acc = position / pow(timeSecs,2); //acc measured in time seconds squared
  //std::cout<<"PURPLE -- posacc ="<<pos_acc<<"\n";
  uint32_t velocity_apt = (position * 53.68)/2; //not sure what this constant value represents - vel in this case means max velocity
  //uint32_t velocity_apt = position / timeSecs;
  //std::cout<<"PURPLE -- velocity ="<<velocity_apt<<"\n";
  //need to know what to set max and min speeds to
  // byte 7 is at pos 6 so 6 and 7 are the chanel identifier

  cmd[6] = chan;
  cmd[7] = 0x00;
  //min velocity next four bytes
  cmd[8] = 0x00;
  cmd[9] = 0x00;
  cmd[10] = 0x00;
  cmd[11] = 0x00;
  //acceleration next four bytes
  cmd[12] = pos_acc & 0xFF;
  cmd[13] = (pos_acc >> 8) & 0xFF;
  cmd[14] = (pos_acc >> 16) & 0xFF;
  cmd[15] = (pos_acc >> 24) & 0xFF;
  //max velocity
  cmd[16] = velocity_apt & 0xFF;
  cmd[17] = (velocity_apt >> 8) & 0xFF;
  cmd[18] = (velocity_apt >> 16) & 0xFF;
  cmd[19] = (velocity_apt >> 24) & 0xFF;
  /*for(int i = 0; i < 20;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
  }*/
}
/*
 * encodes the jog switch parameters that are to be set for BSC203 linear actuators
 * will not set the min or max velocity
 */
void thordrive::encodeBSCJogParams(unsigned char* cmd,double mm_dist)
{
  double BSC203 = 409600;
  // these values are for stage DRV013 only
  uint32_t acceleration =  4506 * mm_dist; //using default values for 1mm/ sec
  // default max velocity
  uint32_t velocity_apt = (BSC203 * mm_dist) * 53.68; //using default of 1mm/sec/sec
  //need to know what to set max and min speeds to
  u_int32_t step_size = 2048 * 100; //in step size

  cmd[6] = 0x01;  // this is always channel 1 of the slot
  cmd[7] = 0x00;
  //jogmode
  cmd[8] = 0x02;
  cmd[9] = 0x00;
  //jog step size
  cmd[10] = step_size & 0xFF;
  cmd[11] = (step_size >> 8) & 0xFF;
  cmd[12] = (step_size >> 16) & 0xFF;
  cmd[13] = (step_size >> 24) & 0xFF;
   //min velocity
  cmd[14] = 0x00;
  cmd[15] = 0x00;
  cmd[16] = 0x00;
  cmd[17] = 0x00;
  /*//jog acceleration
  cmd[18] = 0x00;
  cmd[19] = 0x00;
  cmd[20] = 0x00;
  cmd[21] = 0x00;*/
  //jog acceleration
  cmd[18] = acceleration & 0xFF;
  cmd[19] = (acceleration >> 8) & 0xFF;
  cmd[20] = (acceleration >> 16) & 0xFF;
  cmd[21] = (acceleration >> 24) & 0xFF;
  //max velocity
  cmd[22] = velocity_apt & 0xFF;
  cmd[23] = (velocity_apt >> 8) & 0xFF;
  cmd[24] = (velocity_apt >> 16) & 0xFF;
  cmd[25] = (velocity_apt >> 24) & 0xFF;
  // stop mode
  cmd[26] = 0x02;
  cmd[27] = 0x00;
  /*for(int i = 0; i < 28;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }*/
}

void thordrive::encodeLimitSwitchParams(unsigned char* cmd, unsigned char chan, double cw_slimit,double ccw_slimit)
{
    //std::cout<<"Clockwise param limit  "<<cw_slimit<<"  counter param clockwise limit  "<<ccw_slimit<<"\n";
    uint32_t scalefactor = 134218; //= 1mm
    uint32_t CWsoft_limit,CCWsoft_limit;
    CWsoft_limit = scalefactor * cw_slimit;
    CCWsoft_limit = scalefactor * ccw_slimit;
    //std::cout<<"Clockwise limit  "<<CWsoft_limit<<"  counter clockwise limit  "<<CCWsoft_limit<<"\n";
  //encode the limit switch parameters
   // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
    cmd[6] = chan; //channel identifier either 0x01,0x02,0x03
    cmd[7] = 0x00;
    //next 2 bytes are cw hardware limit switch we want either 01 = ignore, 02 = makes on contact, 03 breaks on contact
    cmd[8] = 0x03; // this is what it is currently set at
    cmd[9] = 0x00;
    // next two bytes are ccw hardware limit switch we want either 01 = ignore, 02 = makes on contact, 03 breaks on contact
    cmd[10] = 0x03;
    cmd[11] = 0x00;
    //next four bytes are cw software limits in position steps with a scalling factor of 1mm = 134218 (uint32_t)
    cmd[12] = CWsoft_limit & 0xFF; //static cast
    cmd[13] = (CWsoft_limit >> 8) & 0xFF;
    cmd[14] = (CWsoft_limit >> 16)& 0xFF;
    cmd[15] = (CWsoft_limit >> 24)& 0xFF;
    //next four bytes are ccw software limits in position steps with a scalling factor of 1mm = 134218 (uint32_t)
    cmd[16] = CCWsoft_limit & 0xFF; //static cast
    cmd[17] = (CCWsoft_limit >> 8) & 0xFF;
    cmd[18] = (CCWsoft_limit >> 16)& 0xFF;
    cmd[19] = (CCWsoft_limit >> 24)& 0xFF;

    // next two bytes are limit mode - this will either be 01 = ignore, 02 = stop immediate at limit, 03 = profiled stop at limit
    cmd[20] = 0x02;
    cmd[21] = 0x00;
    /*for(int i = 0; i < 22;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }*/
    //return ncmd;

}


/*
 * encodes the relative and absolute move parameters for the move_relative command given the hardware type and channel
 */
void thordrive::encodeMoveParams(signed char* cmd, signed char chan, double distMM){
 //need to know what to set max and min speeds to
  double PRM1 = 1919.64;
  int32_t dist;
  double BSC203 = 409600;
  if(tdc){
    dist = static_cast<int32_t>((PRM1 * distMM));  // add 0.5 because of rounding
  }
  else{
    dist = static_cast<int32_t>((BSC203 * distMM));
    //std::cout<<"Here in BSC203 and setting the distance and distance is " <<dist<<"\n";
  }
    // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
    cmd[6] = chan;
    cmd[7] = 0x00;
    //next 4 bytes are the distance
    cmd[8] = dist & 0xFF; //static cast
    cmd[9] = (dist >> 8) & 0xFF;
    cmd[10] = (dist >> 16)& 0xFF;
    cmd[11] = (dist >> 24)& 0xFF;
    /*for(int i = 0; i < 12;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }
    //return ncmd;
    std::cout<<"********************************"<<std::endl;*/
}

void thordrive::encodeBSCPowerParams(unsigned char* cmd, unsigned char chan)
{
    //encode the limit switch parameters
    // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
    cmd[6] = chan; //channel identifier either 0x01,0x02,0x03
    cmd[7] = 0x00;
    //next 2 bytes are rest factor - setting hard coded value of 15%
    cmd[8] = 0x0f; // this is what it is currently set at
    cmd[9] = 0x00;
    // next two bytes are Move Factor, apparantly not used but setting to 30%
    cmd[10] = 0x1e;
    cmd[11] = 0x00;

    /*for(int i = 0; i < 12;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }*/
}

/*
 * encodes the move absolute parameters for BSC203
 */
void thordrive::encodeMoveAbsParams(unsigned char* cmd, unsigned char chan)
{
  //encode the limit switch parameters
    // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
    cmd[6] = chan; //channel identifier either 0x01,0x02,0x03
    cmd[7] = 0x00;
    //next 4 bytes are the absolute position to set
    cmd[8] = 0x00; // this is what it is currently set at
    cmd[9] = 0x00;
    cmd[10] = 0x00;
    cmd[11] = 0x00;

    /*for(int i = 0; i < 12;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }*/
}



/*
 * encodes the 14 bytes of homing parameters for the home command this function will only be used by the BSC203 controller
 */
void thordrive::encodeHomeParams(unsigned char* cmd,unsigned char chan, double vel, int home_dir){
  //vel = mm travel per second
  //need to know what to set max and min speeds to
  //double PRM1 = 1919.64;
  uint32_t velocity;
  double BSC203 = 409600;
  //double T = 2048 / 6 * 1000000;
  std::cout<<"************In encode home Velocity************"<<std::endl;
  if(tdc){
    // the formula is supposed to be velocity = PRM1 * T * 65536 * vel;
    // where t is supposed to be time sampling parameter but the vel parameter gives us a value in mm per second so
    // not really sure how they have calculated the values as they do not seem to correspond to anything - ask FRED
    velocity = 42941.66;
    std::cout<<"************TDC is true************"<<std::endl;

  }
  else{
    std::cout<<"************BSC Velocity************"<<std::endl;
    velocity = BSC203 * (53.68 * vel);
    std::cout<<"************Velocity is ************"<<velocity <<std::endl;
  }

    // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
    cmd[6] = chan; //channel identifier either 0x01,0x02,0x03
    cmd[7] = 0x00;
    //next 2 bytes are homing direction which is ignored in this implementation - according to thorlabs
    //not our implementation
    if(home_dir == 1){
      cmd[8] = 0x01; // this is implemented, home dir two is outward facing(reverse), 1 retracts into actuator(forward)
      cmd[9] = 0x00;
       // next two bytes are the limit switch params
      cmd[10] = 0x04;
      cmd[11] = 0x00;
    }
    else{
      cmd[8] = 0x02; // this is implemented, home dir two is outward facing(reverse), 1 retracts into actuator(forward)
      cmd[9] = 0x00;
       // next two bytes are the limit switch params
      cmd[10] = 0x01;
      cmd[11] = 0x00;
    }


    //next four bytes are the homing velocity (signed long)
    cmd[12] = velocity & 0xFF; //static cast
    cmd[13] = (velocity >> 8) & 0xFF;
    cmd[14] = (velocity >> 16)& 0xFF;
    cmd[15] = (velocity >> 24)& 0xFF;
    //next four bytes are the offset distance? not sure what that means but this is hard coded to follow windows apt software example for
    //BSC controllers not to be used for tdc controller.
    if(!tdc){
      cmd[16] = 0x00;
      cmd[17] = 0x58;
      cmd[18] = 0x02;
      cmd[19] = 0x00;
    }
    else{ // this is still to be set if used for tdc001
      cmd[16] = 0x00;
      cmd[17] = 0x00;
      cmd[18] = 0x00;
      cmd[19] = 0x00;
    }
    /*for(int i = 0; i < 20;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }
    //return ncmd;
    std::cout<<"********************************"<<std::endl;*/
}
/*
 * encodes the params for changing the direction that the motor controller will move in
 */

void thordrive::encodePMDStageAxisParams(unsigned char* cmd)
{
  uint16_t stageID= 0;
  uint16_t axisID = 0;
  uint32_t serialnum = 0, countsPerUnit=0, minPos=0, maxPos=0, maxAccl=0, maxDecl=0, maxVel=0;
  // byte 7 is at pos 6 so 6 and 7 are the channel identifier tdc = 1 channel
  cmd[6] = 0x01; //channel identifier always 0x01 for all actuators as BSC203 has slot numbers
  cmd[7] = 0x00;
  // bytes 8 and 9 are for stageID
  cmd[8] = stageID & 0xFF; //static cast
  cmd[9] = (stageID >> 8) & 0xFF;
  // bytes 10 and 11 are for axisID
  cmd[10] = axisID & 0xFF; //static cast
  cmd[11] = (axisID >> 8) & 0xFF;
  //next 16 bytes are for the partnumber / axis
  cmd[12] = 0x00;
  cmd[13] = 0x00;
  cmd[14] = 0x00;
  cmd[15] = 0x00;
  cmd[16] = 0x00;
  cmd[17] = 0x00;
  cmd[18] = 0x00;
  cmd[19] = 0x00;
  cmd[20] = 0x00;
  cmd[21] = 0x00;
  cmd[22] = 0x00;
  cmd[23] = 0x00;
  cmd[24] = 0x00;
  cmd[25] = 0x00;
  cmd[26] = 0x00;
  cmd[27] = 0x00;
  // next four bytes are for the serial number
  cmd[28] = serialnum & 0xFF; //static cast
  cmd[29] = (serialnum >> 8) & 0xFF;
  cmd[30] = (serialnum >> 16) & 0xFF; //static cast
  cmd[31] = (serialnum >> 24) & 0xFF;
  // next four bytes are for counts per unit
  cmd[32] = countsPerUnit & 0xFF; //static cast
  cmd[33] = (countsPerUnit >> 8) & 0xFF;
  cmd[34] = (countsPerUnit >> 16) & 0xFF; //static cast
  cmd[35] = (countsPerUnit >> 24) & 0xFF;
  //next four bytes are for minumum position
  cmd[36] = minPos & 0xFF; //static cast
  cmd[37] = (minPos >> 8) & 0xFF;
  cmd[38] = (minPos >> 16) & 0xFF; //static cast
  cmd[39] = (minPos >> 24) & 0xFF;
  //next four bytes are for maximum position
  cmd[40] = maxPos & 0xFF; //static cast
  cmd[41] = (maxPos >> 8) & 0xFF;
  cmd[42] = (maxPos >> 16) & 0xFF; //static cast
  cmd[43] = (maxPos >> 24) & 0xFF;
  //next four bytes are for maximum acceleration
  cmd[44] = maxAccl & 0xFF; //static cast
  cmd[45] = (maxAccl >> 8) & 0xFF;
  cmd[46] = (maxAccl >> 16) & 0xFF; //static cast
  cmd[47] = (maxAccl >> 24) & 0xFF;
  //next four bytes are for maximum deceleration
  cmd[48] = maxDecl & 0xFF; //static cast
  cmd[49] = (maxDecl >> 8) & 0xFF;
  cmd[50] = (maxDecl >> 16) & 0xFF; //static cast
  cmd[51] = (maxDecl >> 24) & 0xFF;
  //next four bytes are for maximum velocity
  cmd[52] = maxVel & 0xFF; //static cast
  cmd[53] = (maxVel >> 8) & 0xFF;
  cmd[54] = (maxVel >> 16) & 0xFF; //static cast
  cmd[55] = (maxVel >> 24) & 0xFF;
  // the next 24 bytes are reserved so no values passed
  cmd[56] = 0x00;
  cmd[57] = 0x00;
  cmd[58] = 0x00;
  cmd[59] = 0x00;
  cmd[60] = 0x00;
  cmd[61] = 0x00;
  cmd[62] = 0x00;
  cmd[63] = 0x00;
  cmd[64] = 0x00;
  cmd[65] = 0x00;
  cmd[66] = 0x00;
  cmd[67] = 0x00;
  cmd[68] = 0x00;
  cmd[69] = 0x00;
  cmd[70] = 0x00;
  cmd[71] = 0x00;
  cmd[72] = 0x00;
  cmd[73] = 0x00;
  cmd[74] = 0x00;
  cmd[75] = 0x00;
  cmd[76] = 0x00;
  cmd[77] = 0x00;
  cmd[78] = 0x00;
  cmd[79] = 0x00;
  /* for(int i = 0; i < 80;i++){
      std::cout<<std::hex <<static_cast<int>(cmd[i])<<std::endl;
    }
    //return ncmd;
    std::cout<<"********************************"<<std::endl;*/
}

/*************************************************************************************************************************************
 *******************************************************GET FUNCTIONS ****************************************************************
 *************************************************************************************************************************************/
/*
 * gets the scaled negative position for the currently active actuator
 * */

double thordrive::getScaledActuatorPosition(char destination){

  if(destination == 0x21){

    return scaled_x;
  }
  else if(destination == 0x22){
    return scaled_y;
  }
  else if (destination == 0x50){
    return scaled_z;
  }
  else{
    //return scaled_z;
    //error condition
    return 500;
  }
}

/*
 * gets info about the apt controller from the the response buffer
 */
void thordrive::getInfo(){
  struct _HWINFO d;
      char modelnum[8];
      //std::cout<<" about to receive response"<<std::endl;
      d.serialNum = static_cast<uint32_t>(buf[6]) + static_cast<uint32_t>(buf[7] << 8) + static_cast<uint32_t>(buf[8]<< 16) + static_cast<uint32_t>(buf[9]<<24);
      modelnum[0] = (char) buf[10];
      modelnum[1] = (char) buf[11];
      modelnum[2] = (char) buf[12];
      modelnum[3] = (char) buf[13];
      modelnum[4] = (char) buf[14];
      modelnum[5] = (char) buf[15];
      modelnum[6] = (char) buf[16];
      modelnum[7] = (char) buf[17];
      d.modelNum = modelnum;
      d.hwType = static_cast<uint16_t>(buf[18]) + static_cast<uint16_t>(buf[19]<<8);
      d.softwareVer = static_cast<uint32_t>(buf[20]) + static_cast<uint32_t>(buf[21] << 8) + static_cast<uint32_t>(buf[22]<< 16)  + static_cast<uint32_t>(buf[23]<<24); //this is opposite to
      int x = 0;
      int end = 24 + 48; // 24 is current index in response buffer (18 was not used) and 48 is the size of the notes field
      for(int i = 24; i <= end;i++){
        d.notes[x]= buf[i];
        x++;
      }
      //next 12 bytes are not used
      d.hwVersion = static_cast<uint16_t>(buf[84])+ static_cast<uint16_t>(buf[85]<< 8);
      d.modState = static_cast<uint16_t>(buf[86])+ static_cast<uint16_t>(buf[87]<< 8);
      d.numChannels = static_cast<uint16_t>(buf[88])+ static_cast<uint16_t>(buf[89]<< 8);
      //use model number or serial number to identify which instument the port is connected to
      /*if(d.serialNum >= 830000){
    tdc = true; //check if this is working
    std::cout<<"TDC TRUE"<<d.hwType<<std::endl;
      }*/
      std::string outputsting(d.notes);
      std::cout<<"Serial number: \t"<< d.serialNum<<std::endl;
      std::cout<<"model number: \t"<<d.modelNum<<std::endl;
      std::cout<<"Hardware Type: \t"<<d.hwType<<std::endl;
      std::cout<<"Software Version: \t"<<d.softwareVer<<std::endl;
      std::cout<<"Notes: \t"<<outputsting<<std::endl;
      std::cout<<"Hardware Version: \t"<<d.hwVersion<<std::endl;
      std::cout<<"Mod State: \t"<<d.modState<<std::endl;
      std::cout<<"Number of Channels: \t"<<d.numChannels<<std::endl;
      /*for(int i = 0; i < buffSize; i++){
      //std::cout.setf(std::ios::hex, std::ios::basefield);
      std::cout << "0x" << std::hex << static_cast<uint16_t>(buf[i]) << std::endl;
    }*/

}
/*
 * gets the general move params which is actually the backlash settings
 */
void thordrive::getGenMoveParams()
{
   uint32_t chan,backlash;
   chan = static_cast<uint32_t>(buf[6])+static_cast<uint32_t>(buf[7]<<8);
   backlash = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8) + static_cast<uint32_t>(buf[10]<<16) + static_cast<uint32_t>(buf[11]<<24);

   //std::cout<<"Channel : \t"<< chan<<std::endl;
   //std::cout<<"backlash : \t"<< backlash<<std::endl;
}

 /*
 * gets the BSC power parameters for the given channel
 */
void thordrive::getBSCPowerParams(){
   uint32_t chan, restFactor,moveFactor;
   chan = static_cast<uint32_t>(buf[6])+static_cast<uint32_t>(buf[7]<<8);
   restFactor = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8 );
   moveFactor = static_cast<uint32_t>(buf[10]<<16) + static_cast<uint32_t>(buf[11]<<24);

   //std::cout<<"Channel : \t"<< chan<<std::endl;
   //std::cout<<"Rest Factor : \t"<< restFactor<<std::endl;
   //std::cout<<"Move Factor : \t"<<moveFactor<<std::endl;
}

/*
 * gets the velocity parameters for the given channel
 */
void thordrive::getVelocityParams(){
   uint32_t chan, minVal,maxVal,acceleration;
   chan = static_cast<uint32_t>(buf[6])+static_cast<uint32_t>(buf[7]<<8);
   minVal = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8 )+static_cast<uint32_t>(buf[10]<<16) + static_cast<uint32_t>(buf[11]<<24);
   acceleration = static_cast<uint32_t>(buf[12])+ static_cast<uint32_t>(buf[13]<<8) +static_cast<uint32_t>(buf[14] <<16) + static_cast<uint32_t>(buf[15] <<24);
   maxVal = static_cast<uint32_t>(buf[16])+ static_cast<uint32_t>(buf[17] <<8) +static_cast<uint32_t>(buf[18] <<16) + static_cast<uint32_t>(buf[19] <<24);

   std::cout<<"Channel : \t"<< chan<<std::endl;
   std::cout<<"Min val : \t"<< minVal<<std::endl;
   std::cout<<"max val : \t"<<acceleration<<std::endl;
   std::cout<<"acceleration : \t"<<maxVal<<std::endl;
   if(tdc){
     double j = maxVal / 42941.66;
     std::cout<<"scaled acceleration : \t"<<j<<std::endl;
   }
}
/**
 * gets the relative move params from the response buffer
 */
void thordrive::getMoveRelParams()
{
  int chan;
  uint32_t reldist;
  double distance;
  chan = (int)buf[6]; // there will only be 3 channels, 2 will be chan 1 and 1 will either be chan2 or 3
  reldist = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8 )+static_cast<uint32_t>(buf[10]<<16) + static_cast<uint32_t>(buf[11]<<24);
  //have to do the scaling to cm so / by the encoder values
  if(tdc){
    distance = reldist / 1919.64;
  }
  else{
    distance = reldist / 409600.0;
  }
  std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"Distance \t"<<reldist<<std::endl;
  std::cout<<"Scaled Distance \t"<<distance<<std::endl;
}

/**
 * gets the absolute move params from the response buffer
 */
void thordrive::getMoveAbsParams()
{
  int chan;
  uint32_t absdist;
  double distance;
  chan = (int)buf[6]; // there will only be 3 channels, 2 will be chan 1 and 1 will either be chan2 or 3
  absdist = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8 )+static_cast<uint32_t>(buf[10]<<16) + static_cast<uint32_t>(buf[11]<<24);
  //have to do the scaling to cm so / by the encoder values
  if(tdc){
    distance = absdist / 1919.64;
  }
  else{
    distance = absdist / 409600.0;
  }
  std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"Distance \t"<<absdist<<std::endl;
  std::cout<<"scaled Distance \t"<<distance<<std::endl;
}
/**
 * gets the home params for the given stage from the response buffer
 */
void thordrive::getHomeParams()
{
  int chan;
  uint16_t homeDirection,limitSwitch;
  uint32_t homeVel,offsetDist;
  chan = (int)buf[6];
  homeDirection = static_cast<uint16_t>(buf[8])+ static_cast<uint16_t>(buf[9]<<8);
  limitSwitch = static_cast<uint16_t>(buf[10])+ static_cast<uint16_t>(buf[11]<<8);
  homeVel = static_cast<uint32_t>(buf[12])+ static_cast<uint32_t>(buf[13]<<8) + static_cast<uint32_t>(buf[14]<<16)+ static_cast<uint32_t>(buf[15]<<24);
  offsetDist = static_cast<uint32_t>(buf[16])+ static_cast<uint32_t>(buf[17]<<8) + static_cast<uint32_t>(buf[18]<<16)+ static_cast<uint32_t>(buf[19]<<24);
  double h_velocity,vel2;
  if(tdc){
    double enccnt = 1919.64; //scaling factor for gears
    double time = 2048 / 6  * pow(10,6); //sampling interval
    int c = 65536;//constant value
    vel2 = enccnt / time / 65536.0 /homeVel;
    h_velocity = ((homeVel /c) /time) / enccnt;
    //offsetDist = offsetDist / enccnt; //this results in 4 - is this degrees or mm
  }
  else{
    int32_t enccnt = 409600; //absolute number of microsteps
    h_velocity = (homeVel/enccnt) / 53.68;
  }
  std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"home direction \t"<<homeDirection<<std::endl;
  std::cout<<"limit switch: \t"<<limitSwitch<<std::endl;
  std::cout<<"home velocity \t"<<homeVel<<std::endl;
  std::cout<<"offset distance: \t"<<offsetDist<<std::endl;
  std::cout<<"Scaled velocity \t"<<h_velocity<<std::endl;

}

/*
 * gets the limit switch parameters for the given actuator
 */
void thordrive::getLimitSwitchParams()
{
  unsigned char chan;
  uint16_t cwHardLim,ccwHardLim,limitMode;
  uint32_t cwSoftlim,ccwSoftlim;

  chan = buf[6];
  cwHardLim = static_cast<uint16_t>(buf[8])+ static_cast<uint16_t>(buf[9]<<8);
  ccwHardLim = static_cast<uint16_t>(buf[10])+ static_cast<uint16_t>(buf[11]<<8);
  cwSoftlim = static_cast<uint32_t>(buf[12])+ static_cast<uint32_t>(buf[13]<<8) + static_cast<uint32_t>(buf[14]<<16)+ static_cast<uint32_t>(buf[15]<<24);
  ccwSoftlim = static_cast<uint32_t>(buf[16])+ static_cast<uint32_t>(buf[17]<<8) + static_cast<uint32_t>(buf[18]<<16)+ static_cast<uint32_t>(buf[19]<<24);
  limitMode = static_cast<uint16_t>(buf[20])+ static_cast<uint16_t>(buf[21]<<8);

  std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"clockwise hardware limit \t"<<cwHardLim<<std::endl;
  std::cout<<"counter clockwise hardware limit \t"<<ccwHardLim<<std::endl;
  std::cout<<"clockwise software limit \t"<<cwSoftlim<<std::endl;
  std::cout<<"counter clockwise software limit \t"<<ccwSoftlim<<std::endl;
  std::cout<<"limit mode \t"<<limitMode<<std::endl;
  int cwscaled, ccwscaled;
  cwscaled = cwSoftlim /134218;
  ccwscaled = ccwSoftlim / 134218;
  std::cout<<"scaled cw softlimit \t"<<cwscaled<<std::endl;
  std::cout<<"scaled ccw softlimit \t"<<ccwscaled<<std::endl;
}
/*
 * gets the stopped and move completed status params
 */
void thordrive::getStoppedParams()
{
   getStatusUpdates();
}
/*
 * gets the  status updates for the BSC controllers only
 */
void thordrive::getStatusUpdates()
{
  uint32_t unsigned_pos, position,encnt,statusBits;
  int chan;
  double scaled_position;
  int32_t signed_pos;
  chan = (int)buf[6];
  if(!homing){
    //std::cout<<"not homing\t"<<std::endl;
    signed_pos = static_cast<int32_t>(buf[8])+ static_cast<int32_t>(buf[9]<<8) + static_cast<int32_t>(buf[10]<<16)+ static_cast<int32_t>(buf[11]<<24);
    scaled_position = double(signed_pos) / 409600.0;
    position = signed_pos;
  }
  else{
    unsigned_pos = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8) + static_cast<uint32_t>(buf[10]<<16)+ static_cast<uint32_t>(buf[11]<<24);
    scaled_position = double(unsigned_pos) / 409600.0;
    position = unsigned_pos;
  }
  encnt = static_cast<uint32_t>(buf[12])+ static_cast<uint32_t>(buf[13]<<8) + static_cast<uint32_t>(buf[14]<<16)+ static_cast<uint32_t>(buf[15]<<24);
  statusBits = static_cast<uint32_t>(buf[16])+ static_cast<uint32_t>(buf[17]<<8) + static_cast<uint32_t>(buf[18]<<16)+ static_cast<uint32_t>(buf[19]<<24);


  homed = statusBits & 0x00000400;
  moving = statusBits & 0x00000010;//forward
  if(!moving){
      moving = statusBits & 0x00000020;//reverse
  }

  /**std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"position \t"<<position<<std::endl;
  std::cout<<"scaled position \t"<<scaled_position <<std::endl;
  std::cout<<"encoder count \t"<<encnt<<std::endl;
  std::cout<<"statusbits \t"<<statusBits<<std::endl;
  std::cout<<"homed \t"<<homed<<std::endl;*/

  if(activeDrive == "x"){
    x = position;
    scaled_x = scaled_position;
  }
  else{
    y = position;
    scaled_y = scaled_position;
  }

}
/*
 * gets the status updates for the tdc001 controller altered from unsigned pos to signed17-25-1
 * */

void thordrive::getDCStatusUpdates()
{
  uint32_t position,statusBits;
  uint16_t velocity;
  int32_t signed_pos;
  int chan;
  chan = (int)buf[6];
  signed_pos = static_cast<int32_t>(buf[8])+ static_cast<int32_t>(buf[9]<<8) + static_cast<int32_t>(buf[10]<<16)+ static_cast<int32_t>(buf[11]<<24);
  //position = static_cast<uint32_t>(buf[8])+ static_cast<uint32_t>(buf[9]<<8) + static_cast<uint32_t>(buf[10]<<16)+ static_cast<uint32_t>(buf[11]<<24);
  velocity = static_cast<uint16_t>(buf[12])+ static_cast<uint16_t>(buf[13]<<8);
  statusBits = static_cast<uint32_t>(buf[16])+ static_cast<uint32_t>(buf[17]<<8) + static_cast<uint32_t>(buf[18]<<16)+ static_cast<uint32_t>(buf[19]<<24);
  /**std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"position \t"<<position<<std::endl;
  std::cout<<"velocity \t"<<velocity<<std::endl;*/
  homed = statusBits & 0x00000400;
  moving = statusBits & 0x00000010;//forward
  if(!moving){
      moving = statusBits & 0x00000020;//reverse
  }
  z = signed_pos;
  scaled_z = double(z / 1919.64);
}

/*
 * Gets a response message from the controller containing notes and an error code
 *
 */
void thordrive::getRichResponse()
{
  //std::cout<<"Getting response \t"<<std::endl;
  uint16_t msgID,msgCode;
  char notes[64];//info string

  msgID = static_cast<uint16_t>(buf[7])+ static_cast<uint16_t>(buf[7]<<8);
  msgCode = static_cast<uint16_t>(buf[8])+ static_cast<uint16_t>(buf[9]<<8);
  int index = 0;
  for(int i = 10; i<74; i++){
    notes[index]= buf[i];
    index++;
  }
  //std::cout<<"Message ID: \t"<<msgID<<std::endl;
  //std::cout<<"Message Code: \t"<<msgCode<<std::endl;
  //std::cout<<"Notes:"<<std::endl;
  /*for(int i = 0; i< 64; i++){
    std::cout<<notes[i];
  }
  std::cout<<std::endl;*/

}

/*
 * gets jog params
 */
void thordrive::getJogParams()
{
  int chan;
  uint16_t jogmode,jogStopMode;
  uint32_t jogStepSize,jogMinVel,jogAcc,jogMaxVel;
  chan = (int)buf[6];
  jogmode = static_cast<uint16_t>(buf[8])+ static_cast<uint16_t>(buf[9]<<8);
  jogStepSize = + static_cast<uint32_t>(buf[10])+ static_cast<uint32_t>(buf[11]<<8) + static_cast<uint32_t>(buf[12]<<16)+ static_cast<uint32_t>(buf[13]<<24);
  jogMinVel = + static_cast<uint32_t>(buf[14])+ static_cast<uint32_t>(buf[15]<<8) + static_cast<uint32_t>(buf[17]<<16)+ static_cast<uint32_t>(buf[17]<<24);
  jogAcc = + static_cast<uint32_t>(buf[18])+ static_cast<uint32_t>(buf[19]<<8) + static_cast<uint32_t>(buf[20]<<16)+ static_cast<uint32_t>(buf[21]<<24);
  jogMaxVel = + static_cast<uint32_t>(buf[22])+ static_cast<uint32_t>(buf[23]<<8) + static_cast<uint32_t>(buf[24]<<16)+ static_cast<uint32_t>(buf[25]<<24);
  jogStopMode = + static_cast<uint32_t>(buf[26])+ static_cast<uint32_t>(buf[27]<<8);

  std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"Jog Mode: \t"<<jogmode<<std::endl;
  std::cout<<"Step Size: \t"<<jogStepSize<<std::endl;
  std::cout<<"Min velocity: \t"<<jogMinVel<<std::endl;
  std::cout<<"acceleration: \t"<<jogAcc<<std::endl;
  std::cout<<"Max velocity: \t"<<jogMaxVel<<std::endl;
  std::cout<<"Jog Stop Mode: \t"<<jogStopMode<<std::endl;
}

void thordrive::getChanEnableState()
{
  int chan, enable_state,source;
  chan = (int)buf[2];
  enable_state = (int)buf[3];
  source = (int) buf[5];
  std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"enabled: \t"<<enable_state<<std::endl;
  std::cout<<"source: \t"<<source<<std::endl;
}

void thordrive::getRackBayUsed()
{
  int bayIDent, bay_state;
  bayIDent = (int)buf[2];
  bay_state = (int)buf[3];
  std::cout<<"Bay: \t"<<bayIDent<<std::endl;
  std::cout<<"state: \t"<<bay_state<<std::endl;
}

/*
 * Gets the PMD Stage axis params from the read buffer
 *
 */
void thordrive::getPMDStageAxisParams(){
  int chan;
  char partnum_axis[16];
  uint16_t stageID, axisID;
  uint32_t serialnum, countsPerUnit, minPos, maxPos, maxAccl, maxDecl, maxVel;
  chan = (int)buf[6];
  stageID = static_cast<uint16_t>(buf[8])+ static_cast<uint16_t>(buf[9]<<8);
  axisID = static_cast<uint32_t>(buf[10])+ static_cast<uint32_t>(buf[11]<<8);
  partnum_axis[0] = (char)buf[12];
  partnum_axis[1]= (char)buf[13];
  partnum_axis[2] = (char)buf[14];
  partnum_axis[3]= (char)buf[15];
  partnum_axis[4] = (char)buf[16];
  partnum_axis[5]= (char)buf[17];
  partnum_axis[6] = (char)buf[18];
  partnum_axis[7]= (char)buf[19];
  partnum_axis[8] = (char)buf[20];
  partnum_axis[9]= (char)buf[21];
  partnum_axis[10] = (char)buf[22];
  partnum_axis[11]= (char)buf[23];
  partnum_axis[12] = (char)buf[24];
  partnum_axis[13]= (char)buf[25];
  partnum_axis[14] = (char)buf[26];
  partnum_axis[15]= (char)buf[27];
  serialnum = static_cast<uint32_t>(buf[28])+ static_cast<uint32_t>(buf[29]<<8) + static_cast<uint32_t>(buf[30]<<16)+ static_cast<uint32_t>(buf[31]<<24);
  countsPerUnit = static_cast<uint32_t>(buf[32])+ static_cast<uint32_t>(buf[33]<<8) + static_cast<uint32_t>(buf[38]<<16)+ static_cast<uint32_t>(buf[39]<<24);
  minPos = static_cast<uint32_t>(buf[36])+ static_cast<uint32_t>(buf[37]<<8) + static_cast<uint32_t>(buf[24]<<16)+ static_cast<uint32_t>(buf[25]<<24);
  maxPos = static_cast<uint32_t>(buf[40])+ static_cast<uint32_t>(buf[41]<<8)+ static_cast<uint32_t>(buf[42]<<16)+ static_cast<uint32_t>(buf[43]<<24);
  maxAccl = static_cast<uint32_t>(buf[44])+ static_cast<uint32_t>(buf[45]<<8)+ static_cast<uint32_t>(buf[46]<<16)+ static_cast<uint32_t>(buf[47]<<24);
  maxDecl = static_cast<uint32_t>(buf[48])+ static_cast<uint32_t>(buf[49]<<8)+ static_cast<uint32_t>(buf[50]<<16)+ static_cast<uint32_t>(buf[51]<<24);
  maxVel = static_cast<uint32_t>(buf[52])+ static_cast<uint32_t>(buf[53]<<8)+ static_cast<uint32_t>(buf[54]<<16)+ static_cast<uint32_t>(buf[55]<<24);
  // the rest of the buf string is not necessary

  /*std::cout<<"Channel: \t"<<chan<<std::endl;
  std::cout<<"stage ID: \t"<<stageID<<std::endl;
  std::cout<<"Axis ID: \t"<<axisID<<std::endl;
  std::cout<<"partnum: \t";
  for(int i = 0; i< 16; i++){
    std::cout<<partnum_axis[i];
  }
  std::cout<<"serial number: \t"<<serialnum<<std::endl;
  std::cout<<"Counts per unit: \t"<<countsPerUnit<<std::endl;
  std::cout<<"Min position: \t"<<minPos<<std::endl;
  std::cout<<"Max position: \t"<<maxPos<<std::endl;
  std::cout<<"Max acceleration: \t"<<maxAccl<<std::endl;
  std::cout<<"Max deceleration: \t"<<maxDecl<<std::endl;
  std::cout<<"Max velocity: \t"<<maxVel<<std::endl;*/

}
/**************************************************************************************************************************************
**************************************************** PROCESS COMMAND FUNCTIONS ********************************************************
***************************************************************************************************************************************/
/*
 * Processes the signed data recived from statusupdate when moving motors
 * */

void thordrive::processSignedData(){
  //delete the buffer to free up the memory
  getStatusUpdates();
  releasesignedBufferMemory();
}
/*
 * reads the header from the buffer and determines what the response command is
 */
enum thordrive::command_t thordrive::lookupCommand(){
  // response header command will be in first byte
  unsigned char c = buf[0];
  unsigned char type = buf[1];
  if(type == 0x00 || type == 0x02){
    if(c == 0x81){
      return  HW_RICHRESPONSE;
    }
    if(c== 0x06){
      return HW_GET_INFO;
    }
    if(c == 0x12){
       return MOD_GET_CHANENABLESTATE;
     }
     if(c== 0x61){
      return RACK_GET_BAYUSED;
     }
  }
  else{ // the rest are motor control commands as opposed to generic system control commands

     if(c== 0x47){
      return  MOT_GET_MOVERELPARAMS;
     }
     if(c== 0x52){
      return  MOT_GET_MOVEABSPARAMS;
     }
     if(c== 0x42){
      return MOT_GET_HOMEPARAMS;
     }
     if(c== 0x15){
      return MOT_GET_VELPARAMS;
     }
     if(c== 0x18){
      return MOT_GET_JOGPARAMS;
     }
     if(c== 0x44){
      return  MOT_MOVE_HOMED;//does not require any further processing
     }
     if(c== 0x64){
      return MOT_MOVE_COMPLETED;
     }
     if(c== 0x66){
      return MOT_MOVE_STOPPED;
     }
     if(c== 0x81){
      return MOT_GET_STATUSUPDATE;
     }
     if(c== 0x91){
      return MOT_GET_DCSTATUSUPDATE;
     }
     if(c== 0x25){
      return MOT_GET_LIMSWITCHPARAMS;
     }
     if(c== 0xF2){
      return MOT_GET_PMDSTAGEAXISPARAMS;
     }
     if(c== 0x28){
      return MOT_GET_POWERPARAMS;
     }
     if(c== 0x3c){
      return MOT_GET_GENMOVEPARAMS;
     }
  }
  return MOD_UNKNOWN;
}
/*
 * processes the response buffer
 *
 */
void thordrive::processRespose(command_t c)
{
  //std::cout<<" In Process response and command is "<<c<<std::endl;
  //read depending on command type
  switch(c)
  {
    case HW_GET_INFO:
    {
      //std::cout<<" command is HW_GET_INFO "<<std::endl;
      getInfo();
      break;
    }
    case MOT_GET_STATUSUPDATE:
    {
      //std::cout<<" get status update"<<std::endl;
      getStatusUpdates();
      break;
    }
    case MOT_GET_DCSTATUSUPDATE:
    {
      //std::cout<<" get dc status update"<<std::endl;
      getDCStatusUpdates();
      break;
    }
    case MOT_GET_HOMEPARAMS:
    {
      //std::cout<<" Igetting home params"<<std::endl;
      getHomeParams();
      break;
    }
    case MOT_GET_VELPARAMS:
    {
      //std::cout<<" getting velocity params for "<<std::endl;
      getVelocityParams();
      break;
    }
    case MOT_GET_MOVERELPARAMS:
    {
      //std::cout<<" get MOVE RELATIVE PARAMS"<<std::endl;
      getMoveRelParams();
      break;
    }
    case MOT_GET_MOVEABSPARAMS:
    {
      //std::cout<<" get MOVE Abs PARAMS"<<std::endl;
      getMoveAbsParams();
      break;
    }
    case MOT_MOVE_STOPPED:
    {
      //std::cout<<" get MOVE stopped"<<std::endl;
      isActive = false;
      getStoppedParams();
      break;
    }
    case MOT_GET_LIMSWITCHPARAMS:
    {
      //std::cout<<" get limitSwitch PARAMS"<<std::endl;
      getLimitSwitchParams();
      break;
    }
    case MOT_MOVE_COMPLETED:
    {
      //std::cout<<" MOVE completed"<<std::endl;
      moveCompleted = true;
      getStatusUpdates();
      break;
    }
    case MOT_MOVE_HOMED:
    {
      //std::cout<<" Homed *****************"<<std::endl;
      break;
    }
    case MOT_GET_JOGPARAMS:
    {
      //std::cout<<" GET JOG PARAMS"<<std::endl;
      getJogParams();
      break;
    }
    case MOD_GET_CHANENABLESTATE:
    {
      //std::cout<<" GET CHANENABLESTATE"<<std::endl;
      getChanEnableState();
      break;
    }
    case RACK_GET_BAYUSED:
    {
      //std::cout<<" GET Bay used PARAM"<<std::endl;
      getRackBayUsed();
      break;
    }
    case HW_RICHRESPONSE:
    {
      //std::cout<<" Getting some response"<<std::endl;
      getRichResponse();
      break;
    }
    case MOT_GET_PMDSTAGEAXISPARAMS:
    {
      //std::cout<<" Getting PMDStageAxisParams"<<std::endl;
      getPMDStageAxisParams();
      break;
    }
    case MOT_GET_POWERPARAMS:
    {
      //std::cout<<" Getting PowerParams"<<std::endl;
      getBSCPowerParams();
      break;
    }
    case MOT_GET_GENMOVEPARAMS:
    {
      //std::cout<<" Getting GENMOVEParams"<<std::endl;
      getGenMoveParams();
      break;
    }
    default:
      std::cout<<" unknown command"<<std::endl;

  }
  //delete the buffer to free up the memory
  releaseBufferMemory();
}

/**************************************************************************************************************************************
**************************************************** CREATE COMMAND FUNCTION ********************************************************
***************************************************************************************************************************************/

/*
 * gets the command to send in byte format
 * param is a parameter that holds in it the value of the parameter for the given header when requested
 * most get methods will not be found here because they are response messages and are dealt with as input
 * whereas set move and request are output commands
 */
void thordrive::getByteCommand(command_t c,unsigned char hexcomm[],unsigned char param,unsigned char param2,unsigned char destination){

  switch(c)
  {
    case MOD_INDENTIFY:
    {
      hexcomm[0] = 0x23;
      hexcomm[1] = 0x02;
      hexcomm[2] = 0x00;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;//0x50this is for generic usb -could be different for 3 channel controller
      hexcomm[5] = 0x01;//host pc
      buffSize = 0;
      break;
    }
    case HW_REQ_INFO:
    {
      hexcomm[0] = 0x05;
      hexcomm[1] = 0x00;
      hexcomm[2] = 0x00;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;//0x50
      hexcomm[5] = 0x01;//host pc
      buffSize = 90;
      break;
    }
    case MOD_REQ_CHANENABLESTATE:
    {
      //std::cout << "Send MOD_REQ_CHANENABLESTATE\n";
      hexcomm[0] = 0x11;
      hexcomm[1] = 0x02;
      hexcomm[2] = param; // channel identification for channel we want info for
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;//0x50
      hexcomm[5] = 0x01;//host pc
      buffSize = 6;
      break;
    }
    case MOD_SET_CHANENABLESTATE:
    {
      //std::cout << "Send MOD_SET_CHANENABLESTATE\n";
      hexcomm[0] = 0x10;
      hexcomm[1] = 0x02;
      hexcomm[2] = param;//channel
      hexcomm[3] = param2;//state
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;//host pc
      buffSize = 0;
      break;
    }
    case MOT_MOVE_HOME:
    {
      //std::cout << "Send MOT_MOVE_HOME\n";
      hexcomm[0] = 0x43;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 6; //message returned on homed just header
      homing = true;
      setActiveDrive(destination);
      break;
    }
    case HW_DISCONNECT:
    {
      hexcomm[0] = 0x02;
      hexcomm[1] = 0x00;
      hexcomm[2] = 0x00;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_VELPARAMS:
    {
      //std::cout << "Send MOT_REQ_VELPARAMS\n";
      hexcomm[0] = 0x14;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 20; //message returned on homed just header
      break;
    }
    case MOT_SET_VELPARAMS:
    {
      //std::cout << "Send MOT_SET_VELPARAMS\n";
      hexcomm[0] = 0x13;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x0e;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_MOVERELPARAMS:
    {
      hexcomm[0] = 0x46;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 12;
      break;
    }
    case MOT_SET_MOVERELPARAMS:
    {
      hexcomm[0] = 0x45;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x06;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_MOVEABSPARAMS:
    {
      hexcomm[0] = 0x51;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 12;
      break;
    }
    case MOT_SET_MOVEABSPARAMS:
    {
      hexcomm[0] = 0x50;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x06;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_HOMEPARAMS:
    {
      hexcomm[0] = 0x41;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 20;
      break;
    }
    case MOT_SET_HOMEPARAMS:
    {
      //std::cout << "Send MOT_SET_HOMEPARAMS\n";
      hexcomm[0] = 0x40;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x0E; // channel
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_JOGPARAMS:
    {
      //std::cout << "Send MOT_REQ_JOGPARAMS\n";
      hexcomm[0] = 0x17;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 28;
      break;
    }
    case MOT_SET_JOGPARAMS:
    {
      //std::cout << "Send MOT_SET_JOGPARAMS\n";
      hexcomm[0] = 0x16;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x16;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_STATUSUPDATE:
    {
      //std::cout << "Send MOT_REQ_STATUSUPDATE\n";
      hexcomm[0] = 0x80;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 20;
      //std::cout << "buff size is: @"<<buffSize<<"\n";
      break;
    }
    case MOT_REQ_DCSTATUSUPDATE:
    {
      hexcomm[0] = 0x90;
      hexcomm[1] = 0x04;
      hexcomm[2] = param;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 20;
      break;
    }
    case MOT_MOVE_RELATIVE:
    {
      //here the long version of command is used, data packet must be appended to header; it encodes the new desired relative
      //distance param, this is better than first setting the parameter and then giving the short version of the command
      hexcomm[0] = 0x48;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x06; //defines the data size appended to command
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 20;
      setActiveDrive(destination);
      break;
    }
    case MOT_MOVE_ABSOLUTE:
    {
      //here the long version of command is used, data packet must be appended to header; it encodes the new desired absolute
      //distance param, this is better than first setting the parameter and then giving the short version of the command
      hexcomm[0] = 0x53;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x06; //need to ensure this param is correct for both TDS001 and BSC203 motor channels
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 20;
      setActiveDrive(destination);
      break;
    }
    case MOT_MOVE_VELOCITY:
    {
      //this command will move the motor continuously at previously set speed, until either a stop is recieved or limit is reached
      hexcomm[0] = 0x57;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //need to ensure this param is correct for both TDC001 and BSC203 motor channels
      hexcomm[3] = param2; // direction of movement 0x01 = forward cw rod moves inward retracting into actuator, 0x02 = back acw rod moves outward
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 0;
      setActiveDrive(destination);
      break;
    }
     case MOT_MOVE_JOG:
    {
      //this command will move the motor continuously at previously set speed, until either a stop is recieved or limit is reached
      hexcomm[0] = 0x6A;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //need to ensure this param is correct for both TDS001 and BSC203 motor channels
      hexcomm[3] = param2; // direction of movement 0x01 = forward cw, 0x02 = back acw
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 20;
      setActiveDrive(destination);
      break;
    }
     case MOT_MOVE_STOP:
    {
      //this command will stop the motor in profiled mode - param at byte 4 = 0x02, 0x01 stops immediately
      hexcomm[0] = 0x65;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //need to ensure this param is correct for both TDS001 and BSC203 motor channels
      hexcomm[3] = 0x02; // how to stop
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 20;
      break;
    }
    case MOT_REQ_LIMSWITCHPARAMS:
    {
      //
      hexcomm[0] = 0x24;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //need to ensure this param (channel) is correct for both TDS001 and BSC203 motor channels
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 22;
      break;
    }
     case MOT_SET_LIMSWITCHPARAMS:
    {
      //std::cout << "Send MOT_SET_LIMSWITCHPARAMS\n";
      //this command will stop the motor in profiled mode - param at byte 4 = 0x02, 0x01 stops immediately
      hexcomm[0] = 0x23;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x10; //need to ensure this param (chanel) is correct for both TDS001 and BSC203 motor channels
      hexcomm[3] = 0x00;
      hexcomm[4] = destination |0x80 ;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case RACK_REQ_BAYUSED:
    {
      //std::cout << "Send RACK_REQ_BAYUSED\n";
      hexcomm[0] = 0x60;
      hexcomm[1] = 0x00;
      hexcomm[2] = param; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 6;
      break;
    }
    case MOT_REQ_PMDSTAGEAXISPARAMS:
    {
      //std::cout << "Send MOT_REQ_PMDSTAGEAXISPARAMS\n";
      hexcomm[0] = 0xF1;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 80;
      break;
    }
    case MOT_SET_PMDSTAGEAXISPARAMS:
    {
      //std::cout << "Send MOT_SET_PMDSTAGEAXISPARAMS\n";
      hexcomm[0] = 0xF2;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x4A;
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
     case HW_NO_FLASH_PROGRAMMING:
    {
      //this is the first command that needs to be sent on startup
      //std::cout << "Send HW_NO_FLASH_PROGRAMMING\n";
      hexcomm[0] = 0x18;
      hexcomm[1] = 0x00;
      hexcomm[2] = 0x00; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_SET_POWERPARAMS:
    {
      //this is the first command that needs to be sent on startup
      //std::cout << "Send MOT_SET_POWERPARAMS\n";
      hexcomm[0] = 0x26;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x06; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 0;
      break;
    }
    case MOT_REQ_POWERPARAMS:
    {
      //this is the first command that needs to be sent on startup
      //std::cout << "Send MOT_REQ_POWERPARAMS\n";
      hexcomm[0] = 0x27;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 12;
      break;
    }
     case MOT_REQ_GENMOVEPARAMS:
    {
      //this is the first command that needs to be sent on startup
      //std::cout << "Send MOT_REQ_GENMOVEPARAMS\n";
      hexcomm[0] = 0x3b;
      hexcomm[1] = 0x04;
      hexcomm[2] = param; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination;
      hexcomm[5] = 0x01;
      buffSize = 12;
      break;
    }
     case MOT_SET_GENMOVEPARAMS:
    {
      //this is the first command that needs to be sent on startup
      //std::cout << "Send MOT_SET_GENMOVEPARAMS\n";
      hexcomm[0] = 0x3a;
      hexcomm[1] = 0x04;
      hexcomm[2] = 0x06; //bay identifier
      hexcomm[3] = 0x00;
      hexcomm[4] = destination | 0x80;
      hexcomm[5] = 0x01;
      buffSize = 12;
      break;
    }
    default:
      hexcomm[0] = 0x00;
      hexcomm[1] = 0x00;
      hexcomm[2] = 0x00;
      hexcomm[3] = 0x00;
      hexcomm[4] = 0x00;
      hexcomm[5] = 0x00;
      buffSize = 0;
  }
}
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

*********************************************************************************************************************************************
**************************************************** HIGH LEVEL FUNCTIONS *******************************************************************
*********************************************************************************************************************************************
**&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

/*********************************************************************************************************************************************
**************************************************** INIT FUNCTIONS *******************************************************************
*********************************************************************************************************************************************/
void thordrive::init()
{
  if(tdc){
    initTDC();
  }
  else{
    initDRV();
  }
}
void thordrive::initDRV()
{

  //bsc203 - talking to the main controller board
  unsigned char channel,state;
  channel = 0x01;
  state = 0x01;
  int home_dir = 0x01; // reverse direction
  std::cout<<"***************************** Initialising ******************************"<<std::endl;
  doHWFlash(0x11);
  doHWFlash(0x21);
  doHWFlash(0x22);
  std::cout<<"***************************** Requesting Info ******************************"<<std::endl;
  getInfo(0x11);
  std::cout<<"***********************************************************"<<std::endl;
  getBayUsedState(0x00,0x00,0x11);
  //this message is sent to motherboard of controller 0x11 asking about channel 1
  getBayUsedState(0x01,0x00,0x11); // Channel 0x01, destination 0x11 (motherboard)
  //this message is sent to motherboard of controller 0x11 asking about channel 2
  getBayUsedState(0x02,0x00,0x11); // Channel 0x02, destination 0x11 (motherboard)
  getInfo(0x11);
  std::cout<<"***************************** setting all Slot 1 settings ******************************"<<std::endl;
  //this is addressed directly to the slot or bay which may have two channels, we are addressing channel 1 of
  //slot 1, then setting it and requesting the values again
  setEnabledState(channel,state,0x11);
  setEnabledState(channel,state,0x21);
  std::cout<<"***************************** channel enabled ******************************"<<std::endl;
  double mm = 1.0;//5.0;
  // we want to move 1mm in 5 sec - lets slow the motors down
  double t_secs = 6.0;
  setVelParams(channel,0x21,mm,t_secs);
  setJogParams(channel,0x21);
  // limitswitch params set here for windows but their default home dir is different to ours
  setPowerParams(channel,0x21);
  setGenMoveParams(channel,0x21);
  setHomeParams(channel,0x21,home_dir);
  setLimitSwitchParams(channel, 0x21,0,0);
  setMoveRelParams(channel,0.00385,0x21);
  setMoveAbsParams(channel,0x21);
  std::cout<<"***************************** setting all Slot 2 settings ******************************"<<std::endl;
  setEnabledState(channel,state,0x11);
  setEnabledState(channel,state,0x22);
  std::cout<<"***************************** channel enabled ******************************"<<std::endl;
  setVelParams(channel,0x22,mm, t_secs);
  setJogParams(channel,0x22);
  // limitswitch params set here for windows but their default home dir is different to ours
  setPowerParams(channel,0x22);
  setGenMoveParams(channel,0x22);
  setHomeParams(channel,0x22,home_dir);
  setLimitSwitchParams(channel, 0x22,0,0);
  setMoveRelParams(channel,0.00385,0x22);
  setMoveAbsParams(channel,0x22);

  std::cout<<"***************************** slot 1 Homing ******************************"<<std::endl;
  moveHome(0x01,0x21);
  std::cout<<"***************************** slot 2 Homing ******************************"<<std::endl;
  moveHome(0x01,0x22);
  getVelParams(0x01,0x21);
  getVelParams(0x01,0x22);
  // centre the two linear actuators
  moveAbsolute(0x01,-7.8,0x21);
  moveAbsolute(0x01,-7.8,0x22);
}
void thordrive::initTDC()
{
  doHWFlash(0x50);
  identifyChannel(0x50);
  getInfo(0x50);
  double vel,acc;
  vel = 0.75; //reduced the speed by a quarter based on 1 mm per sec
  acc = 0.5; // reduce the accelleration by half 0.5
  //acc = 1;
  setTDCVelParams(0x01,0x50,vel,acc);
  std::cout<<"velocity params for tdc are: \n";
  getVelParams(0x01,0x50);
  moveHome(0x01,0x50);

}




/*********************************************************************************************************************************************
**************************************************** PARAMETER GETTERS AND SETTERS ***********************************************************
*********************************************************************************************************************************************/
void thordrive::getVelParams(unsigned char chan,unsigned char destination){
   control_comm = thordrive::MOT_REQ_VELPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   sendByteCommand(comarray,6);
   unsigned char expected_comm[2];
   expected_comm[0]= 0x15;
   expected_comm[1]= 0x04;
   receiveData(expected_comm);
   control_comm = lookupCommand();
   processRespose(control_comm);
}
void thordrive::getMoveRelParams(unsigned char chan,unsigned char destination){
   control_comm = thordrive::MOT_REQ_MOVERELPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   sendByteCommand(comarray,6);
   //wait until command completes and get results - should be a move completed command = getstatus update
   unsigned char expected_comm[2];
   expected_comm[0]= 0x47;
   expected_comm[1]= 0x04;
   receiveData(expected_comm);
   control_comm = lookupCommand();
   processRespose(control_comm);
}
void thordrive::getJogParams(unsigned char chan,unsigned char destination){
   control_comm = thordrive::MOT_REQ_JOGPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   sendByteCommand(comarray,6);
   //wait until command completes and get results - should be a move completed command = getstatus update
   unsigned char expected_comm[2];
   expected_comm[0]= 0x18;
   expected_comm[1]= 0x04;
   receiveData(expected_comm);
   control_comm = lookupCommand();
   processRespose(control_comm);
}
void thordrive::getMoveAbsParams(unsigned char chan,unsigned char destination){
   control_comm = thordrive::MOT_REQ_MOVEABSPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   sendByteCommand(comarray,6);
   //wait until command completes and get results - should be a move completed command = getstatus update
   unsigned char expected_comm[2];
   expected_comm[0]= 0x52;
   expected_comm[1]= 0x04;
   receiveData(expected_comm);
   control_comm = lookupCommand();
   processRespose(control_comm);
}
void thordrive::getHomeParams(unsigned char chan, unsigned char destination){

    control_comm = thordrive::MOT_REQ_HOMEPARAMS;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    sendByteCommand(comarray,6);
    unsigned char expected_comm[2];
    expected_comm[0]= 0x42;
    expected_comm[1]= 0x04;
    receiveData(expected_comm);
    control_comm = lookupCommand();
    processRespose(control_comm);

}
void thordrive::getLimitswitchParams(unsigned char chan,unsigned char destination){

     control_comm = thordrive::MOT_REQ_LIMSWITCHPARAMS;
     unsigned char comarray[6];
     getByteCommand(control_comm, comarray,chan,0x00,destination);
     sendByteCommand(comarray,6);
     unsigned char expected_comm[2];
     expected_comm[0]= 0x25;
     expected_comm[1]= 0x04;
     receiveData(expected_comm);
     control_comm = lookupCommand();
     processRespose(control_comm);
}
void thordrive::stopMotor(unsigned char chan,unsigned char destination){

    //issue a stop command
    control_comm = thordrive::MOT_MOVE_STOP;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    sendByteCommand(comarray,6);
    isActive = false;
    activeDrive = "";
}
void thordrive::getEnabledState( int chan,unsigned char destination){
    control_comm = thordrive::MOD_REQ_CHANENABLESTATE;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    sendByteCommand(comarray,6);
    unsigned char expected_comm[2];
    expected_comm[0]= 0x12;
    expected_comm[1]= 0x02;
    receiveData(expected_comm);
    control_comm = lookupCommand();
    processRespose(control_comm);
}
void thordrive::getBayUsedState(int bay,unsigned char state,unsigned char destination){

    control_comm = thordrive::RACK_REQ_BAYUSED;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,bay,state,destination);
    sendByteCommand(comarray,6);
    unsigned char expected_comm[2];
    expected_comm[0]= 0x61;
    expected_comm[1]= 0x00;
    receiveData(expected_comm);
    control_comm = lookupCommand();
    processRespose(control_comm);
}
void thordrive::getPMDStageAxisParams(unsigned char chan, unsigned char destination)
{
  control_comm = thordrive::MOT_REQ_PMDSTAGEAXISPARAMS;
  unsigned char comarray[6];
  getByteCommand(control_comm, comarray,chan,0x00,destination);
  sendByteCommand(comarray,6);
  unsigned char expected_comm[2];
  expected_comm[0]= 0xF2;
  expected_comm[1]= 0x04;
  receiveData(expected_comm);
  control_comm = lookupCommand();
  processRespose(control_comm);
}


void thordrive::setMoveAbsParams(unsigned char chan, unsigned char destination)
{
   control_comm = thordrive::MOT_SET_MOVEABSPARAMS;
   unsigned char comarray[12];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   encodeMoveAbsParams(comarray,chan);
   sendByteCommand(comarray,12);
}

void thordrive::setMoveRelParams(unsigned char chan, double distmm, unsigned char destination)
{

   control_comm = thordrive::MOT_SET_MOVERELPARAMS;
   unsigned char comarray[6];
   signed char command[12];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
    for(int i = 0; i < 6; i++){
     command[i] = static_cast<signed char>(comarray[i]);
   }
   encodeMoveParams(command,chan,distmm);
   sendSignedByteCommand(command,12);
}
void thordrive::setGenMoveParams(unsigned char chan, unsigned char destination)
{
   control_comm = thordrive::MOT_SET_GENMOVEPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,0x00,0x00,destination);
   signed char command[12];
   for(int i = 0; i < 6; i++){
     command[i] = static_cast<signed char>(comarray[i]);
   }
   encodeMoveParams(command,chan,0.00125);
   sendSignedByteCommand(command,12);
}


void thordrive::setPowerParams(unsigned char chan, unsigned char destination)
{
   control_comm = thordrive::MOT_SET_POWERPARAMS;
   unsigned char comarray[12];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   encodeBSCPowerParams(comarray,chan);
   sendByteCommand(comarray,12);
}
void thordrive::setVelParams(unsigned char chan, unsigned char destination,double mm, double t_secs)
{
   control_comm = thordrive::MOT_SET_VELPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   unsigned char command[20];
   for(int i = 0; i < 6; i++){
     command[i] = comarray[i];
   }
   setBSCVelocityParams(command,mm,t_secs, chan);
   sendByteCommand(command,20);
}

void thordrive::setTDCVelParams(unsigned char chan, unsigned char destination,double vel, double acc)
{
   control_comm = thordrive::MOT_SET_VELPARAMS;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   unsigned char command[20];
   for(int i = 0; i < 6; i++){
     command[i] = comarray[i];
   }
   setTDCVelocityParams(command,vel,acc);
   sendByteCommand(command,20);
}

void thordrive::setHomeParams(unsigned char chan, unsigned char destination, int home_dir){
    double vel = 5.0;
    control_comm = thordrive::MOT_SET_HOMEPARAMS;
    unsigned char comarray[20];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    encodeHomeParams(comarray,chan,vel,home_dir); //1 mm per second
    sendByteCommand(comarray,20);
}

void thordrive::setJogParams(unsigned char chan, unsigned char destination){
    //double vel = 1.0;
    control_comm = thordrive::MOT_SET_JOGPARAMS;
    unsigned char comarray[28];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    encodeBSCJogParams(comarray,1.0); //1 mm per second
    sendByteCommand(comarray,28);
    // c.receiveData();
    // c.control_comm = c.lookupCommand();
    // c.processRespose(c.control_comm);
}

void thordrive::setEnabledState(int chan,unsigned char state,unsigned char destination){
    control_comm = thordrive::MOD_SET_CHANENABLESTATE;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,chan,state,destination);
    sendByteCommand(comarray,6);
}


void thordrive::setLimitSwitchParams(signed char chan, signed char destination,double cw, double ccw)
{
    control_comm = thordrive::MOT_SET_LIMSWITCHPARAMS;
    unsigned char comarray[22];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    //signed char command[22];
    encodeLimitSwitchParams(comarray,chan,cw,ccw);
    sendByteCommand(comarray,22);

    //c.receiveData();
    //c.control_comm = c.lookupCommand();
    //c.processRespose(c.control_comm);
}

void thordrive::setPMDStageAxisParams(unsigned char destination)
{
  control_comm = thordrive::MOT_REQ_PMDSTAGEAXISPARAMS;
  unsigned char comarray[80];
  getByteCommand(control_comm, comarray,0x00,0x00,destination);
  encodePMDStageAxisParams(comarray);
  sendByteCommand(comarray,80);
}

void thordrive::setIsTDC(bool isTDC)
{
  tdc = isTDC;
}
/*********************************************************************************************************************************************
**************************************************** GENERAL GETSTATUS AND INFO FUNCTIONS ****************************************************
*********************************************************************************************************************************************/
void thordrive::getStatusUpdates(unsigned char destination, unsigned char chan){
  unsigned char expected_comm[2];
  if(tdc){
      control_comm = thordrive::MOT_REQ_DCSTATUSUPDATE;
      expected_comm[0]= 0x91;
      expected_comm[1]= 0x04;
    }
    else{
      control_comm = thordrive::MOT_REQ_STATUSUPDATE;
      expected_comm[0]= 0x81;
      expected_comm[1]= 0x04;
    }
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    sendByteCommand(comarray,6);
    receiveData(expected_comm);
    control_comm = lookupCommand();
    processRespose(control_comm);
}
void thordrive::getInfo(unsigned char destination){
   control_comm = thordrive::HW_REQ_INFO;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,0x00,0x00,destination);
   sendByteCommand(comarray,6);
   unsigned char expected_comm[2];
   expected_comm[0]= 0x06;
   expected_comm[1]= 0x00;
   receiveData(expected_comm);
   //identify which command is being returned in response buffer
   control_comm = lookupCommand();
   //process response based on command
   processRespose(control_comm);
}
/*
 * tested on tdc and working
 */
void thordrive::identifyChannel(unsigned char destination){

  control_comm = thordrive::MOD_INDENTIFY;
  unsigned char comarray[6];
  getByteCommand(control_comm, comarray,0x00,0x00,destination);//param1 used for chanel when necessary, param2 used for any other param necessary for given command
  sendByteCommand(comarray,6);

}
/*
 * testing on tdc and working
 */
void thordrive::disconnect(unsigned char destination){
   control_comm = thordrive::HW_DISCONNECT;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,0x00,0x00,destination);
   sendByteCommand(comarray,6);
}

void thordrive::doHWFlash(unsigned char destination)
{
    control_comm = thordrive::HW_NO_FLASH_PROGRAMMING;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,0x00,0x00,destination);
    sendByteCommand(comarray,6);
}

void thordrive::checkWithinLimit(unsigned char chan, unsigned char destination)
{
  //std::cout<<"Polling for position of actuator......\n";
  //destination will determine which actuator position to get and store
  bool wait = true;
  unsigned char comarray[6];
  while(wait){
    control_comm = thordrive::MOT_REQ_STATUSUPDATE;   // needs to be reset each time because it is changed when a reponse is received

    getByteCommand(control_comm, comarray,chan,0x00,destination); // must do this to set the buffer size and memory again
    sendByteCommand(comarray,6);
    //std::cout<<"sent req status update"<<control_comm << "\n";
    unsigned char expected_comm[2];
    expected_comm[0]= 0x81;
    expected_comm[1]= 0x04;
    receiveData(expected_comm);
    control_comm = lookupCommand();
    processRespose(control_comm);
    //std::cout<<"actuator position is now "<<getScaledActuatorPosition(destination)<<"\n";
    if(homed || getScaledActuatorPosition(destination) <= -16.5){
      wait = false;
      //send stop command
      //std::cout<<"sending stop now *********\n";
      stopMotor(chan,destination);
    }
    else{
     //sleep(0.5);
      /**count ++;
      if(count > 15){
    std::cout<<" quiting due to loop count\n";
    break;
      }**/
    }
  }

}
/**
 * this function ensures that the limit is never exceeded when issuing a relative move command
 */
double thordrive::getMaxAllowableRelMoveDistance(unsigned char destination, double movedist)
{
  double limit = -17;
  double maxAllowableRelMoveDist = 0;
   // first get the currently active drive's encoder position
  double currentEncPos = 0;

  currentEncPos = getScaledActuatorPosition(destination); // is this the scaled position?
  //std::cout<<"current pos for active actuator is "<<currentEncPos<<"\n";

  if(movedist < 0){
    // movedist is negative, this means that we need to check if it will go beyond the limit
    if((movedist > limit ) && ((currentEncPos + movedist) > limit)){
        //does not exceed the limit, move is allowed
        maxAllowableRelMoveDist = movedist;
        //std::cout<<"can move the distance wanted...\n";
    }
    else{
      //exceeded the limit
        // moved beyond the limit so give the leftover relative value that takes it to the limit
        maxAllowableRelMoveDist = limit - (currentEncPos);
        std::cout<<"MAXALLOWABLE is ..." << maxAllowableRelMoveDist<<"\n";
    }
  }
  else{
      // the move is positive don't go beyond 0
      if((currentEncPos + movedist) > 0){
          // make sure that value still remains positive
          maxAllowableRelMoveDist = abs(0 - currentEncPos);

      }
      else{
          //does not go beyond home limit so can move desired amount
          maxAllowableRelMoveDist = movedist;
      }
      std::cout<<"can move the distance wanted...\n";
  }
  return maxAllowableRelMoveDist;

   //either return the existing distance or then the max allowable distance.
}
/**
 * this function ensures that the limit is never exceeded when issuing a relative move command
 */
double thordrive::getMaxAllowableAbsMoveDistance(double movedist)
{
  double limit = -17;
  double maxAllowableMoveDist = 0;

  //calculate whether the current position + the relative move distance are witin the limit
  if(movedist > limit){
    std::cout<<"can move the distance wanted...\n";
    return movedist;
  }
  else{
    //std::cout<<"move distance needs to be smaller to fit in limit...\n";
    maxAllowableMoveDist = limit;
    std::cout<<"Max allowable distance is now "<<maxAllowableMoveDist<<"\n";
  }
  return maxAllowableMoveDist;

   //either return the existing distance or then the max allowable distance.
}

/*********************************************************************************************************************************************
**************************************************** MOVE FUNCTIONS **************************************************************************
*********************************************************************************************************************************************/
void thordrive::moveRelative(unsigned char chan,double distmm,unsigned char destination){

   control_comm = thordrive::MOT_MOVE_RELATIVE;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,0x00,0x00,destination);
   signed char command[12];
   for(int i = 0; i < 6; i++){
     command[i] = static_cast<signed char>(comarray[i]);
   }
   if(!tdc){
    distmm = getMaxAllowableRelMoveDistance(destination, distmm);
    //distmm = convertToAbsolutePosition(distmm,destination);
   }
   encodeMoveParams(command,chan,distmm);
   sendSignedByteCommand(command,12);
}
void thordrive::moveAtVelocity(unsigned char chan, double distmm, unsigned char destination, unsigned char direction)
{
  control_comm = thordrive::MOT_MOVE_VELOCITY;
  unsigned char comarray[6];
  getByteCommand(control_comm, comarray,chan,direction,destination);
  sendByteCommand(comarray,6);
  bool stop = false;
  while(!stop){
    getStatusUpdates(destination,chan);
    double currentPos = getScaledActuatorPosition(destination);
    if(currentPos == distmm){// FIXME this does not work! because its all a case of timing, - what is gone past?
      stopMotor(chan,destination);
      stop = true;
    }

  }
}

void thordrive::moveJog(unsigned char chan, unsigned char direction,unsigned char destination){
   control_comm = thordrive::MOT_MOVE_JOG;
   unsigned char comarray[6];
   getByteCommand(control_comm, comarray,chan,direction,destination);//jog direction
   sendByteCommand(comarray,6);
   //wait until command completes and get results - should be a move completed command = getstatus update
   //c.receiveData();
   //c.control_comm = c.lookupCommand();
   //c.processRespose(c.control_comm);
}
void thordrive::moveAbsolute(signed char chan,double distmm,signed char destination){

   control_comm = thordrive::MOT_MOVE_ABSOLUTE;
   unsigned char comarray[12];
   getByteCommand(control_comm, comarray,chan,0x00,destination);
   signed char command[12];
   for(int i = 0; i < 12; i++){
     command[i] = static_cast<signed char>(comarray[i]);
   }
   /*if(!tdc){
    distmm = getMaxAllowableAbsMoveDistance(destination);
   }*/
   encodeMoveParams(command,chan,distmm);
   sendSignedByteCommand(command,12);
   //ensureMoveCompleted(chan,destination,distmm);
}
void thordrive::moveHome(unsigned char chan,unsigned char destination){
    control_comm = thordrive::MOT_MOVE_HOME;
    unsigned char comarray[6];
    getByteCommand(control_comm, comarray,chan,0x00,destination);
    sendByteCommand(comarray,6);
    std::cout<<" start waiting\n";
    waitWhileHoming(chan,destination);
    std::cout<<" end waiting\n";

}
/*********************************************************************************************************************************************
**************************************************** WAIT FUNCTIONS **************************************************************************
*********************************************************************************************************************************************/
void thordrive::waitWhileHoming(unsigned char chan, unsigned char destination)
{
  std::cout<<"Polling for status of motor......\n";
  bool wait = true;
  double position;
  unsigned char expected_comm[2];
  control_comm = thordrive::MOT_REQ_STATUSUPDATE;
  unsigned char comarray[6];
  int count = 0; // a counter because we do not want to wait infinitely - motor should be homed within 60 seconds
  while(wait){
     if(tdc){
      control_comm = thordrive::MOT_REQ_DCSTATUSUPDATE;
    }
    else{
      control_comm = thordrive::MOT_REQ_STATUSUPDATE;   // needs to be reset each time because it is changed when a reponse is received
    }
    getByteCommand(control_comm, comarray,chan,0x00,destination); // must do this to set the buffer size and memory again
    sendByteCommand(comarray,6);
    if(tdc){
       expected_comm[0]= 0x91;
       expected_comm[1]= 0x04;

    }
    else{
       expected_comm[0]= 0x81;
       expected_comm[1]= 0x04;
    }
    receiveData(expected_comm);
    control_comm = lookupCommand();

    processRespose(control_comm);
    if(getActiveDrive() == "x"){
      position = getXactuatorPosition();
      //std::cout<<"X actuator position is now "<<position<<"\n";
    }
    else if(getActiveDrive() == "y"){
      position = getYactuatorPosition();
      //std::cout<<"Y actuator position is now "<<position<<"\n";
    }
    else{
      position = getScaledZ();
      //std::cout<<"Z actuator position is now "<<position<<"\n";
    }
    if(homed){
      //if(count>5){
        //if(position == 0){
            //wait = false;
            //homing =false;
        //}
      //}
      //else{
      wait = false;
      homing = false;
      //}
    }

    //sleep(0.5);
    //std::cout<<"sleeping 5 ......"<<"\n";
    count ++;
  }
  /**std::cout<<"checking if the homed message is sent \n";
  expected_comm[0]= 0x44;
  expected_comm[1]= 0x04;
  c.receiveData(expected_comm);
  c.control_comm = c.lookupCommand();
  c.processRespose(c.control_comm); **/

}


void thordrive::ensureMoveCompleted(unsigned char chan, unsigned char destination,double endPos)
{
  //std::cout<<"Polling for position of actuator......\n";
  //destination will determine which actuator position to get and store
  // for relative moves value will be positive so > than 0, else its negative
  bool wait = true;
  //unsigned char comarray[6];
  double currentPos;
  int count = 0; // a counter because we do not want to wait infinitely - motor should be homed within 60 seconds
  while(wait){
    getStatusUpdates(destination,chan);
    currentPos = getScaledActuatorPosition(destination);
    //std::cout<<"actuator position is now "<<currentPos<<" and dist is "<<endPos<< "\n";
    if(currentPos == endPos){
      wait = false;
    //std::cout<<"exiting 1 \n";
    }
    else if((currentPos == 0) & (endPos > 0)){
      wait = false;
    //std::cout<<"exiting 2 \n";
    }
    else{
      // this may occur when motor start pos in no longer at expected because it has started to move ie startpos =-15
      // movedist = 5, currentpos = -14 as actuator has already started to move by the time the pos is requested so end pos is then
      //-14 + 5 = -9 when it should be -10.  Stops waiting indefinately.
      count++;
    }
    if(count > 1500){
      wait = false;
      //std::cout<<"exiting 3 \n";
    }
    /*if(distmm >= 0){
      if(currentPos >= distmm){
    wait = false;
    std::cout<<"exiting 1 \n";
      }
      else{
    count++;
      }
    }
    else{
      if(currentPos <= distmm){
    wait = false;
    std::cout<<"exiting 2 \n";
      }
      else{
    count++;
      }
    }
    //count++;
    if(count > 1500){
      wait = false;
      std::cout<<"exiting 3 \n";
    }
    /*if(lastPos == currentPos){
      count++;
    }
    else{
      lastPos = currentPos;
    }
    if(count == 10){
      wait = false;
      std::cout<<"*******Move Completed*******\n";
    }*/
  }

}

/*
 * takes a relative distance and converts it to an absolute value in mm
 * */

double thordrive::convertToAbsolutePosition(double movedist, char destination)
{
  double currentEncPos = 0;
  if (destination == 0x21){
    currentEncPos = getXactuatorPosition(); // is this the scaled position?
  }
  else if(destination == 0x22){
    currentEncPos = getYactuatorPosition();
  }
   //std::cout<<"the distance is now "<<currentEncPos + movedist<<"\n";
   return currentEncPos + movedist;

}

void thordrive::waitWhileMoving(){
    bool wait = true;
    int count = 0; // failsafe
    while(wait){
        unsigned char expected_comm[2];
        expected_comm[0]= 0x64;
        expected_comm[1]= 0x04;
        receiveData(expected_comm);
        control_comm = lookupCommand();
        processRespose(control_comm);
        if(getMoveCompleted()){
          wait = false;
          //std::cout<<" end waiting, move completed received\n";
    }
    else{
      //std::cout<<" waiting ...\n";
      count ++;
    }
    if(count > 12){
      wait = false;
      //std::cout<<" end waiting Failsafe triggered, *** move completed not received\n";
    }
  }
  setMoveCompleted(false);

}
/*
 * A temporary function to end move motor loop.  This will not work with any values that are not wholes
 * Only used for validation of vision and hardware.
 */

bool thordrive::isMoveCompleted(double mDist){

   if(tdc){
       getStatusUpdates(0x50,0x01);//desination, channel
       //if(round(scaled_z) == mDist){
       if(isApproximatelyCompleted(scaled_z, mDist,tdc)){
           isActive = false;
           return true;
       }
   }
   else{
       if(getActiveDrive() == "x"){
           getStatusUpdates(0x21,0x01);//desination, channel
           //if(round(scaled_x) == mDist){
           if(isApproximatelyCompleted(scaled_x,mDist,false)){
               isActive = false;
               return true;
           }
       }
       else if(getActiveDrive() == "y"){
           getStatusUpdates(0x22,0x01);//desination, channel
           //if(round(scaled_y) == mDist){
           if(isApproximatelyCompleted(scaled_y,mDist,false)){
               isActive = false;
               return true;
           }
       }
   }
   return false;
}

/*
 * Checks whether the first value is approximately the same as the secord value
 * within a given margin - for translations a value of 0.00025, and for rotations
 * 0.05
 * BEWARE should only be used when moves are absolute! this is because the desired position
 * value will be an absolute position whereas relative moves are relative to a given position
 * so for example move to -3 vs move -3 from current position.  To use this function with relative moves,
 * desired position is absolute equivalent of relative position.
 * @param currentvalue the current value of the actuator position
 * @param desiredvalue the value of the desired stop position
 */
bool thordrive::isApproximatelyCompleted(double currentvalue, double desiredvalue, bool istdc){
    double tdcmargin = 0.05;
    double bscmargin = 0.8;
    if(istdc){
        //std::cout<<"tdc approx control, desired value"<<desiredvalue<<"\n";
        // this axis rotates
        double approxvalPlus = desiredvalue + tdcmargin;
        double approxvalMinus = desiredvalue - tdcmargin;
        //std::cout<<"approx value plus = "<<approxvalPlus<<"\n";
        //std::cout<<"approx value minus = "<<approxvalMinus<<"\n";
        //std::cout<<"current value = "<<currentvalue<<"\n";
        //if((currentvalue >= approxvalMinus && currentvalue <= approxvalPlus)){
        if(approxvalMinus < currentvalue && currentvalue < approxvalPlus){
            //std::cout<<"returning true\n";
            return true;
        }
    }
    else{
        if(desiredvalue == 0){
            //std::cout<<"bsc approx control, desired value = 0\n";
            double approxval = desiredvalue - bscmargin;
            //std::cout<<"approx value = "<<approxval<<"\n";
            if(currentvalue >= approxval && currentvalue <= desiredvalue  ){
                //td::cout<<"returning true\n";
                return true;
            }
        }
        else{
            //std::cout<<"bsc approx control, desired value"<<desiredvalue<<"\n";
            double approxvalPlus = desiredvalue + bscmargin;
            double approxvalMinus = desiredvalue - bscmargin;
            //std::cout<<"approx value plus = "<<approxvalPlus<<"\n";
            //std::cout<<"approx value minus = "<<approxvalMinus<<"\n";
            //std::cout<<"current value minus = "<<currentvalue<<"\n";
            if(currentvalue > approxvalMinus && currentvalue < approxvalPlus){
                //std::cout<<"returning true\n";
                return true;
            }
        }
    }

    return false;
}
/**
 * @brief thordrive::isDriveMoving calls getstatus updates and returns true if
 * drive is still moving, false otherwise
 * @return bool, true is drive is in motion
 */

bool thordrive::isDriveMoving(){
    if(tdc){
        getStatusUpdates(0x50,0x01);//desination, channel
    }
    else{
        if(getActiveDrive() == "x"){
            getStatusUpdates(0x21,0x01);//desination, channel
        }
        else if(getActiveDrive() == "y"){
            getStatusUpdates(0x22,0x01);//desination, channel
        }
    }

    if(!moving){
        isActive = false;
    }
    return moving;
}
/**
 * @brief thordrive::UpdateDrivePositions calls getstatus updates, ensuring that the current position
 * values are updated regularly
 * @return none
 */

void thordrive::updateDrivePositions(){
   bool xactive = false, yactive = false;
   if(tdc){
       getStatusUpdates(0x50,0x01);//desination, channel
   }
   else{
       // one may be active while the other is not, updates active drive only
       if(getActiveDrive() == "x"){
           xactive == true;

       }
       else if(getActiveDrive() == "y"){
           yactive == true;
       }
       if(xactive){
           //get status update, then set y active get update unset y active active
            getStatusUpdates(0x21,0x01);//desination, channel - x axis
            setActiveDrive(0x22);
            getStatusUpdates(0x22,0x01);//desination, channel
            setActiveDrive(0x21);
       }
       if(yactive){
           //set x active get status update, then set y active get update
            setActiveDrive(0x21);
            getStatusUpdates(0x21,0x01);//desination, channel - x axis
            setActiveDrive(0x22);
            getStatusUpdates(0x22,0x01);//desination, channel

       }
       if(!xactive && !yactive){
           //set x active get status update, then set y active get update, set z active because it must be the active drive
           setActiveDrive(0x21);
           getStatusUpdates(0x21,0x01);//desination, channel - x axis
           setActiveDrive(0x22);
           getStatusUpdates(0x22,0x01);//desination, channel
           setActiveDrive(0x50);
       }
   }
}
