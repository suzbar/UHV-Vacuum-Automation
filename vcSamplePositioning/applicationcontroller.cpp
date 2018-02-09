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

#include "applicationcontroller.h"
#include "moc_applicationcontroller.cpp"
#include <sys/stat.h>
#include <stdio.h>
#include <ctime>

/**
 * Constructor
 */
applicationcontroller::applicationcontroller(bool stereo,QObject *parent) :
    QObject(parent), tdcDrive(true),bscDrives(false)
{
    basePath = "/home/szb/Documents/";
    positionSample = false;
    initAllEquipment(stereo);
    // prepare the motor drive position mapping
    fillmapX();
    fillmapY();
    //positions of linear actuators - prevents a bug in the system
    lastx = 0;
    lasty = 0;
}

/**
 * Destructor
 */
applicationcontroller::~applicationcontroller(){
    if(frameGrabber2.isConnected){
      frameGrabber2.close();
    }
    if(frameGrabber3.isConnected){
      frameGrabber3.close();
    }
    //close displays
    d2.close(img2);
    d3.close(img3);
    //disconect from actuators
    tdcDrive.disconnect(0x50);
    bscDrives.disconnect(0x21);


}
/**
 * Initialises all the hardware peripherals
 */
void applicationcontroller::initAllEquipment(bool stereo){

    //read file data on current ports in use etc. - static for debugging
    portnumC2 = 19; //should be first line of input file
    portnumC3 = 17;
    // initialise the run path and output files
    if(stereo){
        experimentPath = "sample_tracking/stereo/";
    }
    else{
        experimentPath = "sample_tracking/";
    }
    runName = getCurrentDT();
    std::string newfilepath = basePath + experimentPath + "run_data/" + runName + "/";
    // first folder - no need to check the return value of makefolder because if it returns then it is 1
    char *fp_char = const_cast<char*>(newfilepath.c_str());
    makeFolder(fp_char);
    //second level folder
    newfilepath += "image_out/";
    char *fpnext_char = const_cast<char*>(newfilepath.c_str());
    makeFolder(fpnext_char);
    //third level folder
    std::string nfp1,nfp2;
    nfp1 = newfilepath + "cam2/";
    char *nfp1_char = const_cast<char*>(nfp1.c_str());
    makeFolder(nfp1_char);
    nfp2 = newfilepath + "cam3/";
    char *nfp2_char = const_cast<char*>(nfp2.c_str());
    makeFolder(nfp2_char);
    nfp1 += "data/";
    nfp2 += "data/";
    nfp1_char = const_cast<char*>(nfp1.c_str());
    makeFolder(nfp1_char);
    nfp2_char = const_cast<char*>(nfp2.c_str());
    makeFolder(nfp2_char);

    initCameras();
    // initialise displays
    //d2.setDownScalingFactor(vpDisplay::SCALE_5);
    //d3.setDownScalingFactor(vpDisplay::SCALE_5);
    d2.init(img2, 0, 0, "Camera2 - sample tracking");
    d3.init(img3,10, 10, "Camera3 - sample tracking");
    /**d2.display(img2);
    d3.display(img3);
    d2.flush(img2);
    d3.flush(img3);*/

    //display captured images
    vpDisplay::display(img2);
    vpDisplay::display(img3);
    vpDisplay::flush(img2);
    vpDisplay::flush(img2);
    if(stereo){
        initStereoTracker();
    }
    else{
        //initialise trackers
        initTrackers();
    }


}
/**
 * Creates the data folder for all output
 */
int applicationcontroller::makeFolder(char* foldername){
    const int dir_err = mkdir(foldername, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err)
    {
        std::string fname(foldername);
        printf("Error creating directory!n");
        std::cout<<fname<<std::endl;
        exit(1);
    }
    else{
        return 1;
    }
}
/**
 * Current date and time for data folder name
 */
std::string applicationcontroller::getCurrentDT(){
        time_t currentTime;
        struct tm *localTime;

        time( &currentTime );                   // Get the current time
        localTime = localtime( &currentTime );  // Convert the current time to the local time
        int day    = localTime->tm_mday;
        int month  = localTime->tm_mon + 1;
        int year   = localTime->tm_year + 1900;
        int hour   = localTime->tm_hour;
        int min    = localTime->tm_min;
        //int sec    = localTime->tm_sec;
        std::stringstream ss;
        ss<<year<<"-"<<month<<"-"<<day<<"_"<<hour<<"-"<<min;
        //string temp = ss.str();
        std::string timestring = ss.str();

        /*timestring += "-" + std::string(itoa(month));
        timestring += "-" + std::string(itoa(day));
        timestring += "-" + std::string(itoa(hour));
        timestring += "-" + std::string(itoa(min));*/
        return timestring;
}

/**
 * Prints the error Vectors to file in csv format
 */
void applicationcontroller::printErrVector(std::ofstream &errwriter, vpColVector errvec){
   int errvec_size =  errvec.size();
   for(int i = 0; i< errvec_size;i++){
       errwriter<<errvec[i];
       if(i != errvec_size - 1){
          errwriter<<", ";
        }
   }
  errwriter<<std::endl;
  errwriter.flush();
}
/**
 * Initialises the cameras
 */
void applicationcontroller::initCameras(){
  try{
      frameGrabber2.open(img2);
      std::cout << "camera 2 is online" << std::endl;
      frameGrabber3.open(img3);
      std::cout << "camera 3 is online" << std::endl;
   }
   catch(...){
     perror("Opening vpUeyeFrameGrabber Failed");
     exit(1);

   }
   if(frameGrabber2.isConnected){
     //check the ports and based on that initialise the cameras params
     frameGrabber2.initialiseCamera(12);
   }
   if(frameGrabber3.isConnected){
     frameGrabber3.initialiseCamera(15);
   }
   try{
       //read the images from framegrabbers
       frameGrabber2.acquire(img2);
       frameGrabber3.acquire(img3);
   }
   catch(...){
       std::cout << "Cannot read one or both images "<< std::endl;
   }
   initialisedAndReady = true;

}
/**
 * Initialises the model based trackers
 */
void applicationcontroller::initTrackers(){
    //input file names for camera configuration, and pose initialisation
    std::string configFilec3,configFilec2, initFile2,initFile3, modelFileCao;
    std::string CONFIG_PATH = basePath + experimentPath + "config/";
    //camera parameters input file paths
    configFilec3 = CONFIG_PATH + "cam_settings/2016-03/camera3160.xml";//normal
    configFilec2 = CONFIG_PATH + "cam_settings/2016-03/camera2160.xml";//normal
    //sampleholder model file
    //modelFileCao = CONFIG_PATH + "sampleholder-october16.cao";
    modelFileCao = CONFIG_PATH + "sampleholder-march17.cao"; // latest version with tilts
    //initial pose input file paths
    if(portnumC3 == 17){
        initFile3 = CONFIG_PATH + "modposSH_03-17.pos";
    }
    else{
        initFile3 = CONFIG_PATH + "modpos.12.pos";
    }
    if(portnumC2 == 4){
        initFile2 = CONFIG_PATH +  "modpos.04.pos";
      }
    else if(portnumC2 == 9){
        initFile2 = CONFIG_PATH +  "modpos.09.pos";
    }
    else{
        initFile2 = CONFIG_PATH +  "modposSH_02-19.pos";
    }

    tracker2.setDisplayFeatures(true);
    tracker3.setDisplayFeatures(true);
    tracker2.setGoodMovingEdgesRatioThreshold(0.1);
    //tracker2.setProjectionErrorComputation(true);
    tracker3.setGoodMovingEdgesRatioThreshold(0.1);
    //tracker3.setProjectionErrorComputation(true);
    //ray casting for visibility tests
    //tracker2.setNbRayCastingAttemptsForVisibility(4);
    //tracker2.setGoodNbRayCastingAttemptsRatio(0.70);
    //tracker3.setNbRayCastingAttemptsForVisibility(4);
    //tracker3.setGoodNbRayCastingAttemptsRatio(0.70);
    //advanced visibility testing
    //tracker2.setScanLineVisibilityTest(true);
    //tracker3.setScanLineVisibilityTest(true);
    //loads the camera parameter files
    tracker2.loadConfigFile(configFilec2.c_str());
    tracker3.loadConfigFile(configFilec3.c_str());
    //camera parameter objects

    tracker2.getCameraParameters(cam2);
    tracker3.getCameraParameters(cam3);
    tracker2.loadModel(modelFileCao);
    tracker3.loadModel(modelFileCao);

    //initialise the tracker
    tracker2.initFromPose(img2,initFile2.c_str());
    tracker3.initFromPose(img3,initFile3.c_str());
    tracker2.getPose(c2I_cmo);
    tracker3.getPose(c3I_cmo);
    vpDisplay::flush(img2);
    vpDisplay::flush(img3);

}
/**
 * Sets sample positioning variables
 */
void applicationcontroller::doSamplePositioning(std::map<std::string, double> moveMap){
    std::cout<<"signal received \n";
    desired_pose.push_back(moveMap.find("z")->second);
    desired_pose.push_back(moveMap.find("y")->second);
    desired_pose.push_back(moveMap.find("x")->second);
    moves = moveMap;
    // get current drive positions
    std::cout<<"getting the current drive positions \n";
    getCurrentDrivePositions();
    // get the correct motor move values, relative movements
    calculateMovesFromCurrentPose(true);
    std::cout<<"moves have been calculated \n";
    positionSample = true;

}
/**
 * @brief applicationcontroller::doTrackSamplePositioning - copy of sample tracking function except that in this case
 * this method only exits when the application exits - or when stop tracking button pressed.
 * @param moves
 */
void applicationcontroller::doTrackSamplePositioning(){
    std::cout<<"In doSamplePositioning \n";
    track = true;
    isMoving = false;
    int savingcount = 0;
    //bool printstats = false;
    //output filenames for pose data and names with c for residuals
    std::string outpath2,outpath3,resoutpathc2,resoutpathc3,actuatorsout,cam2out,cam3out;
    //output file names for stats
    std::string outstats2,outstats3,errstats2,errstats3,ext,errWeights2,errWeights3;
    //output file names for images
    std::string pathoutimg2,pathoutimg3,imgExt;
    //for output filenames
    std::stringstream ss;
    bool end = false;
    imgExt=".png";
    ext = ".csv";
    std::string outputfilepath = basePath + experimentPath + "run_data/" + runName + "/";
    std::cout << "first files" << std::endl;
    //output stats file paths
    outstats2 = outputfilepath + "outstats2.dat";//all errors- whole error vector per frame
    outstats3 = outputfilepath + "outstats3.dat";
    //errstats2 = outputfilepath+ "errstats2_";//position data for errors and errors in pixel value
    //errstats3 = outputfilepath + "errstats3_";
    errWeights2 = outputfilepath + "errWeights2.csv";
    errWeights3 = outputfilepath + "errWeights3.csv";
    //output images with tracking results overlay
    std::string imageoutBase = outputfilepath + "image_out/";
    pathoutimg2 = imageoutBase + "cam2/track_out";
    pathoutimg3 = imageoutBase + "cam3/track_out";
    cam2out = imageoutBase + "cam2/data/img";
    cam3out = imageoutBase + "cam3/data/img";
    //pose data output file paths
    outpath2= outputfilepath +"outfile2.dat";
    outpath3= outputfilepath + "outfile3.dat";
    //residuals and normalised residuals output paths
    resoutpathc2= outputfilepath +"residuals2.csv";
    resoutpathc3= outputfilepath + "residuals3.csv";
    actuatorsout = outputfilepath + "actuatorsout.csv";

    //rgb image for output
    vpImage<vpRGBa> outimg2,outimg3;
    //open output streams for writing data to
    outstream2.open(outpath2.c_str(),std::ios_base::app);
    outstream3.open(outpath3.c_str(),std::ios_base::app);
    outstream4.open(resoutpathc2.c_str(),std::ios_base::app);
    outstream5.open(resoutpathc3.c_str(), std::ios_base::app);
    outstream6.open(actuatorsout.c_str(), std::ios_base::app);
    outstream7.open(outstats3.c_str(),std::ios_base::app);
    outstream8.open(errWeights2.c_str(), std::ios_base::app);
    outstream9.open(errWeights3.c_str(), std::ios_base::app);
    outstream1.open(outstats2.c_str(),std::ios_base::app);
    bool isMoving = false;
    int frame_num = 0;
    try{
        int i = 0;
        double moveVal;
        while (track){
            //std::cout<<"track = "<<track<<std::endl;

            try{
                //read the images from framegrabbers
                frameGrabber2.acquire(img2);
                frameGrabber3.acquire(img3);
            }
            catch(...){
                std::cout << "Cannot read one or both images "<< std::endl;
                //close all output streams
                outstream2.close();
                outstream3.close();
                outstream4.close();
                outstream5.close();
                outstream6.close();
                outstream7.close();
                outstream8.close();
                outstream9.close();
                outstream1.close();
                track = false;
                end=true;

            }
            //display images
            vpDisplay::display(img2);
            vpDisplay::display(img3);
            // Track the model
            if(i > 30){
                // gives the drives enough time to centre before starting to track
                tracker2.track(img2);
                tracker3.track(img3);
                //std::cout<<"i > 15 tracking now "<<std::endl;
            }

            //display the results of tracking, model and frame
            tracker2.display(img2,cMo2,cam2,vpColor::yellow,1);
            vpDisplay::displayFrame(img2,cMo2,cam2,0.05,vpColor::none,2);
            vpDisplay::flush(img2);
            tracker3.display(img3,cMo3,cam3,vpColor::yellow,1);
            vpDisplay::displayFrame(img3,cMo3,cam3,0.05,vpColor::none,2);
            vpDisplay::flush(img3);
            //save images - only want to do this while positioning is taking place and initially
            //(first 10 frames for finding correct sampleholder transformation)
            if(i < 30 || positionSample){
                vpDisplay::getImage(img2, outimg2);
                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<pathoutimg2<<frame_num<<imgExt;
                vpImageIo::write(outimg2,ss.str());
                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<cam2out<<frame_num<<imgExt;
                vpImageIo::write(img2,ss.str());

                vpDisplay::getImage(img3, outimg3);
                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<cam3out<<frame_num<<imgExt;
                vpImageIo::write(img3,ss.str());

                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<pathoutimg3<<frame_num<<imgExt;
                vpImageIo::write(outimg3,ss.str());
                frame_num++;
                //std::cout<<"saving images and i is:"<<i<<"\n";
            }
            //get the pose data
            tracker2.getPose(cMo2);
            tracker3.getPose(cMo3);
            //calculate output data from homogenous pose matrix
            //thetau?
            vpPoseVector pv2(cMo2);
            vpPoseVector pv3(cMo3);
            vpRotationMatrix rm2(pv2[3],pv2[4],pv2[5]);
            vpRotationMatrix rm3(pv3[3],pv3[4],pv3[5]);
            vpRzyxVector eulvec2(rm2);
            vpRzyxVector eulvec3(rm3);
            //emit the signal for frontend updating of tracking position
            cc2 = getCurrentStagePose(cMo2,c2I_cmo);
            cc3 = getCurrentStagePose(cMo3, c3I_cmo);
            emit posesChanged(cc2,cc3);
            //get drive positions and emit update signal?
            //std::cout<<" About to update the drive positions \n";
            tdcDrive.updateDrivePositions();
            bscDrives.updateDrivePositions();
            getCurrentDrivePositions(); // gets updated values and sets them in current drive position (cdp) vector
            emit driveStatusUpdated(cdp);

            //std::cout<<"poses vals emitted "<<"\n";
            if(positionSample){
                std::cout<<" in motor moves if block\n";
                isMoving = tdcDrive.isDriveMoving();
                std::cout<<"is moving "<<isMoving<<"\n";
                if(!isMoving){
                    //tdc not active - check other drives
                    isMoving = bscDrives.isDriveMoving();
                    std::cout<<"checking if dsc drives are moving "<<isMoving<<"\n";
                    if(!isMoving){
                        std::cout<<"not moving "<<isMoving<<"\n";
                        //drives not moving
                        if(!moves.empty()){
                            std::cout<<"moves not empty \n";
                            //z+rotation first
                            if(moves.find("z")!= moves.end()){

                                moveVal = moves.find("z")->second;

                                std::cout<<"the z move value is "<<moveVal<<"\n";
                                //need to only move in small increments when rotating, anything above 50 degrees will
                                //move 50 at a time, anything less than 50 will move at 10 degree increments
                                /**if(fabs(moveVal) > 50){
                                    // because the rotations will be using relative moves, negative values are possible
                                    if(moveVal < 0){
                                        std::cout<<"move is larger than a - 50 degrees \n";
                                        //negative direction ensure that the value is reduced correctly
                                        std::cout<<"new moveval is now "<<moveVal +50<<"\n";
                                        moves.find("z")->second =  moveVal + 50;
                                        tdcDrive.moveRelative(0x01,-50.00,0x50);
                                    }
                                    else{
                                        std::cout<<"move is greater than 50 degrees\n";
                                        //positive direction ensure val is reduced correctly
                                        std::cout<<"MAGEnTA new moveval is now "<<moveVal -50<<"\n";
                                        moves.find("z")->second = moveVal - 50;
                                        tdcDrive.moveRelative(0x01,50.00,0x50);
                                    }
                                }
                                else if(50.00 >fabs(moveVal) && fabs(moveVal) > 10.00){
                                    std::cout<<"move is between 50 and 10 \n";
                                    if(moveVal < 0){
                                        std::cout<<"negative direction\n";
                                        //negative direction ensure that the value is reduced correctly
                                        std::cout<<"White new moveval is now "<<moveVal +10<<"\n";
                                        moves.find("z")->second = moveVal + 10;
                                        tdcDrive.moveRelative(0x01,-10.00,0x50);
                                    }
                                    else{
                                        std::cout<<"positive direction \n";
                                        //positive direction ensure val is reduced correctly
                                        std::cout<<"PEARL new moveval is now "<<moveVal-10<<"\n";
                                        moves.find("z")->second = moveVal - 10;
                                        tdcDrive.moveRelative(0x01,10.00,0x50);
                                    }
                                }
                                else{*/
                                    // only have less than 10 degrees left to move, do whole move
                                    if(!moveVal == 0){
                                        std::cout<<"doing whole move\n";
                                        tdcDrive.moveRelative(0x01,moveVal,0x50);
                                        //tdcDrive.moveAbsolute(0x01,moveVal,0x50);
                                    }
                                    //empty the moves hash map of this move element
                                    moves.erase("z");
                                //}
                            }
                            else if(moves.find("y")!= moves.end()){
                                std::cout<<"moving y \n";
                                moveVal = moves.find("y")->second;
                                std::cout<<"the y move value is "<<moveVal<<"\n";
                                if(!moveVal == 0){
                                    //bscDrives.moveAbsolute(0x01,moveVal,0x22);
                                    bscDrives.moveRelative(0x01,moveVal,0x22);
                                }
                                moves.erase("y");
                            }
                            else if(moves.find("x")!= moves.end()){
                                std::cout<<"moving x\n";
                                moveVal = moves.find("x")->second;
                                std::cout<<"the x move value is "<<moveVal<<"\n";
                                if(!moveVal == 0){
                                    //bscDrives.moveAbsolute(0x01,moveVal,0x21);
                                    bscDrives.moveRelative(0x01,moveVal,0x21);
                                }
                                moves.erase("x");
                            }
                            else{
                                // some error has occurred because there are only supposed
                                // to be three keys
                                std::cout<<"****CASE1-ERROR\n";
                                positionSample = false;
                            }

                        }
                        else{
                            //moves have all been issued, motors are not moving - evaluate
                            std::cout<<"moves is empty \n";
                            bool reposition = evaluateReposition();
                            if(reposition){
                                calculateMovesFromCurrentPose(true);
                                std::cout<<"$%%&*£***!! \n";
                            }
                            else{

                                std::cout<<"saving count is:"<<savingcount<<" \n";
                                //positioning cycle can temporarily be halted
                                if(savingcount ==15){
                                    emit moveCompleted();
                                    positionSample = false;
                                    savingcount = 0;
                                    std::cout<<"position sample stopping now \n";
                                }
                                savingcount++;
                            }
                        }
                    }
                }

                //step 2 - start motor drives if not already moving


                /** File output**/
                // poses
                outstream2 << vpMath::deg( eulvec2[2])<< "\t"<< vpMath::deg( eulvec2[1]) << "\t"<< vpMath::deg(eulvec2[0])<< "\t"<<pv2[0]<< "\t"<<pv2[1]<< "\t"<<pv2[2]<<std::endl;
                outstream3 << vpMath::deg( eulvec3[2])<< "\t"<< vpMath::deg( eulvec3[1]) << "\t"<< vpMath::deg(eulvec3[0])<< "\t"<<pv3[0]<< "\t"<<pv3[1]<< "\t"<<pv3[2]<<std::endl;
                //errors / residuals
                outstream4<<sqrt( (tracker2.getError()).sumSquare()) <<","<< sqrt( (tracker2.getError()).sumSquare())/tracker2.getError().size() <<","<< tracker2.getError().size() << ","<< tracker2.getProjectionError()<< std::endl;
                outstream5<<sqrt( (tracker3.getError()).sumSquare()) <<","<< sqrt( (tracker3.getError()).sumSquare())/tracker3.getError().size() <<","<< tracker3.getError().size() << ","<< tracker3.getProjectionError()<< std::endl;
                //motors - actuators
                outstream6<<cdp.at(0)<<","<<cdp.at(1)<<","<<cdp.at(2)<<std::endl;
                //whole error vectors
                printErrVector(outstream1,tracker2.getError());
                printErrVector(outstream7,tracker3.getError());
                printErrVector(outstream8, tracker2.getRobustWeights());
                printErrVector(outstream9, tracker3.getRobustWeights());


            }
            i++;
      }
    }
    catch(...){
      outstream2.close();
      outstream3.close();
      outstream4.close();
      outstream5.close();
      outstream6.close();
      outstream7.close();
      outstream8.close();
      outstream9.close();
      outstream1.close();
      end=true;
    }
    if(!end){
       outstream2.close();
       outstream3.close();
       outstream4.close();
       outstream5.close();
       outstream6.close();
       outstream7.close();
       outstream8.close();
       outstream9.close();
       outstream1.close();
     }

}

/**
 * @brief applicationcontroller::getPosesAsStdVector places the six pose parameters into a std vector
 * @param pv the position - translations
 * @param eulvec - rotations as a matrix
 * @return
 */

std::vector<double> applicationcontroller::getPosesAsStdVector(vpPoseVector pv, vpRzyxVector eulvec){
    std::vector<double> vec;
    vec.push_back(pv[0]);
    vec.push_back(pv[1]);
    vec.push_back(pv[2]);
    vec.push_back(vpMath::deg( eulvec[2]));
    vec.push_back(vpMath::deg( eulvec[1]));
    vec.push_back(vpMath::deg( eulvec[0]));
    return vec;
}

/**
 * @brief applicationcontroller::stopMotors
 * calls stop on actuator drives
 */

void applicationcontroller::stopMotors(){
    //only one motor is active at a time find out which and stop it
    if(tdcDrive.getIsActive()){
        tdcDrive.stopMotor(0x01, 0x50); //channel and destimation
        std::cout<<"Stopped tdc drive \n";
    }
    else if(bscDrives.getIsActive()){
        if(bscDrives.getActiveDrive() == "x"){
            bscDrives.stopMotor(0x01, 0x21);
            std::cout<<"Stopped bsc x drive \n";
        }
        else if(bscDrives.getActiveDrive() == "y"){
            bscDrives.stopMotor(0x01, 0x22);
            std::cout<<"Stopped bsc y drive \n";
        }
        else{
            //if here then an error has occurred where none of the motors is supposedly active
            emit stopProblem(1);
        }
    }
    else{
        //if here then an error has occurred where none of the motors is supposedly active
        emit stopProblem(2);
    }
    //exit motor drive loop
    isMoving = false;
    positionSample = false;
    moves.clear();
    std::cout<<"Stopped moves is now "<< moves.size()<<" in size";
}

/**
 * @brief applicationcontroller::getAndDisplayImage - simple testing function that
 * mirrors the tracking function but does not have any connections to motor drives
 * @param portNum
 * @param name
 */

void applicationcontroller::getAndDisplayImage(int portNum, std::string name)
{
   //input file names for camera configuration, and pose initialisation
   std::string configFile,initFile,ext, basePath;
   //output filenames for pose data and names with r for residuals
   std::string outpath,outpathR,outstats,errWeights;
   //output file names for images
   std::string pathoutimg,imgExt;
   //input file names for image input, modelfiles
   std::string ipath,modelFileCao;
   //for output filenames
   std::stringstream ss;
   bool end = false;
   bool done = false;

   imgExt=".png";
   ext = ".csv";
   basePath = "/home/szb/Documents/eval/guiruns/";
   //basePath = "resources/";
   //std::string CONFIG_PATH = "/home/szb/Documents/development/ViSP-images/mbt/chamber_models/";
   std::string CONFIG_PATH = basePath + "config/";
   //output stats file paths
   outstats = basePath + name + "outstats.dat";//all errors- whole error vector per frame
   //errstats = basePath + "errstats_";//position data for errors and errors in pixel value
   //errWeights = basePath + name + "errWeights.csv";
   //output images with tracking results overlay

   //pathoutimg = basePath + "image_out/track_out";
   //pose data output file paths
   outpath= basePath + name + "_outfile.dat";
   //residuals and normalised residuals output paths
   //outpathR= basePath + name + "residuals.csv";
   //std::cout << "second files" << std::endl;
   // image input path names
   std::string portFolder = boost::lexical_cast<std::string>(portNum);
   ipath = basePath + "image_data/" + portFolder + "/img0000.png";
   std::cout << "image path is" << ipath<<std::endl;
   //camera parameters inout file paths
   if(portNum == 17 || portNum == 12 ){
      //configFile = CONFIG_PATH + "cam_settings/2016-03/camera3160.xml";//normal
      configFile = CONFIG_PATH + "cam_settings/2016-03/" + "camera3160.xml";//normal
      //model file input paths - complete model for these two ports
      //modelFileCao = "/home/szb/Documents/development/ViSP-images/mbt/chamber_models/modelNOWLNR1.cao";
      modelFileCao = CONFIG_PATH + "modelNOWLNR1.cao";
      //initial pose input file paths
      if(portNum == 17){
        initFile = CONFIG_PATH + "modpos.17.pos";
      }
      else{
    initFile = CONFIG_PATH + "modpos.12.pos";
      }
   }
   else{
      //configFile = CONFIG_PATH + "cam_settings/2016-03/camera2160.xml";//normal
      configFile = CONFIG_PATH + "camera2160.xml";//normal
      if(portNum == 4){
    initFile = CONFIG_PATH +  "modpos.04.pos";
    //model file input path
    //modelFileCao = "/home/szb/Documents/development/ViSP-images/mbt/chamber_models/modelNOWLNR1_p4.cao";
    modelFileCao =CONFIG_PATH + "modelNOWLNR1_p4.cao";
      }
      else if(portNum == 9){
    initFile = CONFIG_PATH +  "modpos.09.pos";
    //std::cout<<"JUST LOADED THE CORRECT POS FILE \t" <<initFile<<"\n";
    //model file input path
    //modelFileCao = "/home/szb/Documents/development/ViSP-images/mbt/chamber_models/modelNOWLNR1_p9.cao";
    modelFileCao = CONFIG_PATH +"modelNOWLNR1_p9.cao";
      }
      else{
    initFile = CONFIG_PATH +  "modpos.19.pos";
    //model file input path
    //modelFileCao = "/home/szb/Documents/development/ViSP-images/mbt/chamber_models/modelNOWLNR1_p19.cao";
    modelFileCao = CONFIG_PATH + "modelNOWLNR1_p19.cao";
      }
   }

   //grey level images
   vpImage<unsigned char> img;
   //rgb image for output
   //vpImage<vpRGBa> O;
   //std::cout << "streams" << std::endl;
   //output streams
   std::ofstream outstream2(outpath.c_str());
   //std::ofstream outstream3(outpathR.c_str());
   //std::ofstream outstream4(outstats.c_str());
   //std::ofstream outstream5(errWeights.c_str());
   //std::cout << "Starting cameras" << std::endl;
   try{
     //read the images from file using IO utility class instead of a reader
     vpImageIo::read(img, ipath); // Convert the color image in a gray level image
   }
   catch(...){
    std::cout << "Cannot read image "<< std::endl;

   }
  //std::cout << "acquired image and about to initialise displays" << std::endl;
  //Display initialisation
  // #if defined VISP_HAVE_X11
  //vpDisplayX d2,d3;
  # if defined(VISP_HAVE_OPENCV)
    vpDisplayOpenCV d(img, 0, 0, "Pose estimation Experiment");
  #endif
  //d2.init(img, 0, 0, "Pose estimation Experiment");
  vpDisplay::display(img);
  vpDisplay::flush(img);

  //Model-based tracker initialisation
  vpMbEdgeTracker tracker2;

  tracker2.setDisplayFeatures(true);
  //not yet implemented in the ViSP version currently in use
  tracker2.setGoodMovingEdgesRatioThreshold(0.1);
  //tracker2.setProjectionErrorComputation(true);
  vpHomogeneousMatrix cMo2;

  tracker2.loadConfigFile(configFile.c_str());
  std::cout << "loaded config file" << std::endl;
  vpCameraParameters cam;
  tracker2.getCameraParameters(cam);
  tracker2.loadModel(modelFileCao);
  std::cout << "loaded model file 1" << std::endl;
  std::cout<<" Read model file"<<std::endl;
  tracker2.initFromPose(img,initFile.c_str());
  vpDisplay::flush(img);
  //save the initial images
  //vpDisplay::getImage(img, O);
  //ss<<pathoutimg<<1002<<imgExt;
  //vpImageIo::write(O,ss.str());
  //clear the string stream of our outputpath for camera 2
  //ss.str( std::string() ); //clear it
  //ss.clear();
  //now start tracking
  tracker2.track(img);
  //Pose computation
  tracker2.getPose(cMo2);
  try{
    int i = 0;
    int framecounter = 0;
    while (!done)
    {
      try{
    //read the images from file using IO utility class instead of a reader
    vpImageIo::read(img, ipath); // Convert the color image in a gray level image
      }
      catch(...){
    std::cout << "Cannot read one or both images "<< std::endl;
    //close all output streams

      }
      //display images
      vpDisplay::display(img);
      // Track the model
      //tracker2.track(img);
      //display the reults of tracking, model and frame
      tracker2.display(img,cMo2,cam,vpColor::yellow,3);
      vpDisplay::displayFrame(img,cMo2,cam,0.05,vpColor::none,2);
      vpDisplay::flush(img);
      //save images
      //vpDisplay::getImage(img, O);
      //ss.str( std::string() ); //clear it
      //ss.clear();
      //ss<<pathoutimg<<i<<imgExt;
      //vpImageIo::write(O,ss.str());

      //get the pose data
      tracker2.getPose(cMo2);
      //if(framecounter == 10){
      char val[1];
      vpDisplay::getKeyboardEvent(img,val,true);
      //cMo2 = modifyPose(cMo2,done,val);
      tracker2.setPose(img,cMo2);
    //framecounter = 0;
      //}
      vpPoseVector pv2(cMo2);
      vpRotationMatrix rm2(pv2[3],pv2[4],pv2[5]);
      vpRzyxVector eulvec2(rm2);
      // poses
      outstream2 << vpMath::deg( eulvec2[2])<< "\t"<< vpMath::deg( eulvec2[1]) << "\t"<< vpMath::deg(eulvec2[0])<< "\t"<<pv2[0]<< "\t"<<pv2[1]<< "\t"<<pv2[2]<<std::endl;
      //errors
      //outstream3<<sqrt( (tracker2.getError()).sumSquare()) <<","<< sqrt( (tracker2.getError()).sumSquare())/tracker2.getError().size() <<","<< tracker2.getError().size() << ","<< tracker2.getProjectionError()<< std::endl;

      //output the whole error vector
      /**vpColVector err2 = tracker2.getError();
      printErrVector(outstream4,err2);
      vpColVector weights2 = tracker2.getRobustWeights();
      printErrVector(outstream5,weights2);*/
      framecounter++;
      i++;
    }
  }
  catch(...){
    outstream2.close();
    vpDisplay::close(img);
    //outstream3.close();
    //outstream4.close();
    //outstream5.close();
    end=true;
  }

  if(!end){
    outstream2.close();
    vpDisplay::close(img);
    //outstream3.close();
    //outstream4.close();
    //outstream5.close();
  }

}

/**
 * @brief applicationcontroller::printStats prints stats about tracking errors
 * @param filename
 * @param stats
 */


void applicationcontroller::printStats(std::string filename,std::vector<std::vector<double > > stats){
  //print out some stuff about the error
  std::ofstream outfile;
  outfile.open(filename.c_str(), std::ios_base::app);
  int stats_size = stats.size();
  for(int i = 0; i  < stats_size; i++) {
    std::vector<double > s = stats[i];
    int sVec_size = s.size();
    for(int j = 0; j< sVec_size; j++){
      outfile<<s[j];
      if(j != sVec_size - 1){
        outfile<<",";
      }
    }
    outfile<<"\n";
    }
   outfile.flush();
   outfile.close();
}

/**
 * @brief applicationcontroller::samplePositioningComplete toggles tracking
 */

void applicationcontroller::samplePositioningComplete(){
    // set the tracker to false so that tracking stops
    track = false;
}


/**
 * @brief applicationcontroller::shutdown stops all image acquisition and motor
 * connections
 */

void applicationcontroller::shutdown(){
    //here every object is closed.
    //close displays
    vpDisplay::close(img2);
    vpDisplay::close(img3);
    //or alternatively
    //d2.close(img2);
    //d3.close(img3);
    //close cameras
    frameGrabber2.close();
    frameGrabber3.close();
    // close usb connections
    tdcDrive.disconnect(0x50);
    bscDrives.disconnect(0x21);
    bscDrives.disconnect(0x22);
    //all other streams are already closed.

}

/**
 * @brief applicationcontroller::stopTracking toggles tracking - duplicate function
 */
void applicationcontroller::stopTracking(){
    track = false;
}

/**
 * @brief applicationcontroller::startTracking starts the tracking process
 */

void applicationcontroller::startTracking(){
    doTrackSamplePositioning();
}

/**
 * @brief applicationcontroller::calculateMovesFromCurrentPose
 * @brief given a desired position for the sample holder, the drive moves are
 * calculated based on the current position extrapolated from the two camera poses.
 * Will calculate all three move parameters at once
 */


void applicationcontroller::calculateMovesFromCurrentPose(bool relative){
    std::cout<<"BLACK in Calculate moves and drive positions are\n";
    //update the current motor position, incase this is used as reference
    getCurrentDrivePositions();
    std::cout<<"in calculate moves \n";
    bool xset = false;
    bool yset = false;
    // validate best camera pose against motor drive postions, only interested in the y rotation x and y translation
    // base decision on degrees because the range of these is far larger
    // select one camera as reference position for calculating next move, or then use motors current position
    // this is a very simplistic approach because it does not take into account whether the rotation is the same as
    // previously requested (move val has not been changed), also means that with camera pose changing all the time,
    // the motor could therefore constantly be moving too even though user has not requested a change - this will have to
    // be addressed
    // get the current position of stage as viewed from a camera.
    std::vector<double> cstage_pos2 = getCurrentStagePose(cMo2, c2I_cmo);
    std::vector<double> cstage_pos3 = getCurrentStagePose(cMo3, c3I_cmo);
    //std::cout<<"current desired pos is this size:"<<cdp.size()<<"\n";
    std::cout<<"BLACK current stage y/ zrotation is:"<<cstage_pos2.at(4)<<"\n";
    double diff = cstage_pos3.at(4) - cdp.at(0);
    std::cout<<"diff is"<<diff<<"\n";
    std::vector<double>reference;
    std::cout<<"diff is"<<diff<<"\n";
    //pose vectors as vprowvector are arranged x,y,z translation, followed by x,y,z rotation
    //desired_pose vector is arranged z,y,x where z = rotaion around camera's y axis, y = z translation in
    //camera's view, and x = x translation

    //reference camera is 3, if there is too much of a difference then 2 becomes reference cam, if
    //that fails then go for actuator
    /**if( fabs(diff)< 10){
        std::cout<<"difference between ref cam (3) and stage rotation is < 10 \n";
        reference.push_back(cstage_pos3[4]); //rotation
        reference.push_back(cstage_pos3[2]);//y translation
        reference.push_back(cstage_pos3[0]); // x translation
    }
    else{
        diff = cstage_pos2.at((4))- cdp.at(0);
        if( fabs(diff < 10)){
            std::cout<<"difference between ref cam (2) and stage rotation is < 10 \n";
            reference.push_back(cstage_pos2[4]); //z rotation = cam y rotation
            reference.push_back(cstage_pos2[2]);//y translation = cam z trans
            reference.push_back(cstage_pos2[0]); // x translation
        }
        else{
            std::cout<<"last resort \n";
            reference.push_back(cdp.at(0)); //rotation
            reference.push_back(cdp.at(1));//y translation
            reference.push_back(cdp.at(2)); // x translation
        }
    }*/
    /**
     * actuator driven block
     *
     */
    reference.push_back(cdp.at(0)); //rotation
    reference.push_back(cdp.at(1));//y translation
    reference.push_back(cdp.at(2)); // x translation

    // this will calculate moves in relative terms
    if(relative){
        if(lastx == moves.find("x")->second){
            moves.find("x")->second = 0;
            xset = true;
            lastx = 0;
        }
        if(lasty == moves.find("y")->second){
            moves.find("y")->second = 0;
            yset = true;
            lasty = 0;
        }
        if(!yset){
            // set the last desired move before it becomes a relative move
            lasty = moves.find("y")->second;
            std::cout<<"calculating relative moves \n";
            //lookup current y translation in moves map
            double val = std::trunc(reference.at(1));
            std::cout<<"the stages position - y truncated is"<<val<<"\n";
            //altered because our reference is in terms of the motor anyway
            double currYInMotorMap = reference.at(1);//movesmap.find(std::trunc(reference.at(1)))->second;
            //lookup desired move position in map
            double yMoveTo = movesmapY.find(std::trunc(moves.find("y")->second))->second;

            //get the decimal value of the current move to add to the mapped move
            if(reference.at(1)>0){
                currYInMotorMap += reference.at(1)- (long)reference.at(1);
            }
            else{
                currYInMotorMap -= reference.at(1)- (long)reference.at(1);
            }
            //get the decimal value of the desired move to add to the mapped move
            /*if(moves.find("y")->second >0){
                yMoveTo += moves.find("y")->second - (long)moves.find("y")->second;
            }
            else{
                yMoveTo += moves.find("y")->second + (long)moves.find("y")->second;
            }*/
            //now find the relative value of the move
            if(currYInMotorMap < yMoveTo){
                moves.find("y")->second= abs(currYInMotorMap) - abs(yMoveTo);
            }
            else{
                double temp = abs(yMoveTo) - abs(currYInMotorMap);
                moves.find("y")->second = temp * -1;
            }
        }
        if(!xset){
            //set last x move before it becomes a relative move
            lastx = moves.find("x")->second;
            /* X vals*/
            //lookup current x translation in moves map
            std::cout<<"the stages position - x truncated is"<<std::trunc(reference.at(2))<<"\n";
            //altered because our reference is in terms of the motor anyway
            double currXInMotorMap = reference.at(2);//movesmap.find(std::trunc(reference.at(2)))->second;
            std::cout<<"equivalent current x in moves map\n";
            std::cout<<currXInMotorMap<<"\n";

            //the chamber's x axis is a reflection of y axis, mapping is done according to y axis,
            //lookup desired move position in map
            double xMoveTo = movesmapX.find(std::trunc(moves.find("x")->second))->second;
            std::cout<<"desired x \n";
            std::cout<<xMoveTo<<"\n";
            if(reference.at(2)>0){
                currXInMotorMap += reference.at(2) - (long)reference.at(2);
                std::cout<<"currentin motor map is jow \n";
                std::cout<<currXInMotorMap<<"\n";
            }
            else{
                currXInMotorMap -= reference.at(2) - (long)reference.at(2);
                std::cout<<"currentin motor map is now \n";
                std::cout<<currXInMotorMap<<"\n";
            }
            //get the decimal value of the desired move to add to the mapped move
            /*if(moves.find("x")->second>0){
                xMoveTo += moves.find("x")->second - (long)moves.find("x")->second;
                std::cout<<"current desired x  is now\n";
                std::cout<<xMoveTo<<"\n";
            }
            else{
                xMoveTo += moves.find("x")->second + (long)moves.find("x")->second;
                std::cout<<"current desired x  is now2\n";
                std::cout<<xMoveTo<<"\n";
            }*/
            //now find the relative value of the move
            if(currXInMotorMap < xMoveTo){
                moves.find("x")->second= abs(currXInMotorMap) - abs(xMoveTo);
            }
            else{
                double temp = abs(xMoveTo) - abs(currXInMotorMap);
                moves.find("x")->second = temp * -1;
            }
        }
        /*std::cout<<"equivalent current y in moves map\t current x in moves map\n";
        std::cout<<currYInMotorMap<<"\t"<<currXInMotorMap<<"\n";
        std::cout<<"desired y \t desired x in \n";
        std::cout<<yMoveTo<<"\t"<<xMoveTo<<"\n";*/

        std::cout<<"in moves map\n";
        std::cout<<moves.find("y")->second<<"\t"<<moves.find("x")->second<<"\n";

    }
    else{
        std::cout<<"absolute moves\n";
        // absolute value just get the value in the lookup table / map
        //lookup desired move position in map
        double yMoveTo = movesmapY.find(std::trunc(moves.find("y")->second))->second;
        double xMoveTo = movesmapX.find(std::trunc(moves.find("x")->second))->second;
        //get the decimal value of the desired move to add to the mapped move because the value was previously
        //truncated.
        if(moves.find("y")->second > 0){
            yMoveTo += moves.find("y")->second - (long)moves.find("y")->second;
        }
        else{
            yMoveTo -= moves.find("y")->second + (long)moves.find("y")->second;
        }
        if(moves.find("x")->second > 0){
            xMoveTo += moves.find("x")->second - (long)moves.find("x")->second;
        }
        else{
            xMoveTo -= moves.find("x")->second + (long)moves.find("x")->second;
        }
    }
    // do z rotation value - this will always be a relative value
    std::cout<<"finding relative rotations\n";
    double curr_rotation = reference.at(0);
    std::cout<<"current rotation value is "<<curr_rotation<<"\n";
    double desired_rotation = moves.find("z")->second;
    std::cout<<"desired rotation value is "<<desired_rotation<<"\n";
    //there is a problem when desired rotation is 0 - correct the problem by making a rotate to home = 360
    //which is equivalent but makes the calculations correct
    if(desired_rotation == 0){
        desired_rotation = 360;
    }
    double rot_dir1 = abs(desired_rotation - curr_rotation);
    double rot_dir2,move;
    std::cout<<"rotation dir 1 is "<<rot_dir1<<"\n";
    // choose the lowest value because this is the shortest way around, unfortunately cannot just take the min of the two
    // values because need to know more info about which was chosen due to the fact that the direction depends on which one
    //is selected as lowest, and whether or not the desired location is larger than the current location
    //double move = min(rot_dir1,rot_dir2);
    //decide on which direction to take
    if(desired_rotation > curr_rotation){
        std::cout<<"desired is greater "<<"\n";
        rot_dir2 = abs(360 - desired_rotation)+ curr_rotation;
        std::cout<<"rot dir 2 value is "<<rot_dir2<<"\n";
        if(rot_dir1 < rot_dir2){
            // value must be positive - moving clockwise
            move = rot_dir1;
            std::cout<<"rotation direction 1\n";
        }
        else{
            move = rot_dir2 * -1; // negative direction, this will also be true for any value at 180
            std::cout<<"rotation direction 2 \n";
        }
    }
    else{
        std::cout<<"desrired rotation is less than current rotation\n";
        // the desired rotation value is smaller than the current location
        rot_dir2 = abs(360 - curr_rotation) + desired_rotation;
        if(rot_dir1 < rot_dir2){
            // value must be negative - moving anticlockwise
            move = rot_dir1 * -1;
            std::cout<<"rot dir 1 is "<<rot_dir1<<"\n";
        }
        else{
            move = rot_dir2; // negative direction
            std::cout<<"rot dir 2  is "<<rot_dir2<<"\n";
        }
    }
    moves.find("z")->second= move;
    std::cout<<"in moves map\n";
    std::cout<<moves.find("z")->second<<"\t"<<moves.find("y")->second<<"\t"<<moves.find("x")->second<<"\n";

}


/**
  * @brief applicationcontroller::fillmap prepares the move mapping data structure for y moves which in this case
  * is an unordered map
  */

 void applicationcontroller::fillmapY(){

    movesmapY[-9.00] = -17.00;
    movesmapY[-8.00] = -16.00;
    movesmapY[-7.00] = -15.00;
    movesmapY[-6.00] = -14.00;
    movesmapY[-5.00] = -13.00;
    movesmapY[-4.00] = -12.00;
    movesmapY[-3.00] = -11.00;
    movesmapY[-2.00] = -10.00;
    movesmapY[-1.00] = -9.00;
    movesmapY[0.00] = -8.00;
    movesmapY[1.00] = -7.00;
    movesmapY[2.00] = -6.00;
    movesmapY[3.00] = -5.00;
    movesmapY[4.00] = -4.00;
    movesmapY[5.00] = -3.00;
    movesmapY[6.00] = -2.00;
    movesmapY[7.00] = -1.00;
    movesmapY[8.00] = 0.00;
    movesmapY[9.00] = 0.00;
}
 /*
 * @brief applicationcontroller::fillmap prepares the move mapping data structure for x moves which in this case
 * is an unordered map
 */

void applicationcontroller::fillmapX(){

   movesmapX[9.00] = -17.00;
   movesmapX[8.00] = -16.00;
   movesmapX[7.00] = -15.00;
   movesmapX[6.00] = -14.00;
   movesmapX[5.00] = -13.00;
   movesmapX[4.00] = -12.00;
   movesmapX[3.00] = -11.00;
   movesmapX[2.00] = -10.00;
   movesmapX[1.00] = -9.00;
   movesmapX[0.00] = -8.00;
   movesmapX[-1.00] = -7.00;
   movesmapX[-2.00] = -6.00;
   movesmapX[-3.00] = -5.00;
   movesmapX[-4.00] = -4.00;
   movesmapX[-5.00] = -3.00;
   movesmapX[-6.00] = -2.00;
   movesmapX[-7.00] = -1.00;
   movesmapX[-8.00] = 0.00;
   movesmapX[-9.00] = 0.00;
}

 /**
 * @brief applicationcontroller::evaluateReposition
 * @brief evaluates whether repositioning is necessary based on the current
 * pose and the desired pose plus or minus a small padding value because position
 * is recalculated at each frame, positions are therefore not static, also, the motors
 * may overshoot or under rotate
 * @return bool true if reposition is necessary
 */

bool applicationcontroller::evaluateReposition(){
    //pose vectors as std::vectors are arranged x,y,z translation, followed by x,y,z rotation
    //desired_pose vector is arranged z,y,x where z = rotaion around camera's y axis, y = z translation in
    //camera's view, and x = x translation
    // padding is a value that is more or less (on either side of) near desired value
    double rotation_padding= 0.05;
    double translation_padding = 0.00005;
    //get current position of sample holder relative to current camera position in euler because
    //rememeber - rotations still in rads
    std::vector<double> co_vfc2_pose = getCurrentStagePose(cMo2,c2I_cmo);
    std::vector<double> co_vfc3_pose = getCurrentStagePose(cMo3,c3I_cmo);
    //check whether positioning is within an acceptable range - current object's position should be near the desired pose
    double cc2_resx,cc2_resy, cc2_resz,cc3_resx,cc3_resy,cc3_resz;
    cc2_resx = fabs(co_vfc2_pose[0] - desired_pose[2]);
    double deg = vpMath::deg(co_vfc2_pose[4]);
    cc2_resy = fabs(deg - desired_pose[0]);//rotation
    cc2_resz = fabs(co_vfc2_pose[2] - desired_pose[1]);
    cc3_resx = fabs(co_vfc3_pose[0] - desired_pose[2]);
    deg = vpMath::deg(co_vfc3_pose[4]);
    cc3_resy = fabs( deg- desired_pose[0]);//rotation
    cc3_resz = fabs(co_vfc3_pose[2] - desired_pose[1]);
    if(cc2_resy < rotation_padding){
        //check the other two axes
        if(cc2_resx < translation_padding && cc2_resz < translation_padding){
            return true;
        }
    }
    else if(cc3_resy < rotation_padding){
        if(cc3_resx < translation_padding && cc3_resz < translation_padding){
            return true;
        }
    }
    // return either true or false
    return false;
}

/**
 * @brief applicationcontroller::getCurrentStagePose
 * @brief calculates the current position of the stage given the homogenous matrix
 * of the current camera pose and the initial camera pose which describes the relative position
 * of the camera from origin, which is where the stage is when centred.
 * @param hmatC - current camera pose
 * @return
 */

std::vector<double> applicationcontroller::getCurrentStagePose(vpHomogeneousMatrix hmatC, vpHomogeneousMatrix hmatI){
    //get current position of sample holder relative to current camera position in euler because
    //current camera positions in thetau
    vpPoseVector cc_pv(hmatC);
    //initial camera positions
    vpPoseVector ic_pv(hmatI);
    //convert thetau to roation matrices
    vpRotationMatrix cc_rm(cc_pv[3],cc_pv[4],cc_pv[5]);
    vpRotationMatrix ic_rm(ic_pv[3],ic_pv[4],ic_pv[5]);
    //front end is provided in euler - get rotations in euler (rads)
    vpRzyxVector cc_eulvec(cc_rm);
    vpRzyxVector ic_eulvec(ic_rm);
    std::vector<double> cc_pose = getPosesAsStdVector(cc_pv,cc_eulvec);
    std::vector<double> ic_pose = getPosesAsStdVector(ic_pv,ic_eulvec);
    //subtract current from the initial position to get the current object's position without the cams relative position
    //rememeber - rotations still in rads
    //std::cout<<"calculating first part of pose and the result is "<<cc_pose.at(0) - ic_pose.at(0)<<"\n";
    //there is a difference in how the camera rotations are provided and how the front end input and motors expect
    // rotation moves to be given.  Cameras work in pi -180 to 180
    //convert the rotations to be in motor expected format
    convertBetweenCamDegsAndMotorDegs(cc_pose);
    //std::cout<<"Purple** - deg val is now \n";
    //std::cout<<cc_pose[4]<<"\n";
    convertBetweenCamDegsAndMotorDegs(ic_pose);
    std::vector<double> cStage_pose;
    cStage_pose.push_back(cc_pose.at(0) - ic_pose.at(0));
    cStage_pose.push_back(cc_pose.at(1) - ic_pose.at(1));
    cStage_pose.push_back(cc_pose.at(2) - ic_pose.at(2));
    cStage_pose.push_back(cc_pose.at(3) - ic_pose.at(3));
    cStage_pose.push_back(cc_pose.at(4) - ic_pose.at(4));
    cStage_pose.push_back(cc_pose.at(5) - ic_pose.at(5));
    //std::cout<<"the size of the pose vector is: \t"<<cStage_pose.size()<<"\n";
    return cStage_pose;
}

std::vector<double> applicationcontroller::getCurrentStagePoseAsStdVec(vpHomogeneousMatrix hmatC, vpHomogeneousMatrix hmatI){
/**    std::vector<double> poseAsStdVec;
    vpRowVector pose = getCurrentStagePose(hmatC,hmatI);
    for(int i = 0; i< pose.size(); i++){
        poseAsStdVec.push_back(pose[i]);
    }
    return poseAsStdVec;*/
}

void applicationcontroller::testVector(std::vector<double> v){
    for(int i = 0; i< v.size(); i++){
        std::cout<<v.at(i);
    }
}

void applicationcontroller::getCurrentDrivePositions(){
    std::vector<double> current_drive_positions;
    current_drive_positions.push_back(tdcDrive.getScaledZ());
    current_drive_positions.push_back(bscDrives.getScaledY());
    current_drive_positions.push_back(bscDrives.getscaledX());
    cdp = current_drive_positions;

}

void applicationcontroller::doStereoTracking(){
    track = true;
    isMoving = false;
    //bool printstats = false;
    //output filenames for pose data and names with c for residuals
    //camera poses,residuals, actuators, images without overlay
    std::string outpath2,outpath3,resoutpathc2,resoutpathc3,actuatorsout,cam2out,cam3out;

    //output file names for images
    std::string pathoutimg2,pathoutimg3,imgExt;
    //for output filenames
    std::stringstream ss;
    bool end = false;
    imgExt=".png";
    std::string outputfilepath = basePath + experimentPath + "run_data/" + runName + "/";

    //output images with tracking results overlay
    std::string imageoutBase = outputfilepath + "image_out/";
    pathoutimg2 = imageoutBase + "cam2/track_out";
    pathoutimg3 = imageoutBase + "cam3/track_out";
    cam2out = imageoutBase + "cam2/data/img";
    cam3out = imageoutBase + "cam3/data/img";
    //pose data output file paths
    outpath2= outputfilepath +"outfile2.dat";
    outpath3= outputfilepath + "outfile3.dat";
    //residuals and normalised residuals output paths
    resoutpathc2= outputfilepath +"residuals2.csv";
    resoutpathc3= outputfilepath + "residuals3.csv";
    actuatorsout = outputfilepath + "actuatorsout.csv";
    //output file names for stats
    std::string outstats2,outstats3,ext,errWeights2,errWeights3;
    ext =".csv";

    //rgb image for output
    vpImage<vpRGBa> outimg2,outimg3;
    //open output streams for writing data to
    outstream2.open(outpath2.c_str(),std::ios_base::app);
    outstream3.open(outpath3.c_str(),std::ios_base::app);
    outstream4.open(resoutpathc2.c_str(),std::ios_base::app);
    outstream5.open(resoutpathc3.c_str(), std::ios_base::app);
    outstream6.open(actuatorsout.c_str(), std::ios_base::app);
    outstream8.open(errWeights2.c_str(), std::ios_base::app);
    outstream1.open(outstats2.c_str(),std::ios_base::app);

    //bool isMoving = false;
    int frame_num = 0;
    int savingcount = 0;
    try{
        int i = 0;
        double moveVal;
        while (track){
            //std::cout<<"track = "<<track<<std::endl;

            try{
                //read the images from framegrabbers
                frameGrabber2.acquire(img2);
                frameGrabber3.acquire(img3);
                //std::cout<<"got images"<<std::endl;
            }
            catch(...){
                std::cout << "Cannot read one or both images "<< std::endl;
                //close all output streams
                outstream2.close();
                outstream3.close();
                outstream4.close();
                outstream5.close();
                outstream6.close();
                outstream1.close();
                outstream8.close();
                track = false;
                end=true;

            }
            //display images
            vpDisplay::display(img2);
            vpDisplay::display(img3);
            // Track the model
            if(i > 30){
                // gives the drives enough time to centre before starting to track
                tracker->track(img2,img3);
                //std::cout<<"i > 15 tracking now "<<std::endl;
            }
            //std::cout<<"displaying"<<std::endl;
            //display the results of tracking, model and frame
            tracker->display(img2, img3, cMo2, cMo3, cam2, cam3, vpColor::orange, 2);
            vpDisplay::displayFrame(img2,cMo2,cam2,0.05,vpColor::none,2);
            vpDisplay::displayFrame(img3,cMo3,cam3,0.05,vpColor::none,2);
            vpDisplay::flush(img2);
            vpDisplay::flush(img3);
            //save images - only want to do this while positioning is taking place and initially
            //(first 10 frames for finding correct sampleholder transformation)
            if(i < 30 || positionSample){
                //std::cout<<"about to save"<<std::endl;
                vpDisplay::getImage(img2, outimg2);
                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<pathoutimg2<<frame_num<<imgExt;
                vpImageIo::write(outimg2,ss.str());
                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<cam2out<<frame_num<<imgExt;
                vpImageIo::write(img2,ss.str());
                //std::cout<<"now cam 3"<<std::endl;
                vpDisplay::getImage(img3, outimg3);
                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<cam3out<<frame_num<<imgExt;
                vpImageIo::write(img3,ss.str());

                ss.str( std::string() ); //clear it
                ss.clear();
                ss<<pathoutimg3<<frame_num<<imgExt;
                vpImageIo::write(outimg3,ss.str());
                frame_num++;
                //std::cout<<"saving images and i is:"<<i<<"\n";
            }

            //get the pose data
            tracker->getPose(cMo2,cMo3);
            //calculate output data from homogenous pose matrix
            //thetau?
            vpPoseVector pv2(cMo2);
            vpPoseVector pv3(cMo3);
            vpRotationMatrix rm2(pv2[3],pv2[4],pv2[5]);
            vpRotationMatrix rm3(pv3[3],pv3[4],pv3[5]);
            vpRzyxVector eulvec2(rm2);
            vpRzyxVector eulvec3(rm3);
            //emit the signal for frontend updating of tracking position
            cc2 = getCurrentStagePose(cMo2,c2I_cmo);
            cc3 = getCurrentStagePose(cMo3, c3I_cmo);
            emit posesChanged(cc2,cc3);
            //std::cout<<"poses vals emitted "<<"\n";
            //step 2 - start motor drives if not already moving

            //get drive positions and emit update signal?
            //std::cout<<" About to update the drive positions \n";
            tdcDrive.updateDrivePositions();
            bscDrives.updateDrivePositions();
            getCurrentDrivePositions(); // gets updated values and sets them in current drive position (cdp) vector
            emit driveStatusUpdated(cdp);

            if(positionSample){
                std::cout<<" in motor moves if block\n";
                isMoving = tdcDrive.isDriveMoving();
                std::cout<<"is moving "<<isMoving<<"\n";
                if(!isMoving){
                    //tdc not active - check other drives
                    isMoving = bscDrives.isDriveMoving();
                    std::cout<<"checking if dsc drives are moving "<<isMoving<<"\n";
                    if(!isMoving){
                        std::cout<<"not moving "<<isMoving<<"\n";
                        //drives not moving
                        if(!moves.empty()){
                            std::cout<<"moves not empty \n";
                            //z+rotation first
                            if(moves.find("z")!= moves.end()){
                                std::cout<<"doing z move "<<isMoving<<"\n";
                                moveVal = moves.find("z")->second;
                                std::cout<<"the z move value is "<<moveVal<<"\n";
                                //need to only move in small increments when rotating, anything above 50 degrees will
                                //move 50 at a time, anything less than 50 will move at 10 degree increments
                                /**if(fabs(moveVal) > 50){
                                    // because the rotations will be using relative moves, negative values are possible
                                    if(moveVal < 0){
                                        std::cout<<"move is more than - 50 degrees \n";
                                        //negative direction ensure that the value is reduced correctly
                                        moves.find("z")->second =  moveVal + 50;
                                        tdcDrive.moveRelative(0x01,-50.00,0x50);
                                    }
                                    else{
                                        std::cout<<"move is greater than 50 degrees\n";
                                        //positive direction ensure val is reduced correctly
                                        moves.find("z")->second = moveVal - 50;
                                        tdcDrive.moveRelative(0x01,50.00,0x50);
                                    }
                                }
                                else if(50.00 >fabs(moveVal) && fabs(moveVal) > 10.00){
                                    std::cout<<"move is between 50 and 10 \n";
                                    if(moveVal < 0){
                                        std::cout<<"negative direction\n";
                                        //negative direction ensure that the value is reduced correctly
                                        moves.find("z")->second = moveVal + 10;
                                        tdcDrive.moveRelative(0x01,-10.00,0x50);
                                    }
                                    else{
                                        std::cout<<"positive direction \n";
                                        //positive direction ensure val is reduced correctly
                                        moves.find("z")->second = moveVal - 10;
                                        tdcDrive.moveRelative(0x01,10.00,0x50);
                                    }
                                }
                                else{*/
                                    // only have less than 10 degrees left to move, do whole move
                                    std::cout<<"doing whole move\n";
                                    if(!moveVal == 0){
                                        tdcDrive.moveRelative(0x01,moveVal,0x50);
                                    }
                                    //empty the moves hash map of this move element
                                    moves.erase("z");
                                //}
                            }
                            else if(moves.find("y")!= moves.end()){
                                std::cout<<"moving y \n";
                                moveVal = moves.find("y")->second;
                                std::cout<<"the y move value is "<<moveVal<<"\n";
                                if(!moveVal == 0){
                                    //bscDrives.moveAbsolute(0x01,moveVal,0x22);
                                    bscDrives.moveRelative(0x01,moveVal,0x22);
                                }
                                moves.erase("y");
                            }
                            else if(moves.find("x")!= moves.end()){
                                std::cout<<"moving x\n";
                                moveVal = moves.find("x")->second;
                                std::cout<<"the x move value is "<<moveVal<<"\n";
                                if(!moveVal == 0){
                                    //bscDrives.moveAbsolute(0x01,moveVal,0x21);
                                    bscDrives.moveRelative(0x01,moveVal,0x21);
                                }
                                moves.erase("x");
                            }
                            else{
                                // some error has occurred because there are only supposed
                                // to be three keys
                                std::cout<<"****CASE1-ERROR\n";
                                positionSample = false;
                            }

                        }
                        else{
                            //moves have all been issued, motors are not moving - evaluate
                            std::cout<<"moves is empty \n";
                            bool reposition = false;//evaluateReposition();
                            if(reposition){
                                calculateMovesFromCurrentPose(true);
                                std::cout<<"$%%&*£***!! \n";
                            }
                            else{

                                std::cout<<"saving count is:"<<savingcount<<" \n";
                                //positioning cycle can temporarily be halted
                                if(savingcount == 15){
                                    emit moveCompleted();
                                    positionSample = false;
                                    savingcount = 0;
                                    std::cout<<"position sample stopping now \n";
                                }
                                savingcount++;
                            }
                        }
                    }
                }


                /** File output**/
                // poses
                outstream2 << vpMath::deg( eulvec2[2])<< "\t"<< vpMath::deg( eulvec2[1]) << "\t"<< vpMath::deg(eulvec2[0])<< "\t"<<pv2[0]<< "\t"<<pv2[1]<< "\t"<<pv2[2]<<std::endl;
                outstream3 << vpMath::deg( eulvec3[2])<< "\t"<< vpMath::deg( eulvec3[1]) << "\t"<< vpMath::deg(eulvec3[0])<< "\t"<<pv3[0]<< "\t"<<pv3[1]<< "\t"<<pv3[2]<<std::endl;
                //errors / residuals
                outstream4<<sqrt( (tracker->getError()).sumSquare()) <<","<< sqrt( (tracker->getError()).sumSquare())/tracker->getError().size() <<","<< tracker->getError().size() << ","<< tracker->getProjectionError()<< std::endl;
                //motors - actuators
                outstream6<<cdp.at(0)<<","<<cdp.at(1)<<","<<cdp.at(2)<<std::endl;
                //whole error vectors
                printErrVector(outstream1,tracker->getError());
                printErrVector(outstream8, tracker->getRobustWeights());


            }
            i++;
      }
    }
    catch(...){
      outstream2.close();
      outstream3.close();
      outstream4.close();
      outstream5.close();
      outstream6.close();
      outstream8.close();
      outstream1.close();
      end=true;
    }
    if(!end){
       outstream2.close();
       outstream3.close();
       outstream4.close();
       outstream5.close();
       outstream6.close();
       outstream8.close();
       outstream1.close();
     }
}

void applicationcontroller::initStereoTracker(){
    //input file names for camera configuration, and pose initialisation
    std::string configFilec2,initFilec2, configFilec3,initFilec3,modelFileCao,trackingHome;
    //std::cout << "second files" << std::endl;
    trackingHome = "sample_tracking/";
    std::string inHomogenousTrans = basePath + trackingHome +"config/cam2_HM_cam3-i12.txt";

    std::string inHomogenousC2, inHomogenousC3;
    inHomogenousC2 = basePath + trackingHome + "config/cam2_i7HM.txt";
    inHomogenousC3 = basePath + trackingHome +"config/cam3-i12HM.txt";
    //camera parameters inout file paths
    configFilec3 = basePath + trackingHome + "config/cam_settings/2016-03/camera3160.xml";//normal
    configFilec2 = basePath + trackingHome+"config/cam_settings/2016-03/camera2160.xml";//normal
    //std::cout << configFile << std::endl;
    //model file input paths - first is for single model second and third are for individualised models
    //modelFileCao = basePath + trackingHome + "config/sampleholder-october16.cao"; //latest version with slightly corrected widths
    modelFileCao = basePath + trackingHome + "config/sampleholder-march17.cao"; //latest version with tilts added
    //std::cout << modelFileCao << std::endl;
    //initFilec3 = basePath + trackingHome + "config/modposSH_03-17-i5.pos";
    //initFilec2 = basePath + trackingHome +"config/modposSH_02-19-i5.pos";
    std::ifstream file_c3Mc2;
    file_c3Mc2.open(inHomogenousTrans.c_str());
    c3Mc2.load(file_c3Mc2);

    std::ifstream cameraPoseInputstream;
    cameraPoseInputstream.open(inHomogenousC2.c_str());
    cMo2.load(cameraPoseInputstream);
    cameraPoseInputstream.close();

    std::ifstream cameraPoseInputstream3(inHomogenousC3.c_str());
    cMo3.load(cameraPoseInputstream3);
    cameraPoseInputstream3.close();

    tracker = new vpMbEdgeMultiTracker(2);
    //errors and moving edge config
    tracker->setGoodMovingEdgesRatioThreshold(0.1);
    //tracker.setProjectionErrorComputation(true);

    //ray casting for visibility tests
    //tracker.setNbRayCastingAttemptsForVisibility(4);
    //tracker.setGoodNbRayCastingAttemptsRatio(0.70);
    //advanced visibility testing
    //tracker.setScanLineVisibilityTest(true);

    // load the config file and get the cam params from it
    tracker->loadConfigFile(configFilec2,configFilec3);
    tracker->getCameraParameters(cam2,cam3);

    //load model and config display
    tracker->loadModel(modelFileCao);
    tracker->setDisplayFeatures(true);
    std::cout<<"!!22222222222222222222222222\n";
    //set the transformation matrix
    std::map<std::string, vpHomogeneousMatrix> mapOfCameraTransformationMatrix;
    mapOfCameraTransformationMatrix["Camera1"] = vpHomogeneousMatrix();
    mapOfCameraTransformationMatrix["Camera2"] = c3Mc2;
    tracker->setCameraTransformationMatrix(mapOfCameraTransformationMatrix);
    //init from the homogenous matrix poses
    tracker->initFromPose(img2,img3,cMo2, cMo3, true);
    //get init matrices
    tracker->getPose(c2I_cmo, c3I_cmo);
    //display
    vpDisplay::flush(img2);
    vpDisplay::flush(img3);
    std::cout<<"!!3333333333333333333333333333\n";

}


/**
 * there is a difference in how the camera rotations are provided and how the front end input and motors expect
 * rotation moves to be given.  Cameras work in pi -180 to 180 range while motors work in 0 - 360
 * this function converts the rotations to be in format expected by motor
 */

void applicationcontroller::convertBetweenCamDegsAndMotorDegs(std::vector<double> &posevector){
    // based on right handed rule, clockwise rotations are only need to do camera y == motor z
    double degVal = posevector[4];
    /**if(degVal < 0){
        //std::cout<<"GREEN - deg val is negative "<<degVal<<"\n";
        degVal *= -1;
        //reset to 360 range
        posevector[4] = degVal;
        //std::cout<<"GREEN - deg val is now "<<posevector[4]<<"\n";

    }
    else{ // positive anyway but due to right hand rule this falls on the side 180 to 360 side so
        // map it to correct equivalent rotation in 360 range
        //reset to 360 range
        posevector[4] = 360 - degVal;
    }*/
}
