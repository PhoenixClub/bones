/*
	Bones - Tracker.cpp

	Written by Matthew Beckett. 2013.
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
			#ifdef _DEBUG
			std::cout << "A Kinect Sensor has been found" << std::endl;
			#endif
			Noise("succeed.wav");

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr)) 
	{
		#ifdef _DEBUG
		std::cout << "No Kinect Sensor Found" << std::endl;
		#endif
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
	m_GenerateInput();
	#ifdef _DEBUG
		m_Mapinput();
	#endif
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

void Tracker::m_SkeletonTracked(const NUI_SKELETON_DATA skeleton)
{
	m_leftHand = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];//get and store in a usable format
	m_leftShoulder = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
	m_leftElbow = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
	m_leftHip = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HIP_LEFT];
	m_leftAnkle = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_ANKLE_LEFT];
	m_leftFoot = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_FOOT_LEFT];
	m_leftWrist = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT];
	m_leftKnee = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_KNEE_LEFT];
	m_rightHand = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
	m_rightShoulder = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
	m_rightElbow = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
	m_rightHip = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HIP_RIGHT];
	m_rightAnkle = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_ANKLE_RIGHT];
	m_rightFoot = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_FOOT_RIGHT];
	m_rightKnee = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_KNEE_RIGHT];
	m_rightWrist = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT];
	m_centerShoulder = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];
	m_centerHip = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER];
	m_Head = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_HEAD];
	m_Spine = skeleton.SkeletonPositions[NUI_SKELETON_POSITION_SPINE];
	
}

void Tracker::m_GenerateInput()
{
	Vector4 lHand = m_leftHand;
	Vector4 rHand = m_rightHand;
	Vector4 lShoulder = m_leftShoulder;
	Vector4 rShoulder = m_rightShoulder;
	
	////////////////////////////////////////////////////////// 
	/*			
				*** Mouse Input ***
	*/
	///////////////////////////////////////////////////////////

			/*
				Mouse(rHand.x*100, -rHand.y*100);
				if (lHand.y >= lShoulder.y)
					Left_Click();
			*/

	////////////////////////////////////////////////////////// 
	/*			
				*** Semaphore Input ***
	*/
	///////////////////////////////////////////////////////////

		// A
		if ((lHand.x > lShoulder.x) && (lHand.y < lShoulder.y) && (lHand.x < rShoulder.x) && (rHand.x > rShoulder.x+0.2) && (rHand.y < rShoulder.y))
		{
				ReturnKeys(0x41);
				#ifdef _DEBUG
				std::cout << "Printing the letter A"<<std::endl;
				#endif
				Sleep(1000);
		}

		// B
		if ((lHand.x > lShoulder.x) && (lHand.x < rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2) && (rHand.x > rShoulder.x) && (lHand.y < lShoulder.y))
		{
				ReturnKeys(0x42);
				#ifdef _DEBUG
				std::cout << "Printing the letter B"<<std::endl;
				#endif
				Sleep(1000);
		}

		// C
		if ((rHand.x > rShoulder.x) && (rHand.y > lShoulder.y+0.2) && (lHand.x > lShoulder.x) && (lHand.x < rShoulder.x) && (lHand.y < lShoulder.y))
		{
				ReturnKeys(0x43);
				#ifdef _DEBUG
				std::cout << "Printing the letter C"<<std::endl;
				#endif
				Sleep(1000);
		}

		// D
		if ((rHand.x < rShoulder.x) && (rHand.y > rShoulder.y) && (lHand.x > lShoulder.x) && (lHand.y < lShoulder.y))
		{
				ReturnKeys(0x44);
				#ifdef _DEBUG
				std::cout << "Printing the letter D"<<std::endl;
				#endif
				Sleep(1000);
		}	
		
		// E
		if ((lHand.x < rShoulder.x) && (lHand.y > lShoulder.y+0.2) && (rHand.x < rShoulder.x) && (rHand.x > lShoulder.x) && (rHand.y < rShoulder.y))
		{
				ReturnKeys(0x45);
				#ifdef _DEBUG
				std::cout << "Printing the letter E"<<std::endl;
				#endif
				Sleep(1000);
		}

		// F
		if ((lHand.x < rShoulder.x) && (lHand.y < lShoulder.y+0.2) && (lHand.y > lShoulder.y-0.2) && (rHand.x < rShoulder.x) && (rHand.x > lShoulder.x) && (rHand.y < rShoulder.y))
		{
				ReturnKeys(0x46);
				#ifdef _DEBUG
				std::cout << "Printing the letter F"<<std::endl;
				#endif
				Sleep(1000);
		}
		
		// G
		if ((lHand.x < rShoulder.x+0.2) && (lHand.y < lShoulder.y) && (rHand.x < rShoulder.x) && (rHand.x > lShoulder.x) && (rHand.y < rShoulder.y))
		{
				ReturnKeys(0x47);
				#ifdef _DEBUG
				std::cout << "Printing the letter G"<<std::endl;
				#endif
				Sleep(1000);
		}

		// H
		if ((lHand.x > rShoulder.x) && (lHand.y < rShoulder.y) && (rHand.x > rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2))
		{
				ReturnKeys(0x48);
				#ifdef _DEBUG
				std::cout << "Printing the letter H"<<std::endl;
				#endif
				Sleep(1000);
		}
		
		// I
		if ((lHand.x > rShoulder.x) && (lHand.y > rShoulder.y) && (rHand.x > rShoulder.x) && (rHand.y < rShoulder.y-0.2))
		{
				ReturnKeys(0x49);
				#ifdef _DEBUG
				std::cout << "Printing the letter I"<<std::endl;
				#endif
				Sleep(1000);
		}

		// J
		if ((rHand.x < rShoulder.x) && (rHand.y > rShoulder.y) && (rHand.x > lShoulder.x) && (lHand.x < rShoulder.x) && (lHand.y < lShoulder.y+0.2) && (lHand.y > lShoulder.y-0.2))
		{
				ReturnKeys(0x4A);
				#ifdef _DEBUG
				std::cout << "Printing the letter J"<<std::endl;
				#endif
				Sleep(1000);
		}

		// K
		if ((lHand.x > lShoulder.x) && (lHand.x < rShoulder.x) && (lHand.y > lShoulder.y) && (rHand.x > rShoulder.x+0.2) && (rHand.y < rShoulder.y-0.2))
		{
				ReturnKeys(0x4B);
				#ifdef _DEBUG
				std::cout << "Printing the letter K"<<std::endl;
				#endif
				Sleep(1000);
		}

		// L
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y+0.2) && (rHand.x > rShoulder.x+0.2) && (rHand.y < rShoulder.y-0.2))
		{
				ReturnKeys(0x4C);
				#ifdef _DEBUG
				std::cout << "Printing the letter L"<<std::endl;
				#endif
				Sleep(1000);
		}

		// M
		if ((rHand.x > rShoulder.x+0.2) && (rHand.y < rShoulder.y-0.2) && (lHand.x < rShoulder.x) && (lHand.y < lShoulder.y+0.2) && (lHand.y > lShoulder.y-0.2))
		{
				ReturnKeys(0x4D);
				#ifdef _DEBUG
				std::cout << "Printing the letter M"<<std::endl;
				#endif
				Sleep(1000);
		}

		// N
		if ((rHand.x > rShoulder.x+0.3) && (rHand.y < rShoulder.y-0.2) && (lHand.x < lShoulder.x-0.3) && (lHand.y < lShoulder.y))
		{
				ReturnKeys(0x4E);
				#ifdef _DEBUG
				std::cout << "Printing the letter N" << std::endl;
				#endif
				Sleep(1000);
		}

		// O
		if ((rHand.x > rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2) && (lHand.x > rShoulder.x) && (lHand.y > rShoulder.y))
		{
				ReturnKeys(0x4F);
				#ifdef _DEBUG
				std::cout << "Printing the letter O" << std::endl;
				#endif
				Sleep(1000);
		}

		// P
		if ((lHand.x > lShoulder.x) && (lHand.x < rShoulder.x) && (lHand.y > lShoulder.y) && (rHand.x > rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2))
		{
				ReturnKeys(0x50);
				#ifdef _DEBUG
				std::cout << "Printing the letter P" << std::endl;
				#endif
				Sleep(1000);
		}
		
		// Q
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y+0.25) && (rHand.x > rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2))
		{
				ReturnKeys(0x51);
				#ifdef _DEBUG
				std::cout << "Printing the letter Q" << std::endl;
				#endif
				Sleep(1000);
		}

		// R
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y-0.2) && (lHand.y < lShoulder.y+0.2) && (rHand.x > rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2))
		{
				ReturnKeys(0x52);
				#ifdef _DEBUG
				std::cout << "Printing the letter R" << std::endl;
				#endif
				Sleep(1000);
		}

		// S
		if ((lHand.x < lShoulder.x-0.2) && (lHand.y < lShoulder.y-0.2) && (rHand.x > rShoulder.x) && (rHand.y < rShoulder.y+0.2) && (rHand.y > rShoulder.y-0.2))
		{
				ReturnKeys(0x53);
				#ifdef _DEBUG
				std::cout << "Printing the letter S" << std::endl;
				#endif
				Sleep(1000);
		}

		// T
		if ((lHand.x > lShoulder.x) && (lHand.x < rShoulder.x) && (lHand.y > lShoulder.y) && (rHand.x > rShoulder.x) && (rHand.y > rShoulder.y+0.2))
		{
				ReturnKeys(0x54);
				#ifdef _DEBUG
				std::cout << "Printing the letter T" << std::endl;
				#endif
				Sleep(1000);
		}

		// U
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y+0.2) && (rHand.x > rShoulder.x) && (rHand.y > rShoulder.y+0.2))
		{
				ReturnKeys(0x55);
				#ifdef _DEBUG
				std::cout << "Printing the letter U" << std::endl;
				#endif
				Sleep(1000);
		}

		// V
		if ((lHand.x < lShoulder.x-0.2) && (lHand.y < lShoulder.y-0.2) && (rHand.x < rShoulder.x) && (rHand.x > lShoulder.x) && (rHand.y > rShoulder.y))
		{
				ReturnKeys(0x56);
				#ifdef _DEBUG
				std::cout << "Printing the letter V" << std::endl;
				#endif
				Sleep(1000);
		}

		// W
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y-0.2) && (lHand.y < lShoulder.y+0.2) && (rHand.x < lShoulder.x) && (rHand.y > lShoulder.y))
		{
				ReturnKeys(0x57);
				#ifdef _DEBUG
				std::cout << "Printing the letter W" << std::endl;
				#endif
				Sleep(1000);
		}

		// X
		if ((lHand.x < lShoulder.x-0.2) && (lHand.y < lShoulder.y-0.2) && (rHand.x < lShoulder.x) && (rHand.y > lShoulder.y))
		{
				ReturnKeys(0x58);
				#ifdef _DEBUG
				std::cout << "Printing the letter X" << std::endl;
				#endif
				Sleep(1000);
		}

		// Y
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y-0.2) && (lHand.y < lShoulder.y+0.2) && (rHand.x > rShoulder.x) && (rHand.y > rShoulder.y+0.2))
		{
				ReturnKeys(0x59);
				#ifdef _DEBUG
				std::cout << "Printing the letter Y" << std::endl;
				#endif
				Sleep(1000);
		}

		// Z
		if ((lHand.x < lShoulder.x) && (lHand.y > lShoulder.y-0.2) && (lHand.y < lShoulder.y+0.2) && (rHand.x < lShoulder.x) && (rHand.y < lShoulder.y))
		{
				ReturnKeys(0x5A);
				#ifdef _DEBUG
				std::cout << "Printing the letter Z" << std::endl;
				#endif
				Sleep(1000);
		}

		// Space
		if ((m_leftElbow.x > lShoulder.x) && (m_rightElbow.x < rShoulder.x))
		{
				ReturnKeys(0x20);
				#ifdef _DEBUG
				std::cout << "Printing SPACE" << std::endl;
				#endif
				Sleep(1000);
		}



	////////////////////////////////////////////////////////// 
	/*			
				*** Slide Control ***
	*/
	///////////////////////////////////////////////////////////

	/*
	
	if ((lHand.x >= lShoulder.x) && (rHand.x <= rShoulder.x))  //back a page
		{
			ReturnKeys(VK_PRIOR);
			#ifdef _DEBUG
			std::cout << "Back"<<std::endl;
			#endif
			Sleep(1000);	
		}

	else if ((lHand.x <= lShoulder.x+0.2) && (rHand.x >= rShoulder.x+0.2))  // Forward a page
		{
			ReturnKeys(VK_NEXT);
			#ifdef _DEBUG
			std::cout << "Forward"<<std::endl;
			#endif
			Sleep(1000);	
		}
	*/

	// Print to debug console if there is no action

	else
	{
		#ifdef _DEBUG
		std::cout << "Nothing" << std::endl;
		#endif
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

#ifdef _DEBUG
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
#endif

bool Tracker::m_Timer(int length)
{
	clock_t timer = clock();
	while ((((double)(clock() - timer)) / (double) CLOCKS_PER_SEC) != length);
	return true;
}


void Tracker::Go() 
{
	#ifdef _DEBUG
	Debugwindow();
	#endif
	if (m_StartKinect()==E_FAIL)
	{
		#ifdef _DEBUG
		std::cin.get();
		#endif
		return;
	};
	
	for(;;) m_Update(); 
}       

#ifdef _DEBUG

void Tracker::m_Mapinput()
{
	std::ofstream file;
	file.open ("map.txt", std::ios::out | std::ios::app);
	if (file.is_open())
	{
		file <<"Item " << "x " << " y " << "z" << std::endl;
		file << "Left Hand " << m_leftHand.x << " " << m_leftHand.y << " "  << m_leftHand.z << std::endl;
		file << "Right Hand " << m_rightHand.x << " " << m_rightHand.y << " "  << m_rightHand.z << std::endl << std::endl;
		file.close();
		Sleep(1000);
	}
}

#endif

void Tracker::Mouse(long dx, long dy)
{ 
	INPUT Input; 
	ZeroMemory(&Input, sizeof(Input)); 
	Input.type = INPUT_MOUSE; 
	Input.mi.dwFlags = MOUSEEVENTF_MOVE;
	Input.mi.mouseData = 0;
	Input.mi.dx = dx;
	Input.mi.dy = dy;
	SendInput(1, &Input, sizeof(INPUT));
	return;	
}

void Tracker::Left_Click()
{ 
	INPUT Input; 
	ZeroMemory(&Input, sizeof(Input)); 
	Input.type = INPUT_MOUSE; 
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
	ZeroMemory(&Input, sizeof(Input)); 
	Input.type = INPUT_MOUSE; 
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));
	return;	
}

void Tracker::m_MapBones(const NUI_SKELETON_DATA &, NUI_SKELETON_POSITION_INDEX, NUI_SKELETON_POSITION_INDEX)
{
	
}