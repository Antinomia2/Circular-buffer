#ifndef cbuffer_H
#define cbuffer_H

#include <iostream>
#include <cassert>

/**
  @file cbuffer.h
  @brief Declaration of the cbuffer class
*/

/**
  @brief circular buffer class

  Class that represents a circular array of generic elements of type <T>.
  The maximum capacity of the array is defined when constructing the cbuffer.
  Once the maximum capacity is reached, new elements overwrite
  the oldest ones, a classic FIFO structure.
*/
template <typename T>
class cbuffer{
    public:
        typedef unsigned int size_type;
        typedef T value_type;

        /**
            @brief Default constructor (Fundamental method)

            Default constructor to instantiate an empty cbuffer.
        */
        cbuffer(): _buffer(nullptr), _capacity(0), _currentSize(0), _head(0), _tail(0) {
            #ifndef NDEBUG
            std::cout<<"cbuffer()"<<std::endl;
            #endif
        }

        /**
            @brief Secondary constructor (Fundamental method)

            Secondary constructor that creates a cbuffer with a user-defined capacity

            @param capacity maximum capacity of the cbuffer
        */
        explicit cbuffer(size_type capacity): _buffer(nullptr), _capacity(0), _currentSize(0), _head(0), _tail(0){
            _buffer = new value_type[capacity];
            _capacity=capacity;
            #ifndef NDEBUG
            std::cout<< "cbuffer(size_type)" << std::endl;
            #endif
        }

        /**
            @brief Copy constructor (Fundamental method)

            Copy constructor from one cbuffer to another.

            @param other cbuffer to copy
        */
        cbuffer(const cbuffer &other): _buffer(nullptr), _capacity(0), _currentSize(0), _head(0), _tail(0){
            this->_buffer = new value_type[other._capacity];
            try{
                this->_capacity = other._capacity;
                this->_currentSize = other._currentSize;
                this->_head = other._head;
                this->_tail = other._tail;
                for(size_type i=0; i < this->_currentSize; ++i)
                    (*this)[i] = other[i];
            } catch(...){
                clear();
                throw;
            }
            #ifndef NDEBUG
            std::cout<< "cbuffer(const cbuffer&)" << std::endl;
            #endif
        }
        
        /**
            @brief Assignment operator (Fundamental method)

            Assignment operator that copies the content of 
            other into *this.

            @param other cbuffer to copy
            @return reference to this cbuffer
        */
        cbuffer<T>& operator=(const cbuffer<T> &other) {
            if (this != &other) {
                cbuffer tmp(other);
                tmp.swap(*this);
            }
            #ifndef NDEBUG
            std::cout<< "operator=(const cbuffer<T>&)" << std::endl;
            #endif
            return *this;
        }

        /**
            @brief Method that swaps two cbuffers

            Swaps the contents of two cbuffers

            @param other the cbuffer that swaps contents with the caller
        */
        void swap(cbuffer &other) {
            std::swap(this->_buffer, other._buffer);
            std::swap(this->_capacity, other._capacity);
            std::swap(this->_currentSize, other._currentSize);
            std::swap(this->_head, other._head);
            std::swap(this->_tail, other._tail);
        }

        /**
            @brief Destructor (Fundamental method)

            Destructor of the cbuffer class
        */
        ~cbuffer(){
            clear();
            #ifndef NDEBUG
            std::cout<< "~cbuffer()" << std::endl;
            #endif
        }

        /**
            @brief Empties the cbuffer and frees memory

            Frees from the heap the allocated array,
            while setting the variables to a safe and consistent state.
        */
        void clear(){
            delete[] _buffer;
            _buffer = nullptr;
            _capacity = 0;
            _currentSize = 0;
            _head=0;
            _tail=0;
        }

        /**
            @brief Logical print of the elements

            Prints the elements of the cbuffer in logical view.
            Prints the elements in order from the oldest ( cbuffer[0] ) to the most recent ( cbuffer[_currentSize-1] ).
        */
        void print_logical_buffer() const{
            if(_currentSize!=0){
                std::cout << "Stampa degli elementi inseriti a vista logico: ";
                for(size_type i = 0; i < _currentSize; ++i)
                    std::cout <<"cbuffer["<<i<<"]="<< (*this)[i] << "  ";
                std::cout << std::endl;
            }
            else
                std::cout <<"il cbuffer è vuoto" <<std::endl;
        }

        /**
            @brief Physical print of the elements

            Prints the elements of the cbuffer in physical view.
            Prints the corresponding index of the physical array where each element is stored.
        */
        void print_physical_buffer() const{
            if(_currentSize!=0){
                std::cout << "Stampa degli elementi inseriti a vista fisico: ";
                for(size_type i=0; i<_currentSize;i++)
                    std::cout << "_buffer["<<(i+_head) % _capacity<<"]="<<*(_buffer+ ((i+_head) % _capacity)) <<"  ";
                std::cout << std::endl;
            }
            else
                std::cout <<"il cbuffer è vuoto" <<std::endl;
        }

        /** 
            @brief Builds a cbuffer from a sequence of generic iterators with user-defined capacity.
        
            Constructor that allows creating
            a cbuffer starting from a generic data sequence
            of type Q identified by a pair of generic iterators

            @param capacity capacity of the cbuffer to create
            @param begin begin iterator of the sequence
            @param end end iterator of the sequence

            @throw std::bad_alloc possible allocation exception
        */
        template <typename Q>
        cbuffer(size_type capacity, Q begin, Q end): _buffer(nullptr), _capacity(0), _currentSize(0), _head(0), _tail(0){
            _buffer = new value_type[capacity];
            _capacity=capacity;
            try{
                for( ; begin!=end; ++begin)
                    add_tail(static_cast<T>(*begin));
            } catch (...){
                clear();
                throw;
            }
        }

        /**
            @brief Requirement #2 method

            Allows reading and writing at cbuffer[index].
            This function is implemented so that:
            cbuffer[0] is a reference to the oldest element in the physical array;
            cbuffer[_currentSize - 1] is a reference to the most recent element in the physical array.

            @param index logical array cell to read/write

            @return reference to cbuffer[index]

            @pre index < get_currentSize()
        */
        value_type& operator[](size_type index) {
            assert(index < _currentSize);
            return *(_buffer + ((_head + index) % _capacity));
        }

        /**
            @brief Requirement #2 method (const version)

            Allows reading at cbuffer[index].
            This function is implemented so that:
            cbuffer[0] is a reference to the oldest element in the physical array;
            cbuffer[_currentSize - 1] is a reference to the most recent element in the physical array.

            @param index logical array cell to read

            @return reference to cbuffer[index]

            @pre index < get_currentSize()
        */
        const value_type& operator[](size_type index) const {
            assert(index < _currentSize);
            return *(_buffer + ((_head + index) % _capacity));
        }
        
        /**
            @brief Requirement #3 method

            Adds an element to the tail of the buffer

            @param value element to insert at the tail of the buffer

            @post The new element is inserted at the tail of the buffer
            @post If the physical array was not "full", _currentSize is incremented by 1.
            @post If the physical array was full, _head is incremented by 1 to point to the new oldest element
            @post The tail index is updated to point to the new most recent element

            @throw std::bad_alloc possible allocation exception
        */
        void add_tail(const T& value){
            _buffer[(_head + _currentSize) % _capacity] = value;
            if (_currentSize < _capacity)
                ++_currentSize;
            else
                _head = (_head + 1) % _capacity;
            _tail = (_head + _currentSize - 1) % _capacity;
        }

        /**
            @brief Requirement #4 method

            Deletes the oldest element present in the physical array

            @pre get_currentSize() > 0
            @post the oldest element in the physical array is "deleted"
            @post _currentSize is decremented by 1
        */
        void remove_head(){
            assert(_currentSize > 0);
            _head = (_head + 1) % _capacity;
            --_currentSize;
            if (_currentSize == 0){
                _head=0;
                _tail=0;
            }
        }

        /**
            @brief Requirement #5 method

            Deletes the most recent element present in the physical array

            @pre get_currentSize() > 0
            @post the most recent element in the physical array is "deleted"
            @post _currentSize is decremented by 1
        */
        void remove_tail(){
            assert(_currentSize > 0);
            _tail = (_tail + _capacity - 1) % _capacity;
            --_currentSize;
            if (_currentSize == 0){
                _head=0;
                _tail=0;
            }
        }

        /**
            @brief getter of the head index

            Returns the index in the array where the oldest element is stored

            @return (physical) index of the oldest element in the array
        */
        const size_type& get_head_physical_index() const{
            return _head;
        }

        /**
            @brief getter of the tail index

            Returns the index in the array where the most recent element is stored

            @return (physical) index of the most recent element in the array
        */
        const size_type& get_tail_physical_index() const{
            return _tail;
        }

        /**
            @brief head getter

            Allows access to the oldest element

            @return the oldest element present in the array
        */
        const value_type& get_head() const{
            return (*this)[0];
        }

        /**
            @brief tail getter

            Allows access to the most recent element

            @return the most recent element present in the array
        */
        const value_type& get_tail() const{
            return (*this)[_currentSize-1];
        }

        /**
            @brief getter of the number of elements present

            Returns a value equal to the number of elements currently present.

            @return number of elements present
        */
        const size_type& get_currentSize() const{
            return _currentSize;
        }

        /**
            @brief getter of the maximum capacity of the array

            Returns a value equal to the maximum capacity of the circular buffer.

            @return maximum capacity
        */
        const size_type& get_capacity() const{
            return _capacity;
        }

        /**
            @brief Implementation of Requirement #6

            Data structure that contains the minimal code to support forward iterators for reading and writing
        */
        struct iterator {
            value_type *buffer; // Pointer to the array
            size_type capacity; // used to know the end of the physical array
            size_type index; // array index
            size_type count; // used to implement circular iteration

            /**
                @brief constructor

                initialization of the variables
            */
            iterator(value_type *buf, size_type cap, size_type ind, size_type c): buffer(buf), capacity(cap), index(ind), count(c) {}

            /**
                @brief Overload of operator *()

                So that dereferencing the object (iterator) yields
                the element at position index of the physical array.
                This function also allows reading/writing the element
                obtained by dereferencing at position index.

                @return element at position index of the array
            */
            T& operator*() {
                return buffer[index];
            }

            /**
                @brief Overload of pre-increment operator

                Implements the logic so that
                when the iterator reaches the end of the physical array it wraps around

                @return returns the iterator updated after using pre-increment
            */
            iterator& operator++() {
                index = (index + 1) % capacity;
                --count;
                return *this;
            }

            /**
                @brief Overload of operator !=

                Implements the logic so that
                begin equals end only when all elements have been iterated,
                using the variable count as the evaluation criterion

                @return true if the two iterators have different count values
            */
            bool operator!=(const iterator& other) const {
                return count!= other.count;
            }

        };

        // Returns the iterator to the beginning of the data sequence
        iterator begin() {
            return iterator(_buffer,_capacity,_head,_currentSize);
        }

        // Returns the iterator to the end of the data sequence
        iterator end() {
            return iterator(_buffer,_capacity,_tail+1, 0);
        }

        /**
            @brief Implementation of Requirement #6 (const)

            Data structure that contains the minimal code to support forward iterators for reading
        */
        struct const_iterator {
            const value_type *buffer; // Pointer to the array
            size_type capacity; // used to know the end of the physical array
            size_type index; // array index
            size_type count; // used to implement circular iteration

            /**
                @brief constructor

                initialization of the variables
            */
            const_iterator(value_type *buf, size_type cap, size_type ind, size_type c): buffer(buf), capacity(cap), index(ind), count(c) {}

            /**
                @brief Overload of operator *()

                So that dereferencing the object (iterator) yields
                the element at position index of the physical array.
                This function allows only reading the element
                obtained by dereferencing at position index.

                @return element at position index of the array
            */
            const T& operator*() const {
                return buffer[index];
            }

            /**
                @brief Overload of pre-increment operator

                Implements the logic so that
                when the iterator reaches the end of the physical array it wraps around

                @return returns the iterator updated after using pre-increment
            */
            const_iterator& operator++() {
                index = (index + 1) % capacity;
                --count;
                return *this;
            }

            /**
                @brief Overload of operator !=

                Implements the logic so that
                begin equals end only when all elements have been iterated,
                using the variable count as the evaluation criterion

                @return true if the two iterators have different count values
            */
            bool operator!=(const const_iterator& other) const {
                return count!= other.count;
            }

        };

        // Returns the iterator to the beginning of the data sequence
        const_iterator begin() const {
            return const_iterator(_buffer,_capacity,_head,_currentSize);
        }

        // Returns the iterator to the end of the data sequence
        const_iterator end() const {
            return const_iterator(_buffer,_capacity,_tail+1, 0);
        }
        
    private:
        value_type *_buffer; // Pointer to the array
        size_type _capacity; // maximum capacity of the array
        size_type _currentSize; // elements currently present in the array
        size_type _head; // index of the cell containing the oldest element in the array
        size_type _tail; // index of the cell containing the most recent element in the array
};

#endif