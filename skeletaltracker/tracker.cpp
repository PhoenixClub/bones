/*
	Skeletal Tracker - Tracker.cpp

	Written by Matthew Beckett. 2012.
*/

#include "tracker.h"

HRESULT Tracker::m_StartKinect() 
{    
	INuiSensor *pNuiSensor = (INuiSensor*)NULL;
	m_pNuiSensor = (INuiSensor*)NULL;
    int iSensorCount = 0;
	HRESULT hr = NuiGetSensorCount(&iSensorCount);
    
	if (FAILED(hr)) return hr;

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i) 
	{
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        
		if (FAILED(hr)) continue;

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
		if (SUCCEEDED(hr)) m_pNuiSensor = pNuiSensor; break;

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release(), pNuiSensor = NULL;
    }

    if (NULL != m_pNuiSensor ) 
	{
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
        
		if (SUCCEEDED(hr)) 
		{
            // Create an event that will be signaled when skeleton data is available
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
			std::cout << "A Kinect Sensor has been found";
			Noise("succeed.wav");

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr)) 
	{
		std::cout << "No Kinect Sensor Found";
		Noise("fail.wav");
        return E_FAIL;
    }

    return hr;
}

void Tracker::m_Update() 
{
	// Wait for 0ms, just quickly test if it is time to process a skeleton
	if (WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0)) 
	{
		NUI_SKELETON_FRAME skeletonFrame = {0};
        // Get the skeleton frame that is ready
        if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame))) m_ProcessFrame(&skeletonFrame);
	}
}

void Tracker::m_ProcessFrame(NUI_SKELETON_FRAME* pSkeletonFrame) 
{
	for (int i = 0; i < NUI_SKELETON_COUNT; i++) 
	{
		const NUI_SKELETON_DATA &skeleton = pSkeletonFrame->SkeletonData[i];
        switch (skeleton.eTrackingState) 
		{
	        case NUI_SKELETON_TRACKED: m_SkeletonTracked(skeleton); break;
			case NUI_SKELETON_POSITION_ONLY: m_SkeletonPosition(skeleton.Position); break;			
		}
	}
}

// TO BE COMPLETED
void Tracker::m_SkeletonTracked(const NUI_SKELETON_DATA skeleton)
{
	 Vector4 lefthand = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
	 Vector4 leftshoulder = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
	 Vector4 righthand = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
	 Vector4 rightshoulder = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];

	 if (lefthand.x==(leftshoulder.x+5) && righthand.x==(rightshoulder.x+5)) 
	 {
		Sleep(200);
		if (lefthand.x==(leftshoulder.x-5) && righthand.x==(rightshoulder.x-5)) ReturnKeys(VK_RIGHT);
	 }

	 if (lefthand.x==(leftshoulder.x-5) && righthand.x==(rightshoulder.x-5)) 
	 {
		 Sleep(200);
		 if (lefthand.x==(leftshoulder.x+5) && righthand.x==(rightshoulder.x+5)) ReturnKeys(VK_RIGHT);
	 }
}

void Tracker::Noise(LPCSTR a)
{
	PlaySound(TEXT(a), NULL, SND_ASYNC + SND_NOSTOP);
}
 
void Tracker::ReturnKeys(BYTE vk)
{ 
	INPUT Input; 
	ZeroMemory(&Input, sizeof(Input)); 
	Input.type = INPUT_KEYBOARD; 
	Input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY; 
	Input.ki.wVk = vk; 
	SendInput(1, &Input, sizeof(INPUT));
	return;	
}

void Tracker::Debugwindow()
{
	AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
}


void Tracker::Go() 
{
	#ifdef _DEBUG
	Debugwindow();
	#endif
	m_StartKinect();
	for(;;) m_Update();
}       
 