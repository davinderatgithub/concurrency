#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;

class UberRide {
private:
    mutex mtx;
    condition_variable democratWaiting, republicanWaiting;
    int democratCount = 0;
    int republicanCount = 0;

public:

    void seatDemocrat() {
        unique_lock <mutex> lock(mtx);
        ++democratCount;

        // check if we can form valid ride
        if (democratCount >= 4) {
            // allocate seat to all to democrats
            democratWaiting.notify_all();

            // reset for next ride
            democratCount -= 4;
        }
        else if (democratCount >= 2 && republicanCount >= 2) {
            // allocate seat to all to 2 democrats and 2 republican
            democratWaiting.notify_one();
            republicanWaiting.notify_all();

            democratCount -= 2;
            republicanCount -= 2;
        }
        else {
            democratWaiting.wait(lock, [this] {return democratCount >= 2 &&
                                                      republicanCount >= 2; });
        }
        // Take the ride
        cout << "Democrats thread took the ride." << endl;
    }

    void seatRepublican() {
        unique_lock <mutex> lock(mtx);
        ++republicanCount;

        if (republicanCount >= 4) {
            republicanWaiting.notify_all();

            republicanCount -= 4;
        }
        else if (republicanCount >= 2 && democratCount >= 2) {
            republicanWaiting.notify_one();
            democratWaiting.notify_all();

            democratCount -= 2;
            republicanCount -= 2;
        }
        else {
            republicanWaiting.wait(lock, [this] {return democratCount >= 2 &&
                                                        republicanCount >= 2; });
        }
        // Take the ride
        cout << "Republicans thread took the ride." << endl;
    }
};

int main() {
    UberRide uberRide;

    vector<thread> threads;
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&, i] () {
            if (i % 2 == 0) {
                uberRide.seatDemocrat();
            } else {
                uberRide.seatRepublican();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}