// Free for non-commercial, non-military, and non-critical 
// use unless incorporated in OpenCV. 
// Inherits OpenCV Licence if in OpenCV.


#include "Optimizer.hpp"
#include "Optimizer.cuh"
#include <opencv2/gpu/stream_accessor.hpp>
#include <iostream>

using namespace std;
void Optimizer::setDefaultParams(){
    thetaStart =    20.0;
    thetaMin   =     10.0;
    thetaStep  =      .97;
    epsilon    =       .01;
    lambda     =       .01;
}

Optimizer::Optimizer(CostVolume cv) : cv(cv)
{
    setDefaultParams();
}

void Optimizer::initOptimization(){
    theta=thetaStart;
    computeSigmas();
    initA();
    initQD();


}
void Optimizer::initQD(){

    _a.copyTo(_d);
    _qx.create(cv.rows,cv.cols,CV_32FC1);
    _qx=0.0;
    _qy.create(cv.rows,cv.cols,CV_32FC1);
    _qy=0.0;
    cacheGValues();
}
void Optimizer::initA() {
    cv.loInd.copyTo(_a);
}
bool Optimizer::optimizeA(){
    using namespace cv::gpu::device::dtam_optimizer;
    localStream = cv::gpu::StreamAccessor::getStream(cvStream);

    bool doneOptimizing = theta <= thetaMin;
    int layerStep = cv.rows * cv.cols;
    float* d = (float*) _d.data;
    float* a = (float*) _a.data;

//    loadConstants(cv.rows, cv.cols, cv.layers, layerStep, a, d, cv.data, (float*)cv.lo.data,
//            (float*)cv.hi.data, (float*)cv.loInd.data);
    minimizeACaller  ( theta,lambda);
    theta*=thetaStep;
    return doneOptimizing;
}


