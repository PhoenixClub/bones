#include "tracker.h"

HRESULT tracker::startkinect(){
    INuiSensor * pNuiSensor = (INuiSensor*)NULL;
	m_pNuiSensor = (INuiSensor*)NULL;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))
    {
        return hr;
    }

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
        {
            continue;
        }

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if (NULL != m_pNuiSensor )
    {
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when skeleton data is available
            m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
			std::cout<<"kinect found";
            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0); 
        }
    }

    if (NULL == m_pNuiSensor || FAILED(hr))
    {
		std::cout<<"no kinect found";
        //SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}

void tracker::update()
	{
      // Wait for 0ms, just quickly test if it is time to process a skeleton
      if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
      {
        NUI_SKELETON_FRAME skeletonFrame = {0};

        // Get the skeleton frame that is ready
        if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame)))
        {
          // Process the skeleton frame
          processframe(&skeletonFrame);
        }
      }
    }

void tracker::processframe(NUI_SKELETON_FRAME* pSkeletonFrame){
	 for (int i = 0; i < NUI_SKELETON_COUNT; i++)
    {
        const NUI_SKELETON_DATA & skeleton = pSkeletonFrame->SkeletonData[i];
 
        switch (skeleton.eTrackingState)
        {
        case NUI_SKELETON_TRACKED:
            skeletontracked(skeleton);
            break;
 
        case NUI_SKELETON_POSITION_ONLY:
            skeletonposition(skeleton.Position);
            break;
        }
	}
}
 void tracker::skeletontracked(NUI_SKELETON_DATA skeleton){
	
}

 void tracker::skeletonposition(Vector4 position){

 }

 void tracker::go(){
		startkinect();
 }       
 