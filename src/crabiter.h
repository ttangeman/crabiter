#pragma once

#include <assert.h>
#include <stdint.h>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

// Forward declarations
namespace crab {       
    template<typename T> struct Optional;

    template<typename DerivedType> struct Adapter;
    template<typename IterType> class Filter;

    template<typename ContainerType> class Iterator;
    template<typename ContainerType> class IteratorMut;
    template<typename ContainerType> class IntoIterator;

    namespace detail {
        // Necessary for being able to derive the ValueType from the Adapter
        // (i.e. the iterator type is a derived CRTP parameter on the
        // adapter and it cannot refer to member types in the iterator type 
        // as the base class).
        template<typename DerivedType> struct IterTraits;
    }
}

namespace crab {
    template<typename ContainerType>
    class Iterator : public Adapter<Iterator<ContainerType>> {
    public:
        using ValueType = typename detail::IterTraits<Iterator<ContainerType>>::ValueType;

        Iterator(ContainerType const& container) 
            : m_begin(container.begin()), m_end(container.end()) {}

        Optional<ValueType> next() {
            if (m_begin != m_end) {     
                return *m_begin++;
            }
            return {};
        }    

    private:
        ContainerType::const_iterator m_begin;
        ContainerType::const_iterator m_end;
    };

    template<typename ContainerType>
    class IteratorMut {
    public:
        using ValueType = typename detail::IterTraits<IteratorMut<ContainerType>>::ValueType;

        IteratorMut(ContainerType& container) 
            : m_begin(container.begin()), m_end(container.end()) {}

        Optional<ValueType&> next() {
            if (m_begin != m_end) {     
                return *m_begin++;
            }
            return {};
        }    

    private:
        ContainerType::iterator m_begin;
        ContainerType::iterator m_end;    
    };

    template<typename ContainerType>
    class IntoIterator {
    public:
        using ValueType = typename detail::IterTraits<IntoIterator<ContainerType>>::ValueType;

        IntoIterator(ContainerType&& container) 
            : m_begin(container.begin()), m_end(container.end()) {}

        Optional<ValueType> next() {
            if (m_begin != m_end) {     
                return std::move(*m_begin++);
            }
            return {};
        }    

    private:
        ContainerType::iterator m_begin;
        ContainerType::iterator m_end;
    };

    template<typename IterType>
    class Filter : public Adapter<Filter<IterType>> {
    public:
        using ValueType = typename IterType::ValueType;

        Filter(IterType iter, std::function<bool(ValueType const&)>&& functor) 
            : m_iter(iter), filter(std::move(functor)) {}

        Optional<ValueType> next() {
            while (auto item = m_iter.next()) {
                if (filter(item.value())) {
                    return item;
                }
            }
            return {};
        }                    

    private:
        IterType m_iter;
        std::function<bool(ValueType const&)> filter;
    };

    template<typename DerivedType>
    struct Adapter {
        using ValueType = typename detail::IterTraits<DerivedType>::ValueType;

        Filter<DerivedType> filter(std::function<bool(ValueType const&)>&& functor) {
            return Filter<DerivedType> {
                derive(), std::move(functor)
            };
        }

        std::vector<ValueType> collect() {
            std::vector<ValueType> result;
            while (auto&& item = derive().next()) {
                result.push_back(item.value());
            }
            return result;
        }

    private:
        DerivedType& derive() {
            return static_cast<DerivedType&>(*this);
        }
    };

    template<typename ContainerType>
    Iterator<ContainerType> iter(ContainerType const& container) {
        return Iterator<ContainerType> {
            container
        };
    }

    template<typename ContainerType>
    IteratorMut<ContainerType> iter_mut(ContainerType& container) {
        return IteratorMut<ContainerType> {
            container
        };
    }

    template<typename ContainerType>
    IntoIterator<ContainerType> into_iter(ContainerType&& container) {
        return IntoIterator<ContainerType> {
            std::move(container)
        };
    }        
}

// Optional type for C++11: may or may not have used 
// SerenityOS::AK::Optional as a loose reference :^)
namespace crab {
    template<typename T>
    struct Optional {
        Optional() = default;

        Optional(T const& value) {        
            m_has_value = true;
            new (&m_storage) T(value);
        }

        Optional(T&& value) {        
            m_has_value = true;
            new (&m_storage) T(std::move(value));
        }

        ~Optional() {
            clear();
        }

        Optional(Optional<T> const& other) {
            clear();
            m_has_value = other.has_value();

            if (other.has_value()) {        
                new (&m_storage) T(other.value());
            }
        }

        Optional<T>& operator=(Optional<T> const& other) {
            clear();
            m_has_value = other.has_value();

            if (other.has_value()) {        
                new (&m_storage) T(other.value());
            }
            return *this;
        }

        Optional(Optional<T>&& other) {
            clear();
            m_has_value = other.has_value();

            if (other.has_value()) {        
                new (&m_storage) T(other.take());
            }
        }

        Optional<T>& operator=(Optional<T>&& other) {
            clear();
            m_has_value = other.has_value();

            if (other.has_value) {        
                new (&m_storage) T(other.take());
            }
            return *this;
        }

        bool has_value() const { return m_has_value; }
        explicit operator bool() const { return m_has_value; }

        T const& value() const { 
            assert(m_has_value);
            return *reinterpret_cast<T const*>(&m_storage);         
        }

        T& value() { 
            assert(m_has_value);
            return *reinterpret_cast<T*>(&m_storage); 
        }

        T value_or(T const& alternative) const {
            return m_has_value ? value() : alternative;
        }

        T value_or(T&& alternative) {
            return m_has_value ? value() : std::move(alternative);
        }    

        T take() {
            assert(m_has_value);
            
            T result = std::move(value());
            value().~T();
            m_has_value = false;

            return result;
        }

        T take_or(T const& alternative) const {
            return m_has_value ? take() : alternative;
        }

        T take_or(T&& alternative) {
            return m_has_value ? take() : std::move(alternative);
        }

        void clear() {
            if (m_has_value) {
                value().~T();
                m_has_value = false;
            }
        }    

    private:
        bool m_has_value{false};    
        alignas(T) uint8_t m_storage[sizeof(T)];
    };
}

// Internal iterator trait implementations
namespace crab {
    namespace detail {
        template<typename ContainerType>
        struct IterTraits<Iterator<ContainerType>> {
            using ValueType = typename ContainerType::value_type;
        };

        template<typename ContainerType>
        struct IterTraits<IteratorMut<ContainerType>> {
            using ValueType = typename ContainerType::value_type;
        };

        template<typename ContainerType>
        struct IterTraits<IntoIterator<ContainerType>> {
            using ValueType = typename ContainerType::value_type;
        };

        template<typename IterType>
        struct IterTraits<Filter<IterType>> {
            using ValueType = typename IterTraits<IterType>::ValueType;
        };
    }
}
