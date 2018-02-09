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

#ifndef APPLICATIONCONTROLLER_H
#define APPLICATIONCONTROLLER_H
#include <QObject>
#include <visp/vpConfig.h>
#include <visp/vpImage.h>
#include <visp/vpImageConvert.h>
#include <visp/vpMbEdgeTracker.h>
#include <visp3/mbt/vpMbEdgeMultiTracker.h>
#include <visp/vpImageSimulator.h>
#include <visp/vpOpenCVGrabber.h>
#include <visp/vpDisplayOpenCV.h>
#include <visp/vpVideoReader.h>
#include <visp/vpPose.h>
#include <visp/vpMbtDistanceLine.h>
#include "vpUeyeFrameGrabber.h"
#include <boost/lexical_cast.hpp>
#include <visp/vpImageIo.h>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <fstream>
#include <thordrive.h>
#include <map>
#include <unordered_map>
#include "vcuserinputwindow.h"

namespace fs = boost::filesystem;

class applicationcontroller : public QObject
{
    Q_OBJECT
public:
    explicit applicationcontroller(bool stereo,QObject *parent = 0);
    ~applicationcontroller();
    void getAndDisplayImage(int portNum, std::string name);
    void doTrackSamplePositioning();    
    void doStereoTracking();
signals:
    void posesChanged(std::vector<double> c2, std::vector<double> c3);
    void driveStatusUpdated(std::vector<double> drivePositions);
    void moveCompleted();
    void stopProblem(int);
public slots:
    void stopMotors();
    void doSamplePositioning(std::map<std::string, double> moves);
    void samplePositioningComplete();
    void shutdown();
    void stopTracking();
    void startTracking();
private:
    void initAllEquipment(bool stereo);
    void initTrackers();
    void moveMotors();
    void printErrVector(std::ofstream& errwriter, vpColVector errvec);
    void calculateMovesFromCurrentPose(bool relative);
    std::string getCurrentDT();
    void initCameras();
    int makeFolder(char* foldername);
    void printStats(std::string filename,std::vector<std::vector<double > > stats);
    std::vector<double> getPosesAsStdVector(vpPoseVector pv, vpRzyxVector eulvec);
    void fillmapX();
    void fillmapY();
    void initStereoTracker();
    bool evaluateReposition();
    std::vector<double> getCurrentStagePose(vpHomogeneousMatrix hmatC,vpHomogeneousMatrix hmatI);
    std::vector<double>getCurrentStagePoseAsStdVec(vpHomogeneousMatrix hmatC, vpHomogeneousMatrix hmatI);

    vpUeyeFrameGrabber frameGrabber3,frameGrabber2;
    vpCameraParameters cam2,cam3;
    thordrive tdcDrive;
    thordrive bscDrives;
    std::vector<double> desired_pose;
    vpHomogeneousMatrix c2I_cmo,c3I_cmo;//the initial poses of the cameras

    bool initialisedAndReady,positionSample,track,isMoving;
    std::string basePath,experimentPath,runName;
    int portnumC2,portnumC3,activeDrive;
    vpImage<unsigned char> img2,img3;
    std::ofstream outstream1,outstream2,outstream3,outstream4, outstream5,outstream6;
    std::ofstream outstream7, outstream8,outstream9;
    vpMbEdgeTracker tracker2,tracker3;
    vpMbEdgeMultiTracker *tracker;
    vpHomogeneousMatrix cMo2,cMo3,c3Mc2;
    vpDisplayOpenCV d2,d3;
    std::map<std::string, double> moves;
    std::unordered_map<double,double> movesmapX,movesmapY;
    double lastx,lasty;
    std::vector<double> cc2,cc3,cdp;// current camera 2 (cc2) pose, current camera 3 pose, current drive positions(cdp)
    void testVector(std::vector<double> v);
    void getCurrentDrivePositions();
    void convertBetweenCamDegsAndMotorDegs(std::vector<double> &posevector);
};

#endif // APPLICATIONCONTROLLER_H
