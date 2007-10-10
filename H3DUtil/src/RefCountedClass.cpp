
#include <H3DUtil/RefCountedClass.h>

using namespace H3DUtil;

RefCountedClass::RefCountedClass( ):
      ref_count( 0 ),
      name( "" ),
      type_name( "RefCountedClass" ),
      is_initialized( false ),
      manual_initialize( false ),
      use_ref_count_lock( false ),
      ref_count_lock_pointer(0) {
}

RefCountedClass::RefCountedClass( bool _use_lock ):
      ref_count( 0 ),
      name( "" ),
      type_name( "RefCountedClass" ),
      is_initialized( false ),
      manual_initialize( false ),
      use_ref_count_lock( _use_lock ){
  if( _use_lock ) {
    ref_count_lock_pointer = new MutexLock();
  }
}

RefCountedClass::~RefCountedClass() {
#ifdef REF_COUNT_DEBUG
  Console(1) << "~RefCountedClass: " << this << endl;
#endif
  if( use_ref_count_lock ) {
    delete ref_count_lock_pointer;
  }
}

void RefCountedClass::ref() {
  if( use_ref_count_lock )
    ref_count_lock_pointer->lock();
  ref_count++;
#ifdef REF_COUNT_DEBUG
  Console(1) << "Ref " << getName() << " " << this << ": " 
    << ref_count << endl;
#endif
  if( !manual_initialize && ref_count == 1 ) {
    initialize();
  }
  if( use_ref_count_lock )
    ref_count_lock_pointer->unlock();
}

void RefCountedClass::unref() {
  if( use_ref_count_lock )
    ref_count_lock_pointer->lock();
  ref_count--;
#ifdef REF_COUNT_DEBUG
  Console(1) << "Unref " << getName() << " " << this << ": " 
    << ref_count << endl;
#endif
  if( ref_count == 0 ) {
    if( use_ref_count_lock )
      ref_count_lock_pointer->unlock();
    delete this;
  }
  else {
    if( use_ref_count_lock )
      ref_count_lock_pointer->unlock();
  }
}
