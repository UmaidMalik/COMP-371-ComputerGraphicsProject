/*
*	COMP 371 - Assignment 2
*
*	Umaid Malik 27576110
*	Noor Alali
*	Vishal Senewiratne
*
	***************************************************** 
	*                                                   *
	*   NOTE: CLICK RIGHT MOUSE BUTTON TO SHOW RENDER   *
	*                                                   *
	*****************************************************
*
*	CONTROLS:
*
*		VIEW SELECTION CONTROLS
*		==============================================================
*
*       	ESCAPE              : QUIT PROGRAM
*
*		==============================================================
*		X					: TOGGLE TEXTURE ON/OFF
*
*		E					: PERSPECTIVE PROJECTION
*
*		V					: ORTHOGRAPHIC PROJECTION
*
*		B					: RENDER TRIANGLES
*
*		N					: RENDER LINES
*
*		M					: RENDER POINTS
*
*		C					: HOLD TO DISABLE BACKFACE CULLING
*
*		HOME					: RETURN TO INITIAL POSTION
*
*		MOUSE SCROLL UP				: ZOOM IN
*
*		MOUSE SCROLL DOWN			: ZOOM OUT
*
*
*		PAN AND TILT (MOUSE MOVEMENT) WHILE HOLDING RIGHT MOUSE BUTTON
*		==============================================================
*
*		W		: MOVE FORWARD
*
*		S		: MOVE BACKWARD
*
*		A		: MOVE LEFT
*
*		D		: MOVE RIGHT
*
*		Z		: MOVE NEGATIVE Y-DIRECTION
*
*		SPACE		: MOVE POSITIVE Y-DIRECTION
*
*		SHIFT		: INCREASE MOVEMENT SPEED
*
*		==============================================================
*
*
*		MODEL CONTROL
*		ENTER EITHER 1, 2, 3, 4 OR 5 TO SELECT MODELS - LEFT MOUSE CLICK WILL DESELECT ALL MODELS
*		====================================================================================================================
*
*		TAB						: TOGGLE BETWEEN INCREMENTAL OR CONTINUOUS MOVEMENT
*
*		W						: MOVE MODEL IN +Z DIRECTION
*
*		S						: MOVE MODEL IN -Z DIRECTION
*
*		A						: MOVE MODEL IN -X DIRECTION
*
*		D						: MOVE MODEL IN +X DIRECTION
*
*		Z						: MOVE MODEL IN -Y DIRECTION
*
*		SPACE						: MOVE MODEL IN +Y DIRECTION
*
*		R						: RESET MODEL TO INITIAL POSITION
*
*		SHIFT + R					: RESET MODEL TO INITIAL SIZE
*
*		LCTRL + SHIFT + R				: RESET MODEL ORIENTATION
*
*		U						: UPSCALE MODEL
*
*		J						: DOWNSCALE MODEL
*
*		T						: ROTATE MODEL +X DIRECTION
*
*		Y						: ROTATE MODEL -X DIRECTION
*
*		G						: ROTATE MODEL +Y DIRECTION
*
*		H						: ROTATE MODEL -Y DIRECTION
*
*		B						: ROTATE MODEL +Z DIRECTION
*
*		N						: ROTATE MODEL -Z DIRECTION
*
*		SHIFT + W/A/S/D/X/SPACE				: INCREASE MOVEMENT SPEED
*
*		SHIFT + U/J					: INCREASE SCALING SPEED
*
*		ALT						: RANDOMIZE THE POSITION OF THE MODEL WITHIN THE GRID AREA
*
*		ARROW UP					: +Y ORIENTATION
*
*       ARROW DOWN						: -Y ORIENTATION
*
*		ARROW LEFT					: -X ORIENTATION
*
*		ARROW RIGHT					: +X ORIENTATION
*
*
*
*		RUBIKS CUBE CONTROL - PRESS 6 TO SELECT
*		====================================================================================================================
*		
*		Q/A						: +/-X ROTATION LAYER 1
*
*		W/S						: +/-X ROTATION LAYER 2
*
*		E/D						: +/-X ROTATION LAYER 3
*
*		R/F						: +/-Y ROTATION LAYER 1
*
*		T/G						: +/-Y ROTATION LAYER 2 
*
*		Y/H						: +/-Y ROTATION LAYER 3
*
*		U/J						: +/-Z ROTATION LAYER 1
*
*		I/K						: +/-Z ROTATION LAYER 2
*
*		O/L						: +/-Z ROTATION LAYER 3
*
*
*		TIMER CONTROL
*		====================================================================================================================
*
*		0						: RESET TIMER