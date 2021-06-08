
#ifndef UNTITLED2_EXAMDETAILS_H
#define UNTITLED2_EXAMDETAILS_H

#define MONTH_LENGTH 30

#include <iostream>

namespace mtm {

    using std::string; // Should we use the using in the h file?


    class ExamDetails {
        int course;
        int exam_month;///Enum?
        int exam_day;
        double exam_hour; ///Should it be double? something else?
        int exam_length;
        string exam_link;
        static double fraction(double num); //??? private


    public:
        ExamDetails(int course, int exam_month, int exam_day, double exam_hour,
                    int exam_length, const string &exam_link = "");

        ExamDetails(const ExamDetails &other_exam_details);

        ExamDetails &operator=(const ExamDetails &exam_details);

        friend bool operator<(const ExamDetails &exam1, const ExamDetails &exam2);

        int operator-(const ExamDetails &other) const;

        static ExamDetails makeMatamExam();

        const string &getLink() const;

        void setLink(const string &examLink);

        friend std::ostream &operator<<(std::ostream &os, const ExamDetails &details);

        class InvalidDateException {
        };

        class InvalidTimeException {
        };

        class InvalidArgsException {
        };

        //TODO check for static consts
        //static const double  EXAM_HOUR = 13.5;
        ~ExamDetails() = default;

    //private:


    };

}
#endif //UNTITLED2_EXAMDETAILS_H
