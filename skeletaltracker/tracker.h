/*
	Skeletal Tracker - Tracker.h
	----------------------------

	Written by Matthew Beckett. 2012
*/

#include "common.h"
#define _DEBUG 

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
		void m_Update();		
		void m_ProcessFrame(NUI_SKELETON_FRAME*);
		void m_SkeletonTracked(const NUI_SKELETON_DATA);
		void m_SkeletonPosition(Vector4){};
		void m_MapBones(const NUI_SKELETON_DATA &, NUI_SKELETON_POSITION_INDEX, NUI_SKELETON_POSITION_INDEX);
		void Debugwindow();
		bool m_Timer(int length);
};
