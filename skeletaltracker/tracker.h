/*
	Skeletal Tracker - Tracker.h
	----------------------------

	Written by Matthew Beckett. 2012
*/

#include "common.h"


class Tracker
{
	public:
		Tracker(){}; 
		virtual ~Tracker(){};
		void Go();
		void Noise(LPCSTR a);
		void ReturnKeys(BYTE vk);

	private:
		HANDLE m_hNextSkeletonEvent;
	    INuiSensor* m_pNuiSensor;
		HRESULT m_StartKinect();
		Vector4 m_rightElbow, m_rightAnkle, m_rightFoot, m_rightShoulder, m_rightHand, m_rightHip, m_rightWrist, m_rightKnee;
		Vector4 m_leftElbow, m_leftAnkle, m_leftFoot, m_leftShoulder, m_leftHand, m_leftHip, m_leftWrist, m_leftKnee;
		Vector4 m_centerShoulder, m_centerHip;
		Vector4 m_Head, m_Spine;
		void m_Update();		
		void m_ProcessFrame(NUI_SKELETON_FRAME*);
		void m_SkeletonTracked(const NUI_SKELETON_DATA);
		void m_GenerateInput();
		void m_SkeletonPosition(Vector4){};
		void m_MapBones(const NUI_SKELETON_DATA &, NUI_SKELETON_POSITION_INDEX, NUI_SKELETON_POSITION_INDEX);
		void Debugwindow();
		bool m_Timer(int length);
};
