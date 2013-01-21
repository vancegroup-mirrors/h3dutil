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
/// \file RefCountedClass.h
/// \brief Header file for RefCountedClass class.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __REFCOUNTEDCLASS_H__
#define __REFCOUNTEDCLASS_H__

#include <H3DUtil/Threads.h>
#include <H3DUtil/Console.h>
#include <string>
#include <iostream>

using namespace std;

namespace H3DUtil {

  /// Base class for all classes that should be reference counted.
  class H3DUTIL_API RefCountedClass {
  public:

    /// Constructor.
    RefCountedClass( );

    /// Constructor
    /// \param _use_lock If true locks will be used when reference counting.
    /// Used to make class thread safe.
    RefCountedClass( bool _use_lock );


    /// Destructor.
    virtual ~RefCountedClass();

    /// Initialize is called once upon the first reference of the 
    /// RefCountedClass.
    virtual void initialize() {
      is_initialized = true;
    }

    /// Increase the reference count for this instance.
    void ref();

    /// If true, the initialize() function will not be called automatically
    /// on first reference of the instance but must be called manually
    /// by the creator of the instance.
    inline void setManualInitialize( bool b ) {
      manual_initialize = b;
    }

    /// Returns if the RefCountedClass has to be manually initialized or not.
    inline bool getManualInitialize() {
      return manual_initialize;
    }

    /// Decrease the reference count for this instance. If the reference
    /// count reaches 0 it is deleted.
    void unref();

    /// Get the name of the node.
    inline string getName() { 
      if( name == "" )
        return string( "Unnamed " ) + getTypeName();
      else 
        return name; 
    }

    /// Set the name of the node.
    inline void setName( const string &_name ) { 
      name = _name;
    }
    
    /// Returns true if this node has been given a name.
    inline bool hasName() { return name != ""; }

    /// Get the name of this Node type. E.g. if the Node is an IndexedFaceSet
    /// it should return "IndexedFaceSet"
    inline string getTypeName() {
      return type_name;
    }

    /// Returns true if the initialize() function has been called.
    inline bool isInitialized() {
      return is_initialized;
    }

  protected:
    /// The number of references to this instance.
    unsigned int ref_count; 

    /// The name
    string name;

    /// String version of the name of the type.
    string type_name;

    /// true if initialize() function has been called.
    bool is_initialized;

    /// If true, the initialize() function will not be called automatically
    /// on first reference of the instance but must be called manually
    /// by the creator of the instance.
    bool manual_initialize;

    /// Lock for thread access to ref_count.
    MutexLock *ref_count_lock_pointer;
  };
    
}

#endif
