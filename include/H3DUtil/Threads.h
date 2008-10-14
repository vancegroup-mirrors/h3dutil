//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
//
//    This file is part of H3DUtil.
//
//    H3DUtil is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3DUtil is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3DUtil; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file Threads.h
/// \brief Header file for thread handling functions.
///
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __THREADS_H__
#define __THREADS_H__

#include <H3DUtil/H3DUtil.h>
#include <list>
#include <vector>
#include <pthread.h>

#ifdef WIN32
#define DEFAULT_THREAD_PRIORITY THREAD_PRIORITY_NORMAL
#else
#define DEFAULT_THREAD_PRIORITY 0
#endif

namespace H3DUtil {
  /// Mutual exclusion lock class for synchronisation between threads. 
  /// Most common usage is to make sure that only one thread enters a 
  /// "critical section" at a time. E.g. if both threads uses the same 
  /// variable we must put a lock around the access to make sure that
  /// two threads does not access it at once.
  class H3DUTIL_API MutexLock {
  public:
    /// Constructor.
    MutexLock();
    
    /// Destructor.
    ~MutexLock();

    /// Locks the mutex. If already locked, waits until it is unlocked and
    /// then locks it.
    void lock();

    /// Unlocks the mutex.
    void unlock();

    /// Try to lock the mutex, if the lock is not available false is returned.
    bool tryLock();
        
  protected:
    pthread_mutex_t mutex;
  };


  /// The ConditionLock is a little more advanced version of MutexLock in that
  /// it can wait for an arbitrary action in the other thread.
  /// 
  /// Thread 1        
  /// l.lock()
  /// l.wait()  - lock will be released and the thread will wait for a signal
  ///             from another thread before continuing. When it continues the
  ///             lock is aquired again.
  /// l.unlock()
  ///
  /// Thread 2
  /// l.lock()
  /// do some stuff
  /// l.signal() - wake the waiting thread  
  /// l.unlock()
  class H3DUTIL_API ConditionLock: public MutexLock {
  public:
    /// Constructor.
    ConditionLock();
    
    /// Destructor.
    ~ConditionLock();

    /// Wait for the conditional to get a signal. The lock will be released
    /// while waiting and reaquired when a signal is received.
    void wait();

    /// Wait for the conditional to get a signal, but only wait a
    /// certain time. If the time exceeds the specified time false
    /// is returned. If signal is received true is returned.
    bool timedWait( unsigned int ms );

    /// Wakes up at least one thread blocked on this condition lock.
    void signal();

    /// This wakes up all of the threads blocked on the condition lock.
    void broadcast();
        
  protected:
    pthread_cond_t cond; 
  };

 
  /// The abstract base class for threads.
  class H3DUTIL_API ThreadBase {
  public:
    /// Destructor.
    virtual ~ThreadBase() {}

    typedef pthread_t ThreadId;

    /// Returns the id of the thread this function is called in.
    static ThreadId getCurrentThreadId();

    /// Returns the id of the main thread.
    static ThreadId getMainThreadId() {
      return main_thread_id;
    }
    
    /// Returns true if the call was made from the main thread.
    static bool inMainThread();

    /// Returns the thread id for this thread.
    inline ThreadId getThreadId() { return thread_id; }

    /// Only for Windows Visual Studio users. Sets the name of the 
    /// thread, specified by id, as it appears in the Visual Studio debugger.
    static void setThreadName( ThreadId id, const std::string &name );

    /// Only for Windows Visual Studio users. Sets the name of the 
    /// thread as it appears in the Visual Studio debugger.
    void setThreadName( const std::string &name );
  protected:
    /// the id of the thread.
    ThreadId thread_id;
    
    /// The id of the main thread.
    static ThreadId main_thread_id;
  };

  /// The abstract base class for threads that have a main loop and that allow
  /// you to add callback functions to be run in that loop.
  class H3DUTIL_API PeriodicThreadBase: public ThreadBase {
  public:
    /// Return code for callback functions. 
    typedef enum {
      /// The callback is done and should not be called any more.
      CALLBACK_DONE,
      /// The callback should be rescheduled and called the next loop 
      /// again.
      CALLBACK_CONTINUE
    } CallbackCode;

    /// Constructor.
    PeriodicThreadBase();

    /// Callback function type.
    typedef CallbackCode (*CallbackFunc)(void *data); 

    /// Add a callback function to be executed in this thread. The calling
    /// thread will wait until the callback function has returned before 
    /// continuing.
    virtual void synchronousCallback( CallbackFunc func, void *data ) = 0;

    /// Add a callback function to be executed in this thread. The calling
    /// thread will continue executing after adding the callback and will 
    /// not wait for the callback function to execute.
    /// Returns a handle to the callback that can be used to remove
    /// the callback.
    virtual int asynchronousCallback( CallbackFunc func, void *data ) = 0;

    /// Attempts to remove a callback. returns true if succeded. returns
    /// false if the callback does not exist. This function should be handled
    /// with care. It can remove the wrong callback if the callback that
    /// returned the callback_handle id is removed and a new callback is added.
    /// Callbacks are removed if they return CALLBACK_DONE or a call to this
    /// function is made.
    virtual bool removeAsynchronousCallback( int callback_handle ) = 0;

  protected:
    // internal function used to generate id for each callback.
    inline int genCallbackId() {
      if( free_ids.empty() ) {
        return next_id++;
      } else {
        int id = free_ids.back();
        free_ids.pop_back();
        return id;
      }
    }

    // the next id to use.
    int next_id;

    // if an id have been used and is freed it is stored here.
    std::list< int > free_ids;
  };

  /// The interface base class for all threads that are used for haptics
  /// devices.
  class H3DUTIL_API HapticThreadBase {
  public:
    /// Constructor.
    HapticThreadBase();
    
    /// Destructor.
    virtual ~HapticThreadBase();

    /// Add a callback function that is to be executed when all the haptic
    /// threads have been synchronised, so it will be a thread safe 
    /// callback between all haptic threads.
    static void synchronousHapticCB( PeriodicThreadBase::CallbackFunc func, 
                                     void *data );

    /// Returns true if the call was made from within a HapticThreadBase
    /// thread.
    static bool inHapticThread();
  protected:
    // Callback function for synchronising all haptic threads.
    static PeriodicThreadBase::CallbackCode sync_haptics( void * );

    // The haptic threads that have been created.
    static std::vector< HapticThreadBase * > threads;

    // Lock used to get the haptic threads to wait for the callback 
    // function to finish.
    static ConditionLock haptic_lock; 

    // Lock used to get the thread calling the synchronousHapticCB
    // function to wait for all the haptic threads to synchronize.
    static ConditionLock sg_lock; 

    // Counter used when synchronizing haptic threads. It tells how
    // many more haptic threads that are left to synchronize.
    static int haptic_threads_left;
  };

  /// The SimpleThread class creates a new thread to run a function. The
  /// thread is run until the function returns.
  class H3DUTIL_API SimpleThread : public ThreadBase {
  public:
    /// Constructor.
    /// \param func The function to run in the thread. 
    /// \param args Arguments to the function in argument func.
    /// \param thread_priority The priority of the thread.
    SimpleThread( void *(func)(void *),
                  void *args = NULL,
                  int thread_priority = DEFAULT_THREAD_PRIORITY );
    
    /// Wait for thread to complete.
    /// Returns 0 on success.
    int join();

    /// Destructor.
    virtual ~SimpleThread();
  }; 

  /// The PeriodicThread class is used to create new threads and provides an interface
  /// to add callback functions to be executed in the new thread that can be 
  /// used by other threads.
  class H3DUTIL_API PeriodicThread : public PeriodicThreadBase {
  public:
    /// Constructor.
    /// \param thread_priority The priority of the thread.
    /// \param thread_frequency The frequence of the thread loop. -1 means
    /// run as fast as possible.
    PeriodicThread( int thread_priority = DEFAULT_THREAD_PRIORITY,
            int thread_frequency = -1 );
    
    /// Destructor.
    virtual ~PeriodicThread();
    
    /// Add a callback function to be executed in this thread. The calling
    /// thread will wait until the callback function has returned before 
    /// continuing. 
    virtual void synchronousCallback( CallbackFunc func, void *data );

    /// Add a callback function to be executed in this thread. The calling
    /// thread will continue executing after adding the callback and will 
    /// not wait for the callback function to execute.
    /// Returns a handle to the callback that can be used to remove
    /// the callback.
    virtual int asynchronousCallback( CallbackFunc func, void *data );

    /// Add several asynchronous callbacks at once in order to minimize
    /// synchronization time in transferring the callbacks.
    /// The InputIterator must be an iterator where *i is of 
    /// type pair< CallbackFunc, void * >
    /// No handles to the functions will be given so the functions them 
    /// self must finish with CALLBACK_DONE in order for them to be removed.
    template< class InputIterator >
    void asynchronousCallbacks( InputIterator begin, 
                                                InputIterator end ) {
      callback_lock.lock();
      // signal the thread that a new callback is available if it is waiting for one.
      if( frequency < 0 && callbacks.size() == 0 ) callback_lock.signal();    
      
      for( InputIterator i = begin; i != end; i++ ) {
        int cb_id = genCallbackId();
        // add the new callback
        callbacks.push_back( make_pair( cb_id, *i ) );
      }
      callback_lock.unlock();
    }

    /// Attempts to remove a callback. returns true if succeded. returns
    /// false if the callback does not exist. This function should be handled
    /// with care. It can remove the wrong callback if the callback that
    /// returned the callback_handle id is removed and a new callback is added.
    /// Callbacks are removed if they return CALLBACK_DONE or a call to this
    /// function is made.
    virtual bool removeAsynchronousCallback( int callback_handle );

    /// Exit the thread_func. Will not destroy the PeriodicThread instance.
    inline void exitThread() {
      thread_func_is_running = false;
    }

  protected:
    // The function that handles callbacks. Is also the main function that
    // is run in the thread.
    static void *thread_func( void * );

    typedef std::list< std::pair< int, std::pair< CallbackFunc, void * > > >
      CallbackList;
    // A list of the callback functions to run.
    CallbackList callbacks;
    // A lock for synchronizing changes to the callbacks member.
    ConditionLock callback_lock;
    
    /// The priority of the thread.
    int priority;

    /// Thre frequency of the thread. -1 means run as fast as possible.
    int frequency;

    /// Flag used to exit the thread by making thread_func return when
    /// set to false.
    bool thread_func_is_running;

  };

  /// HapticThread is a thread class that should be used by haptics devices
  /// when creating threads. It is the same as PeriodicThread, but also inherits
  /// from HapticThreadBase to make it aware that it is a haptic thread.
  class H3DUTIL_API HapticThread : public HapticThreadBase,
                                public PeriodicThread {
  public:
    /// Constructor.
    HapticThread( int thread_priority =  DEFAULT_THREAD_PRIORITY,
		int thread_frequency = -1 ):
      PeriodicThread( thread_priority, thread_frequency ) {
    }
  };
}

#endif



