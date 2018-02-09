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
#include "ui_vcuserinputwindow.h"

VCUserInputWindow::VCUserInputWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VCUserInputWindow)
{
    ui->setupUi(this);

    //ui->zrot_in->setValue(0.00);
    ui->xtrans_in->setValue(0.00);
    ui->ytrans_in->setValue(0.00);
    ui->XTValLabel_d1->setText(getValAsQString(-7.80));
    ui->YTValLabel_d2->setText(getValAsQString(-7.80));

    connect(ui->stop_button,SIGNAL(clicked()), this,SLOT(stopInitiated()));
    connect(ui->stop_button,SIGNAL(clicked()), this,SLOT(enablePosControls()));
    connect(this,SIGNAL(moveNoValueSet()), this,SLOT(enablePosControls()));
    connect(ui->goto_button,SIGNAL(clicked()), this,SLOT(disablePosControls()));
    connect(ui->goto_button,SIGNAL(clicked()), this,SLOT(preparePositioningParams()));
}

VCUserInputWindow::~VCUserInputWindow()
{
    delete ui;
}

void VCUserInputWindow::updateDrivePositions(std::vector<double> drivePositions){
    if(!drivePositions.empty()){
        //element 0 = z drive in degrees
        ui->ZRValLabel_d3->setText(getValAsQString(drivePositions.at(0)));
        //element 1 = y drive in meters
        ui->YTValLabel_d2->setText(getValAsQString(drivePositions.at(1)));
        //element 2 = x drive in meters
        ui->XTValLabel_d1->setText(getValAsQString(drivePositions.at(2)));
    }

}
void VCUserInputWindow::updateSamplePosition(std::vector<double> c2, std::vector<double> c3){

    if(!c2.empty()){
        // x trans
        ui->XTValLabel_c2->setText(getValAsQString(c2.at(0)));
        // y trans
        ui->YTValLabel_c2->setText(getValAsQString(c2.at(1)));
        // z trans
        ui->ZTValLabel_c2->setText(getValAsQString(c2.at(2)));

        // x rotation
        ui->XRValLabel_c2->setText(getValAsQString(c2.at(3)));
        // y rotation
        ui->YRValLabel_c2->setText(getValAsQString(c2.at(4)));
        // z rotation
        ui->ZRValLabel_c2->setText(getValAsQString(c2.at(5)));

    }
    if(!c3.empty()){
        // x trans
        ui->XTValLabel_c3->setText(getValAsQString(c3.at(0)));
        // y trans
        ui->YTValLabel_c3->setText(getValAsQString(c3.at(1)));
        // z trans
        ui->ZTValLabel_c3->setText(getValAsQString(c3.at(2)));

        // x rotation
        ui->XRValLabel_c3->setText(getValAsQString(c3.at(3)));
        // y rotation
        ui->YRValLabel_c3->setText(getValAsQString(c3.at(4)));
        // z rotation
        ui->ZRValLabel_c3->setText(getValAsQString(c3.at(5)));
    }
}

void VCUserInputWindow::disablePosControls(){
    // disable input controls
    ui->zrot_in->setEnabled(false);
    ui->xtrans_in->setEnabled(false);
    ui->ytrans_in->setEnabled(false);
    //disable goto button
    ui->goto_button->setEnabled(false);
    //enable stop buttom
    ui->stop_button->setEnabled(true);
}

void VCUserInputWindow::enablePosControls(){
    //reset the control values to 0 and then enable them
    //ui->zrot_in->setValue(0.00);
    //ui->xtrans_in->setValue(0.00);
    //ui->ytrans_in->setValue(0.00);
    ui->zrot_in->setEnabled(true);
    ui->xtrans_in->setEnabled(true);
    ui->ytrans_in->setEnabled(true);
    //enable goto button
    ui->goto_button->setEnabled(true);
    //disable stop button
    ui->stop_button->setEnabled(false);
}

std::string VCUserInputWindow::getValAsString(double val){
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4);
    ss<<val;
    std::string retString = ss.str();
    ss.str( std::string() ); //clear it
    ss.clear();// clears any error bits
    return retString;
}

QString VCUserInputWindow::getValAsQString(double val){
    QString valueAsString = QString::number(val);
    return valueAsString;
}

void VCUserInputWindow::showStopWarning(int origin){
    QMessageBox mesBox;
    if(origin == 1){
        mesBox.warning(0,"Error","A stop error has occured! One of the bscDrives was thought to be active but was not.");
    }
    else{
        mesBox.warning(0,"Error","A stop error has occured - no motors were active !");
    }
    //messageBox.setFixedSize(500,200);
    mesBox.exec();
}

void VCUserInputWindow::preparePositioningParams(){
    std::cout<<"moves not empty\n";
    // get the values from the spinboxes, create map and emit the signal
    std::map<std::string, double> moves;
    //if(ui->zrot_in->value() != 0){
    moves["z"] = ui->zrot_in->value();
    //}
    //if(ui->ytrans_in->value() != 0){
    moves["y"] = ui->ytrans_in->value();
    //}
    //if(ui->xtrans_in->value()){
    moves["x"] = ui->xtrans_in->value();
    //}
    std::cout<<"Preparing moves \n";
    if(!moves.empty()){
        std::cout<<"moves not empty\n";
        emit positionSample(moves);
        std::cout<<"Sposition sample emitted !!!";
    }
    else{
        emit moveNoValueSet();
        std::cout<<"Smove no value set ||Z|";
    }
}

void VCUserInputWindow::stopInitiated(){
    std::cout<<"STOP PRESSED ££££\n";
    emit doStopMotors();
}
