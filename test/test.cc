#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <string>
#include <set>
#include <map>

struct  subject_unit{
    char subject_name[256];
    char type[256];
    char teacher_name[256];
    char test_type[256];
    char class_name[256];
    int student_num;

    bool operator<(const struct subject_unit & other)const{
       return this->student_num >= other.student_num;
    }
};
std::vector<struct subject_unit > subjects;

struct subject{
    std::string subject_name;
    int total_student_name;
    std::set<struct subject_unit> subjects;
};

bool SubjectCmp(struct subject a,struct subject b){
    return a.total_student_name > b.total_student_name;
}
struct classroom{
    char class_site[256];
    int class_capacity;
};
std::vector<struct classroom > classrooms;
std::vector<struct classroom > available_classroom;

// 12.28 12.29 12.30 12.31
int num_days = 4;
const char* days_str[4]={
    "12.28",
    "12,29",
    "12.30",
    "12.31"
};

// 2 in morning, 2 in afternoon
int num_time_period = 4;
const char* period_str[4]={
    "8:00-9:40",
    "10:00-11:40",
    "14:00-15:40",
    "16:00-17:40"
};

struct Period{
    int day_id; //0-3
    int period_id; //0-3

    bool operator==(const struct Period & other)const{
       return (this->day_id == other.day_id && this->period_id == other.period_id);
    }
};
std::map<std::string,struct Period> class_to_last_period;



void LoadSubject(){
    FILE *subject_fp = NULL; 
    subject_fp = fopen("./src/Subject.txt", "r");
    if(subject_fp == NULL){
        printf("Fail to open file\n");
        return;
    }
    struct subject_unit tmp_subject ;
    while(fscanf(subject_fp, "%s %s %s %s %s %d",tmp_subject.subject_name,tmp_subject.type,tmp_subject.teacher_name,tmp_subject.test_type,tmp_subject.class_name,&tmp_subject.student_num)!=EOF){
        subjects.push_back(tmp_subject);
    }
    fclose(subject_fp);
    
    // for(auto iter = subjects.begin(); iter!=subjects.end();iter++){
    //     printf("%s %s %s %s %s %d\n",iter->subject_name,iter->type,iter->teacher_name,iter->test_type,iter->class_name,iter->student_num);
    // }
}

void LoadClassRoom(){
    FILE *classroom_fp = NULL; 
    classroom_fp = fopen("./src/Classroom.txt", "r");
    if(classroom_fp == NULL){
        printf("Fail to open file\n");
        return ;
    }
    struct classroom tmp_classroom ;
    while(fscanf(classroom_fp, "%s %d",tmp_classroom.class_site,&tmp_classroom.class_capacity)!=EOF){
        classrooms.push_back(tmp_classroom);
        // printf("%s %d\n",tmp_classroom.class_site,&tmp_classroom.class_capacity);
    }

    fclose(classroom_fp);
}

std::vector<struct subject> subject_sets;
void CollectSubjects(){
    std::map<std::string,std::set<struct subject_unit> > subject_collect;
    for(auto iter = subjects.begin(); iter!=subjects.end();iter++){
        // printf("%s %s %s %s %s %d\n",iter->subject_name,iter->type,iter->teacher_name,iter->test_type,iter->class_name,iter->student_num);   
        subject_collect[std::string(iter->subject_name)].insert(*iter);
    }

    struct subject tmp_subject;
    for(auto iter = subject_collect.begin() ; iter != subject_collect.end() ; iter++){
        tmp_subject.subject_name = iter->first;
        tmp_subject.subjects = iter->second;
        tmp_subject.total_student_name = 0;
        for(auto iter_2 = tmp_subject.subjects.begin() ; iter_2 != tmp_subject.subjects.end(); iter_2++){
            tmp_subject.total_student_name += iter_2->student_num;
        }
        subject_sets.push_back(tmp_subject);
    }
    std::sort(subject_sets.begin(),subject_sets.end(),SubjectCmp);
    // for(auto iter = subject_sets.begin();iter != subject_sets.end();iter++){
    //     printf("subject_name:%s total_student_name:%d\n",iter->subject_name.c_str(),iter->total_student_name);
    //     for(auto iter_2 =  iter->subjects.begin() ; iter_2 != iter->subjects.end(); iter_2++){
    //         printf("%s %s %s %s %d\n",iter_2->type,iter_2->teacher_name,iter_2->test_type,iter_2->class_name,iter_2->student_num);   
    //     }
    // }
}

bool CheckClassroom(struct subject& cur_subject){
    if(available_classroom.size()< cur_subject.subjects.size()){
        return false;
    }
    auto pos = available_classroom.begin();
    for(auto iter = cur_subject.subjects.begin();iter!=cur_subject.subjects.end();iter++){
        if(pos == available_classroom.end()) return false;
        if(iter->student_num > pos->class_capacity) return false;
        pos++;
    }

    return true;
}


bool Check(struct subject& cur_subject,struct Period& cur_period){
    for(auto iter = cur_subject.subjects.begin(); iter != cur_subject.subjects.end(); iter++){
        auto pos = class_to_last_period.find(std::string(iter->class_name)); 
        if(pos != class_to_last_period.end() && pos->second==cur_period){
            return false;
        }
    }
    return true;
}

int main(){
    LoadSubject();
    LoadClassRoom();

    // 统计所有科目下的班级
    CollectSubjects();

    // 所有可用的考试场次
    // 1. 同一个班级 同一个时段只能有一门考试
    // 2. 同一门考试 所有班级一起考
    for(int i = 0 ; i < num_days ; i++){
        for(int j = 0 ; j < num_time_period ; j++){
            available_classroom = classrooms;
            struct Period cur_period{i,j};
            // AssignSubject 
            // get class of top Subject s
            for(auto iter = subject_sets.begin() ; iter != subject_sets.end(); ){
                // check if there are enough classrooms for subject s
                if(!CheckClassroom(*iter)){
                    iter++;
                    continue;
                }
                // check every class in s if there are conflicts
                if(Check(*iter,cur_period)){
                    // Assign classroom for all class in s
                    printf("%s\n",iter->subject_name.c_str());
                    auto pos = available_classroom.begin();
                    for(auto iter_2 = iter->subjects.begin();iter_2!=iter->subjects.end();iter_2++){
                        class_to_last_period[std::string(iter_2->class_name)]=cur_period;
                        printf("%s:[%s] %s 考试人数:%d %s 教室人数:%d\n",days_str[cur_period.day_id],period_str[cur_period.period_id],iter_2->class_name,iter_2->student_num,pos->class_site,pos->class_capacity); 
                        pos = available_classroom.erase(pos);
                    }
                    // erase cur subject from queue
                    iter=subject_sets.erase(iter);
                }else{
                    // if there conflicts, skip this subject
                    iter++;
                }
            }
        }
    }

    if(subject_sets.size()!=0){
        printf("There are subjects not allocated\n");
    }

    return 0;
}