

#include "ExamDetails.h"

ExamDetails::ExamDetails(const int course,const int examMonth,const int examDay,const double examHour,const int examLength, const string &examLink) {}

ExamDetails::ExamDetails(const ExamDetails &other_exam_details){
    course = other_exam_details.course;
    exam_month = other_exam_details.exam_month;
    exam_day = other_exam_details.exam_day;
    exam_hour = other_exam_details.exam_hour;
    exam_length = other_exam_details.exam_length;
    exam_link = other_exam_details.exam_link;
}

