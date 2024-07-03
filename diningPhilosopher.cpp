#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

mutex cout_mutex;
class diningPhilosopher
{
private:
    /* data */
    vector<mutex> forks;
    int num_philosophers;
public:
    diningPhilosopher(int n) : forks(n), num_philosophers(n) {};

    void dine(int id) {
        while (true)
        {
            think(id);
            eat(id);
        }
    }

    void think(int id) {
        cout_mutex.lock();
        cout << "Philospher: " << id << " thinking" << endl;
        cout_mutex.unlock();
        //simulate thinking
        this_thread::sleep_for(chrono::seconds(1));
    }

    void eat(int id) {
        int left_fork = id;
        int right_fork = (id + 1)%num_philosophers;
        
        // Lock the forks in a consistent order to avoid circular deadlock
        if (id%2 == 0) {
            forks[left_fork].lock();
            forks[right_fork].lock();
        }
        else {
            forks[right_fork].lock(); 
            forks[left_fork].lock();           
        }


        cout_mutex.lock();
        cout << "Philospher: " << id << " eating" << endl;
        cout_mutex.unlock();
        //simulate eating
        this_thread::sleep_for(chrono::seconds(1));

        forks[left_fork].unlock();
        forks[right_fork].unlock();       
    }
};

int main () {
    int num_philosohers = 5;

    diningPhilosopher dp(num_philosohers);

    vector <thread> philosophers;
    for (int i = 0; i < num_philosohers; ++i) {
        philosophers.push_back(thread (&diningPhilosopher::dine, &dp, i));
    }

    for(auto& philosopher : philosophers) {
        philosopher.join();
    }

    return 0;
}

