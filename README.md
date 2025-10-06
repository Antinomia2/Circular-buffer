# Circular-buffer

``cbuffer<T>`` is a data structure that implements a fixed size circular array.<br>
When full, new elements automatically overwrite the oldest (FIFO).<br>

## Features

- Raw by design: no STL containers, no smart pointers, manual memory management only
- Fixed capacity defined at construction time
- Insertion/removal in O(1): add_tail(), remove_head(), remove_tail()
- Minimal support to forward iterators
- Ctors: default; with capacity; cctor; from generic data sequence that comes with iterator begin-end
- Read/write by logical index: operator[](x) accesses the x-th oldest element
- Utilities: print_logical_buffer(), print_physical_buffer()

## Private data members

- _buffer: pointer to the array
- _capacity: maximum capacity of the array
- _currentSize: number of elements currently stored
- _head: index of the oldest element in the array
- _tail: index of the most recent element in the array

## Conventions

- _buffer[_head] = oldest element
- _buffer[_tail] = most recent element
- cbuffer[0] = oldest element
- cbuffer[_currentSize - 1] = most recent element
- _currentSize <= _capacity
- cbuffer.begin() = iterator to the oldest element
- cbuffer.end() = iterator one step past the most recent element (dummy, does not point to a real element in the buffer)
