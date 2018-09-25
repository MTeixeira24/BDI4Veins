/*
 * Timer.h
 *
 *  Created on: 20/09/2018
 *      Author: miguel
 */

#ifndef UTILITIES_TIMER_H_
#define UTILITIES_TIMER_H_

#include <chrono>
#include <vector>

class Timer {
public:
    Timer(){};
    virtual ~Timer(){};
    void startTimer();
    void stopTimer();
    double getAverage();
    //double getMedian();

private:
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
    std::vector<double> time_list;
};

#endif /* UTILITIES_TIMER_H_ */
