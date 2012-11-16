#include "common.h"

class tracker
{
	public:
		tracker(){};
		virtual ~tracker(){};
		void go();
		void noise(LPCSTR a);
	private:
		HANDLE m_hNextSkeletonEvent;
	    INuiSensor* m_pNuiSensor;
		HRESULT startkinect();
		void update();		
		void processframe(NUI_SKELETON_FRAME*);
		void skeletontracked(NUI_SKELETON_DATA){};
		void skeletonposition(Vector4){};
		void mapbones(const NUI_SKELETON_DATA &, NUI_SKELETON_POSITION_INDEX, NUI_SKELETON_POSITION_INDEX);
		
};