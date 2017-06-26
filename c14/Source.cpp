#include <string>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <memory>
#include <vld.h>

class A {
	private:
	double* _arr;
	unsigned _size;
public:
	A(unsigned size) {
		_size = size;
		_arr = new double[_size];
		for (unsigned i = 0; i < _size; i++) {
			_arr[i] = i;
		}
	}
	A(const A& a) {
		_size = a._size;
		_arr = new double[_size];
		for (unsigned i = 0; i < _size; i++) {
			_arr[i] = a._arr[i];
		}
	}
	A(A&& a) {
		_size = a._size;
		_arr = a._arr;
		a._arr = nullptr;
		a._size = 0;
	}

	A& operator=(const A& other) 
	{
		if (this != &other) { 
			if (other._size != _size) {         
				delete[] _arr;              
				_size = 0;           
				_arr = new double[other._size]; 
				_size = other._size;
			}
			for (unsigned i = 0; i < other._size; i++) {
				_arr[i] = other._arr[i];
			}
		}
		return *this;
	}

	A& operator=(A&& other) noexcept {
		if (this != &other) {
			delete[] _arr;
			_arr = std::exchange(other._arr, nullptr);
			_size = std::exchange(other._size, 0);
		}
		return *this;
	}

	~A() {
		if (_arr == nullptr)
			return;
		delete[] _arr;
	}

	friend std::ostream& operator<<(std::ostream& os, const A& obj) {
		os << "{ ";
		for (unsigned i = 0; i < obj._size; i++) {
			os << obj._arr[i] << " ";
		}
		os << "}";
		return os;
	}
};

namespace my {
	using sizeT = unsigned;
	template<typename ValueType>
	class list {
		class inner_list {
		public:
			std::shared_ptr<inner_list> _next{ nullptr };
			std::weak_ptr<inner_list> _before;

			std::shared_ptr<ValueType> _val;

		public:
			inner_list() {  }
		};

		
		std::shared_ptr<inner_list> _first{ nullptr };
		std::shared_ptr<inner_list> _last{ nullptr };
		sizeT _size{ 0 };
	public:

		
		class iterator {
		private:
			std::shared_ptr<inner_list> _ptr{ nullptr };
		public:
			iterator() {}
			iterator(std::shared_ptr<inner_list>& il) {
				_ptr = il;
			}

			std::shared_ptr<ValueType>& operator*() {
				return _ptr->_val;
			}

			iterator& operator++() {
				_ptr = _ptr->_next;
				return *this;
			}

			friend bool operator!=(typename const list::iterator& c1, typename const list::iterator & c2) {
				if (c1._ptr != c2._ptr) {
					return true;
				}
				return false;
			}
		};

		
		list() { 
			_first = std::make_shared<inner_list>(); 
			_last = _first;
		}

		list(const list& l) {
			_first = std::make_shared<inner_list>();
			_last = _first;
			_size = l._size;
			if (l._size == 0) {
				return;
			}

			auto tempS = l._first;
			auto tempD = _first;
			while (tempS->_next != nullptr) {
				tempD->_val = std::make_shared<ValueType>(*(tempS->_val));
				tempD->_next = std::make_shared<inner_list>();
				tempD->_next->_before = tempD;
				tempD = tempD->_next;
				tempS = tempS->_next;
			}
			tempD->_val = std::make_shared<ValueType>(*(tempS->_val));
			tempD->_next = nullptr;
			_last = tempD;
			
		}
		iterator begin() {
			return iterator(_first);
		}

		iterator end() {
			return iterator();
		}

		template<typename ArgType>
		void push_front(ArgType&& val) {
			if (_size == 0) {
				_first->_val = std::make_shared<ValueType>(std::forward<ArgType>(val));
			}
			else {
				auto temp = std::make_shared<inner_list>();
				temp->_val = std::make_shared<ValueType>(std::forward<ArgType>(val));
				temp->_next = _first;
				_first->_before = temp;
				_first = temp;
			}
			_size++;
		}

		void pop_front() {
			if (_size == 0) {
				return;
			}
			else if (_size == 1) {
				_first->_val = 0;
			}
			else {
				_first = _first->_next;
			}
			_size--;
		}


	};

}

int main() {

	auto p = std::make_shared<my::list<A>>();
	p->push_front(10);
	p->push_front(6);
	auto pp = std::make_shared<my::list<A>>();
	*pp = *p;
	
	for (auto q : *p) {
		std::cout << *q << std::endl;
	}
	p.reset();
	for (auto q : *pp) {
		std::cout << *q << std::endl;
	}
	A f(1);
	my::list<A> listA{};
	listA.push_front(std::move(A(5)));
	listA.push_front(A(4));
	listA.push_front(A(3));
	listA.push_front(f);
	auto b = listA.begin();
	auto it_end = listA.end();
	while (b != it_end) {
		std::cout << **b << std::endl;
		++b;
	}
	for (auto aaa : listA) {
		std::cout << *aaa << std::endl;
	}
	return 0;
}
