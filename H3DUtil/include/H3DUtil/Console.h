//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2013, SenseGraphics AB
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
/// \file Console.h
/// \brief Header file for debug console stream.
///
//  This class provides a static stream that all error and warning messages
//  are sent to. The stream can be redirected to any arbitrary stream, and
//  defaults to cerr. The stream can be controlled to set a minimum 
//  error level to show (setting 0 will show all messages). The default level
//  is 3.
//
//  Example usage:
//
//  Console.setOutputStream( cout );
//  Console.setOutputLevel( 2 );
//  Console(0) << "Warning:" << endl;
//  Console    << " This is a warning" << endl;
//  Console    << " This is still the same warning" << endl;
//
//  Console(1) << "Level 1" << endl;
//  Console(2) << "Level 2" << endl;
//  Console.setShowTime( true );
//  Console(3) << "Level 3, with time" << endl;
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <H3DUtil/H3DUtil.h>
#include <H3DUtil/TimeStamp.h>

#include <ostream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

namespace H3DUtil {

  /// A string buffer class that stores different messages depending
  /// on internal parameters and sends this to an output stream.
  /// See basic_dostream for example usage.
  template <class CharT, class TraitsT = std::char_traits<CharT> >
  class basic_debugbuf : public std::basic_stringbuf<CharT, TraitsT>  {
    /// Minimum warning level that will be sent to output stream.
    /// Must be 0 or above.
    int outputlevel;
    /// The level of the message to be sent to output stream. Must be above
    /// outputlevel to be displayed.
    int level;
    /// The output stream to send the content of the string buffer to.
    ostream *outputstream;
    /// The time when an instance of this class is created.
    TimeStamp starttime;
    /// If true the time passed since starttime will be sent to the output
    /// stream.
    bool showtime;
    /// If true the warning level used will be sent to the output stream.
    bool showlevel;
  public:
    /// Constructor
    basic_debugbuf(  ) : 
      outputlevel( 3 ),
      level( 0 ),
      outputstream( &cerr ),
      showtime(false),
      showlevel( true ){
      setLockMutexFunction( NULL );
      setUnlockMutexFunction( NULL );
    }

    /// Destructor
    virtual ~basic_debugbuf() {
      outputlevel=-1;
      sync();
    }

    /// Set a function(and optional argument) to be called before
    /// data is written to the current output stream. Can be used
    /// to do for example mutex locking if the stream is not 
    /// thread safe.
    inline void setLockMutexFunction( void (*func) (void * ), 
                                      void *arg = NULL ) {
      lock_mutex_func = make_pair( func, arg );
    }

    /// Set a function(and optional argument) to be called after
    /// data is written to the current output stream. Can be used
    /// to do for example mutex locking if the stream is not 
    /// thread safe.
    inline void setUnlockMutexFunction( void (*func) (void * ), 
                                        void *arg = NULL ) {
      unlock_mutex_func = make_pair( func, arg );
    }

    /// Set the variable showtime.
    void setShowTime( bool show ) { showtime = show; }

    /// Set the variable showlevel.
    void setShowLevel( bool show ) { showlevel = show; }

    /// Set the output stream to use.
    void setOutputStream( ostream &s ) { outputstream = &s; }

    /// Set the variable outputlevel.
    void setOutputLevel( int _outputlevel ) { outputlevel = _outputlevel; }

    /// Get the value of the variable outputlevel.
    int getOutputLevel() { return outputlevel; };

    /// Set the variable level.
    void setLevel( int _level ) { level = _level; }

    /// Get the ostream that is used as output stream.
    ostream &getOutputStream() { 
      return *outputstream;
    }
  
  protected:
    /// Send content of string buffer to output stream. Add information
    /// about level and time if it should be added.
    int sync() {
      if( lock_mutex_func.first )
        lock_mutex_func.first( lock_mutex_func.second );
      TimeStamp time;
      
      if ( outputlevel >= 0  &&  level >= outputlevel ) {
        if ( showlevel || showtime ){
          *outputstream << "[";
        }
        
        if ( showlevel ) {
          if ( level <= 2 ) {
            *outputstream << "I"; }
          else {
            *outputstream << "W"; }
        }
        
        if ( showlevel && showtime ) {
          *outputstream << " ";
        }
        
        if ( showtime ) {
          *outputstream << std::setfill('0')
                        << std::setprecision(2)
                        << std::setiosflags(std::ios::fixed)
                        << std::setw(6)
                        << (time-starttime)
            // Reset to default
                        << std::setfill(' ')
                        << std::setprecision(6)
                        << std::resetiosflags(std::ios::floatfield);
        }
        if ( showlevel || showtime ) {
          *outputstream << "] ";
        }
        
        *outputstream << std::basic_stringbuf<CharT, TraitsT>::str().c_str();
      }
      
      this->str( std::basic_string<CharT>() ); // Clear the string buffer
      
      if( unlock_mutex_func.first )
        unlock_mutex_func.first( unlock_mutex_func.second );
      return 0;
    }

  protected:
    pair< void (*)(void *), void * > lock_mutex_func;
    pair< void (*)(void *), void * > unlock_mutex_func;
    
  };


  /// Output stream used to print debug information. Use cerr by default.
  /// An instance of basic_dostream<char> is exported as "Console". See
  /// Console.h for example usage information of Console.
  template<class CharT, class TraitsT = std::char_traits<CharT> >
  class basic_dostream : public std::basic_ostream<CharT, TraitsT> {
  public:
  
    /// Constructor
    basic_dostream() : 
      std::basic_ostream<CharT, TraitsT>(new basic_debugbuf<CharT, TraitsT>()) {
    }

    /// Destructor
    ~basic_dostream() {
      delete std::ios::rdbuf(); 
    }

    /// Set a function(and optional argument) to be called before
    /// data is written to the current output stream. Can be used
    /// to do for example mutex locking if the stream is not 
    /// thread safe.
    inline void setLockMutexFunction( void (*func) (void * ), 
                                      void *arg = NULL ) {
      static_cast< basic_debugbuf<CharT, TraitsT>* >
        (std::ios::rdbuf())->setLockMutexFunction( func,arg );
    }

    /// Set a function(and optional argument) to be called after
    /// data is written to the current output stream. Can be used
    /// to do for example mutex locking if the stream is not 
    /// thread safe.
    inline void setUnlockMutexFunction( void (*func) (void * ), 
                                        void *arg = NULL ) {
      static_cast< basic_debugbuf<CharT, TraitsT>* >
        (std::ios::rdbuf())->setUnlockMutexFunction( func,arg );
    }

    /// Tell the output stream if time since instance of stream was created
    /// should be displayed.
    void setShowTime( bool show ) { 
      static_cast< basic_debugbuf<CharT, TraitsT>* >(std::ios::rdbuf())->
        setShowTime( show );  
    }

    /// Tell the output stream if it should display the warning level set.
    void setShowLevel( bool show ) { 
      static_cast< basic_debugbuf<CharT, TraitsT>* >(std::ios::rdbuf())->
        setShowLevel( show );  
    }

    /// Set the ostream to use as output stream.
    void setOutputStream( ostream &s ) { 
      static_cast< basic_debugbuf<CharT, TraitsT>* >(std::ios::rdbuf())->
        setOutputStream( s );  
    }

    /// Get the ostream that is used as output stream.
    ostream &getOutputStream() { 
      return static_cast< basic_debugbuf<CharT, TraitsT>* >(std::ios::rdbuf())->
        getOutputStream();  
    }

    /// Set the minimum level that will be used before displaying anything.
    void setOutputLevel( int _outputlevel ) {
      static_cast<basic_debugbuf<CharT, TraitsT>* >( std::ios::rdbuf() )->
        setOutputLevel( _outputlevel ); 
    }

    /// Get the minimum level that will be used before displaying anything.
    int getOutputLevel() {
      return
        static_cast<basic_debugbuf<CharT, TraitsT>* >( std::ios::rdbuf() )->
        getOutputLevel(); 
    }

    /// Set level of message. Must be greater than the minimum level set.
    void setLevel( int _level ) { 
      static_cast< basic_debugbuf<CharT, TraitsT>* >(std::ios::rdbuf())->
        setLevel( _level );  
    }

    /// Another way to call setLevel.
    basic_dostream & operator ()( int l ) {
      setLevel( l );
      return *this;
    }

  };

  typedef basic_dostream<char>    ConsoleStream;

  // Instance of basic_dostream<char>
  extern H3DUTIL_API ConsoleStream Console;

}
#endif
