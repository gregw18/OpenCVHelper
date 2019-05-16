/*
 * File:   funcs.cpp
 * Author: gregw
 * 
 * 
 * Some helper functions I've created to simplify reading in images and for debugging
 * image processing programs.
 * 
 * 
 */

#include "funcs.h"

// Function to read in given image and resize it to approximately 800x600, so will
// fit easily on screen. However, want to keep aspect ratio same, so calculate
// scale factors for each axis, use smallest one.
cv::Mat get_small_image(const std::string filename, cv::ImreadModes readMode, bool displayCalc) {
    
    int x1 = 0, y1 = 2600;
    int width = 800, height = 600;
    cv::Mat orig_img = cv::imread(filename, readMode);
    
    // Calculate smallest scale factor, to get image below target size.
    int rows = orig_img.rows, cols = orig_img.cols;
    if (displayCalc)
        std::cout << "rows:" << rows << ", cols:" << cols << std::endl;
    float col_factor = (float) width / cols;
    float row_factor = (float) height / rows;
    float final_factor;
    if ( col_factor <= row_factor ) final_factor = col_factor;
    else final_factor = row_factor;

    if (displayCalc)
        std::cout << "col_factor=" << col_factor << ", row_factor=" << row_factor << std::endl;
    // Round down, to two decimal places.
    final_factor = round ( (final_factor - 0.005) * 100) / 100;
    if (displayCalc)
        std::cout << "final=" << final_factor << std::endl;
      
    cv::Mat small_img;
    cv::resize(orig_img, small_img, cv::Size(), final_factor, final_factor);

    if (displayCalc) {
    describe_mat(orig_img, "original");
    describe_mat(small_img, "small");
    }
    
    return small_img;
}


// Helper function, prints out given name, along with size, # of channels and element size for given matrix.
void describe_mat(cv::Mat mat1, std::string name) {

    std::cout << name << " dims: " << mat1.dims << " size: ";// << mat1.size()
    for (int i = 0; i < mat1.dims; ++i) {
        if( i > 0) std::cout << " x ";
        std::cout << mat1.size[i];
    }
    std::cout << ", " << mat1.channels() << " channels, " 
            << mat1.depth() << " depth";
    if (mat1.dims < 3) {
        double min, max;
        cv::minMaxLoc(mat1, &min, &max);
        std::cout << ", min val: " << min
                << ", max val: " << max;
    }
    else {
        
    }
    std::cout << std::endl;
    
}


// Function to extract stills from videos. Goes through a video frame by frame,
// pausing after displaying each image. If user hits s, still is saved, with video name
// followed by frame number. Q quits process, C plays rest of video without pausing,
// any other key advances to next frame. Shows current/total frames on image.
void getStillsFromVideo(std::string videoName) {
    
    cv::VideoCapture cap;
    cap.open(videoName);  // To test on the same video.
    std::cout << "Starting capture for file " << videoName << std::endl;

    cv::Mat origImg, tmpImage;
    int total_frames, curr_frame = 0;
    total_frames = cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);
    bool no_stop = false;
    while(1) {
        ++curr_frame;
        cap >> origImg;
        if (!origImg.data) exit;
        
        tmpImage = origImg.clone();
        std::string frameString = std::to_string(curr_frame) + "/" + std::to_string(total_frames);
        cv::putText(tmpImage, frameString, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(255, 255, 255), 3);
        
        cv::imshow(videoName, tmpImage);
        if (no_stop) {
            cv::waitKey(70);
        }
        else {
            char lastkey = cv::waitKey(0);
            //std::cout << "lastkey=" << lastkey << std::endl;
            if (lastkey == 'Q' || lastkey == 'q') {      // Quit.
                break;
            }
            else if (lastkey == 'C' || lastkey == 'c') { // Play remaining frames without delay.
                no_stop = true;
            }
            else if (lastkey == 'S' || lastkey == 's') {
                std::string fname = videoName + std::to_string(curr_frame) + ".jpg";
                cv::imwrite(fname, origImg);
            }
        }
    }
    cap.release();

    return;
}
