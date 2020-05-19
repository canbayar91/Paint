#define _CRT_SECURE_NO_DEPRECATE
#define MAX_POINTS 5000
#define PI_2 6.2832

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <GL\freeglut_ext.h>
#include <GLFW\glfw3.h>

// Default window size
GLsizei windowWidth = 960;
GLsizei windowHeight = 640;

// Current options
GLfloat red = 0.0f;
GLfloat green = 0.0f;
GLfloat blue = 0.0f;
GLfloat opacity = 1.0f;
int style = GL_LINE_LOOP;
int lineWidth = 3;

// Mouse positions for the first point for lines, rectangles and ellipses
int pointX;
int pointY;

// Menu items
int selected = 1;			// selected drawing style
int selectMenuId;
int colorMenuId;
int opacityMenuId;
int thicknessMenuId;
int fillMenuId;

// Store information
int points_array[MAX_POINTS][2];		// positions of the vertices
int shapes_array[MAX_POINTS][4];		// stores to vertices in the list manner along with drawing style and line width
GLfloat colors_array[MAX_POINTS][4];	// stores the vertex colors
int current_shape = -1;					// no shape has been created so far
int current_point = 0;					// currently created point


// Creates a new shape
void newShape(){
    current_shape++;

	// the properties of the new shape
    shapes_array[current_shape][0] = current_point;
    shapes_array[current_shape][2] = style;
	shapes_array[current_shape][3] = lineWidth;

	// the colors of the new shape
	colors_array[current_shape][0] = red;
	colors_array[current_shape][1] = green;
	colors_array[current_shape][2] = blue;
	colors_array[current_shape][3] = opacity;
}

// Adds a new point to the current shape
void addPoint(int x , int y){
	// if no shape has been created already, create the first one
    if(current_shape == -1)
        newShape();
	
	// send position to the array
    if (current_point < MAX_POINTS){
        points_array[current_point][0] = x;
        points_array[current_point][1] = y;
        current_point++;
        shapes_array[current_shape][1] = current_point;		// point to the next vertex
    }
}

// Creates a line in the given coordinates
void createLine(int x1, int x2, int y1, int y2){
	addPoint(x2, y2);
}

// Creates a rectangle in the given coordinates
void createRectangle(int x1, int x2, int y1, int y2){
	addPoint(x2, y1);
	addPoint(x2, y2);
	addPoint(x1, y2);
}

// Creates an circle in the given coordinates
void createCircle(int x1, int x2, int y1, int y2){
	int n = 60;								// number of segments
	GLfloat x, y;							// current width and height values
	GLfloat angleIncrement = PI_2/n;		// amount of increment

	// to transform into coordinate system with floating point values
	GLfloat aspect = (GLfloat)windowWidth / (GLfloat)windowHeight;
	GLfloat centerX = 2 * ((GLfloat) x1/windowWidth) - 1.0;
	GLfloat centerY = 2 * ((GLfloat) y1/windowHeight) - 1.0;
	GLfloat releaseX = 2 * ((GLfloat) x2/windowWidth) - 1.0;
	GLfloat releaseY = 2 * ((GLfloat) y2/windowHeight) - 1.0;

	// adjust according to the aspect
	if(aspect > 1.0){
		centerX *= aspect;
		releaseX *= aspect;
	}
	else{
		centerY *= aspect;
		releaseY *= aspect;
	}

	// calculate radius
	GLfloat radius = sqrt(pow(releaseX-centerX, 2) + pow(releaseY-centerY, 2));

	for(GLfloat theta = 0.0f; theta < PI_2; theta += angleIncrement){
		// calculates the vertex positions according to the angle increment value
		x = radius * cos(theta) + centerX;
		y = radius * sin(theta) + centerY;

		// back to the pixels
		if(aspect > 1.0)
			x /= aspect;
		else
			y /= aspect;

		// add the points to the array
		x = ((x + 1.0) / 2) * windowWidth;
		y = ((y + 1.0) / 2) * windowHeight;
		addPoint((int)x, (int)y);
	}
}

// Saves the window into an image file
void captureViewPort(const char *fileName){
	FILE *fp;
    unsigned long imageSize;
    GLbyte *data = NULL;
    GLint viewPort[4];
    GLenum lastBuffer;

	// Define file and info header
    BITMAPFILEHEADER fileHeader;	// type, size and layout
    BITMAPINFOHEADER infoHeader;	// colors and dimensions

    fileHeader.bfType = 'MB';		// file type, must be MB
    fileHeader.bfReserved1 = 0;		// reserved, must be 0
    fileHeader.bfReserved2 = 0;		// reserved, must be 0
    fileHeader.bfOffBits = 54;		// offset from the beginning

	// define the image size
    glGetIntegerv(GL_VIEWPORT, viewPort);
    imageSize = (viewPort[2] + (4 - viewPort[2]%4)%4) * viewPort[3] * 3 + 2;
    fileHeader.bfSize = imageSize + sizeof(fileHeader) + sizeof(infoHeader);
    data = (GLbyte *)malloc(imageSize);

	// store pizels
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_PACK_SWAP_BYTES, 1);

	// read buffer and screen
    glGetIntegerv(GL_READ_BUFFER, (GLint *) &lastBuffer);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, viewPort[2], viewPort[3], GL_BGR,GL_UNSIGNED_BYTE, data);
    data[imageSize-1] = 0;
    data[imageSize-2] = 0;
    glReadBuffer(lastBuffer);

	// open file
    fp=fopen(fileName, "wb");

    infoHeader.biSize = 40;					// number of bytes required
    infoHeader.biWidth = viewPort[2];		// width in pixels
    infoHeader.biHeight = viewPort[3];		// height in pixels
    infoHeader.biPlanes = 1;				// number of planes
    infoHeader.biBitCount = 24;				// number of bits per pixel (RGB)
    infoHeader.biCompression = 0;			// type of compression
    infoHeader.biSizeImage = imageSize;		// image size in bytes
    infoHeader.biXPelsPerMeter = 45089;		// horizontal resolution
    infoHeader.biYPelsPerMeter=45089;		// vertical resolution
    infoHeader.biClrUsed = 0;				// number of color indexes
    infoHeader.biClrImportant = 0;			// all colors are required

	// write the information into file
    fwrite(&fileHeader, sizeof(fileHeader), 1, fp);
    fwrite(&infoHeader, sizeof(infoHeader), 1, fp);
    fwrite(data, imageSize, 1, fp);
    free(data);
    fclose(fp);
}

// Opens an image into the window
void openImage(const char *fileName){
	GLuint texture;
    unsigned char *data;
    FILE *fp = fopen(fileName, "rb");

    //read in RAW file
	data = (unsigned char *)malloc(windowWidth * windowHeight * 3);
    fread(data, windowWidth * windowHeight * 3, 1, fp);
    fclose(fp);

    glGenTextures(1, &texture); 
    glBindTexture(GL_TEXTURE_2D, texture); 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 

    // better quality
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // edge of the shape
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);
}

// Called when a new shape is drawn
void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear the buffers
	glMatrixMode(GL_MODELVIEW);								// switch to the drawing perspective
	glLoadIdentity();										// reset the drawing perspective
	
	// runs through all the points that have been created
    for(int i=0; i<=current_shape; i++){
		// sets line width and color
		glColor4f(colors_array[i][0], colors_array[i][1], colors_array[i][2], colors_array[i][3]);
		glLineWidth(shapes_array[i][3]);

		// drawing lines
        glBegin(shapes_array[i][2]);
        for(int j=shapes_array[i][0]; j<shapes_array[i][1]; j++){
            glVertex2i(points_array[j][0], points_array[j][1]);
		}
        glEnd();

		// drawing points
        glBegin(GL_POINTS);
        for(int j=shapes_array[i][0]; j<shapes_array[i][1]; j++)
            glVertex2i(points_array[j][0], points_array[j][1]);
        glEnd();
    }

    glutPostRedisplay();	// current window is redisplayed
	glutSwapBuffers();		// send the scene to the screen
	glFlush();				// render function
}


// Called when the mouse is moved while it is still clicked
void motion(int x, int y){
	// for polygons
	if(selected == 4){
		points_array[current_point-1][0] = x;
		points_array[current_point-1][1] = windowHeight - y;
		glutPostRedisplay();
	}
	// for brush
	else if(selected == 5 || selected == 6){
		addPoint(x, windowHeight-y);
	}
}

// Called when the mouse is clicked
void mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		// for lines, rectangles and ellipses
		if(selected == 1 || selected == 2 || selected == 3){
			newShape();
			pointX = x;
			pointY = windowHeight-y;				
			if(selected != 3)	// skip for ellipse, center point is not needed to be drawn
				addPoint(pointX, pointY);
		}	
		// for polygons
		else if(selected == 4)
			addPoint(x, windowHeight-y);
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		// for line, rectangle and ellipse
		if(selected == 1 || selected == 2 || selected == 3){
			if(selected == 1)
				createLine(pointX, x, pointY, windowHeight-y);
			else if(selected == 2)
				createRectangle(pointX, x, pointY, windowHeight-y);
			else
				createCircle(pointX, x, pointY, windowHeight-y);
		}
		// for brush and sprey
		if(selected == 5 || selected == 6)
			newShape();
	}
	else if(button == GLUT_RIGHT_BUTTON && (state == GLUT_DOWN || state == GLUT_UP))
		newShape();		// safeguard
}

// Called when a button on keyboard is pressed
void keyboard(unsigned char key , int x ,int y){
	switch (key) {
	case 13:	// press enter to finish drawing a polygon
		newShape();
		break;
	case 27:	// press escape to exit
        exit(0);
		break;
	case 's':	// save as image
		captureViewPort("save.bmp");
		break;
	case 'o':	// open image
		openImage("open.bmp");
		break;
    }
}

// Called when the window is resized
void reshape(GLsizei width, GLsizei height){
	// prevents divide by zero
	if(height == 0)
		height = 1;

	// the proportion of the screen
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	glViewport(0, 0, width, height);	// to cover the new window
	glMatrixMode(GL_PROJECTION);		// projection mode
	glLoadIdentity();					// reset the projection matrix

	// set to the new values
	windowWidth = width;
	windowHeight = height;

	// define a 2D orthographic projection matrix
	gluOrtho2D(0.0, (GLdouble)windowWidth, 0.0, (GLdouble)windowHeight);
}

// Called when a menu option is selected
void menu(int id){
	newShape();		// safeguard
	switch(id){
	case 1:
		style = GL_LINE_LOOP;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		selected = 1;	// line
		break;
	case 2:
		style = GL_LINE_LOOP;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		selected = 2;	// rectangle
		break;
	case 3:
		style = GL_LINE_LOOP;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		selected = 3;	// circle
		break;
	case 4:
		style = GL_LINE_LOOP;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		selected = 4;	// polygon
		break;
	case 5:
		style = GL_LINE_STRIP;
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		selected = 5;	// brush
		break;
	case 6:
		style = GL_POINTS;
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
		selected = 6;	// sprey
		break;
	case 7:
		red = 1.0f;
		green = 1.0f;
		blue = 1.0f;
		break;
	case 8:
		red = 0.0f;
		green = 0.0f;
		blue = 0.0f;
		break;
	case 9:
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
		break;
	case 10:
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 11:
		red = 0.0f;
		green = 0.0f;
		blue = 1.0f;
		break;
	case 12:
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
		break;
	case 13:
		red = 0.0f;
		green = 1.0f;
		blue = 1.0f;
		break;
	case 14:
		red = 1.0f;
		green = 0.0f;
		blue = 1.0f;
		break;
	case 15:
		red = 0.5f;
		green = 0.5f;
		blue = 0.5f;
		break;
	case 16:
		red = 1.0f;
		green = 0.5f;
		blue = 1.0f;
		break;
	case 17:
		red = 0.5f;
		green = 0.25f;
		blue = 0.1f;
		break;
	case 18:
		red = 1.0f;
		green = 0.65f;
		blue = 0.0f;
		break;
	case 19:
		opacity = 0.0f;
		break;
	case 20:
		opacity = 0.25f;
		break;
	case 21:
		opacity = 0.5f;
		break;
	case 22:
		opacity = 0.75f;
		break;
	case 23:
		opacity = 1.0f;
		break;
	case 24:
		lineWidth = 1;
		break;
	case 25:
		lineWidth = 3;
		break;
	case 26:
		lineWidth = 5;
		break;
	case 27:
		lineWidth = 8;
		break;
	case 28:
		lineWidth = 10;
		break;
	case 29:
		style = GL_TRIANGLE_FAN;
		break;
	case 30:
		style = GL_LINE_LOOP;
		break;
	default:
		selected = 1;
		red = 0.0f;
		green = 0.0f;
		blue = 0.0f;
		opacity = 1.0f;
		lineWidth = 3;
		style = GL_LINE_LOOP;
		glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		break;
	}
}

// Initializes the contents
void initialize(void){
    glClearColor(1.0, 1.0, 1.0, 1.0);		// set background to white
	glColor4f(red, green, blue, opacity);	// set paint color to black

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

	// Enable features
	glEnable(GL_DEPTH_TEST);				// to enable depth
	glEnable(GL_BLEND);						// to enable transparency
    glEnable(GL_LINE_SMOOTH);				// anti-aliasing
	glEnable(GL_POINT_SMOOTH);				// round points

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glShadeModel(GL_FLAT);  

	// Drawing objects menu
	selectMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Line", 1);
	glutAddMenuEntry("Rectangle", 2);
	glutAddMenuEntry("Circle", 3);
	glutAddMenuEntry("Polygon", 4);
	glutAddMenuEntry("Brush", 5);
	glutAddMenuEntry("Sprey", 6);

	// Color menu
	colorMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);
	glutAddMenuEntry("Red", 9);
	glutAddMenuEntry("Green", 10);
	glutAddMenuEntry("Blue", 11);
	glutAddMenuEntry("Yellow", 12);
	glutAddMenuEntry("Cyan", 13);
	glutAddMenuEntry("Magenta", 14);
	glutAddMenuEntry("Grey", 15);
	glutAddMenuEntry("Pink", 16);
	glutAddMenuEntry("Brown", 17);
	glutAddMenuEntry("Orange", 18);
	
	// Opacity menu
	opacityMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("%0", 19);
	glutAddMenuEntry("%25", 20);
	glutAddMenuEntry("%50", 21);
	glutAddMenuEntry("%75", 22);
	glutAddMenuEntry("%100", 23);

	// Line thickness menu
	thicknessMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("1 px", 24);
	glutAddMenuEntry("3 px", 25);
	glutAddMenuEntry("5 px", 26);
	glutAddMenuEntry("8 px", 27);
	glutAddMenuEntry("10 px", 28);

	// Fill objects menu
	fillMenuId = glutCreateMenu(menu);
	glutAddMenuEntry("Open", 29);
	glutAddMenuEntry("Close", 30);

	// Attach the submenus
	glutCreateMenu(menu);
	glutAddSubMenu("Select", selectMenuId);
	glutAddSubMenu("Color", colorMenuId);
	glutAddSubMenu("Opacity", opacityMenuId);
	glutAddSubMenu("Thickness", thicknessMenuId);
	glutAddSubMenu("Fill", fillMenuId);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Main function
int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("Simple Paint");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	// required for freeglut
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// initialization
	glewInit();
    initialize();

	// standard functions
    glutDisplayFunc(display);
    glutMotionFunc(motion);
    glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}