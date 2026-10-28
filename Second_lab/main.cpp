#include <iostream>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <deque>
#include <cmath>
#include <chrono>
#include <vector>

using namespace std;

// Helper function to check if a number is prime
bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

// Helper function to find the n-th prime number
int findNthPrime(int n) {
    int count = 0;
    int num = 1;
    while (count < n) {
        num++;
        bool prime = true;
        for (int i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                prime = false;
                break;
            }
        }
        if (prime) count++;
    }
    return num;
}

// =========================================
// Task 1.2.1
// =========================================
queue<int> q_121;
mutex mtx_121;
bool data_ready_121 = false;

void DataPreparation_121() {
    int num;
    cout << "Enter numbers (enter 0 to finish):\n";
    while (cin >> num && num != 0) {
        lock_guard<mutex> lk(mtx_121);
        q_121.push(num);
    }
    lock_guard<mutex> lk(mtx_121);
    data_ready_121 = true;
}

void DataProcessing_121() {
    unique_lock<mutex> lk(mtx_121, defer_lock);
    while (true) {
        lk.lock();
        if (data_ready_121) {
            cout << "\nPrime numbers in queue: ";
            while (!q_121.empty()) {
                int val = q_121.front();
                q_121.pop();
                if (isPrime(val)) cout << val << " ";
            }
            cout << "\n";
            lk.unlock();
            break;
        }
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void task_1_2_1() {
    cout << "--- Task 1.2.1 ---\n";
    thread t1(DataPreparation_121);
    t1.detach();
    thread t2(DataProcessing_121);
    t2.join();
}

// =========================================
// Task 1.2.2
// =========================================
condition_variable cv_122;
mutex mtx_122;
int i_val_122 = 0;

void Waits_122(int id) {
    cout << "Thread " << id << " enters waiting state.\n";
    unique_lock<mutex> lk(mtx_122);
    cv_122.wait(lk, [] { return i_val_122 == 1; });
    cout << "Thread " << id << " finished waiting.\n";
}

void Awake_122() {
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "Awake: notifying all threads (notify_all)...\n";
    cv_122.notify_all();

    this_thread::sleep_for(chrono::milliseconds(100));

    {
        lock_guard<mutex> lk(mtx_122);
        i_val_122 = 1;
    }
    cout << "Awake: notifying all threads again (notify_all)...\n";
    cv_122.notify_all();
}

void task_1_2_2() {
    cout << "\n--- Task 1.2.2 ---\n";
    vector<thread> threads;
    for (int i = 1; i <= 3; ++i) {
        threads.push_back(thread(Waits_122, i));
    }
    thread t_awake(Awake_122);
    t_awake.join();
    for (auto& t : threads) {
        t.join();
    }
}

// =========================================
// Task 1.2.3
// =========================================
condition_variable cv_123;
mutex mtx_123;
int flag_123 = 0;

void ThreadWait_123(int id) {
    unique_lock<mutex> lk(mtx_123);
    cv_123.wait(lk, [] { return flag_123 == 1; });
    cout << "Message from thread " << id << "\n";
}

void Notify_123() {
    this_thread::sleep_for(chrono::milliseconds(100));
    {
        lock_guard<mutex> lk(mtx_123);
        flag_123 = 1;
    }
    cout << "Notify: sending notify_one()...\n";
    cv_123.notify_one();
}

void task_1_2_3() {
    cout << "\n--- Task 1.2.3 ---\n";
    thread t1(ThreadWait_123, 1);
    thread t2(ThreadWait_123, 2);
    thread t3(ThreadWait_123, 3);

    t1.detach();
    t2.detach();
    t3.detach();

    thread t_notify(Notify_123);
    t_notify.join();

    this_thread::sleep_for(chrono::milliseconds(50));
}

// =========================================
// Task 1.2.4
// =========================================
queue<int> q_124;
mutex mtx_124;
condition_variable cv_124;
bool input_done_124 = false;

void DataPreparationCV_124() {
    int num;
    cout << "Enter numbers (enter 0 to finish):\n";
    while (cin >> num && num != 0) {
        lock_guard<mutex> lk(mtx_124);
        q_124.push(num);
    }
    {
        lock_guard<mutex> lk(mtx_124);
        input_done_124 = true;
    }
    cv_124.notify_one();
}

void DataProcessingCV_124() {
    unique_lock<mutex> lk(mtx_124);
    cv_124.wait(lk, [] { return !q_124.empty() || input_done_124; });

    cout << "\nPrime numbers in queue: ";
    while (!q_124.empty()) {
        int val = q_124.front();
        q_124.pop();
        if (isPrime(val)) cout << val << " ";
    }
    cout << "\n";
}

void task_1_2_4() {
    cout << "\n--- Task 1.2.4 ---\n";
    thread t1(DataPreparationCV_124);
    t1.detach();
    thread t2(DataProcessingCV_124);
    t2.join();
}

// =========================================
// Task 1.2.5
// =========================================
void task_1_2_5() {
    cout << "\n--- Task 1.2.5 ---\n";
    int n;
    cout << "Enter the index of the prime number (n): ";
    cin >> n;

    // Change between launch::async and launch::deferred to test
    auto launch_policy = launch::async;

    auto start_time = chrono::high_resolution_clock::now();
    future<int> prime_future = async(launch_policy, findNthPrime, n);

    int choice;
    cout << "While computing, choose a function of n (1-sqrt, 2-sin, 3-ln): ";
    cin >> choice;

    if (choice == 1) cout << "sqrt(n) = " << sqrt(n) << "\n";
    else if (choice == 2) cout << "sin(n) = " << sin(n) << "\n";
    else if (choice == 3) cout << "ln(n) = " << log(n) << "\n";

    cout << "Waiting for async calculation to complete...\n";
    int result = prime_future.get();
    auto end_time = chrono::high_resolution_clock::now();

    chrono::duration<double> diff = end_time - start_time;

    cout << n << "-th prime number: " << result << "\n";
    cout << "Execution time: " << diff.count() << " seconds.\n";
}

// =========================================
// Task 1.2.6
// =========================================
deque<packaged_task<int()>> tasks_126;
deque<int> n_values_126;
mutex mtx_126;
condition_variable cv_126;
bool stop_worker_126 = false;

void WorkerThread_126() {
    while (true) {
        packaged_task<int()> task;
        int current_n = 0;
        {
            unique_lock<mutex> lk(mtx_126);
            cv_126.wait(lk, [] { return !tasks_126.empty() || stop_worker_126; });

            if (stop_worker_126 && tasks_126.empty()) return;

            task = move(tasks_126.front());
            tasks_126.pop_front();
            current_n = n_values_126.front();
            n_values_126.pop_front();
        }
        cout << "[Worker] Computing " << current_n << "-th prime number...\n";
        task();
    }
}

void task_1_2_6() {
    cout << "\n--- Task 1.2.6 ---\n";
    thread worker(WorkerThread_126);
    deque<future<int>> results;
    deque<int> asked_n;

    cout << "Enter n to find prime numbers (0 to start/finish input):\n";
    int n;
    while (cin >> n && n != 0) {
        packaged_task<int()> pt(bind(findNthPrime, n));
        results.push_back(pt.get_future());
        asked_n.push_back(n);

        {
            lock_guard<mutex> lk(mtx_126);
            tasks_126.push_back(move(pt));
            n_values_126.push_back(n);
        }
        cv_126.notify_one();
    }

    {
        lock_guard<mutex> lk(mtx_126);
        stop_worker_126 = true;
    }
    cv_126.notify_one();

    for (size_t i = 0; i < results.size(); ++i) {
        cout << asked_n[i] << "-th prime number: " << results[i].get() << "\n";
    }

    worker.join();
}

// =========================================
// Task 1.2.7
// =========================================
void Thread1_127(int n, promise<int> p_nth, promise<bool> p_signal, promise<int> p_nth10) {
    p_nth.set_value(findNthPrime(n));
    p_signal.set_value(true);
    p_nth10.set_value(findNthPrime(n * 10));
}

void Thread2_127(int n, future<bool> f_signal) {
    f_signal.wait();
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Thread 2: Square root of " << n << " = " << sqrt(n) << "\n";
}

void task_1_2_7() {
    cout << "\n--- Task 1.2.7 ---\n";
    int n;
    cout << "Enter number n: ";
    cin >> n;

    promise<int> prom_nth;
    promise<bool> prom_signal;
    promise<int> prom_nth10;

    future<int> fut_nth = prom_nth.get_future();
    future<bool> fut_signal = prom_signal.get_future();
    future<int> fut_nth10 = prom_nth10.get_future();

    thread t1(Thread1_127, n, move(prom_nth), move(prom_signal), move(prom_nth10));
    thread t2(Thread2_127, n, move(fut_signal));

    cout << "Main thread displays results:\n";
    cout << n << "-th prime number: " << fut_nth.get() << "\n";
    cout << (n * 10) << "-th prime number: " << fut_nth10.get() << "\n";

    t1.join();
    t2.join();
}

// =========================================
// Main Menu
// =========================================
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "       LABORATORY WORK 2 MENU           \n";
        cout << "========================================\n";
        cout << "1. Run Task 1.2.1\n";
        cout << "2. Run Task 1.2.2\n";
        cout << "3. Run Task 1.2.3\n";
        cout << "4. Run Task 1.2.4\n";
        cout << "5. Run Task 1.2.5\n";
        cout << "6. Run Task 1.2.6\n";
        cout << "7. Run Task 1.2.7\n";
        cout << "0. Exit\n";
        cout << "Enter your choice (0-7): ";
        cin >> choice;

        switch (choice) {
            case 1: task_1_2_1(); break;
            case 2: task_1_2_2(); break;
            case 3: task_1_2_3(); break;
            case 4: task_1_2_4(); break;
            case 5: task_1_2_5(); break;
            case 6: task_1_2_6(); break;
            case 7: task_1_2_7(); break;
            case 0: cout << "Exiting program...\n"; break;
            default: cout << "Invalid choice! Please try again.\n"; break;
        }
    } while (choice != 0);

    return 0;
}