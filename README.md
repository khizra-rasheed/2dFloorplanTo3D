# 2dFloorplanTo3D

The idea of this project is to generate 3d house layout using 2d floorplan images in games.
This module was to be integrated in a metaverse.
Developed on UE5.1.

**Tools and Techniques used:**
I have used yolov4 darknet to detect the windows and doors in the image.
OpenCV has been used to detect the walls in the image. Then those coordinates were translated in 3d in unreal engine.
The source code is present in Source/FloorPlanGeneration/MyCharacter file.

Video Link:

https://drive.google.com/file/d/1cf_3jOuAGwVTctj5x0yJnkuurn0t0j_H/view?usp=drive_link