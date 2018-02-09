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

#ifndef VPUEYEFRAMEGRABBER_H
#define VPUEYEFRAMEGRABBER_H

#endif
#include <visp/vpFrameGrabber.h>
#include <imalib/ueyeImageGrabber.h>
#include <imalib/imageRGB.h>

class vpUeyeFrameGrabber : public vpFrameGrabber
{
public:
  /*!
  * \brief Constructor.
  */
  vpUeyeFrameGrabber();
  /*!
  * \brief Destructor.
  */
  virtual ~vpUeyeFrameGrabber();
  /*!
  * \brief Close the camera.
  */
  virtual void close();
  /*!
  * \brief Acquire a colour image
  * \param[in] I Pointer to image.
  */
  virtual void acquire(vpImage< vpRGBa >& I);
  /*!
  * \brief Acquire a monochrome image
  * \param[in] I Pointer to image.
  */
  virtual void acquire(vpImage< unsigned char >& I);
  /*!
  * \brief Initialise the camera - colour image
  * \param[in] I Pointer to image.
  */
  virtual void open(vpImage< vpRGBa >& I);
  /*!
  * \brief Initialise the camera - monochrome image
  * \param[in] I Pointer to image.
  */
  virtual void open(vpImage< unsigned char >& I);
  /*!
  * \brief Set the camera number to the internal camera id used to connect to a specific camera.
  * \param[in] id Internal id of the camera.
  */
   void setCameraNumber(int id);
    /*!
  * \brief Get the current Exposure value.
  * \param[out] double the exposure.
  */
  double getExposure();
    /*!
  * \brief Print current camera parameters.
  */
  void printCameraParameters();
  imalib::imageRGB* currImageA;
  imalib::imageRGB theImagec1;
  imalib::ueyeImageGrabber* uGrabberc1;
  void initialiseCamera(int pixClockVal);
  bool isConnected;
private:
  unsigned short imageWidth;
  unsigned short imageHeight;
  //unsigned short imageBWidth;
  //unsigned short imageBHeight;
  unsigned short posX, posY;
  bool upsideDown;
  bool flip;
  int cameraNumber;


};

// VPUEYEFRAMEGRABBER_H
