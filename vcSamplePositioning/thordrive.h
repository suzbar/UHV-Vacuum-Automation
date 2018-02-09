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

#ifndef THORDRIVE_H
#define THORDRIVE_H


#include <iostream>
#include <stdio.h> // standard input / output functions
#include <string.h> // string function definitions
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions
#include <errno.h> // Error number definitions
#include <termios.h> // POSIX terminal control definitionss
#include <time.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <boost/concept_check.hpp>

class thordrive{

public:
   enum command_t
  {
    MOD_UNKNOWN,
    MOD_GET_CHANENABLESTATE,
    MOD_REQ_CHANENABLESTATE,
    MOD_SET_CHANENABLESTATE,
    RACK_REQ_BAYUSED,
    RACK_GET_BAYUSED,
    MOD_INDENTIFY,
    HW_REQ_INFO,
    HW_GET_INFO,
    HW_DISCONNECT,
    HW_START_UPDATEMSGS,
    HW_NO_FLASH_PROGRAMMING,
    MOT_GET_STATUSUPDATE,
    MOT_GET_DCSTATUSUPDATE,
    MOT_REQ_STATUSUPDATE,
    MOT_REQ_DCSTATUSUPDATE,
    MOT_SET_LIMSWITCHPARAMS,
    MOT_GET_LIMSWITCHPARAMS,
    MOT_REQ_LIMSWITCHPARAMS,
    MOT_SET_VELPARAMS,
    MOT_GET_VELPARAMS,
    MOT_REQ_VELPARAMS,
    MOT_SET_JOGPARAMS,
    MOT_REQ_JOGPARAMS,
    MOT_GET_JOGPARAMS,
    MOT_MOVE_JOG,
    MOT_MOVE_COMPLETED,
    MOT_SET_HOMEPARAMS,
    MOT_REQ_HOMEPARAMS,
    MOT_GET_HOMEPARAMS,
    MOT_MOVE_HOME,
    MOT_MOVE_HOMED,
    MOT_SET_MOVERELPARAMS,
    MOT_REQ_MOVERELPARAMS,
    MOT_GET_MOVERELPARAMS,
    MOT_MOVE_RELATIVE,
    MOT_SET_MOVEABSPARAMS,
    MOT_REQ_MOVEABSPARAMS,
    MOT_GET_MOVEABSPARAMS,
    MOT_MOVE_ABSOLUTE,
    MOT_MOVE_VELOCITY,
    MOT_MOVE_STOP,
    MOT_MOVE_STOPPED,
    HW_RICHRESPONSE,
    MOT_SET_PMDSTAGEAXISPARAMS,
    MOT_GET_PMDSTAGEAXISPARAMS,
    MOT_REQ_PMDSTAGEAXISPARAMS,
    MOT_SET_POWERPARAMS,
    MOT_GET_POWERPARAMS,
    MOT_REQ_POWERPARAMS,
    MOT_SET_GENMOVEPARAMS,
    MOT_GET_GENMOVEPARAMS,
    MOT_REQ_GENMOVEPARAMS
  }control_comm;

  typedef struct _HWINFO{
    uint32_t serialNum; //unique 8 digit serial number for controlleer
    std::string modelNum; //alphanumeric modelNum
    uint16_t hwType;
    uint32_t softwareVer; //sw version
    char notes[48];//info string
    uint16_t hwVersion;
    uint16_t modState;
    uint16_t numChannels;//number of channels for operation
  }hwInfo;
  /*!
  * \brief Constructor.
  */
  thordrive(bool is_tdc);

  /*!
  * \brief Constructor.
  * for testing purposes.
  */
  thordrive(){}
  /*!
  * \brief Destructor.
  */
  ~thordrive(){close(USB);}
  /*!
   * \brief Opens connection.
  */
  void openConnector(const char* usbport);
  void getVelParams(unsigned char chan,unsigned char destination);
  void setVelParams(unsigned char chan, unsigned char destination, double mm,double t_secs);
  void setTDCVelParams(unsigned char chan, unsigned char destination,double vel, double acc);
  void setMoveRelParams(unsigned char chan,double distmm,unsigned char destination);
  void getMoveRelParams(unsigned char chan,unsigned char destination);
  void getJogParams(unsigned char chan,unsigned char destination);
  void setMoveAbsParams(unsigned char chan,unsigned char destination);
  void getMoveAbsParams(unsigned char chan,unsigned char destination);
  void getHomeParams(unsigned char chan, unsigned char destination);
  void setHomeParams(unsigned char chan, unsigned char destination, int home_dir);
  void getLimitswitchParams(unsigned char chan,unsigned char destination);
  void stopMotor(unsigned char chan,unsigned char destination);
  void getStatusUpdates(unsigned char destination, unsigned char chan);
  void getInfo(unsigned char destination);
  void identifyChannel(unsigned char destination);
  void getEnabledState( int chan,unsigned char destination);
  void setEnabledState(int chan,unsigned char state,unsigned char destination);
  void getBayUsedState(int bay,unsigned char state,unsigned char destination);
  void disconnect(unsigned char destination);
  void setLimitSwitchParams(signed char chan,signed char destination,double cc,double ccw);
  void setJogParams(unsigned char chan, unsigned char destination);
  void doHWFlash(unsigned char destination);
  void setPMDStageAxisParams(unsigned char destination);
  void getPMDStageAxisParams(unsigned char chan, unsigned char destination);
  void setPowerParams(unsigned char chan, unsigned char destination);
  void setGenMoveParams(unsigned char chan, unsigned char destination);
  void updateDrivePositions();
  //double getActiveActuatorPosition(char destination);

  std::string getActiveDrive(){return activeDrive;}
  void setIsTDC(bool isTDC);
  void moveAtVelocity(unsigned char chan, double distmm, unsigned char destination, unsigned char direction);
  void moveJog(unsigned char chan, unsigned char direction,unsigned char destination);
  void moveAbsolute(signed char chan,double distmm,signed char destination);
  void moveHome(unsigned char chan,unsigned char destination);
  void moveRelative(unsigned char chan,double distmm,unsigned char destination);
  double getScaledActuatorPosition(char destination);
  bool getIsActive(){return isActive;}
  void setIsActive(bool active){isActive=active;}
  bool isMoveCompleted(double mDist);
  bool isDriveMoving();
  double getScaledZ(){return scaled_z;}
  double getScaledY(){return scaled_y;}
  double getscaledX(){return scaled_x;}

private:
  bool moveCompleted;
  //holds terminal connection attributes
  struct termios tty;
  struct termios tty_old;
  fd_set readSet;
  //port handle
  int USB,buffSize;
  unsigned char *buf; //recieve unsigned
  signed char *signed_buf;
  std::string activeDrive;
  double x,y,z;  //positions of actuators x and y are translations, z will be a rotation
  double scaled_x,scaled_y, scaled_z;
  bool homing,homed;
  //identifies which of the two expected types of controller the port is connected to (BSC203)
  bool tdc,isActive;
  bool moving;
  void getInfo();
  void getVelocityParams();
  void getMoveRelParams();
  void getMoveAbsParams();
  void getHomeParams();
  void getLimitSwitchParams();
  void getStoppedParams();
  void getStatusUpdates();
  void getDCStatusUpdates();
  void getJogParams();
  void getChanEnableState();
  void getRackBayUsed();
  void getPMDStageAxisParams();
  void getBSCPowerParams();
  void getGenMoveParams();
  void releaseBufferMemory();
  void releasesignedBufferMemory();
  void waitWhileHoming(unsigned char chan, unsigned char destination);
  void waitWhileMoving();
  void checkWithinLimit(unsigned char chan, unsigned char destination);
  double getMaxAllowableRelMoveDistance(unsigned char destination,double movedist);
  double getMaxAllowableAbsMoveDistance(double movedist);
  void ensureMoveCompleted(unsigned char chan, unsigned char destination, double endPos);
  double convertToAbsolutePosition(double movedist, char destination);
  bool isApproximatelyCompleted(double currenvalue, double desiredvalue, bool istdc);
  //low level functions
  void receiveResponse();
  void receiveData(unsigned char expected_command[], time_t timeout=10);
  void receiveSignedData(time_t timeout=10);
  void processRespose(command_t c);
  void sendByteCommand(unsigned char* cmd,int len);
  void sendSignedByteCommand(signed char* cmd,int len);
  void getByteCommand(command_t c,unsigned char hexcomm[],unsigned char param,unsigned char param2,unsigned char destination);
  void setTDCVelocityParams(unsigned char* cmd,double vel,double acc);
  void setBSCVelocityParams(unsigned char* cmd,double mm,double t_secs,unsigned char chan);
  void encodeMoveAbsParams(unsigned char* cmd, unsigned char chan);
  void encodeMoveParams(signed char* cmd,signed char chan, double distMM);
  void encodeHomeParams(unsigned char* cmd,unsigned char chan, double speedmm, int home_dir);
  void encodeLimitSwitchParams(unsigned char* cmd, unsigned char chan, double cw_slimit,double ccw_slimit);
  void encodeBSCJogParams(unsigned char* cmd,double mm_dist);
  void encodePMDStageAxisParams(unsigned char* cmd);
  void encodeBSCPowerParams(unsigned char* cmd,unsigned char chan);
  void setActiveDrive(unsigned char drive);
  void setTDC(bool is_tdc);
  bool getTDC(){return tdc;}
  void getRichResponse();
  void setHoming(bool val){homing=val;}
  bool getHoming(){return homing;}
  bool getHomed(){return homed;}
  double getXactuatorPosition(){return x;}
  double getYactuatorPosition(){return y;}
  double getZactuatorPosition(){return z;}

  enum command_t lookupCommand();
  void processSignedData();
  bool getMoveCompleted(){return moveCompleted;}
  void setMoveCompleted(bool completed){moveCompleted = completed;}
  void init();
  void initDRV();
  void initTDC();
};

#endif // THORDRIVE_H
