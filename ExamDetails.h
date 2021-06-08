
#ifndef UNTITLED2_EXAMDETAILS_H
#define UNTITLED2_EXAMDETAILS_H

#define MONTH_LENGTH 30

#include <xstring>

using std::string ; // Should we use the using in the h file?
static const

class ExamDetails {
    int course;
    int exam_month;///Enum?
    int exam_day;
    double exam_hour; ///Should it be double? something else?
    int exam_length;
    string exam_link;

public:
    ExamDetails(const int course,const int examMonth,const int examDay,const double examHour,const int examLength, const string &examLink);
    ExamDetails(const ExamDetails &other_exam_details);
    ExamDetails& operator=(const ExamDetails& exam_details);

    bool operator<(const ExamDetails &other) const;
    int operator-(const ExamDetails &other) const;


private:
    ~ExamDetails()=default;

};

bool ExamDetails::operator<(const ExamDetails &other) const {
    if (exam_month < other.exam_month)
        return true;
    if (other.exam_month < exam_month)
        return false;
    if (exam_day < other.exam_day)
        return true;
    if (other.exam_day < exam_day)
        return false;
    return exam_hour<other.exam_hour;
}
int ExamDetails::operator-(const ExamDetails &other) const{
    return (MONTH_LENGTH*exam_month +exam_day) - (MONTH_LENGTH*other.exam_month +other.exam_day);
}


ExamDetails& ExamDetails::operator=(const ExamDetails& exam_details){
    if(this == &exam_details){
        return *this;
    }

}

#endif //UNTITLED2_EXAMDETAILS_H
