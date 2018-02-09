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

#include "vcuserinputwindow.h"
#include <QApplication>

#include "applicationcontroller.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    bool stereo = true;
    VCUserInputWindow vcinput;
    vcinput.show();
    applicationcontroller ac(stereo);
    QObject::connect(&ac,SIGNAL(posesChanged(std::vector<double>,std::vector<double>)),&vcinput,SLOT(updateSamplePosition(std::vector<double>,std::vector<double>)));
    QObject::connect(&ac,SIGNAL(driveStatusUpdated(std::vector<double>)), &vcinput,SLOT(updateDrivePositions(std::vector<double>)));
    QObject::connect(&ac,SIGNAL(moveCompleted()), &vcinput,SLOT(enablePosControls()));
    QObject::connect(&ac,SIGNAL(stopProblem(int)), &vcinput,SLOT(showStopWarning(int)));
    QObject::connect(&vcinput,SIGNAL(positionSample(std::map<std::string,double>)), &ac,SLOT(doSamplePositioning(std::map<std::string,double>)));
    QObject::connect(&vcinput,SIGNAL(doStopMotors()), &ac,SLOT(stopMotors()));
    if(stereo){
        std::cout<<" stereo camera tracking\n";
        ac.doStereoTracking();
    }
    else{
        std::cout<<" dual camera tracking\n";
        ac.doTrackSamplePositioning();
    }
    return app.exec();
}
