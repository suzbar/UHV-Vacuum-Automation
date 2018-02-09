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

Disclaimer: 
This software is provided as is. The software was developed and used in an experimental context for the "Towards Autonomous Sample Positioning For Ultra High Vacuum Chambers" PhD project at the Institute of Mathematics, Physics and Computer Science (IMPACS), Aberystwith University. The software is incomplete in the sense that it does not close the tracking loop but it does place the sample at the positions issued to the actuators ie. it is actuator driven and not camera driven yet. The functions for mapping the camera outputs to the actuator rotational outputs are not yet included in this software version but are provided separately as python code because this was implemented as a post processing step during experiementation.

Who do I talk to? 
Repo owner or admin = Suzana Barreto (suzana.barreto@btinternet.com)
Other community or team contact = Dr. Frederic Labbrose (ffl@aber.ac.uk)
