/*
 * Bag.h
 *
 *  Created on: 12.05.2017
 *      Author: frbe5612
 */

#ifndef ABSTRACTDATATYPES_BAG_HPP
#define ABSTRACTDATATYPES_BAG_HPP

#ifdef NDEBUG
    #define DEBUG_INFO(a)       /**/
    #define DEBUG_WARNING(a)    /**/
    #define DEBUG_ERROR(a)      /**/
#else
    #define DEBUG_INFO(a)       std::cout<<"[\e[0;32mInfo\e[0m]:\t\t"<<__PRETTY_FUNCTION__<<"\n\t\t  -- Message: "<<a<<std::endl;
    #define DEBUG_WARNING(a)    std::cout<<"[\e[0;33mWarning\e[0m]:\t"<<__PRETTY_FUNCTION__<<"\n\t\t  -- Message: "<<a<<std::endl;
    #define DEBUG_ERROR(a)      std::cout<<"[\e[0;31mError\e[0m]:\t"<<__PRETTY_FUNCTION__<<"\n\t\t  -- Message: "<<a<<std::endl;
#endif

#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>
#include <ranges> // REQUIRED FOR C++20

template <class T>
class Bag
{
    protected:
        std::vector<std::shared_ptr<T>> m_bag;

    public:
        typedef typename std::vector<std::shared_ptr<T>>::iterator iterator;
        typedef typename std::vector<std::shared_ptr<T>>::const_iterator const_iterator;

        Bag() = default;

        Bag(const Bag<T>& b)
        {
            insert(b);
        }

        virtual ~Bag()
        {
            this->clear();
        }

        Bag<T> * copy() const
        {
            return new Bag<T>(*this);
        }

        iterator insert(const Bag<T>& b)
        {
            // C++20 Attribute: Tell CPU that inserting an empty bag is rare.
            // This optimizes the instruction pipeline for the "else" case.
            if (b.empty()) [[unlikely]]
            {
                return m_bag.end();
            }

            // Standard reserve (Good practice)
            m_bag.reserve(m_bag.size() + b.size());

#ifndef NDEBUG
            // C++20 OPTIMIZATION: std::ranges::find
            // Unlike std::find(begin, end), this allows the compiler to see the 
            // contiguous memory block and apply SIMD vectorization (comparing multiple items at once).
            for (auto const& item : b)
            {
                if (std::ranges::find(m_bag, item) != m_bag.end())
                {
                    // DEBUG_WARNING("Element " << item << " already present.")
                }
            }
#endif
            return m_bag.insert(m_bag.end(), b.cbegin(), b.cend());
        }

        iterator insert(iterator a, iterator b, iterator c)
        {
#ifndef NDEBUG
            // C++20: use a ranges view over [b,c) instead of manual iterator loops.
            for (const auto& el : std::ranges::subrange(b, c))
            {
                if (std::ranges::find(m_bag, el) != m_bag.end())
                {
                    // DEBUG_WARNING("Element " << el << " already present.")
                }
            }
#endif
            return m_bag.insert(a, b, c);
        }

        iterator insert(iterator a, const std::shared_ptr<T>& b)
        {
#ifndef NDEBUG
            // C++20 OPTIMIZATION
            if (std::ranges::find(m_bag, b) != m_bag.end())
            {
                 // DEBUG_WARNING("Element " << b << " already present.")
            }
#endif
            return m_bag.insert(a, b);
        }

        bool empty() const
        {
            return m_bag.empty();
        }

        std::shared_ptr<T> front() const
        {
            return m_bag.front();
        }
        // NEW EXPERIMENTAL FUNCTIONS

        // O(1) Time - Extremely Fast
        virtual void pop_back()
        {
            if (m_bag.empty()) [[unlikely]]
            {
                return;
            }

            // Ensure we drop our strong reference before removing the slot.
            // This makes the ref-count transition explicit (useful for experiments).
            m_bag.back().reset();
            m_bag.pop_back();
        }

        // O(N) Time - Slow (Causes memory shifting)
        virtual void pop_front()
        {
            if (m_bag.empty()) [[unlikely]]
            {
                return;
            }

            // Drop strong reference, then erase (note: erase(begin) shifts elements).
            m_bag.front().reset();
            m_bag.erase(m_bag.begin());
        }
        

        void clear()
        {
            m_bag.clear();
        }

        unsigned int size() const
        {
            return m_bag.size();
        }

        unsigned int max_size() const
        {
            return m_bag.max_size();
        }

        const std::shared_ptr<T>& operator [] (unsigned int n) const
        {
            return m_bag[n];
        }

        const std::shared_ptr<T>& at(unsigned int n) const
        {
#ifndef NDEBUG
            if (n >= m_bag.size()) [[unlikely]] // C++20 Attribute
            {
                throw std::invalid_argument("Bag.hpp: index out of range");
            }
#endif
            return m_bag[n];
        }

        virtual void add(const std::shared_ptr<T>& el)
        {
#ifndef NDEBUG
            // C++20 OPTIMIZATION
            if (std::ranges::find(m_bag, el) != m_bag.end())
            {
                // DEBUG_WARNING...
            }
#endif
            m_bag.push_back(el);
        }

        virtual void add(const std::shared_ptr<T>& el, int index)
        {
            if(index < 0 || index >= (int)m_bag.size()) [[unlikely]]
            {
                this->add(el);
            }
            else
            {
                this->insert(begin() + index, el);
            }
        }

        virtual void push_back(const std::shared_ptr<T>& el)
        {
            m_bag.push_back(el);
        }

        iterator erase(iterator el)
        {
            if(!m_bag.empty()) [[likely]]
            {
                return m_bag.erase(el);
            }
            
            return m_bag.end();
        }

        virtual iterator erase(const std::shared_ptr<T>& el)
        {
            // C++20 OPTIMIZATION: ranges::find
            // Faster lookup due to contiguous memory hints for the compiler
            auto it = std::ranges::find(m_bag, el);
            
            if(it != m_bag.end()) [[likely]]
            {
                return m_bag.erase(it);
            }

            return it;
        }

        iterator find(const std::shared_ptr<T>& el)
        {
            // C++20 OPTIMIZATION
            return std::ranges::find(m_bag, el);
        }

        bool includes(const std::shared_ptr<T>& el) const
        {
            // C++20 OPTIMIZATION
            return std::ranges::find(m_bag, el) != m_bag.end();
        }

        int index_of(const std::shared_ptr<T>& el)
        {
            // C++20 OPTIMIZATION
            auto it = std::ranges::find(m_bag, el);
            return index_of(it);
        }

        int index_of(iterator it)
        {
            if(it != m_bag.end()) [[likely]]
            {
                return std::distance(m_bag.begin(), it);
            }

            return -1;
        }

        template <class U>
        Bag(Bag<U> const &u)
        {
            // C++20: use ranges algorithm for clarity and potential optimizer friendliness.
            std::ranges::copy(u, std::back_inserter(m_bag));
        }

        virtual const_iterator cbegin() const
        {
            return m_bag.cbegin();
        }

        virtual const_iterator cend() const
        {
            return m_bag.cend();
        }

        virtual iterator begin()
        {
            return m_bag.begin();
        }

        virtual iterator end()
        {
            return m_bag.end();
        }
		//TODO 
		//erase method with std::vector of elements
		//find more use ful 
};

#endif // ABSTRACTDATATYPES_BAG_HPP