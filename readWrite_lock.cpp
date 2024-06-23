#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class ReadWriteLock {
private:
    mutex mutex1;
    condition_variable read_cond, wirte_cond;
    int read_count;
    int write_lock;

public:
    ReadWriteLock() : read_count(0), write_lock(false) {}

    void acquire_read_lock() {
        unique_lock <mutex> lock(mutex1);
        while (write_lock) {
            read_cond.wait(lock);
        }
        ++read_count;
    }

    void release_read_lock() {
        unique_lock <mutex> lock(mutex1);
        --read_count;
        while (read_count == 0) {
            wirte_cond.notify_one();
        }
    }

    void acquire_write_lock() {
        unique_lock <mutex> lock(mutex1);
        while (read_count > 0 || write_lock) {
            wirte_cond.wait(lock);
        }
        write_lock = true;
    }

    void release_write_lock() {
        unique_lock <mutex> lock(mutex1);
        write_lock = false;
        read_cond.notify_all();
        wirte_cond.notify_one();
    }
};


int main() {
    ReadWriteLock lock;

    // Simulate multiple readers
    thread reader1([&] {
        lock.acquire_read_lock();
        cout << "Reader 1 entered the critical section." << endl;
        lock.release_read_lock();
    });

    thread reader2([&] {
        lock.acquire_read_lock();
        cout << "Reader 2 entered the critical section." << endl;
        lock.release_read_lock();
    });

    // Simulate a writer
    thread writer([&] {
        lock.acquire_write_lock();
        cout << "Writer entered the critical section." << endl;
        lock.release_write_lock();
    });

    writer.join();
    reader1.join();
    reader2.join();

    return 0;
}
