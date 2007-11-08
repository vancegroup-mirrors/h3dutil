
#include <H3DUtil/RefCountedClass.h>

using namespace H3DUtil;

RefCountedClass::RefCountedClass( ):
      ref_count( 0 ),
      name( "" ),
      type_name( "RefCountedClass" ),
      is_initialized( false ),
      manual_initialize( false ),
      ref_count_lock_pointer(0) {
}

RefCountedClass::RefCountedClass( bool _use_lock ):
      ref_count( 0 ),
      name( "" ),
      type_name( "RefCountedClass" ),
      is_initialized( false ),
      manual_initialize( false ) {
  if( _use_lock ) {
    ref_count_lock_pointer = new MutexLock();
  }
}

RefCountedClass::~RefCountedClass() {
#ifdef REF_COUNT_DEBUG
  Console(1) << "~RefCountedClass: " << this << endl;
#endif
  if( ref_count_lock_pointer ) {
    delete ref_count_lock_pointer;
    ref_count_lock_pointer = 0;
  }
}

void RefCountedClass::ref() {
  if( ref_count_lock_pointer )
    ref_count_lock_pointer->lock();
  ref_count++;
#ifdef REF_COUNT_DEBUG
  Console(1) << "Ref " << getName() << " " << this << ": " 
    << ref_count << endl;
#endif
  if( !manual_initialize && ref_count == 1 ) {
    initialize();
  }
  if( ref_count_lock_pointer )
    ref_count_lock_pointer->unlock();
}

void RefCountedClass::unref() {
  if( ref_count_lock_pointer )
    ref_count_lock_pointer->lock();
  ref_count--;
#ifdef REF_COUNT_DEBUG
  Console(1) << "Unref " << getName() << " " << this << ": " 
    << ref_count << endl;
#endif
  if( ref_count == 0 ) {
    if( ref_count_lock_pointer )
      ref_count_lock_pointer->unlock();
    delete this;
  }
  else {
    if( ref_count_lock_pointer )
      ref_count_lock_pointer->unlock();
  }
}
