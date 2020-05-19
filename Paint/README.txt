First of all, I did not use any other library than freeglut and glew in my project. So, if those libraries are 
built correctly, it should run without any problems.

I preferred to use visual studio 2012 in the project. All you need to do is to define glut32.lib in the additional dependencies.

When the program is started, you will be able to draw lines with left click and dragging. You can choose the 
other objects from the menu by clicking right mouse button along with various kinds of colors, line thickness, 
opacity and whether the objects will be filled or not.

Line, rectangle and circle drawing is working using left click and drag. Polygon drawing works by clicking to
the points one by one to create the vertices. It will define the final shape of the polygon when you click to
the enter after drawing is finished.

To save the image, press 's' on the keyboard which will save the window into a 'save.bmp' file. To open an image
press 'o' from the keyboard(missing).

By pressing escape from the keyboard you can exit from the program.

Can Bayar.