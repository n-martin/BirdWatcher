#define BirdWatcher_VERSION_MAJOR @BirdWatcher_VERSION_MAJOR@
#define BirdWatcher_VERSION_MINOR @BirdWatcher_VERSION_MINOR@

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

std::string GetDateTime();
int ConfigureTrigger(Spinnaker::GenApi::INodeMap& nodeMap);
int ConfigureCamera(Spinnaker::GenApi::INodeMap& nodeMap);
int GrabNextImageByTrigger(Spinnaker::GenApi::INodeMap& nodeMap, Spinnaker::CameraPtr pCam);
int ResetTrigger(Spinnaker::GenApi::INodeMap& nodeMap);
int PrintDeviceInfo(Spinnaker::GenApi::INodeMap& nodeMap);
int AcquireImages(Spinnaker::CameraPtr pCam, Spinnaker::GenApi::INodeMap& nodeMap, Spinnaker::GenApi::INodeMap& nodeMapTLDevice);
int RunSingleCamera(Spinnaker::CameraPtr pCam);
int main(int argc, const char * argv[]);