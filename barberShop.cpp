#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

const int NUM_CHAIRS = 5;

mutex mtx;
condition_variable customerCV, barberCV;
queue <thread::id> waitingCustomers;
bool barberSleeping = true;

void barber() {
    while (true)
    {
        unique_lock<mutex> lock(mtx); 
        if (waitingCustomers.empty()) {
            cout << "Barber sleeps..." << endl;
            barberSleeping = true;
            barberCV.wait(lock);
            cout << "Barber wakes up!" << endl;
        }

        thread::id customerId = waitingCustomers.front();
        waitingCustomers.pop();
        lock.unlock();

        cout << "Barber is cutting hair for customer " << customerId << endl;
        this_thread::sleep_for(chrono::seconds(2));
        cout << "Barber finished cutting hair for customer " << customerId << endl;

        lock.lock();
        customerCV.notify_one();
    }
}

void customer(thread::id id) {
    unique_lock<mutex> lock(mtx);

    if (waitingCustomers.size() == NUM_CHAIRS) {
        cout << "Customer " << id << " left the shop" << endl;
        return;
    }

    cout << "Customer " << id << " entered the shop" << endl;
    waitingCustomers.push(id);

    if (barberSleeping) {
        cout << "Customer " << id << " woke up the barber" << endl;
        barberSleeping = false;
        barberCV.notify_one();
    }
    else {
        cout << "Customer " << id << " waiting for the barber" << endl;
        customerCV.wait(lock);
    }

    cout << "Customer " << id << " getting haircut" << endl;
}


int main() {
    thread barberThread(barber);

    for (int i = 0; i < 10; ++i) {
        thread customerThread([i]() {customer(this_thread::get_id()); });
        customerThread.detach();
        this_thread::sleep_for(chrono::seconds(1)); // simulating customer ariving
    }

    barberThread.join();
    return 0;
}