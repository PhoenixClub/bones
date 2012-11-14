#include "common.h"

class tracker
{
	public:
		tracker(){};
		virtual ~tracker(){};
		void go();
	private:
		HANDLE m_hNextSkeletonEvent;
	    INuiSensor* m_pNuiSensor;

		HRESULT startkinect();
		void update();
		void processframe(NUI_SKELETON_FRAME*);
		void skeletontracked(NUI_SKELETON_DATA);
		void skeletonposition(Vector4);
};