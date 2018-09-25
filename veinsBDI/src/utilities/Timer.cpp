/*
 * Timer.cpp
 *
 *  Created on: 20/09/2018
 *      Author: miguel
 */

#include "Timer.h"

void Timer::startTimer(){
    start = std::chrono::high_resolution_clock::now();
}

void Timer::stopTimer(){
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    time_list.push_back(duration.count()*1000);
}

double Timer::getAverage(){
    double sum;
    for(long i = 0; i < time_list.size(); i++){
        sum += time_list[i];
    }
    return sum / time_list.size();
}
