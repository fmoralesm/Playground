#include "imagemodifier.h"

ImageModifier::ImageModifier(QObject *parent) :
    QObject(parent){

}

Mat ImageModifier::edgeCanny(Mat &src, int sliderMin, int sliderMax){
    // Data Initialization for Canny
    Mat src_gray, canny_edges;
    int minThr = ceil(sliderMin/100.0*255);
    int maxThr = ceil(sliderMax/100.0*255);
    int kernel_size = 3;

    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Canny detector
    Canny( src_gray, canny_edges, minThr, maxThr, kernel_size );
    return canny_edges;
}

Mat ImageModifier::edgeLaplace(Mat &src, int sliderMin, int sliderMax){
    Mat src_gray, dst;
    int kernel_size = 3;
    int scale = ceil(sliderMin/100.0*8);
    int delta = ceil(sliderMax/100.0*8);
    int ddepth = CV_8UC1;

    cvtColor( src, src_gray, CV_BGR2GRAY );     /// Convert the image to grayscale
    GaussianBlur( src_gray, src_gray, Size(3,3), 0, 0, BORDER_DEFAULT );
    Laplacian( src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );


    return dst;
}

Mat ImageModifier::edgeMorph(Mat &src){
    Mat result, src_gray;
    cvtColor( src, src_gray, CV_BGR2GRAY );     /// Convert the image to grayscale
    morphologyEx(src_gray,result,MORPH_GRADIENT,Mat());
    return result;
}

Mat ImageModifier::edgeScharr(Mat &src, int sliderMin, int sliderMax){
    /// Data Initialization for Sobel and Scharr
    Mat grad_x, grad_y, grad,src_gray;
    Mat abs_grad_x, abs_grad_y;
    int ddepth = CV_16UC1;

    int scale = ceil(sliderMin/100.0*8);
    int delta = ceil(sliderMax/100.0*8);

    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Gradient X
    Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    return grad;
}

Mat ImageModifier::edgeSobel(Mat &src, int sliderMin, int sliderMax){
    /// Data Initialization for Sobel and Scharr
    Mat grad_x, grad_y, grad,src_gray;
    Mat abs_grad_x, abs_grad_y;
    //int scale = 1;
   // int delta = 0;
    int ddepth = CV_16UC1;

    int scale = ceil(sliderMin/100.0*8);
    int delta = ceil(sliderMax/100.0*2);

    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Gradient X
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    return grad;
}

Mat ImageModifier::corHarris(Mat& src, int maxCorners, int sliderMax){
    Mat dst, dst_norm, dst_norm_scaled, src_gray, output;
    dst = Mat::zeros( src.size(), CV_32FC1 );

    output = src.clone();

    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Detector parameters
    int blockSize = 2;
    int apertureSize = 5;
    double k = 0.01;
    int thresh = ceil(sliderMax/100.0*255);
    int maxRow = 0;
    int maxCol = 0;

    /// Detecting corners
    cornerHarris( src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

    /// Normalizing
    normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
    convertScaleAbs( dst_norm, dst_norm_scaled );

    maxRow = ceil(maxCorners/100.0*dst_norm.rows);
    maxCol = ceil(maxCorners/100.0*dst_norm.cols);

    /// Drawing a circle around corners
    for( int j = 0; j < maxRow ; j++ )
       { for( int i = 0; i < maxCol; i++ )
            {
              if( (int) dst_norm.at<float>(j,i) > thresh )
                {
                 circle( output, Point( i, j ), 3,  Scalar(0,0,255), CV_FILLED, 8, 0 );
                }
            }
       }
    return output;
}

Mat ImageModifier::corShi(Mat& src, int maxCorners){
    Mat copy, src_gray;

    /// Parameters for Shi-Tomasi algorithm
    vector<Point2f> corners;
    double qualityLevel = 0.01;
    double minDistance = 10;
    int blockSize = 3;
    bool useHarrisDetector = false;
    double k = 0.04;

    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Copy the source image

    copy = src.clone();

    /// Apply corner detection
    goodFeaturesToTrack( src_gray,
                 corners,
                 maxCorners,
                 qualityLevel,
                 minDistance,
                 Mat(),
                 blockSize,
                 useHarrisDetector,
                 k );


    /// Draw corners detected
    //  cout<<"** Number of corners detected: "<<corners.size()<<endl;
    int r = 2;
    for( int i = 0; i < corners.size(); i++ )
       { circle(copy, corners[i], r, Scalar(0,0,255), CV_FILLED, 8, 0 ); }

    /// Show what you got
    return copy;
}

Mat ImageModifier::Sift(Mat& src, int sliderCorners){

    SIFT sift(sliderCorners);   //number of keypoints

    vector<cv::KeyPoint> key_points;

    Mat descriptors, mascara;
    Mat output_img;

    sift(src,mascara,key_points,descriptors);
    drawKeypoints(src, key_points, output_img);

    return output_img;
}

Mat ImageModifier::doSurf(Mat& src, int sliderCorners){
    SURF surf(sliderCorners);   //number of keypoints

    vector<cv::KeyPoint> key_points;

    Mat descriptors, mascara;
    Mat output_img;

    surf(src,mascara,key_points,descriptors);
    drawKeypoints(src, key_points, output_img);

    return output_img;

}
