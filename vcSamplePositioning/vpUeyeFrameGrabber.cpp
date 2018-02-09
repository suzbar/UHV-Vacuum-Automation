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
#include "vpUeyeFrameGrabber.h"
#include <visp/vpImageConvert.h>
//#include <imalib/ueyeImageGrabber.h>
//#include <imalib/imageRGB.h>
vpUeyeFrameGrabber::vpUeyeFrameGrabber(): theImagec1(1600,1200){
  imageWidth = 1600;
  imageHeight = 1200;
  posX = 0;
  posY = 0;
  upsideDown = false;
  flip = false;
  cameraNumber = 0; //default and will result in connection to first available camera
  //theImagec1.setSize(imageHeight,imageWidth);
  //imalib::imageRGB theImagec2(1600, 1200);
}
vpUeyeFrameGrabber::~vpUeyeFrameGrabber()
{
  delete this;
}

void vpUeyeFrameGrabber::setCameraNumber(int id){

  cameraNumber = id;
}
void vpUeyeFrameGrabber::close()
{
  delete uGrabberc1;
  isConnected = false;
}

void vpUeyeFrameGrabber::acquire(vpImage< vpRGBa >& I)
{

    uGrabberc1->getImage(&theImagec1);
    //now convert the imalib image to the visp format
    for (int x = 0; x < imageHeight; x++){
      for(int y = 0; y < imageWidth; y++){
    unsigned char rgbcomponents[3];
    theImagec1.getPixel(x, y,rgbcomponents);
    I[x][y].R = rgbcomponents[0];
    I[x][y].G = rgbcomponents[1];
    I[x][y].B = rgbcomponents[2];
      }
    }
}

void vpUeyeFrameGrabber::acquire(vpImage< unsigned char >& I)
{
  vpImage< vpRGBa > temp(1200,1600); //height, width//I had this as 1600,1200
  uGrabberc1->getImage(&theImagec1);
    //now convert the imalib image to the visp format
    for (int x = 0; x < imageHeight; x++){
      for(int y = 0; y < imageWidth; y++){
    unsigned char rgbcomponents[3];
    theImagec1.getPixel(y, x,rgbcomponents);
    temp[x][y].R = rgbcomponents[0];
    temp[x][y].G = rgbcomponents[1];
    temp[x][y].B = rgbcomponents[2];
      }
    }
    //now we have an rgb visp image lets convert to a unsigned char image
    vpImage< vpRGBa > const &t = temp;
    vpImageConvert::convert(t,I);

}

void vpUeyeFrameGrabber::open(vpImage< vpRGBa >& I)
{
    uGrabberc1 =  new imalib::ueyeImageGrabber(upsideDown,flip,cameraNumber,posX, posY,imageWidth, imageHeight);
    if(uGrabberc1->isConnected()){
      isConnected = true;
      //initialiseCamera();
    }
}

void vpUeyeFrameGrabber::open(vpImage< unsigned char >& I)
{
    uGrabberc1 =  new imalib::ueyeImageGrabber(upsideDown,flip,cameraNumber,posX, posY,imageWidth, imageHeight);
    if(uGrabberc1->isConnected()){
      isConnected = true;
      //initialiseCamera();
    }

}

void vpUeyeFrameGrabber::initialiseCamera(int pixClockVal){

   int pixclock = (int)uGrabberc1->getCurrentPixelClockValue();
  /** std::cout<<"Pixelclock \t"<<pixclock<<std::endl;
   std::cout<<"EXPOSURE= "<<uGrabberc1->getExposure()<<std::endl;
   std::cout<<"Framerate \t"<<uGrabberc1->getFramerate()<<std::endl;
   std::cout<<"Gain \t"<<uGrabberc1->getGain()<<std::endl;**/
   // set pixelclock
   uGrabberc1->setPixelClockVal(pixClockVal);
   //std::cout<<"Pixelclock is now \t"<<uGrabberc1->getCurrentPixelClockValue()<<std::endl;
   // check autoExposure
   long autoxposure = uGrabberc1->getAutoExposureSetting();
   std::cout<<"AutoExposuresetting =  \t"<<autoxposure<<std::endl;
   if(autoxposure == 1){
     // auto exposure setting is enabled and must be disabled before the pixel clock setting will work.
     uGrabberc1->setAutoExposureControl(0);
     std::cout<<"AutoExposuresetting is now disabled **********  \t"<<std::endl;
   }
   uGrabberc1->setPixelClockVal(pixClockVal);
   //set exposure to the default for the pixelclock setting - 0 for auto, > 0 for manual exposure
   //uGrabberc1->setExposure(500);
   //take a few images // default is 4 before settings take effect.
   for(int i = 0; i < 20; i++){
      uGrabberc1->getImage(&theImagec1);
      std::cout<<i<<std::endl;
   }
   std::cout<<"*********************Settings now at **********  \t"<<std::endl;
   std::cout<<"Pixelclock \t"<<uGrabberc1->getCurrentPixelClockValue()<<std::endl;
   std::cout<<"EXPOSURE= "<<uGrabberc1->getExposure()<<std::endl;
   std::cout<<"Framerate \t"<<uGrabberc1->getFramerate()<<std::endl;
   std::cout<<"Gain \t"<<uGrabberc1->getGain()<<std::endl;
}
double vpUeyeFrameGrabber::getExposure(){
   double exp = uGrabberc1->getExposure();
   std::cout<<"Framerate \t"<<uGrabberc1->getFramerate()<<std::endl;
   std::cout<<"Gain \t"<<uGrabberc1->getGain()<<std::endl;
   std::cout<<"Pixelclock \t"<<uGrabberc1->getCurrentPixelClockValue()<<std::endl;
   return exp;
}
void vpUeyeFrameGrabber::printCameraParameters()
{
   std::cout<<"*********************Settings now at **********  \t"<<std::endl;
   std::cout<<"AutoExposure setting = "<<uGrabberc1->getAutoExposureSetting()<<std::endl;
   std::cout<<"Pixelclock \t"<<uGrabberc1->getCurrentPixelClockValue()<<std::endl;
   std::cout<<"EXPOSURE= "<<uGrabberc1->getExposure()<<std::endl;
   std::cout<<"Framerate \t"<<uGrabberc1->getFramerate()<<std::endl;
   std::cout<<"Gain \t"<<uGrabberc1->getGain()<<std::endl;
}

