#pragma once

#include "yaCommon.h"
#include "yaKinect.h"
#include "yaString.h"

#include "yaQLearning.h"
#include "yaGA.h"
#include "yaCNN.h"

#include "SplashForm.h"


namespace kame {

	using namespace System;
	using namespace std;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace System::Drawing::Imaging;
	using namespace System::Collections::Generic;

	using namespace System::Xml::Serialization;
	using namespace System::IO;

	cv::Mat fieldImg;
	cv::Mat fieldCat;
	vector<cv::Mat> kameImg(9);
	int qvalue[NODENO];
	int q_status = 0;
	int total_move_count = 0;
	int home_distance[STEPS];
	double e[MAXINPUTNO][INPUTSIZE][INPUTSIZE];
	int t[MAXINPUTNO];

	yaKinect kinect;

	/// <summary>
	/// Summary for MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			//kinect.run();

			InitializeComponent();

			progressBar->Maximum = 5;
			progressBar->Minimum = 1;
			progressBar->Step = 1;
			progressBar->Value = 1;

			progressLabel->Text = "Start";

			CloseTimer = gcnew Timer();
			CloseTimer->Stop();
			CloseTimer->Interval = 1000;
			CloseTimer->Tick += gcnew System::EventHandler(this, &MainForm::CloseTimer_Tick);

			CloseTimer->Start();
			sm = gcnew kame::SplashForm();
			sm->ShowDialog();

			progressBar->PerformStep();
			progressLabel->Text = "Init";
			yaQLearning::initq(qvalue);

			kameImg = initKame();

			moveTimer->Enabled = false;
			this->moveTimer->Tick += gcnew System::EventHandler(this, &MainForm::moveKame);
			//
			//TODO: Add the constructor code here
			//

			progressBar->PerformStep();
			progressLabel->Text = "GA";
			System::Random^ rnd = gcnew System::Random();

			Gene* value_pn;
			Gene* value_cn;

			value_pn = (Gene*)malloc(PN * sizeof(Gene));
			value_cn = (Gene*)malloc(CN * sizeof(Gene));

			yaGA::updateGen(value_pn, value_cn);

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	protected:
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::Panel^  fieldPanel;
	private: System::Windows::Forms::PictureBox^  fieldPictureBox;
	private: System::Windows::Forms::Button^  startButton;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::Timer^  moveTimer;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^  kameChart;
	private: System::Windows::Forms::ToolStripProgressBar^  progressBar;
	private: System::Windows::Forms::ToolStripStatusLabel^  progressLabel;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->progressBar = (gcnew System::Windows::Forms::ToolStripProgressBar());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->fieldPanel = (gcnew System::Windows::Forms::Panel());
			this->fieldPictureBox = (gcnew System::Windows::Forms::PictureBox());
			this->startButton = (gcnew System::Windows::Forms::Button());
			this->moveTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->kameChart = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->progressLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->statusStrip1->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			this->fieldPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fieldPictureBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->kameChart))->BeginInit();
			this->SuspendLayout();
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) { this->progressBar, this->progressLabel });
			this->statusStrip1->Location = System::Drawing::Point(0, 554);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(723, 22);
			this->statusStrip1->TabIndex = 0;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// progressBar
			// 
			this->progressBar->Name = L"progressBar";
			this->progressBar->Size = System::Drawing::Size(100, 16);
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->fileToolStripMenuItem });
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(723, 24);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// fieldPanel
			// 
			this->fieldPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->fieldPanel->Controls->Add(this->fieldPictureBox);
			this->fieldPanel->Location = System::Drawing::Point(13, 28);
			this->fieldPanel->Name = L"fieldPanel";
			this->fieldPanel->Size = System::Drawing::Size(518, 518);
			this->fieldPanel->TabIndex = 2;
			// 
			// fieldPictureBox
			// 
			this->fieldPictureBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->fieldPictureBox->BackColor = System::Drawing::Color::White;
			this->fieldPictureBox->Location = System::Drawing::Point(4, 4);
			this->fieldPictureBox->Name = L"fieldPictureBox";
			this->fieldPictureBox->Size = System::Drawing::Size(512, 512);
			this->fieldPictureBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::AutoSize;
			this->fieldPictureBox->TabIndex = 0;
			this->fieldPictureBox->TabStop = false;
			this->fieldPictureBox->Click += gcnew System::EventHandler(this, &MainForm::fieldPictureBox_Click);
			this->fieldPictureBox->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::fieldPictureBox_MouseClick);
			// 
			// startButton
			// 
			this->startButton->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->startButton->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(128)));
			this->startButton->Location = System::Drawing::Point(537, 32);
			this->startButton->Name = L"startButton";
			this->startButton->Size = System::Drawing::Size(99, 27);
			this->startButton->TabIndex = 3;
			this->startButton->Text = L"Start";
			this->startButton->UseVisualStyleBackColor = true;
			this->startButton->Click += gcnew System::EventHandler(this, &MainForm::startButton_Click);
			// 
			// kameChart
			// 
			this->kameChart->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			chartArea1->Name = L"ChartArea1";
			this->kameChart->ChartAreas->Add(chartArea1);
			legend1->Enabled = false;
			legend1->Name = L"Legend1";
			this->kameChart->Legends->Add(legend1);
			this->kameChart->Location = System::Drawing::Point(537, 247);
			this->kameChart->Name = L"kameChart";
			series1->ChartArea = L"ChartArea1";
			series1->IsVisibleInLegend = false;
			series1->Legend = L"Legend1";
			series1->Name = L"Series1";
			this->kameChart->Series->Add(series1);
			this->kameChart->Size = System::Drawing::Size(174, 300);
			this->kameChart->TabIndex = 4;
			this->kameChart->Text = L"chart1";
			this->kameChart->Click += gcnew System::EventHandler(this, &MainForm::kameChart_Click);
			// 
			// progressLabel
			// 
			this->progressLabel->Name = L"progressLabel";
			this->progressLabel->Size = System::Drawing::Size(0, 17);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(723, 576);
			this->Controls->Add(this->kameChart);
			this->Controls->Add(this->startButton);
			this->Controls->Add(this->fieldPanel);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->menuStrip1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MainMenuStrip = this->menuStrip1;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"MainForm";
			this->Text = L"MainForm";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->Resize += gcnew System::EventHandler(this, &MainForm::MainForm_Resize);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->fieldPanel->ResumeLayout(false);
			this->fieldPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->fieldPictureBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->kameChart))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Drawing::Point kamePoint;
			 System::Drawing::Point handPoint;
			 int kameDirection = 0;
			 Timer^ CloseTimer;
			 kame::SplashForm ^sm;

	private: System::Void CloseTimer_Tick(System::Object^  sender, System::EventArgs^e)
	{
		sm->Close();
	}

	private: vector<cv::Mat> initKame()
	{
		vector<cv::Mat> img(9);

		char filename[256];

		for (int i = 0; i < 9; i++) {
			sprintf(filename, "C:\\Users\\ashiy\\Documents\\Visual Studio 2015\\Projects\\kame\\kame\\img\\%02d.png", i + 1);
			printf("%s\n", filename);
			img[i] = imread(filename, IMREAD_UNCHANGED);

			cv::resize(img[i], img[i], cv::Size(), 0.2, 0.2);
		}

		kamePoint.X = fieldPictureBox->Width / 2;
		kamePoint.Y = fieldPictureBox->Height / 2;

		for (int i = 0; i < STEPS; i++) {
			home_distance[i] = sqrt(kamePoint.X*kamePoint.X + kamePoint.Y*kamePoint.Y);
		}

		return img;
	}

			 // cv::Mat to Bitmap
	private: Bitmap^ mat2bmp(cv::Mat img)
	{
		cv::Mat out;
		if (img.channels() == 1) {

			cv::Mat in[] = { img, img, img };//R G B
			out = cv::Mat(img.cols, img.rows, CV_8UC3);

			cv::merge(in, 3, out);
		}
		else {
			out = img.clone();
		}

		const int aligneCol = static_cast<int>(cv::alignSize(out.cols, 4));
		cv::Mat t_mat = cv::Mat(out.rows, aligneCol, out.type()); //row colの順

		cv::Mat roi_mat2 = cv::Mat(t_mat, cv::Rect(0, 0, out.cols, out.rows));//col rowの順

		out.copyTo(roi_mat2);
		Bitmap^ inter_bmp = gcnew Bitmap(roi_mat2.cols, roi_mat2.rows, roi_mat2.step,
			PixelFormat::Format24bppRgb, IntPtr(roi_mat2.ptr()));

		Bitmap^ bmp = gcnew Bitmap(inter_bmp);

		return bmp;
	}

	private: System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e)
	{
		drawBackground(&fieldImg, &fieldCat);

		drawKame(kameImg[1], &fieldImg, &fieldCat, fieldPanel->Width / 2, fieldPanel->Height / 2, 0);

		fieldPictureBox->Image = mat2bmp(fieldImg);
	}

	private: void drawBackground(cv::Mat* backgroundImg, cv::Mat* categoryImg)
	{
		kinect.update();

		kinect.draw();

		kinect.show();

		cv::Mat tmpMat = kinect.getImg();

		cvtColor(tmpMat, tmpMat, cv::COLOR_BGRA2BGR);

		float scale_size = max(fieldPanel->Width / 1920., fieldPanel->Height / 1080.) + 0.1;

		cv::resize(tmpMat, tmpMat, cv::Size(), scale_size, scale_size);

		//cv::flip(tmpMat, tmpMat, 1);

		//処理領域を設定
		cv::Rect roi(max(tmpMat.cols / 2 - fieldPanel->Width / 2, 0), max(tmpMat.rows / 2 - fieldPanel->Height / 2, 0), fieldPanel->Width, fieldPanel->Height);

		handPoint = kinect.getHand();

		handPoint.X = handPoint.X*scale_size - max(tmpMat.cols / 2 - fieldPanel->Width / 2, 0);
		handPoint.Y = handPoint.Y*scale_size - max(tmpMat.rows / 2 - fieldPanel->Height / 2, 0);

		//ROIの設定
		cv::Mat s_roi = cv::Mat(tmpMat, cv::Rect(max(tmpMat.cols / 2 - fieldPanel->Width / 2, 0), max(tmpMat.rows / 2 - fieldPanel->Height / 2, 0), fieldPanel->Width, fieldPanel->Height));
		*backgroundImg = cv::Mat(cv::Size(fieldPanel->Width, fieldPanel->Height), CV_8UC3, cv::Scalar(1, 1, 1)) * 255;
		cv::Mat d_roi = cv::Mat(*backgroundImg, cv::Rect(0, 0, fieldPanel->Width, fieldPanel->Height));

		s_roi.copyTo(d_roi);

		// Show Image
		//cv::imshow("Body", d_roi);

		*categoryImg = cv::Mat(cv::Size(fieldPanel->Width, fieldPanel->Height), CV_8UC3, cv::Scalar(0, 0, 0)) * 255;

		cv::Mat img17 = imread("C:\\Users\\ashiy\\Documents\\Visual Studio 2015\\Projects\\kame\\kame\\img\\17.png", IMREAD_UNCHANGED);

		cv::resize(img17, img17, cv::Size(), 0.2, 0.2);

		drawImg(img17, backgroundImg, categoryImg, 0, 0, 32);

		cv::Mat img18 = imread("C:\\Users\\ashiy\\Documents\\Visual Studio 2015\\Projects\\kame\\kame\\img\\18.png", IMREAD_UNCHANGED);

		cv::resize(img18, img18, cv::Size(), 0.2, 0.2);

		drawImg(img18, backgroundImg, categoryImg, 5000, 5000, 64);

		if (handPoint.X > 0 && handPoint.Y > 0) {
			drawFood(backgroundImg, categoryImg, handPoint.X, handPoint.Y);
		}
	}

	private: void drawFood(cv::Mat* backgroundImg, cv::Mat* categoryImg, int x, int  y)
	{
		cv::Mat img14 = imread("C:\\Users\\ashiy\\Documents\\Visual Studio 2015\\Projects\\kame\\kame\\img\\14.png", IMREAD_UNCHANGED);

		cv::resize(img14, img14, cv::Size(), 0.2, 0.2);

		drawImg(img14, backgroundImg, categoryImg, x, y, 192);
	}

	private: void drawKame(cv::Mat img, cv::Mat* backgroundImg, cv::Mat* categoryImg, int w, int h, int angle)
	{
		int max_size = MAX(img.cols, img.rows);

		cv::Mat rot_img = cv::Mat(cv::Size(1.4*max_size, 1.4*max_size), CV_8UC4, cv::Scalar(0));

		cv::Mat roi_mat = cv::Mat(rot_img, cv::Rect(rot_img.cols / 2 - img.cols / 2, rot_img.rows / 2 - img.rows / 2, img.cols, img.rows));

		img.copyTo(roi_mat);

		cv::Point2f center(rot_img.cols / 2., rot_img.rows / 2.);

		cv::Mat affineTrans = cv::getRotationMatrix2D(center, (double)angle, 1);
		cv::warpAffine(rot_img, rot_img, affineTrans, cv::Size(), cv::INTER_CUBIC);

		drawImg(rot_img, backgroundImg, categoryImg, w, h, 128);
	}

	private: void drawImg(cv::Mat img, cv::Mat* backgroundImg, cv::Mat* categoryImg, int w, int h, int cat)
	{
		vector<cv::Mat> planes_rgba;
		vector<cv::Mat> planes_rgb;
		vector<cv::Mat> planes_aaa;
		vector<cv::Mat> planes_1ma;

		cv::Mat img1;
		cv::Mat img2;
		cv::Mat img4;

		//チャンネルに分解
		cv::split(img, planes_rgba);

		//RGBA画像をRGBに変換   
		planes_rgb.push_back(planes_rgba[0]);
		planes_rgb.push_back(planes_rgba[1]);
		planes_rgb.push_back(planes_rgba[2]);
		merge(planes_rgb, img1);

		//RGBA画像からアルファチャンネル抽出   
		planes_aaa.push_back(planes_rgba[3]);
		planes_aaa.push_back(planes_rgba[3]);
		planes_aaa.push_back(planes_rgba[3]);
		merge(planes_aaa, img2);

		//imshow("alpha", img2);

		//背景用アルファチャンネル   
		planes_1ma.push_back(255 - planes_rgba[3]);
		planes_1ma.push_back(255 - planes_rgba[3]);
		planes_1ma.push_back(255 - planes_rgba[3]);
		merge(planes_1ma, img4);

		//imshow("mask", img4);

		if (backgroundImg->cols < img.cols || backgroundImg->rows < img.rows) {
			return;
		}

		int sx = MIN(MAX(w - img1.cols / 2, 0), backgroundImg->cols - img.cols);
		int sy = MIN(MAX(h - img1.rows / 2, 0), backgroundImg->rows - img.rows);

		cv::Mat roi_mat = cv::Mat(*backgroundImg, cv::Rect(sx, sy, img1.cols, img1.rows));
		cv::Mat roi_mat2 = cv::Mat(*categoryImg, cv::Rect(sx, sy, img1.cols, img1.rows));

		cv::Mat src = img1.mul(img2 / 255) + roi_mat.mul(img4 / 255);
		cv::Mat src2 = cat*img2 / 255. + roi_mat2.mul(img4 / 255);

		src.copyTo(roi_mat);
		src2.copyTo(roi_mat2);

		imshow("category", *categoryImg);

		cv::Mat smallImg;
		cv::resize(*categoryImg, smallImg, cv::Size(INPUTSIZE, INPUTSIZE));
		imshow("category2", smallImg);
	}

	private: System::Void MainForm_Resize(System::Object^  sender, System::EventArgs^  e)
	{
		drawBackground(&fieldImg, &fieldCat);

		kamePoint.X = fieldPictureBox->Width / 2;
		kamePoint.Y = fieldPictureBox->Height / 2;

		drawKame(kameImg[rand() % 9], &fieldImg, &fieldCat, kamePoint.X, kamePoint.Y, 0);

		fieldPictureBox->Image = mat2bmp(fieldImg);
	}

	private: System::Void fieldPictureBox_MouseClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
	{
		//moveKame();
		if (e->Button != System::Windows::Forms::MouseButtons::Left) {
			return;
		}

		Control^ control = dynamic_cast<Control^>(sender);

		System::Drawing::Point clickedPoint = System::Drawing::Point(e->X, e->Y);

		printf("[Mouce click] %d,%d\n", e->X, e->Y);

		drawBackground(&fieldImg, &fieldCat);

		drawFood(&fieldImg, &fieldCat, e->X, e->Y);

		drawKame(kameImg[rand() % 9], &fieldImg, &fieldCat, kamePoint.X, kamePoint.Y, kameDirection % 360);

		fieldPictureBox->Image = mat2bmp(fieldImg);

	}

	private: System::Void moveKame(System::Object ^ sender, System::EventArgs ^ e)
	{
		q_status = yaQLearning::selecta(q_status, qvalue);

		kameDirection=yaLearnBase::move_select(q_status%CHOISES, &kamePoint, &handPoint);

		for (int i = 0; i < STEPS - 1; i++) {
			home_distance[i] = home_distance[i + 1];
		}
		home_distance[STEPS - 1] = sqrt(kamePoint.X*kamePoint.X + kamePoint.Y*kamePoint.Y);

		kameChart->Series->Clear();
		kameChart->Series->Add("Series1");
		for (int i = 0; i < STEPS; i++)
		{
			kameChart->Series["Series1"]->Points->AddY(home_distance[i]);
		}

		if (q_status > (pow(CHOISES, STEPS) - 1) / (CHOISES - 1) - 1) {
			printf("%d %d\n", total_move_count, home_distance[STEPS - 1]);
		}

		drawBackground(&fieldImg, &fieldCat);

		drawKame(kameImg[rand() % 9], &fieldImg, &fieldCat, kamePoint.X, kamePoint.Y, kameDirection % 360);

		fieldPictureBox->Image = mat2bmp(fieldImg);

		qvalue[q_status] = yaQLearning::updateq(q_status, home_distance[STEPS - 1], qvalue);

		total_move_count++;

		if (total_move_count % STEPS == 0) {
			q_status = 0;

			kamePoint.X = fieldPictureBox->Width / 2;
			kamePoint.Y = fieldPictureBox->Height / 2;

			for (int i = 0; i < STEPS; i++) {
				home_distance[i] = sqrt(kamePoint.X*kamePoint.X + kamePoint.Y*kamePoint.Y);
			}
		}
	}

	private: System::Void startButton_Click(System::Object^  sender, System::EventArgs^  e)
	{
		if (startButton->Text == "Start") {
			moveTimer->Enabled = true;
			startButton->Text = "Stop";
		}
		else {
			moveTimer->Enabled = false;
			startButton->Text = "Start";

			yaQLearning::printqvalue(qvalue);
		}
	}

	private: System::Void kameChart_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void fieldPictureBox_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	};
}
