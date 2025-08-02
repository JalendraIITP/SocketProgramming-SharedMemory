#include <iostream>
#include <future>
#include <functional>
#include <thread>
#include <chrono>
using namespace std;
using namespace std::chrono;
// Simple
// template<typename T>
// void print1(T t){
//     cout<<t<<endl;
// }
// template<typename T, typename ... T0>
// void print1(T t, T0... rest){
//     cout<<t<<endl;
//     print1(rest...);
// }

// 2. Complex (Mostly used in generic multi threading)
int func(int a){
     this_thread::sleep_for(2s);
    return a*a;
}
int func1(int a,int b){
    this_thread::sleep_for(4s);
    return a*a+b*b;
}

template<typename F, typename...Args>
auto execute(F&& f, Args&&... args) -> future<decltype(f(args...))>{
   using returnType = decltype(f(args...));
   auto task = make_shared<packaged_task<returnType()>>(bind(forward<F>(f),forward<Args>(args)...));
   future<returnType>res = task->get_future();
   thread([task](){
    (*task)();
   }).detach();
   return res;
}


int main(){
    // Simple
    // print1(1,2,"hello", 2.5f);

    auto start = high_resolution_clock::now();
    auto a = execute(func,10);
    cout<<"*"<<endl;
    cout<<a.get()<<endl;
    auto a1 = execute(func1,10, 10);
    cout<<a1.get()<<endl;
    cout<<"#"<<endl;
    cout<<"&"<<endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);

    cout << "Total time taken: " << duration.count() << "s" << endl;
    return 0;
}