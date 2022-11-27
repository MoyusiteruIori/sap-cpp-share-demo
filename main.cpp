#include <iostream>
#include <vector>
#include <string>
#include <utility>

namespace my {
    template<typename T>
    class vector {
    public:
        using iterator = T*;
        using const_iterator = const T*;
    
    private:
        T* m_data;
        size_t m_capacity;
        size_t m_size;

    public:
        constexpr vector() noexcept : m_data(nullptr), m_capacity(0), m_size(0) {}

        vector(const vector<T>& rhs) {
            this->m_capacity = rhs.m_capacity;
            this->m_size = 0;
            this->m_data = static_cast<T*>(::operator new(sizeof(T) * this->m_capacity));
            try {
                for (size_t i = 0; i < rhs.m_size; ++i) {
                    new(&this->m_data[i]) T(rhs.m_data[i]);
                    this->m_size += 1;
                }
            }
            catch(...) {
                this->destruct();
                throw;
            }
        }

        vector(vector<T>&& rhs) {
            this->m_capacity = rhs.m_capacity;
            this->m_size = rhs.m_size;
            this->m_data = rhs.m_data;
            rhs.m_capacity = 0, rhs.m_size = 0, rhs.m_data = nullptr;
        }

        ~vector() {
            this->destruct();
        }

        template<typename... Ts>
        T& emplace_back(Ts&&... args) {
            if (this->m_size < this->m_capacity) {
                size_t pos = this->m_size;
                new(&this->m_data[pos]) T(std::forward<Ts>(args)...);
                this->m_size += 1;
                return this->m_data[pos];
            }
            size_t new_capacity = this->m_capacity * 2 + 1;
            T* new_data = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
            size_t new_size = 0;
            try {
                for (size_t i = 0; i < this->m_size; ++i) {
                    new(&new_data[i]) T(std::move_if_noexcept(this->m_data[i]));
                    new_size += 1;
                }
                new(&new_data[new_size]) T(std::forward<Ts>(args)...);
                new_size += 1;
            }
            catch (...) {
                for (size_t i = 0; i < new_size; ++i) {
                    new_data[i].~T();
                }
                if (new_data) {
                    ::operator delete(new_data);
                }
                throw;
            }
            for (size_t i = 0; i < this->m_size; ++i) {
                this->m_data[i].~T();
            }
            if (this->m_data) {
                ::operator delete(this->m_data);
            }
            this->m_size = new_size;
            this->m_capacity = new_capacity;
            this->m_data = new_data;
            return this->m_data[new_size - 1];
        }

        void push_back(const T& e) {
            this->emplace_back(e);
        }

        void push_back(T&& e) {
            this->emplace_back(std::move(e));
        }

        bool empty() const noexcept {
            return this->m_size == 0;
        }

        size_t size() const noexcept {
            return this->m_size;
        }

        iterator begin() noexcept {
            return m_data;
        }

        const_iterator begin() const noexcept {
            return this->m_data;
        }

        iterator end() noexcept {
            return this->m_data + this->m_size;
        }

        const_iterator end() const noexcept {
            return this->m_data + this->m_size;
        }

        void clear() {
            for (size_t i = 0; i < this->m_size; ++i) {
                this->m_data[i].~T();
            }
            this->m_size = 0;
        }

    private:
        void destruct() {
            this->clear();
            if (this->m_data) {
                ::operator delete(this->m_data);
            }
        }

    };
}

int main() {
    my::vector<std::string> v;
    v.emplace_back("hello");
    v.emplace_back("hello world", 5);
    v.emplace_back();
    std::cout << v.size() << std::endl;
    for (auto&& s : v) {
        std::cout << s << std::endl;
    }
    v.clear();
    std::cout << v.empty() << std::endl;
    return 0;
}