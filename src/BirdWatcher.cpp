//
//  main.cpp
//  Bird Watcher
//
//  Created by Nicolas Martin on 2021-01-03.
//
// ffmpeg -f image2 -pattern_type glob -framerate 30 -i 'Trigger-20531771-*.jpg' -s 1440x1080 foo.avi
//

#include "BirdWatcher.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

const char* arrayLabelClassification[] = {"no bird", "bird"};
const std::vector<std::string> labelClassification(arrayLabelClassification, end(arrayLabelClassification));

std::string GetDateTime(){
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string s(30, '\0');
    std::strftime(&s[0], s.size(), "%Y%m%d_%H%M%S", std::localtime(&now));
    return s;
}

// This function enables or disables the given chunk data type based on
// the specified entry name.
int SetChunkEnable(INodeMap& nodeMap, const gcstring& entryName, const bool enable)
{
    int result = 0;
    CEnumerationPtr ptrChunkSelector = nodeMap.GetNode("ChunkSelector");

    const CEnumEntryPtr ptrEntry = ptrChunkSelector->GetEntryByName(entryName);
    if (!IsReadable(ptrEntry))
    {
        return -1;
    }

    ptrChunkSelector->SetIntValue(ptrEntry->GetValue());

    // Enable the boolean, thus enabling the corresponding chunk data
    cout << entryName << " ";
    CBooleanPtr ptrChunkEnable = nodeMap.GetNode("ChunkEnable");
    if (!IsAvailable(ptrChunkEnable))
    {
        cout << "not available" << endl;
        return -1;
    }
    if (enable)
    {
        if (ptrChunkEnable->GetValue())
        {
            cout << "enabled" << endl;
        }
        else if (IsWritable(ptrChunkEnable))
        {
            ptrChunkEnable->SetValue(true);
            cout << "enabled" << endl;
        }
        else
        {
            cout << "not writable" << endl;
            result = -1;
        }
    }
    else
    {
        if (!ptrChunkEnable->GetValue())
        {
            cout << "disabled" << endl;
        }
        else if (IsWritable(ptrChunkEnable))
        {
            ptrChunkEnable->SetValue(false);
            cout << "disabled" << endl;
        }
        else
        {
            cout << "not writable" << endl;
            result = -1;
        }
    }

    return result;
}

// This function enables/disables inference on the camera and configures the inference network type
int ConfigureInference(INodeMap& nodeMap, bool isEnabled)
{
    int result = 0;

    if (isEnabled)
    {
        cout << endl << endl << "*** CONFIGURING INFERENCE (CLASSIFICATION) ***" << endl << endl;
    }
    else
    {
        cout << endl << endl << "*** DISABLING INFERENCE ***" << endl << endl;
    }

    try
    {
        if (isEnabled)
        {
            // Set Network Type to Classification
            CEnumerationPtr ptrInferenceNetworkTypeSelector = nodeMap.GetNode("InferenceNetworkTypeSelector");
            if (!IsWritable(ptrInferenceNetworkTypeSelector))
            {
                cout << "Unable to query InferenceNetworkTypeSelector. Aborting..." << endl;
                return -1;
            }

            // Retrieve entry node from enumeration node
            CEnumEntryPtr ptrInferenceNetworkType = ptrInferenceNetworkTypeSelector->GetEntryByName("Classification");
            if (!IsReadable(ptrInferenceNetworkType))
            {
                cout << "Unable to set inference network type to Classification (entry retrieval). Aborting..." << endl << endl;
                return -1;
            }

            ptrInferenceNetworkTypeSelector->SetIntValue(
                static_cast<int64_t>(ptrInferenceNetworkType->GetNumericValue()));

            cout << "Inference network type set to Classification..." << endl;
        }

        // Enable/Disable inference
        cout << (isEnabled ? "Enabling" : "Disabling") << " inference..." << endl;
        CBooleanPtr ptrInferenceEnable = nodeMap.GetNode("InferenceEnable");
        if (!IsWritable(ptrInferenceEnable))
        {
            cout << "Unable to enable inference. Aborting..." << endl;
            return -1;
        }

        ptrInferenceEnable->SetValue(isEnabled);
        cout << "Inference " << (isEnabled ? "enabled..." : "disabled...") << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Unexpected exception : " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function configures camera to run in "inference sync" trigger mode.
int ConfigureTrigger(INodeMap& nodeMap)
{
    cout << endl << endl << "*** CONFIGURING TRIGGER ***" << endl << endl;
    int result = 0;

    try
    {
        // Configure TriggerSelector
        CEnumerationPtr ptrTriggerSelector = nodeMap.GetNode("TriggerSelector");
        if (!IsWritable(ptrTriggerSelector))
        {
            cout << "Unable to configure TriggerSelector. Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrFrameStart = ptrTriggerSelector->GetEntryByName("FrameStart");
        if (!IsReadable(ptrFrameStart))
        {
            cout << "Unable to query TriggerSelector FrameStart. Aborting..." << endl;
            return -1;
        }

        cout << "Configure TriggerSelector to " << ptrFrameStart->GetSymbolic() << endl;
        ptrTriggerSelector->SetIntValue(static_cast<int64_t>(ptrFrameStart->GetNumericValue()));

        // Configure TriggerSource
        CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");
        if (!IsWritable(ptrTriggerSource))
        {
            cout << "Unable to configure TriggerSource. Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrInferenceReady = ptrTriggerSource->GetEntryByName("InferenceReady");
        if (!IsReadable(ptrInferenceReady))
        {
            cout << "Unable to query TriggerSource InferenceReady. Aborting..." << endl;
            return -1;
        }

        cout << "Configure TriggerSource to " << ptrInferenceReady->GetSymbolic() << endl;
        ptrTriggerSource->SetIntValue(static_cast<int64_t>(ptrInferenceReady->GetNumericValue()));

        // Configure TriggerMode
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsWritable(ptrTriggerMode))
        {
            cout << "Unable to configure TriggerMode. Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerOn = ptrTriggerMode->GetEntryByName("On");
        if (!IsReadable(ptrTriggerOn))
        {
            cout << "Unable to query TriggerMode On. Aborting..." << endl;
            return -1;
        }

        cout << "Configure TriggerMode to " << ptrTriggerOn->GetSymbolic() << endl;
        ptrTriggerMode->SetIntValue(static_cast<int64_t>(ptrTriggerOn->GetNumericValue()));
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Unexpected exception : " << e.what();
        result = -1;
    }

    return result;
}

// This function disables trigger mode on the camera.
int DisableTrigger(INodeMap& nodeMap)
{
    cout << endl << endl << "*** DISABLING TRIGGER ***" << endl << endl;
    int result = 0;
    try
    {
        // Configure TriggerMode
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        if (!IsWritable(ptrTriggerMode))
        {
            cout << "Unable to configure TriggerMode. Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrTriggerOff = ptrTriggerMode->GetEntryByName("Off");
        if (!IsReadable(ptrTriggerOff))
        {
            cout << "Unable to query TriggerMode Off. Aborting..." << endl;
            return -1;
        }

        cout << "Configure TriggerMode to " << ptrTriggerOff->GetSymbolic() << endl;
        ptrTriggerMode->SetIntValue(static_cast<int64_t>(ptrTriggerOff->GetNumericValue()));
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Unexpected exception : " << e.what();
        result = -1;
    }

    return result;
}

// This function configures the camera to add inference chunk data to each image.
// When chunk data is turned on, the data is made available in both the nodemap
// and each image.
int ConfigureChunkData(INodeMap& nodeMap)
{
    int result = 0;

    cout << endl << endl << "*** CONFIGURING CHUNK DATA ***" << endl << endl;

    try
    {
        //
        // Activate chunk mode
        //
        // *** NOTES ***
        // Once enabled, chunk data will be available at the end of the payload
        // of every image captured until it is disabled. Chunk data can also be
        // retrieved from the nodemap.
        //
        CBooleanPtr ptrChunkModeActive = nodeMap.GetNode("ChunkModeActive");
        if (!IsWritable(ptrChunkModeActive))
        {
            cout << "Unable to activate chunk mode. Aborting..." << endl;
            return -1;
        }
        ptrChunkModeActive->SetValue(true);
        cout << "Chunk mode activated..." << endl;

        // Enable inference related chunks in chunk data

        // Retrieve the chunk data selector node
        const CEnumerationPtr ptrChunkSelector = nodeMap.GetNode("ChunkSelector");
        if (!IsReadable(ptrChunkSelector))
        {
            cout << "Unable to retrieve chunk selector (enum retrieval). Aborting..." << endl;
            return -1;
        }

        // Enable chunk data inference Frame Id
        result = SetChunkEnable(nodeMap, "InferenceFrameId", true);
        if (result == -1)
        {
            cout << "Unable to enable Inference Frame Id chunk data. Aborting..." << endl;
            return result;
        }

        // Classification network type
        // Enable chunk data inference result
        result = SetChunkEnable(nodeMap, "InferenceResult", true);
        if (result == -1)
        {
            cout << "Unable to enable Inference Result chunk data. Aborting..." << endl;
            return result;
        }

        // Enable chunk data inference confidence
        result = SetChunkEnable(nodeMap, "InferenceConfidence", true);
        if (result == -1)
        {
            cout << "Unable to enable Inference Confidence chunk data. Aborting..." << endl;
            return result;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function disables each type of chunk data before disabling chunk data mode.
int DisableChunkData(INodeMap& nodeMap)
{
    cout << endl << endl << "*** DISABLING CHUNK DATA ***" << endl << endl;

    int result = 0;
    try
    {
        // Retrieve the selector node
        const CEnumerationPtr ptrChunkSelector = nodeMap.GetNode("ChunkSelector");

        if (!IsReadable(ptrChunkSelector))
        {
            cout << "Unable to retrieve chunk selector. Aborting..." << endl;
            return -1;
        }

        result = SetChunkEnable(nodeMap, "InferenceFrameId", false);
        if (result == -1)
        {
            cout << "Unable to disable Inference Frame Id chunk data. Aborting..." << endl;
            return result;
        }

        // Classification network type
        // Disable chunk data inference result
        result = SetChunkEnable(nodeMap, "InferenceResult", false);
        if (result == -1)
        {
            cout << "Unable to disable Inference Result chunk data. Aborting..." << endl;
            return result;
        }

        // Disable chunk data inference confidence
        result = SetChunkEnable(nodeMap, "InferenceConfidence", false);
        if (result == -1)
        {
            cout << "Unable to disable Inference Confidence chunk data. Aborting..." << endl;
            return result;
        }

        // Deactivate ChunkMode
        CBooleanPtr ptrChunkModeActive = nodeMap.GetNode("ChunkModeActive");
        if (!IsWritable(ptrChunkModeActive))
        {
            cout << "Unable to deactivate chunk mode. Aborting..." << endl;
            return -1;
        }
        ptrChunkModeActive->SetValue(false);
        cout << "Chunk mode deactivated..." << endl;

        // Disable Inference
        CBooleanPtr ptrInferenceEnable = nodeMap.GetNode("InferenceEnable");
        if (!IsWritable(ptrInferenceEnable))
        {
            cout << "Unable to disable inference. Aborting..." << endl;
            return -1;
        }
        ptrInferenceEnable->SetValue(false);
        cout << "Inference disabled..." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function prints the device information of the camera from the transport
// layer
int PrintDeviceInfo(INodeMap& nodeMap)
{
    int result = 0;

    cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;

    try
    {
        FeatureList_t features;
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category))
        {
            category->GetFeatures(features);

            FeatureList_t::const_iterator it;
            for (it = features.begin(); it != features.end(); ++it)
            {
                CNodePtr pfeatureNode = *it;
                cout << pfeatureNode->GetName() << " : ";
                CValuePtr pValue = (CValuePtr)pfeatureNode;
                if (IsReadable(pValue))
                {
                    cout << pValue->ToString();
                }
                else
                {
                    cout << "Node not readable";
                }
                cout << endl;
            }
        }
        else
        {
            cout << "Device control information not available." << endl;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

// This function acquires and saves 10 images from a device
int AcquireImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice)
{
    int result = 0;

    cout << endl << "*** IMAGE ACQUISITION ***" << endl << endl;

    try
    {
        // Set acquisition mode to continuous
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        if (!IsWritable(ptrAcquisitionMode))
        {
            cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl;
            return -1;
        }

        CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
        if (!IsReadable(ptrAcquisitionModeContinuous))
        {
            cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl
                 << endl;
            return -1;
        }

        const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

        ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

        cout << "Acquisition mode set to continuous..." << endl;

        // Begin acquiring images
        pCam->BeginAcquisition();

        cout << "Acquiring images..." << endl;

        // Retrieve device serial number for filename
        gcstring deviceSerialNumber("");

        CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
        if (IsReadable(ptrStringSerial))
        {
            deviceSerialNumber = ptrStringSerial->GetValue();
            cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
        }
        cout << endl;

        // Retrieve, convert, and save images
        const int unsigned k_numImages = 1920;
        const unsigned int sleepTime = 10;

        for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
        {
            try
            {
                // Retrieve next received image and ensure image completion
                ImagePtr pResultImage = pCam->GetNextImage(1000);

                if (pResultImage->IsIncomplete())
                {
                    cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl
                         << endl;
                }
                else
                {
                    // Print image information
                    cout << "Grabbed image " << imageCnt << ", width = " << pResultImage->GetWidth()
                         << ", height = " << pResultImage->GetHeight() << endl;

                    const ChunkData chunkData = pResultImage->GetChunkData();

                    const int64_t inferenceFrameID = chunkData.GetInferenceFrameId();
                    cout << "\tInference Frame ID: " << inferenceFrameID << endl;

                    uint64_t inferenceResult = chunkData.GetInferenceResult();
                    cout << "\tInference Result: " << inferenceResult << " ("
                            << (inferenceResult < labelClassification.size() ? labelClassification[inferenceResult] : "N/A") << ")"
                            << endl;

                    double inferenceConfidence = chunkData.GetInferenceConfidence();
                    cout << "\tInference Confidence: " << inferenceConfidence << endl;

                    // Create a unique filename
                    std::string resultName;
                    if (inferenceResult == 0)
                    {
                        
                        resultName = "no_bird";
                    }
                    else if (inferenceResult == 1)
                    {
                        resultName = "bird";
                    }
                    else
                    {
                        resultName = "NA";
                    }

                    std::string filename = "image" + GetDateTime() + "_" + resultName + "_" + std::to_string((int)(inferenceConfidence*100));

                    // Save image
                    pResultImage->Save(filename.c_str(), Spinnaker::ImageFileFormat::JPEG);

                    cout << "Image saved at " << filename << endl;
                }

                // Release image
                pResultImage->Release();

                cout << "Sleeping for " << sleepTime << "s" << endl;
                std::this_thread::sleep_for(std::chrono::seconds(sleepTime));

                cout << endl;
            }
            catch (Spinnaker::Exception& e)
            {
                cout << "Error: " << e.what() << endl;
                result = -1;
            }
        }

        // End acquisition
        pCam->EndAcquisition();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

int RunSingleCamera(CameraPtr pCam)
{
    int result = 0;
    int err = 0;

    try
    {
        // Retrieve TL device nodemap and print device information
        INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();

        result = PrintDeviceInfo(nodeMapTLDevice);

        // Initialize camera
        pCam->Init();

        // Retrieve GenICam nodemap
        INodeMap& nodeMap = pCam->GetNodeMap();

        // Check to make sure camera supports inference
        cout << endl << "Checking camera inference support..." << endl;
        CBooleanPtr ptrInferenceEnable = nodeMap.GetNode("InferenceEnable");
        if (!IsWritable(ptrInferenceEnable))
        {
            cout << "Inference is not supported on this camera. Aborting..." << endl;
            return -1;
        }

        cout << endl << endl << "*** CONFIGURING CAMERA ***" << endl << endl;

        // Configure inference
        err = ConfigureInference(nodeMap, true);
        if (err < 0)
        {
            return err;
        }

        // Configure trigger
        // When enabling inference results via chunk data, the results that accompany a frame
        // will likely not be the frame that inference was run on. In order to guarantee that
        // the chunk inference results always correspond to the frame that they are sent with,
        // the camera needs to be put into the "inference sync" trigger mode.
        // Note: Enabling this setting will limit frame rate so that every frame contains new
        //       inference dataset. To not limit the frame rate, you can enable InferenceFrameID
        //       chunk data to help determine which frame is associated with a particular
        //       inference data.
        err = ConfigureTrigger(nodeMap);
        if (err < 0)
        {
            return err;
        }

        // Configure chunk data
        err = ConfigureChunkData(nodeMap);
        if (err < 0)
        {
            return err;
        }

        // Acquire images
        result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);

        // Disable chunk data
        err = DisableChunkData(nodeMap);
        if (err < 0)
        {
            return err;
        }

        // Disable trigger
        err = DisableTrigger(nodeMap);
        if (err < 0)
        {
            return err;
        }

        // Disable inference
        err = ConfigureInference(nodeMap, false);
        if (err < 0)
        {
            return err;
        }

        // Deinitialize camera
        pCam->DeInit();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error: " << e.what() << endl;
        result = -1;
    }

    return result;
}

int main(int argc, const char * argv[]) {
    FILE* tempFile = fopen("test.txt", "w+");
    if (tempFile == nullptr)
    {
        cout << "Failed to create file in current folder.  Please check "
                "permissions."
             << endl;
        cout << "Press Enter to exit..." << endl;
        getchar();
        return -1;
    }
    fclose(tempFile);
    remove("test.txt");

    int result = 0;

    // Print application build information
    cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;

    SystemPtr system = System::GetInstance();

    // Print out current library version
    const LibraryVersion spinnakerLibraryVersion = system->GetLibraryVersion();
    cout << "Spinnaker library version: " << spinnakerLibraryVersion.major << "." << spinnakerLibraryVersion.minor
         << "." << spinnakerLibraryVersion.type << "." << spinnakerLibraryVersion.build << endl
         << endl;

    CameraList camList = system->GetCameras();

    const unsigned int numCameras = camList.GetSize();

    cout << "Number of cameras detected: " << numCameras << endl << endl;

    if (numCameras == 0)
    {
        camList.Clear();

        system->ReleaseInstance();

        cout << "Not enough cameras!" << endl;
        cout << "Done! Press enter to exit" << endl;
        getchar();

        return -1;
    }

    // Run example on each camera
    for (unsigned int i = 0; i < numCameras; i++)
    {
        cout << endl << "Running example for camera " << i << "..." << endl;

        result = result | RunSingleCamera(camList.GetByIndex(i));

        cout << "Camera " << i << " example complete..." << endl << endl;
    }

    // Clear camera list before releasing system
    camList.Clear();

    // Release system
    system->ReleaseInstance();

    cout << endl << "Done! Press Enter to exit..." << endl;
    getchar();

    return result;
}
