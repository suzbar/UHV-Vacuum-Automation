# UHV-Vacuum-Automation
Repository for the 'Towards Autonomous Sample Positioning for Ultra High Vacuum Chambers'  PhD research project.

What is this repository for?
Automated Vacuum chamber sample tracking and positioning prototype developed for PhD Research project. The developed prototype 
comprises thorlabs TDC01 and BSC203 controllers, 2 UEYE industrial USB cameras and makes use of ViSP and imalib third party 
libraries for realtime tracking and positioning. 
    

How do I get set up?
Summary of set up: Get and install dependencies, download, build and run project.  Unfortunately due to the variability of 
vacuum chambers, drives and controllers, and camera equipment that could be used, you will more than likely have to extend the 
ViSP camera interface to suit your equipment, likewise configuration of init files will be necessary as will any drivers to 
interface with controllers other than the Thorlabs TDC01 and BSC203 controllers.

Configuration: Camera init files can be found in config/init_files/ folder, these should be changed to reflect the init poses 
for dual camera monocular tracking mode. For stereo mode, init poses for each camera are given as homogenous matrices, the 
transformation between the two cameras is also required. Intrinsics and other tracking parameters are found in the .xml files in 
config/cam_settings/ folder. The model file can be found in config folder.

Dependencies: This software depends on Imalib which is an image library developed by Dr. Frederic Labbrose (ffl@aber.ac.uk). 
This software also depends on ViSP (Visual Servoing Platform) developed by the Inria Lagadic Team (https://visp.inria.fr/) which 
is available at https://github.com/lagadic/visp

