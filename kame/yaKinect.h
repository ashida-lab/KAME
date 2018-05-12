#pragma once

#pragma comment(lib,"Kinect20.lib")

//#define _CRT_SECURE_NO_WARNINGS

//#include <Windows.h>
#include <Kinect.h>
#include <opencv2/opencv.hpp>

#include <vector>
#include <array>
#include <sstream>
//#include <stdexcept>




// Error Check Macro
#define ERROR_CHECK( ret )                                        \
    if( FAILED( ret ) ){                                          \
        std::stringstream ss;                                     \
        ss << "failed " #ret " " << std::hex << ret << std::endl; \
        throw std::runtime_error( ss.str().c_str() );             \
    }

// Safe Release
template<class T>
inline void SafeRelease(T*& rel)
{
	if (rel != NULL) {
		rel->Release();
		rel = NULL;
	}
}

// C++ Style Line Types For OpenCV 2.x
#if ( CV_MAJOR_VERSION < 3 )
namespace cv {
	enum LineTypes {
		FILLED = -1,
		LINE_4 = 4,
		LINE_8 = 8,
		LINE_AA = 16
	};
}
#endif


class yaKinect
{
public:
	yaKinect();

	// Destructor
	~yaKinect();

	// Processing
	void run();

	// Update Data
	void update();

	// Draw Data
	void draw();

	// Show Data
	void show();

	cv::Mat getImg();

	System::Drawing::Point getHand();

private:
	// Initialize
	void initialize();

	// Initialize Sensor
	void initializeSensor();

	// Initialize Color
	void initializeColor();

	// Initialize Depth
	void initializeDepth();

	// Initialize Point Cloud
	void initializePointCloud();

	// Keyboard Callback Function
	static void keyboardCallback(const cv::viz::KeyboardEvent& event, void* cookie);

	// Initialize Body
	void initializeBody();

	// Finalize
	void finalize();

	// Update Color
	void updateColor();

	// Update Depth
	void updateDepth();

	// Update Body
	void updateBody();

	// Draw Color
	void drawColor();

	// Draw Depth
	void drawDepth();

	// Draw Color
	void drawColor2Cloud();

	// Draw Point Cloud
	void drawPointCloud();

	// Draw Body
	void drawBody();

	// Draw Circle
	void drawEllipse(cv::Mat& image, const Joint& joint, const int radius, const cv::Vec3b& color, const int thickness = -1);

	// Draw Hand State
	void drawHandState(cv::Mat& image, const Joint& joint, HandState handState, TrackingConfidence handConfidence);

	// Show Depth
	void showDepth();

	// Show Point Cloud
	void showPointCloud();

	// Show Body
	void showBody();

private:
	// Sensor
	IKinectSensor* kinect=NULL;

	// Coordinate Mapper
	ICoordinateMapper* coordinateMapper = NULL;

	// Reader
	IColorFrameReader* colorFrameReader=NULL;
	IDepthFrameReader* depthFrameReader=NULL;
	IBodyFrameReader* bodyFrameReader=NULL;

	// Frame
	IColorFrame* colorFrame = NULL;
	IDepthFrame* depthFrame=NULL;
	IBodyFrame* bodyFrame = NULL;

	IBody* body = NULL;

	// Color Buffer
	std::vector<BYTE> colorBuffer;
	int colorWidth;
	int colorHeight;
	unsigned int colorBytesPerPixel;
	cv::Mat colorMat;
	cv::Mat color2CloudMat;

	// Depth Buffer
	std::vector<UINT16> depthBuffer;
	int depthWidth;
	int depthHeight;
	unsigned int depthBytesPerPixel;
	cv::Mat depthMat;

	// Point Cloud Buffer
	cv::viz::Viz3d viewer;
	cv::Mat cloudMat;

	// Body Buffer
	std::array<IBody*, BODY_COUNT> bodies = { NULL };
	std::array<cv::Vec3b, BODY_COUNT> colors;

	System::Drawing::Point handPoint;
};