/*
	Bones - Tracker.h
	----------------------------

	Written by Matthew Beckett. 2013

	
*/

#include "common.h"

class Tracker
{
	public:
		Tracker(){};						// This comment is to avoid stating the obvoius
		virtual ~Tracker(){};				// ""
		void Go();							// Calls m_Update when sensor is available
		void Noise(LPCSTR a);				// Calls PlaySound
		void ReturnKeys(BYTE vk);			// Produces keyboard input to the system from NUI events
		void Mouse(long dx, long dy);		// Genereates system mouse event for mouse movement
		void Left_Click();					// Left click for mouse input

	private:

		LPSTR choice;						// 
		HANDLE m_hNextSkeletonEvent;		// 
	    INuiSensor* m_pNuiSensor;			// The current available sensor when connected
		HRESULT m_StartKinect();			// Initialisation of the sensor

		Vector4 m_rightElbow, m_rightAnkle, m_rightFoot, m_rightShoulder, m_rightHand, m_rightHip, m_rightWrist, m_rightKnee;	// Variables to store skeletal positions
		Vector4 m_leftElbow, m_leftAnkle, m_leftFoot, m_leftShoulder, m_leftHand, m_leftHip, m_leftWrist, m_leftKnee;			// Variables to store skeletal positions
		
		Vector4 m_centerShoulder, m_centerHip;				// Variables to store skeletal positions
		Vector4 m_Head, m_Spine;							// Variables to store skeletal positions
		void m_Update();									// Calls m_GenerateInput and m_MapInput
		void m_ProcessFrame(NUI_SKELETON_FRAME*);			// Calls m_SkeletonTracked or m_SkeletonPosition, depending on frame state
		void m_SkeletonTracked(const NUI_SKELETON_DATA);	// Set skeletal variable positions from current sensor readings
		void m_GenerateInput();								// Calls various functions for mouse and keyboard inputs
		void m_SkeletonPosition(Vector4){};					// ?
		
		void m_MapBones(const NUI_SKELETON_DATA &, NUI_SKELETON_POSITION_INDEX, NUI_SKELETON_POSITION_INDEX);	// 
		
		void Debugwindow();					// Initialise console for debug mode
		bool m_Timer(int length);			// Timer function used in development
		void m_Mapinput();					// Saves the X, Y and Z coordinates to the file map.txt
};
