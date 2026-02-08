#ifndef ABSTRACTDATATYPES_BAG_HPP
#define ABSTRACTDATATYPES_BAG_HPP

#ifdef NDEBUG
	#define DEBUG_INFO(a)		/**/
	#define DEBUG_WARNING(a)	/**/
	#define DEBUG_ERROR(a)		/**/
#else
	#define DEBUG_INFO(a) 		std::cout<<"[\e[0;32mInfo\e[0m]:\t\t"<<__PRETTY_FUNCTION__<<"\n\t\t  -- Message: "<<a<<std::endl;
	#define DEBUG_WARNING(a) 	std::cout<<"[\e[0;33mWarning\e[0m]:\t"<<__PRETTY_FUNCTION__<<"\n\t\t  -- Message: "<<a<<std::endl;
	#define DEBUG_ERROR(a)		std::cout<<"[\e[0;31mError\e[0m]:\t"<<__PRETTY_FUNCTION__<<"\n\t\t  -- Message: "<<a<<std::endl;
#endif

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <ranges> // C++20

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

        // --- OPTIMIZED BULK INSERT ---
        iterator insert(const Bag<T>& b)
        {
            if (!b.empty())
            {
                // Optimization: Reserve memory ONCE to prevent multiple reallocations
                m_bag.reserve(m_bag.size() + b.size());
                
                // Removed the "includes" check loop. 
                // Bags allow duplicates, checking for them is O(N^2) and slow.
                return m_bag.insert(m_bag.end(), b.cbegin(), b.cend());
            }
            return m_bag.end();
        }

        // --- Standard Iterator Insert ---
        iterator insert(iterator a, iterator b, iterator c)
        {
            // Removed debug loop for performance
            return m_bag.insert(a, b, c);
        }

        // --- Standard Insert ---
        iterator insert(iterator a, const std::shared_ptr<T>& b)
        {
            // Removed debug "includes" check.
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
            if (n >= m_bag.size())
            {
                throw std::invalid_argument("Bag.hpp: index out of range");
            }
#endif
            return m_bag[n];
        }

        // --- OPTIMIZED ADD ---
        virtual void add(const std::shared_ptr<T>& el)
        {
            // Removed "includes" check. 
            // Push back is Amortized O(1). Checking includes is O(N).
            m_bag.push_back(el);
        }

        virtual void add(const std::shared_ptr<T>& el, int index)
        {
            if(index < 0 || index >= (int)m_bag.size())
            {
                m_bag.push_back(el);
            }
            else
            {
                m_bag.insert(m_bag.begin() + index, el);
            }
        }

        virtual void push_back(const std::shared_ptr<T>& el)
        {
            m_bag.push_back(el);
        }

        iterator erase(iterator el)
        {
			// Calling erase on an empty std::vector results in segmentation fault
            if(!m_bag.empty())
            {
                return m_bag.erase(el);
            }
			
            return m_bag.end();
        }

        virtual iterator erase(const std::shared_ptr<T>& el)
        {
            // C++20 Ranges Optimization
            auto it = std::ranges::find(m_bag, el);
            
            if(it != m_bag.end())
            {
                return m_bag.erase(it);
            }
            return m_bag.end();
        }

        iterator find(const std::shared_ptr<T>& el)
        {
            // C++20 Ranges Optimization
            return std::ranges::find(m_bag, el);
        } 

        bool includes(const std::shared_ptr<T>& el) const
        {
            // C++20 Ranges Optimization
            return std::ranges::find(m_bag, el) != m_bag.end();
        }

        int index_of(const std::shared_ptr<T>& el)
        {
            auto it = std::ranges::find(m_bag, el);
            return index_of(it);
        }

        int index_of(iterator it)
        {
            if(it != m_bag.end())
            {
                return std::distance(m_bag.begin(), it);
            }
            return -1;
        }

        template <class U>
        Bag(Bag<U> const &u)
        {
            // Simple copy, relying on vector assignment
            // (Assumes shared_ptr types are compatible)
            for(const auto& item : u)
            {
                m_bag.push_back(item);
            }
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
};

#endif
