// This is an adapted version of the Queue.h file created by Juan Palacios (2013)
// This version was adapted (Benoit Rosa, 2016) to compile in VS 2012, and added a few useful tryPop fuctions
//
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>

#ifndef CONCURRENT_QUEUE_
#define CONCURRENT_QUEUE_

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue
{
 public:

  T pop() 
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    auto val = queue_.front();
    queue_.pop();
    return val;
  }

  void pop(T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
      cond_.wait(mlock);
    }
    item = queue_.front();
    queue_.pop();
  }

  bool tryPop (T & item, std::chrono::milliseconds dur) 
  {
    std::unique_lock<std::mutex> mlock(mutex_);

	  while (queue_.empty())
    {
      cond_.wait_for(mlock, dur);
    }
    if (queue_.empty()) return false;
	  else
	  {
		  item = queue_.front();
		  queue_.pop();
		  return true;
	  }
  }

  bool tryPop (T & item) 
  {
    std::unique_lock<std::mutex> mlock(mutex_);

	  if (queue_.empty())
    {
      return false;
    }
	  item = queue_.front();
	  queue_.pop();
	  return true;
  }

  void push(const T& item)
  {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
    mlock.unlock();
    cond_.notify_one();
  }
  
  // Those prevent successful compilation in VS2012
  //Queue()=default;
  //Queue(const Queue&) = delete;            // disable copying
  //Queue& operator=(const Queue&) = delete; // disable assignment
  
 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

#endif
