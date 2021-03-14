#define BirdWatcher_VERSION_MAJOR @BirdWatcher_VERSION_MAJOR@
#define BirdWatcher_VERSION_MINOR @BirdWatcher_VERSION_MINOR@

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "ChunkDataInference.h"

std::string GetDateTime();
int SetChunkEnable(Spinnaker::GenApi::INodeMap& nodeMap, const Spinnaker::GenICam::gcstring& entryName, const bool enable);
int ConfigureInference(Spinnaker::GenApi::INodeMap& nodeMap, bool isEnabled);
int ConfigureTrigger(Spinnaker::GenApi::INodeMap& nodeMap);
int DisableTrigger(Spinnaker::GenApi::INodeMap& nodeMap);
int ConfigureChunkData(Spinnaker::GenApi::INodeMap& nodeMap);
int DisableChunkData(Spinnaker::GenApi::INodeMap& nodeMap);
int PrintDeviceInfo(Spinnaker::GenApi::INodeMap& nodeMap);
int AcquireImages(Spinnaker::CameraPtr pCam, Spinnaker::GenApi::INodeMap& nodeMap, Spinnaker::GenApi::INodeMap& nodeMapTLDevice);
int RunSingleCamera(Spinnaker::CameraPtr pCam);
int main(int argc, const char * argv[]);