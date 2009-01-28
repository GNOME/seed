typedef void (*glutTimerCallback) (int);
void glutTimerFunc (unsigned int time, 
					glutTimerCallback callback,
					int value);

typedef void (*glutIdleCallback) (void);
void glutIdleFunc (glutIdleCallback callback);

typedef void (*glutKeyboardCallback) (unsigned char, int, int);
void glutKeyboardFunc (glutKeyboardCallback callback);

typedef void (*glutSpecialCallback) (int, int, int);
void glutSpecialFunc (glutSpecialCallback callback);

typedef void (*glutReshapeCallback) (int, int);
void glutReshapeFunc (glutReshapeCallback callback);

typedef void (*glutVisibilityCallback) (int);
void glutVisibilityFunc (glutVisibilityCallback callback);

typedef void (*glutDisplayCallback) (void);
void glutDisplayFunc (glutDisplayCallback callback);

typedef void (*glutMouseCallback) (int, int, int, int);
void glutMouseFunc(glutMouseCallback callback);

typedef void (*glutMotionCallback) (int, int);
void glutMotionFunc (glutMotionCallback callback);

typedef void (*glutPassiveMotionCallback) (int, int);
void glutMotionFunc (glutPassiveMotionCallback callback);

typedef void (*glutEntryCallback) (int);
void glutEntryFunc (glutEntryCallback callback);

typedef void (*glutKeyboardUpCallback) (unsigned char, int, int);
void glutKeyboardUpFunc (glutKeyboardUpCallback callback);

typedef void (*glutSpecialUpCallback) (int, int, int);
void glutSpecialUpFunc (glutSpecialUpCallback callback);

typedef void (*glutJoystickUpCallback) (unsigned int, int, int, int);
void glutJoystickUpFunc (glutJoystickUpCallback callback);

typedef void (*glutMenuStateCallback) (int);
void glutMenuStateFunc (glutMenuStateCallback callback);

typedef void (*glutMenuStatusCallback) (int, int, int);
void glutMenuStatusFunc (glutMenuStatusCallback callback);

typedef void (*glutOverlayDisplayCallback) (void);
void glutOverlayDisplayFunc (glutOverlayDisplayCallback callback);

typedef void (*glutWindowStatusCallback) (int);
void glutWindowStatusFunc (glutWindowStatusCallback callback);

typedef void (*glutSpaceballMotionCallback) (int, int, int);
void glutSpaceballMotionFunc (glutSpaceballMotionCallback callback);

typedef void (*glutSpaceballRotateCallback) (int, int, int);
void glutSpaceballRotateFunc (glutSpaceballRotateCallback callback);

typedef void (*glutSpaceballButtonCallback) (int, int);
void glutSpaceballButtonFunc (glutSpaceballButtonCallback callback);

typedef void (*glutButtonBoxCallback) (int, int);
void glutButtonBoxFunc (glutButtonBoxCallback callback);

typedef void (*glutDialsCallback) (int, int);
void glutDialsFunc (glutDialsCallback callback);

typedef void (*glutTabletMotionCallback) (int, int);
void glutTabletMotionFunc (glutTabletMotionCallback callback);

typedef void (*glutTabletButtonCallback) (int, int, int, int);
void glutTabletButtonFunc (glutTabletButtonCallback callback);

typedef void (*glutCreateMenuCallback) (int);
void glutCreateMenuFunc (glutCreateMenuCallback callback);

