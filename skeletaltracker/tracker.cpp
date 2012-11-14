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
	  
	  mapbones(skeleton, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
      mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
      mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);

      // Render Left Arm
      mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
      mapbones(skeleton, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
      mapbones(skeleton, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);

      // Render Right Arm
      mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
      mapbones(skeleton, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
      mapbones(skeleton, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);
	 
	  // Render Left leg
	  mapbones(skeleton, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_KNEE_LEFT);
	  mapbones(skeleton, NUI_SKELETON_POSITION_KNEE_LEFT, NUI_SKELETON_POSITION_ANKLE_LEFT);
	  mapbones(skeleton, NUI_SKELETON_POSITION_ANKLE_LEFT, NUI_SKELETON_POSITION_FOOT_LEFT);
	  
	  // Render Right leg
	  mapbones(skeleton, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_KNEE_RIGHT);
	  mapbones(skeleton, NUI_SKELETON_POSITION_KNEE_RIGHT, NUI_SKELETON_POSITION_ANKLE_RIGHT);
	  mapbones(skeleton, NUI_SKELETON_POSITION_ANKLE_RIGHT, NUI_SKELETON_POSITION_FOOT_RIGHT);

	  mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_SHOULDER_CENTER);
	  mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_SHOULDER_CENTER);
	  mapbones(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SPINE);
	  mapbones(skeleton, NUI_SKELETON_POSITION_SPINE, NUI_SKELETON_POSITION_HIP_CENTER);
	  mapbones(skeleton, NUI_SKELETON_POSITION_HIP_LEFT, NUI_SKELETON_POSITION_HIP_CENTER);
	  mapbones(skeleton, NUI_SKELETON_POSITION_HIP_RIGHT, NUI_SKELETON_POSITION_HIP_CENTER);
	  
}

 void tracker::skeletonposition(Vector4 position){

 }
 void tracker::mapbones(const NUI_SKELETON_DATA &skeleton, NUI_SKELETON_POSITION_INDEX jointFrom, NUI_SKELETON_POSITION_INDEX jointTo)
      {
        NUI_SKELETON_POSITION_TRACKING_STATE jointFromState = skeleton.eSkeletonPositionTrackingState[jointFrom];
        NUI_SKELETON_POSITION_TRACKING_STATE jointToState = skeleton.eSkeletonPositionTrackingState[jointTo];

        if (jointFromState == NUI_SKELETON_POSITION_NOT_TRACKED || jointToState == NUI_SKELETON_POSITION_NOT_TRACKED)
        {
          return; // nothing to draw, one of the joints is not tracked
        }

        const Vector4& jointFromPosition = skeleton.SkeletonPositions[jointFrom];
        const Vector4& jointToPosition = skeleton.SkeletonPositions[jointTo];

        // Don't draw if both points are inferred
        if (jointFromState == NUI_SKELETON_POSITION_INFERRED || jointToState == NUI_SKELETON_POSITION_INFERRED)
        {
			//DrawNonTrackedBoneLine(jointFromPosition, jointToPosition); // Draw thin lines if either one of the joints is inferred
        }

        // We assume all drawn bones are inferred unless BOTH joints are tracked
        if (jointFromState == NUI_SKELETON_POSITION_TRACKED && jointToState == NUI_SKELETON_POSITION_TRACKED)
        {
          //DrawTrackedBoneLine(jointFromPosition, jointToPosition); // Draw bold lines if the joints are both tracked
        }
      }
 
 void tracker::go(){
		startkinect();
 }       
 
 void tracker::noise(LPCSTR a){
		PlaySound(TEXT(a), NULL, SND_ASYNC + SND_NOSTOP);
 }