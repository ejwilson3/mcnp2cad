//- Class: DLIList
//-
//- Assumptions: All data are stored contiguously in the array with 
//-              empty slots at the end of the array.
//-
//- Owner: Darryl Melander

#ifndef DLILIST_HPP
#define DLILIST_HPP

#include "CubitMessage.hpp"
#include <cstring>
#include <vector>


#define DLI_COUNT_INCREMENT 8
#define DLI_COUNT_FACTOR 1.5

//! A list class, similar to a std::vector<>.
/*! DLIList is implemented as an array that is grown by a
specified amount when the list becomes full.
Most insertions and deletions at any point other than the end
of the list are handled inefficiently 
since all data from that point to the end is bubbled
down/up to fill/create the void. Operators {+} and {+=} 
are provided as an efficient means of assigning one list
to another or appending one list onto another.  The list has
a current position, which is a zero-based index into the 
list.  Many of the member functions operate on the current 
item (the item at the current position) or move the current
position.
*/
template<class X> class DLIList
{
public:

  
  //! Constructor: Create a list, allocating enough storage for \a size elements.
  /*! Although enough space is allocated for \a size elements, the list starts
  out empty (the size() function will return 0).  If the requested memory size
  is 0, then storage is not allocated until the first element is added to the list. 
  Additional storage space will be allocated if the list grows beyond its
  original size.
  \param size The amount of storage to pre-allocate for this list.
  */
  explicit DLIList (int size = 0);

  //! Destructor: Free all resources used by this list.
  /*! The list and its storage space are freed.  Note that if this is a list
  of pointers, this destructor will \a NOT delete the objects whose pointers
  are stored in the list.
  */
  ~DLIList();

  //! Set the pointer to the beginning of the list.
  void reset();
 
  //! Delete all elements in the list.
  /*! This function does not release memory already allocated for the list. This
  call is more efficient than creating a new list repeatedly within a loop.
  */
  void clean_out();
 
  //! Append one list to another list.
  /*!  This is the most efficient way to append two lists together. 
  \param from The list to be appended to this list.
  \return A reference to this list.
  */
  DLIList<X>& operator+=(const DLIList<X>& from);

  //! Gets a reference to the element with the given index.
  /*! The index is zero-based.
  \param index The index to the desired element.
  \return A reference to the indicated element.
  */
  X & operator[](int index) const;

  //! Appends the new item to the list, but only if it isn't already in the list.
  /*! In either case, the current position is not changed.
  \return CUBIT_TRUE if the item was added, otherwise CUBIT_FALSE.
  */
  CubitBoolean append_unique(X new_item);
  
  //! Returns the value of the current item.
  /*! \return The value of the current item. If the list is empty, X(0) is returned.
  */
  X get() const;
 
  //! Returns the current value, then advances the current position by one.
  /*! If the current position is at the end of the list, the function will
  wrap to the beginning of the list.
  \return The value at the current position, before stepping.
  */
  X get_and_step ();

  //! Place an item at the end of the list.
  /*! This is the most efficient way to insert an item to the list.
  The current position is unchanged.
  \param val The value to place at the end of the list.  
  */
  void append(X new_item);

  //! Returns the number of items in the list
  /*! \return The number of items current in the list. */
  int size() const
    { return itemCount; }
 
  //! Return the index of an item in list, or -1 if the item is not in the list.
  /*! The location of the first instance of \a val is returned as a zero-based
  index from the start of the list.  The list is searched starting from the 
  current position, wrapping to the front of the list if necessary.
  \param val The value to search for.
  \return The index of the first instance of \a val, or -1 if the value is not found.
  */
  int where_is_item(X val) const;

  //! Copy this list's contents into an array.
  /*! It is assumed that \a other_array is big enough to hold all of this
  list's elements.  No check is made to verify this.
  \param other_array The array into which this list's contents will be copied.
  */
  void copy_to(X *other_array);

private:

  void lengthen_list(int by_how_much = DLI_COUNT_INCREMENT,
                     double by_what_factor = DLI_COUNT_FACTOR );
    //- Makes the array bigger. Multiply current size 
    //- "by_what_factor" and add 'by_how_much'.
  
  int  index;      // Index of current item in {listArray}
  X   *listArray;  // Array of values
  int  listLength; // Number of allocated spaces in array
  int  itemCount;  // Number of items stored in array
  
};

template <class X> inline
X & DLIList<X>::operator[](int index1) const
{
  if(index1 < 0 || index1 >= itemCount)
    throw std::out_of_range ("Index out of Bounds");
    //assert( index >= 0 && index < itemCount );
    return listArray[index1];
}

template <class X> inline void DLIList<X>::reset()
{
  index = 0;
}

template <class X> inline void DLIList<X>::clean_out()
{
  itemCount = 0;
  index = 0;
}

template <class X> inline void DLIList<X>::append(X new_item)
{
    // see if the list must be lengthened
  if (itemCount == listLength)
    lengthen_list();
  
  listArray[itemCount++] = new_item;
}

template <class X> inline X DLIList<X>::get() const
{
  if ( !itemCount )
  {
    throw std::out_of_range("Attempted get of empty DLIList\n");
    /*PRINT_WARNING("Attempted get of empty DLIList\n");
    return (X)0;*/
  }
  else
  {
    return listArray[index];
  }
}

template <class X> inline int DLIList<X>::where_is_item (X item) const
{
  if (itemCount == 0)
    return -1;
  
    // loop through list searching for item ...
    // if found return index
  
    // Search from current index to end of array
  int i;
  for (i=index; i < itemCount; i++)
    if (listArray[i] == item)
      return i;
  
    // Now search from beginning of array to index...
  for (i = 0; i < index && i < itemCount; i++)
    if (listArray[i] == item)
      return i;
  
    // item is not in array, return -1
  return -1;
}
//- Append this new_item to the list if it doesn't already exist in it.
//- Make sure the index is unchanged.

template <class X> inline CubitBoolean DLIList<X>::append_unique(X new_item)
{
    // Append new_item, if it isn't already there.
  if( where_is_item(new_item) < 0 ) 
  {
    append (new_item);
    return CUBIT_TRUE;
  }
  return CUBIT_FALSE;
}

//FROM DLILIST.CPP

#ifdef INLINE_TEMPLATES
#define MY_INLINE inline
#else
#define MY_INLINE
#endif

template <class X> MY_INLINE void DLIList<X>::copy_to(X *other_array)
    //- copy this list's listArray into other_array
{
  if(other_array == 0)
    throw std::invalid_argument("Array is NULL");
  //assert(other_array != 0);
  if (itemCount)
    memcpy (other_array, listArray, itemCount*sizeof(X));
}


//END OF DLILIST.CPP


#endif //- DLILIST_HPP

