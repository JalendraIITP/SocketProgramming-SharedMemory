#include<iostream>
#include<thread>
#include<vector>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<memory>
#include<future>
using namespace std;
// A thread pool is a group of pre-instantiated, idle threads which stand ready to be given work.
int func(int a){
    this_thread::sleep_for(4s);
    // cout<<"Thread Execution Completed"<<endl;
    return a*a;
}
int func1(int a){
    this_thread::sleep_for(3s);
    // cout<<"Thread Execution Completed"<<endl;
    return a*a*a;
}
class threadPool{
    int nthread;
    bool stop;
    vector<thread>threads;
    queue<function<void()>>tasks;
    mutex mtx;
    condition_variable cv;
    public:
    explicit threadPool(int nthread):nthread(nthread),stop(false){
        for(int i=0;i<nthread;i+=1){
            thread t([this]{
                function<void()>task;
                while(true){
                    {
                        unique_lock<mutex>lock(mtx);
                        cv.wait(lock,[this]{
                            return (!tasks.empty() || stop);
                        });
                        if (stop && tasks.empty())return;
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
            threads.emplace_back(move(t));
        }
    }
    /* 
    void execute(function<void()>f){
        unique_lock<mutex>lock(mtx);
        tasks.push(move(f));
        cv.notify_all();
    }
    */
    template<typename F, typename...Args>
    auto execute(F&& f, Args&&... args) -> future<decltype(f(args...))>{
        using returnType = decltype(f(args...));
        auto task = make_shared<packaged_task<returnType()>>(bind(forward<F>(f),forward<Args>(args)...));
        future<returnType>res = task->get_future();
        unique_lock<mutex>lock(mtx);
        tasks.push([task]()->void{
            (*task)();
        });
        cv.notify_all();
        return res;
    }
    virtual ~threadPool(){
        {
            lock_guard<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for(int i=0;i<nthread;i+=1){
            threads[i].join();
        }
    }
};
int main(){
    threadPool p(2);
    vector<future<int>>v;
    for(int i=0;i<60;i++){
        v.push_back(p.execute(func,2));
        v.push_back(p.execute(func1,2));
    }
    for(int i=0;i<v.size();i+=1){
        cout<<v[i].get()<<endl;
    }
    return 0;
}