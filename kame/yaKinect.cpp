#include "yaKinect.h"

// Constructor
yaKinect::yaKinect()
{
	// Initialize
	initialize();
}

// Destructor
yaKinect::~yaKinect()
{
	// Finalize
	finalize();
}

// Processing
void yaKinect::run()
{
	// Main Loop
	while (true) {
		// Update Data
		update();

		// Draw Data
		draw();

		// Show Data
		show();

		// Key Check
		const int key = cv::waitKey(10);
		if (key == VK_ESCAPE) {
			break;
		}
	}
}

// Initialize
void yaKinect::initialize()
{
	cv::setUseOptimized(true);

	// Initialize Sensor
	initializeSensor();

	// Initialize Color
	initializeColor();

	// Initialize Depth
	initializeDepth();

	// Initialize Point Cloud
	initializePointCloud();

	// Initialize Body
	initializeBody();

	// Wait a Few Seconds until begins to Retrieve Data from Sensor ( about 2000-[ms] )
	Sleep(2000);
}

// Initialize Sensor
void yaKinect::initializeSensor()
{
	// Open Sensor
	ERROR_CHECK(GetDefaultKinectSensor(&kinect));

	ERROR_CHECK(kinect->Open());

	// Check Open
	BOOLEAN isOpen = FALSE;
	ERROR_CHECK(kinect->get_IsOpen(&isOpen));
	if (!isOpen) {
		throw std::runtime_error("failed IKinectSensor::get_IsOpen( &isOpen )");
	}

	// Retrieve Coordinate Mapper
	ERROR_CHECK(kinect->get_CoordinateMapper(&coordinateMapper));
}

// Initialize Color
void yaKinect::initializeColor()
{
	// Open Color Reader
	IColorFrameSource* colorFrameSource = nullptr;
	ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
	ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

	// Retrieve Color Description
	IFrameDescription* colorFrameDescription = nullptr;
	ERROR_CHECK(colorFrameSource->CreateFrameDescription(ColorImageFormat::ColorImageFormat_Bgra, &colorFrameDescription));
	ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth)); // 1920
	ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight)); // 1080
	ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel)); // 4

	// Allocation Color Buffer
	colorBuffer.resize(colorWidth * colorHeight * colorBytesPerPixel);
}

// Initialize Depth
void yaKinect::initializeDepth()
{
	// Open Depth Reader
	IDepthFrameSource* depthFrameSource = nullptr;
	ERROR_CHECK(kinect->get_DepthFrameSource(&depthFrameSource));
	ERROR_CHECK(depthFrameSource->OpenReader(&depthFrameReader));

	// Retrieve Depth Description
	IFrameDescription* depthFrameDescription = nullptr;
	ERROR_CHECK(depthFrameSource->get_FrameDescription(&depthFrameDescription));
	ERROR_CHECK(depthFrameDescription->get_Width(&depthWidth)); // 512
	ERROR_CHECK(depthFrameDescription->get_Height(&depthHeight)); // 424
	ERROR_CHECK(depthFrameDescription->get_BytesPerPixel(&depthBytesPerPixel)); // 2

	// Allocation Depth Buffer
	depthBuffer.resize(depthWidth * depthHeight);
}

// Initialize Point Cloud
void yaKinect::initializePointCloud()
{
	// Create Window
	viewer = cv::viz::Viz3d("Point Cloud");

	// Register Keyboard Callback Function
	viewer.registerKeyboardCallback(&keyboardCallback, this);

	// Show Coordinate System
	viewer.showWidget("CoordinateSystem", cv::viz::WCameraPosition::WCameraPosition(0.5));
}

// Keyboard Callback Function
void yaKinect::keyboardCallback(const cv::viz::KeyboardEvent& event, void* cookie)
{
	// Exit Viewer when Pressed ESC key
	if (event.code == VK_ESCAPE && event.action == cv::viz::KeyboardEvent::Action::KEY_DOWN) {
		// Retrieve Viewer
		cv::viz::Viz3d viewer = static_cast<yaKinect*>(cookie)->viewer;

		// Close Viewer
		viewer.close();
	}
	// Save Point Cloud to File when Pressed 's' key
	else if (event.code == 's' && event.action == cv::viz::KeyboardEvent::Action::KEY_DOWN) {
		// Retrieve Point Cloud and Color
		cv::Mat cloud = static_cast<yaKinect*>(cookie)->cloudMat;
		cv::Mat color = static_cast<yaKinect*>(cookie)->color2CloudMat;

		// Generate File Name
		static int i = 0;
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(3) << i++;
		std::string file = oss.str() + ".ply";

		// Write Point Cloud to File
		cv::viz::writeCloud(file, cloud, color, cv::noArray(), false);
	}
};

// Initialize Body
void yaKinect::initializeBody()
{
	// Open Body Reader
	IBodyFrameSource* bodyFrameSource = nullptr;
	ERROR_CHECK(kinect->get_BodyFrameSource(&bodyFrameSource));
	ERROR_CHECK(bodyFrameSource->OpenReader(&bodyFrameReader));

	// Initialize Body Buffer
	for (int i = 0; i < BODY_COUNT; i++) {
		SafeRelease(bodies[i]);
	}

	// Color Table for Visualization
	colors[0] = cv::Vec3b(255, 0, 0); // Blue
	colors[1] = cv::Vec3b(0, 255, 0); // Green
	colors[2] = cv::Vec3b(0, 0, 255); // Red
	colors[3] = cv::Vec3b(255, 255, 0); // Cyan
	colors[4] = cv::Vec3b(255, 0, 255); // Magenta
	colors[5] = cv::Vec3b(0, 255, 255); // Yellow
}

// Finalize
void yaKinect::finalize()
{
	cv::viz::unregisterAllWindows();
	cv::destroyAllWindows();

	// Release Body Buffer
	for (int i = 0; i < BODY_COUNT; i++) {
		SafeRelease(bodies[i]);
	}

	// Close Sensor
	if (kinect != nullptr) {
		kinect->Close();
	}
}

// Update Data
void yaKinect::update()
{
	// Update Color
	updateColor();

	// Update Depth
	updateDepth();

	// Update Body
	updateBody();
}

// Update Color
void yaKinect::updateColor()
{
	// Retrieve color Frame
	SafeRelease(colorFrame);

	const HRESULT ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
	if (FAILED(ret)) {
		//printf("err Retrieve Color Frame\n");
		return;
	}

	// Convert Format ( YUY2 -> BGRA )
	ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(static_cast<UINT>(colorBuffer.size()), &colorBuffer[0], ColorImageFormat::ColorImageFormat_Bgra));
}

// Update Depth
void yaKinect::updateDepth()
{
	// Retrieve Depth Frame
	SafeRelease(depthFrame);

	const HRESULT ret = depthFrameReader->AcquireLatestFrame(&depthFrame);
	if (FAILED(ret)) {
		return;
	}

	// Retrieve Depth Data
	ERROR_CHECK(depthFrame->CopyFrameDataToArray(static_cast<UINT>(depthBuffer.size()), &depthBuffer[0]));
}

// Update Body
void yaKinect::updateBody()
{
	// Retrieve Body Frame
	SafeRelease(bodyFrame);

	const HRESULT ret = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
	if (FAILED(ret)) {
		//printf("err Retrieve Body Frame\n");
		return;
	}
	/*else {
		printf("ok Retrieve Body Frame\n");
	}*/

	// Release Previous Bodies
	for (int i = 0; i < BODY_COUNT; i++) {
		SafeRelease(bodies[i]);
	}

	// Retrieve Body Data
	ERROR_CHECK(bodyFrame->GetAndRefreshBodyData(static_cast<UINT>(bodies.size()), &bodies[0]));
}

// Draw Data
void yaKinect::draw()
{
	// Draw Color
	drawColor();

	// Draw Depth
	drawDepth();

	// Draw Color
	drawColor2Cloud();

	// Draw Point Cloud
	drawPointCloud();

	// Draw Body
	drawBody();
}

// Draw Color
void yaKinect::drawColor()
{
	// Create cv::Mat from Color Buffer
	colorMat = cv::Mat(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);
}

// Draw Depth
void yaKinect::drawDepth()
{
	// Create cv::Mat from Depth Buffer
	depthMat = cv::Mat(depthHeight, depthWidth, CV_16UC1, &depthBuffer[0]);
}

// Draw Color
void yaKinect::drawColor2Cloud()
{
	// Retrieve Mapped Coordinates
	std::vector<ColorSpacePoint> colorSpacePoints(depthWidth * depthHeight);
	ERROR_CHECK(coordinateMapper->MapDepthFrameToColorSpace(depthBuffer.size(), &depthBuffer[0], colorSpacePoints.size(), &colorSpacePoints[0]));

	// Mapping Color to Depth Resolution
	std::vector<BYTE> buffer(depthWidth * depthHeight * colorBytesPerPixel);

	for (int depthY = 0; depthY < depthHeight; depthY++) {
		unsigned int depthOffset = depthY * depthWidth;
		for (int depthX = 0; depthX < depthWidth; depthX++) {
			unsigned int depthIndex = depthOffset + depthX;
			int colorX = static_cast<int>(colorSpacePoints[depthIndex].X + 0.5f);
			int colorY = static_cast<int>(colorSpacePoints[depthIndex].Y + 0.5f);
			if ((0 <= colorX) && (colorX < colorWidth) && (0 <= colorY) && (colorY < colorHeight)) {
				unsigned int colorIndex = (colorY * colorWidth + colorX) * colorBytesPerPixel;
				depthIndex = depthIndex * colorBytesPerPixel;
				buffer[depthIndex + 0] = colorBuffer[colorIndex + 0];
				buffer[depthIndex + 1] = colorBuffer[colorIndex + 1];
				buffer[depthIndex + 2] = colorBuffer[colorIndex + 2];
				buffer[depthIndex + 3] = colorBuffer[colorIndex + 3];
			}
		}
	}

	// Create cv::Mat from Coordinate Buffer
	color2CloudMat = cv::Mat(depthHeight, depthWidth, CV_8UC4, &buffer[0]).clone();
}

// Draw Point Cloud
void yaKinect::drawPointCloud()
{
	// Retrieve Mapped Coordinates
	std::vector<CameraSpacePoint> cameraSpacePoints(depthWidth * depthHeight);
	ERROR_CHECK(coordinateMapper->MapDepthFrameToCameraSpace(depthBuffer.size(), &depthBuffer[0], cameraSpacePoints.size(), &cameraSpacePoints[0]));

	// Mapping Color to Depth Resolution
	std::vector<cv::Vec3f> buffer(depthWidth * depthHeight, cv::Vec3f::all(std::numeric_limits<float>::quiet_NaN()));

	for (int depthY = 0; depthY < depthHeight; depthY++) {
		unsigned int depthOffset = depthY * depthWidth;
		for (int depthX = 0; depthX < depthWidth; depthX++) {
			unsigned int depthIndex = depthOffset + depthX;
			UINT16 depth = depthBuffer[depthIndex];
			if (500 <= depth && depth < 8000) {
				CameraSpacePoint cameraSpacePoint = cameraSpacePoints[depthIndex];
				buffer[depthIndex] = cv::Vec3f(cameraSpacePoint.X, cameraSpacePoint.Y, cameraSpacePoint.Z);
			}
		}
	}

	// Create cv::Mat from Coordinate Buffer
	cloudMat = cv::Mat(depthHeight, depthWidth, CV_32FC3, &buffer[0]).clone();
}

// Draw Body
void yaKinect::drawBody()
{
	handPoint.X = -1;
	handPoint.Y = -1;

	// Draw Body Data to Color Data
	for (int count = 0; count < BODY_COUNT; count++) {

		body = bodies[count];
		if (body == NULL) {
			continue;
		}

		// Check Body Tracked
		BOOLEAN tracked = FALSE;
		ERROR_CHECK(body->get_IsTracked(&tracked));
		if (!tracked) {
			continue;
		}

		//printf("body count %d\n", count);

		// Retrieve Joints
		std::array<Joint, JointType::JointType_Count> joints;
		ERROR_CHECK(body->GetJoints(static_cast<UINT>(joints.size()), &joints[0]));

		for (int i = 0; i < JointType::JointType_Count; i++) {

			// Check Joint Tracked
			if (joints[i].TrackingState == TrackingState::TrackingState_NotTracked) {
				continue;
			}

			// Draw Joint Position
			drawEllipse(colorMat, joints[i], 5, colors[count]);

			// Draw Left Hand State
			if (joints[i].JointType == JointType::JointType_HandLeft) {
				HandState handState;
				TrackingConfidence handConfidence;
				ERROR_CHECK(body->get_HandLeftState(&handState));
				ERROR_CHECK(body->get_HandLeftConfidence(&handConfidence));

				drawHandState(colorMat, joints[i], handState, handConfidence);

			}

			// Draw Right Hand State
			if (joints[i].JointType == JointType::JointType_HandRight) {
				HandState handState;
				TrackingConfidence handConfidence;
				ERROR_CHECK(body->get_HandRightState(&handState));
				ERROR_CHECK(body->get_HandRightConfidence(&handConfidence));

				drawHandState(colorMat, joints[i], handState, handConfidence);

				// Convert Coordinate System and Draw Joint
				ColorSpacePoint colorSpacePoint;

				ERROR_CHECK(coordinateMapper->MapCameraPointToColorSpace(joints[i].Position, &colorSpacePoint));
				handPoint.X = static_cast<int>(colorSpacePoint.X + 0.5f);
				handPoint.Y = static_cast<int>(colorSpacePoint.Y + 0.5f);
			}
		}
	}
}

// Draw Ellipse
inline void yaKinect::drawEllipse(cv::Mat& image, const Joint& joint, const int radius, const cv::Vec3b& color, const int thickness)
{
	if (image.empty()) {
		return;
	}

	// Convert Coordinate System and Draw Joint
	ColorSpacePoint colorSpacePoint;

	ERROR_CHECK(coordinateMapper->MapCameraPointToColorSpace(joint.Position, &colorSpacePoint));
	const int x = static_cast<int>(colorSpacePoint.X + 0.5f);
	const int y = static_cast<int>(colorSpacePoint.Y + 0.5f);
	if ((0 <= x) && (x < image.cols) && (0 <= y) && (y < image.rows)) {
		cv::circle(image, cv::Point(x, y), radius, static_cast<cv::Scalar>(color), thickness, cv::LINE_AA);
	}
}

// Draw Hand State
inline void yaKinect::drawHandState(cv::Mat& image, const Joint& joint, HandState handState, TrackingConfidence handConfidence)
{
	if (image.empty()) {
		return;
	}

	// Check Tracking Confidence
	if (handConfidence != TrackingConfidence::TrackingConfidence_High) {
		return;
	}

	// Draw Hand State 
	const int radius = 75;
	const cv::Vec3b blue = cv::Vec3b(128, 0, 0), green = cv::Vec3b(0, 128, 0), red = cv::Vec3b(0, 0, 128);
	switch (handState) {
		// Open
	case HandState::HandState_Open:
		drawEllipse(image, joint, radius, green, 5);
		break;
		// Close
	case HandState::HandState_Closed:
		drawEllipse(image, joint, radius, red, 5);
		break;
		// Lasso
	case HandState::HandState_Lasso:
		drawEllipse(image, joint, radius, blue, 5);
		break;
	default:
		break;
	}
}

// Show Data
void yaKinect::show()
{
	// Show Depth
	showDepth();

	// Show Point Cloud
	//showPointCloud();

	// Show Body
	showBody();
}

// Show Depth
void yaKinect::showDepth()
{
	if (depthMat.empty()) {
		return;
	}

	// Scaling ( 0-8000 -> 255-0 )
	cv::Mat scaleMat;
	depthMat.convertTo(scaleMat, CV_8U, -255.0 / 4000.0, 255.0);
	cv::applyColorMap( scaleMat, scaleMat, cv::COLORMAP_JET);

	// Show Image
	cv::imshow("Depth", scaleMat);
}

// Show Point Cloud
inline void yaKinect::showPointCloud()
{
	if (color2CloudMat.empty()) {
		return;
	}

	if (cloudMat.empty()) {
		return;
	}

	// Create Point Cloud
	cv::viz::WCloud cloud(cloudMat, color2CloudMat);

	// Show Point Cloud
	viewer.showWidget("Cloud", cloud);
	viewer.spinOnce();
}

// Show Body
inline void yaKinect::showBody()
{
	if (colorMat.empty()) {
		return;
	}

	// Resize Image
	cv::Mat resizeMat;
	const double scale = 0.5;
	cv::resize(colorMat, resizeMat, cv::Size(), scale, scale);

	// Show Image
	cv::imshow("Body", resizeMat);
}

cv::Mat yaKinect::getImg()
{
	return(colorMat);
}

System::Drawing::Point yaKinect::getHand()
{
	return(handPoint);
}