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

#ifndef VCUSERINPUTWINDOW_H
#define VCUSERINPUTWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <sstream>
#include <iomanip>
#include "applicationcontroller.h"

namespace Ui {
class VCUserInputWindow;
}

class VCUserInputWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VCUserInputWindow(QWidget *parent = 0);
    ~VCUserInputWindow();

signals:
    void positionSample(std::map<std::string, double> moves);
    void doStopMotors();
    void moveNoValueSet();
private slots:
    void updateDrivePositions(std::vector<double>);
    void updateSamplePosition(std::vector<double>,std::vector<double>);
    void disablePosControls();
    void enablePosControls();
    void showStopWarning(int);
    void preparePositioningParams();
    void stopInitiated();
private:
    Ui::VCUserInputWindow *ui;
    std::string getValAsString(double val);
    QString getValAsQString(double val);
};

#endif // VCUSERINPUTWINDOW_H
