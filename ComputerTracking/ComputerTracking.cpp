//ComputerTracking.cpp

#include <opencv\cvaux.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	CvSize size640x480 = CvSize(640, 480);	//Set up initial windows
	CvCapture* p_capWebcam;					//A pointer to a webcam stream

	/*IPL is Image Processing Library used in OpenCV*/

	IplImage* p_OriginalImage;				//Input stream from webcam
	IplImage* p_ProcessedImage;				//Processed Image from the webcam into the computer

	CvMemStorage* p_strStorage;				//Storage for the image

	CvSeq* p_seqCircles;					//pointer to the OpenCV sequence... Will be used to the cvGetSeqElement()

	float* p_fltXYRadius;					//pointer to a 3-ele array of floats
											// [0] => x position of the detected object
											// [1] => y position
											// [2] => the Radius

	int i;									// loop counter
	char charCheckForEscKey;				// Key ESC storage

	p_capWebcam = cvCaptureFromCAM(0);		// The first input webcam;

	if (p_capWebcam == NULL){
		printf("error: Camera not Found!");
		getchar();
		return -1;							//Exit program with error
	}

	cvNamedWindow("Original",CV_WINDOW_AUTOSIZE);					//Original Stream from the webcam
	cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);					//Processed Window for the images;

	p_ProcessedImage = cvCreateImage(size640x480, IPL_DEPTH_8U, 1); // 1 Channel only (grayscale), if it was a color image, use 3

	while (1){ //infinate loop for every frame from the camera
		p_OriginalImage = cvQueryFrame(p_capWebcam);				//get every frame;

		if (p_OriginalImage == NULL){
			printf("Error: Invalid Frame - NULL");
			getchar();
			break;
		}

		cvInRangeS(p_OriginalImage, CV_RGB(175, 0, 0), CV_RGB(256, 100, 100), p_ProcessedImage);
	
		p_strStorage = cvCreateMemStorage(0); // allocate necessary memory storage variable to pass into another function.

		//smooth the image for the system to use it easier
		cvSmooth(p_ProcessedImage, p_ProcessedImage, CV_GAUSSIAN, 9, 9); // Gaussian filter (Average nearby pixels)

		p_seqCircles = cvHoughCircles(p_ProcessedImage, // using processed image to put the circle in
								      p_strStorage,
						              CV_HOUGH_GRADIENT, 2, p_ProcessedImage->height/4, 100, 50, 10, 400);


		for (i = 0; i < p_seqCircles->total; i++){ // check each elements in the sequence
			p_fltXYRadius = (float*)cvGetSeqElem(p_seqCircles, i);

			printf("Bumper Position x = %f, y = %f, r = %f \n", p_fltXYRadius[0], p_fltXYRadius[1], p_fltXYRadius[2]);

			cvCircle(p_OriginalImage, cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])), 3, CV_RGB(0, 255, 0), CV_FILLED);

			cvCircle(p_OriginalImage, cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),
				cvRound(p_fltXYRadius[2]), CV_RGB(255, 0, 0), 3);

		}

			cvShowImage("Original", p_OriginalImage);
			cvShowImage("Processed", p_ProcessedImage);

			cvReleaseMemStorage(&p_strStorage);

			charCheckForEscKey = cvWaitKey(10);
			if (charCheckForEscKey == 27){
				break;
			}

	}

	cvReleaseCapture(&p_capWebcam);
	cvDestroyWindow("Original");
	cvDestroyWindow("Processed");

	return 0;
}