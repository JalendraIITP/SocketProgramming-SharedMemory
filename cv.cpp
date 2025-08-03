#include<iostream>
#include<condition_variable>
#include<thread>
#include<mutex>
using namespace std;
bool flag = false;
mutex mx;
condition_variable cv;
void F1(){
    cout<<"First thread got executed"<<endl;
    this_thread::sleep_for(3s);
    {
        unique_lock<mutex>lock(mx);
        flag=true;
    }
    cv.notify_all();
}
void F2(){
    this_thread::sleep_for(2s);
    unique_lock<mutex>lock(mx);
    cv.wait(lock,[]{
        return flag;
    });
    cout<<"Second thread got executed"<<endl;
}
int main(){
    thread t(F1);
    thread t1(F2);
    thread t2(F2);
    t.join();
    t1.join();
    t2.join();
    return 0;
}