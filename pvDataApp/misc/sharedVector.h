#ifndef SHAREDVECTOR_H
#define SHAREDVECTOR_H

#include <ostream>
#include <algorithm>
#include <stdexcept>
#include <iterator>

#include "pv/sharedPtr.h"

namespace epics { namespace pvData {

template<typename E> class shared_vector;

namespace detail {
    template<typename E>
    struct default_array_deleter {void operator()(E a){delete[] a;}};

    // Implicit casts allowed during copy construction of shared_vector
    template<typename FROM, typename TO>
    struct vector_implicit_cast {
        // There is intentionally no implmentation here to cause
        // compile errors for illegal casts.
        //static std::tr1::shared_ptr<TO> cast(const std::tr1::shared_ptr<FROM>);
    };
    // non-const -> const
    template<typename E>
    struct vector_implicit_cast<E,const E> {
        static std::tr1::shared_ptr<const E> cast(const std::tr1::shared_ptr<E> input)
        {
            return std::tr1::shared_ptr<const E>(input);
        }
    };

    // avoid adding 'const' twice
    template<typename T> struct decorate_const { typedef const T type; };
    template<typename T> struct decorate_const<const T> { typedef const T type; };

    // How values should be passed as arguments to shared_vector methods
    // really should use boost::call_traits
    template<typename T> struct call_with { typedef T type; };
    template<typename T> struct call_with<std::tr1::shared_ptr<T> >
    { typedef const std::tr1::shared_ptr<T>& type; };
    template<> struct call_with<std::string> { typedef const std::string& type; };

    // For lack of C++11's std::move do our own special handling of shared_ptr
    template<typename T>
    struct moveme {
        template<typename arg>
        static void op(const arg& a, const arg& b, const arg& c)
        {std::copy(a,b,c);}
    };
    template<typename T>
    struct moveme<std::tr1::shared_ptr<T> > {
        template<typename arg>
        static void op(arg a, arg b, arg c)
        {
            // "move" with swap to avoid ref counter operations
            for(;a!=b;a++,c++)
                c->swap(*a);
        }
    };

    /* All the parts of shared_vector which
     * don't need special handling for E=void
     */
    template<typename E>
    class shared_vector_base
    {
        // allow specialization for all E to be friends
        template<typename E1> friend class shared_vector_base;
    protected:
        std::tr1::shared_ptr<E> m_data;
        //! Offset in the data array of first element
        size_t m_offset;
        //! Number of visible elements between m_offset and end of data
        size_t m_count;
        //! Total number of elements between m_offset and the end of data
        size_t m_total;

    public:

        //! @brief Empty vector (not very interesting)
        shared_vector_base()
            :m_data(), m_offset(0), m_count(0), m_total(0)
        {}

    protected:
        // helper for constructors
        // Ensure that offset and size are zero when we are constructed with NULL
        void _null_input()
        {
            if(!m_data.get()) {
                m_offset = m_total = m_count = 0;
            }
        }
    public:

        template<typename A>
        shared_vector_base(A v, size_t o, size_t c)
            :m_data(v, detail::default_array_deleter<A>())
            ,m_offset(o), m_count(c), m_total(c)
        {_null_input();}

        template<typename E1>
        shared_vector_base(const std::tr1::shared_ptr<E1>& d, size_t o, size_t c)
            :m_data(d), m_offset(o), m_count(c), m_total(c)
        {_null_input();}


        template<typename A, typename B>
        shared_vector_base(A d, B b, size_t o, size_t c)
            :m_data(d,b), m_offset(o), m_count(c), m_total(c)
        {_null_input();}

        shared_vector_base(const shared_vector_base& O)
            :m_data(O.m_data), m_offset(O.m_offset)
            ,m_count(O.m_count), m_total(O.m_total)
        {}

        template<typename E1>
        shared_vector_base(const shared_vector_base<E1>& o)
            : m_data(vector_implicit_cast<E1,E>::cast(o.m_data))
            , m_offset(o.m_offset), m_count(o.m_count), m_total(o.m_total)
        {_null_input();}

        //! @brief Copy an existing vector
        shared_vector_base& operator=(const shared_vector_base& o)
        {
            if(&o!=this) {
                m_data=o.m_data;
                m_offset=o.m_offset;
                m_count=o.m_count;
                m_total=o.m_total;
            }
            return *this;
        }

        //! @brief Copy an existing vector of a related type
        template<typename E1>
        shared_vector_base& operator=(const shared_vector_base<E1>& o)
        {
            if(&o!=this) {
                m_data=vector_implicit_cast<E1,E>::cast(o.m_data);
                m_offset=o.m_offset;
                m_count=o.m_count;
                m_total=o.m_total;
            }
            return *this;
        }

        //! @brief Swap the contents of this vector with another
        void swap(shared_vector_base& o) {
            if(&o!=this) {
                m_data.swap(o.m_data);
                std::swap(m_count, o.m_count);
                std::swap(m_offset, o.m_offset);
                std::swap(m_total, o.m_total);
            }
        }

        //! @brief Clear contents.
        //! size() becomes 0
        void clear() {
            m_data.reset();
            m_offset = m_total = m_count = 0;
        }

        //! @brief Data is not shared?
        bool unique() const {return !m_data || m_data.unique();}


        //! @brief Number of elements visible through this vector
        size_t size() const{return m_count;}
        bool empty() const{return !m_count;}

        size_t max_size() const{return (size_t)-1;}


        /** @brief Reduce the view of this shared_vector.
         *
         * Reduce the portion of the underlying buffer which
         * is accessible through this shared_vector.
         *
         * When the requested new offset and length are not possible
         * then the following holds.
         *
         * When offset is >= size() then after slice() size()==0.
         * When length >= size()-offset then after slice()
         * size() = old_size-offset.
         *
         @param offset The request new offset relative to the
         *             current offset.
         @param length The requested new length.
         *
         @note offset and length are in units of sizeof(E).
         *     or bytes (1) when E=void.
         */
        void slice(size_t offset, size_t length=(size_t)-1)
        {
            if(offset>m_total)
                offset = m_total;

            m_offset += offset;

            m_total -= offset;

            if(offset>m_count) {
                m_count = 0;
            } else {
                if(length > m_count - offset)
                    length = m_count - offset;
                m_count = length;
            }
        }

        // Access to members.
        const std::tr1::shared_ptr<E>& dataPtr() const { return m_data; }
        size_t dataOffset() const { return m_offset; }
        size_t dataCount() const { return m_count; }
        size_t dataTotal() const { return m_total; }
    };
}

/** @brief A holder for a contigious piece of memory.
 *
 * Data is shared, but offset and length are not.
 * This allows one vector to have access to only a
 * subset of a piece of memory.
 *
 * The ways in which shared_vector is intended to differ from
 * std::vector are outlined in @ref vectordiff .
 *
 * Also see @ref vectormem
 */
template<typename E>
class shared_vector : public detail::shared_vector_base<E>
{
    typedef detail::shared_vector_base<E> base_t;
    typedef typename detail::call_with<E>::type param_type;
public:
    typedef E value_type;
    typedef E& reference;
    typedef typename detail::decorate_const<E>::type& const_reference;
    typedef E* pointer;
    typedef E* iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef typename detail::decorate_const<E>::type* const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    typedef E element_type;
    typedef std::tr1::shared_ptr<E> shared_pointer_type;

    // allow specialization for all E to be friends
    template<typename E1> friend class shared_vector;


    //! @brief Empty vector (not very interesting)
    shared_vector() :base_t() {}

    //! @brief Allocate (with new[]) a new vector of size c
    explicit shared_vector(size_t c)
        :base_t(new E[c], 0, c)
    {}

    //! @brief Allocate (with new[]) a new vector of size c and fill with value e
    shared_vector(size_t c, param_type e)
        :base_t(new E[c], 0, c)
    {
        std::fill_n(this->m_data.get(), this->m_count, e);
    }

    /** @brief Build vector from a raw pointer
     *
     @param v A raw pointer allocated with new[].
     @param o The offset in v or the first element visible to the vector
     @param c The number of elements pointed to by v+o
     */
    template<typename A>
    shared_vector(A v, size_t o, size_t c) :base_t(v,o,c) {}

    /** @brief Build vector from an existing smart pointer
     *
     @param d An existing smart pointer
     @param o The offset in v or the first element visible to the vector
     @param c The number of elements pointed to by v+o
     */
    template<typename E1>
    shared_vector(const std::tr1::shared_ptr<E1>& d, size_t o, size_t c)
        :base_t(d,o,c) {}

    /** @brief Build vector from raw pointer and cleanup function
     *
     @param d An existing raw pointer
     @param b An function/functor used to free d.  Invoked as b(d).
     @param o The offset in v or the first element visible to the vector
     @param c The number of elements pointed to by v+o
     */
    template<typename A, typename B>
    shared_vector(A d, B b, size_t o, size_t c)
        :base_t(d,b,o,c) {}

    //! @brief Copy an existing vector of same type
    shared_vector(const shared_vector& o) :base_t(o) {}

    //! @brief Copy an existing vector of a related type
    template<typename E1>
    shared_vector(const shared_vector<E1>& o) :base_t(o) {}

    size_t capacity() const { return this->m_total; }

    /** @brief Set array capacity
     *
     * A side effect is that array data will be uniquely owned by this instance
     * as if make_unique() was called.  This holds even if the capacity
     * does not increase.
     *
     * For notes on copying see docs for make_exlcusive().
     */
    void reserve(size_t i) {
        if(this->unique() && i<=this->m_total)
            return;
        pointer temp=new E[i];
        try{
            detail::moveme<E>::op(begin(), end(), temp);
            this->m_data.reset(temp, detail::default_array_deleter<pointer>());
        }catch(...){
            delete[] temp;
            throw;
        }
        this->m_offset = 0;
        this->m_total = i;
        // m_count is unchanged
    }

    /** @brief Grow or shrink array
     *
     * A side effect is that array data will be uniquely owned by this instance
     * as if make_unique() was called.  This holds even if the size does not change.
     *
     * For notes on copying see docs for make_exlcusive().
     */
    void resize(size_t i) {
        if(i==this->m_count) {
            make_unique();
            return;
        }
        if(this->m_data && this->m_data.unique()) {
            // we have data and exclusive ownership of it
            if(i<=this->m_total) {
                // We have room to grow (or shrink)!
                this->m_count = i;
                return;
            }
        }
        // must re-allocate :(
        size_t new_total = std::max(this->m_total, i);
        pointer temp=new E[new_total];
        try{
            // Copy as much as possible from old,
            // remaining elements are uninitialized.
            detail::moveme<E>::op(begin(),
                      begin()+std::min(i,this->size()),
                      temp);
            this->m_data.reset(temp, detail::default_array_deleter<pointer>());
        }catch(...){
            delete[] temp;
            throw;
        }
        this->m_offset= 0;
        this->m_count = i;
        this->m_total = new_total;
    }

    /** @brief Grow (and fill) or shrink array.
     *
     * see @ref resize(size_t)
     */
    void resize(size_t i, param_type v) {
        size_t oldsize=this->size();
        resize(i);
        if(this->size()>oldsize) {
            std::fill(begin()+oldsize, end(), v);
        }
    }

    /** @brief Ensure (by copying) that this shared_vector is the sole
     *  owner of the data array.
     *
     * If a copy is needed, memory is allocated with new[].  If this is
     * not desireable then do something like the following.
     @code
       shared_vector<E> original(...);

       if(!original.unique()){
         shared_vector<E> temp(myallocator(original.size()),
                               0, original.size());
         std::copy(original.begin(), original.end(), temp.begin());
         original.swap(temp);
       }
       assert(original.unique());
     @endcode
     */
    void make_unique() {
        if(this->unique())
            return;
        shared_pointer_type d(new E[this->m_total], detail::default_array_deleter<E*>());
        detail::moveme<E>::op(this->m_data.get()+this->m_offset,
                  this->m_data.get()+this->m_offset+this->m_count,
                  d.get());
        this->m_data.swap(d);
        this->m_offset=0;
    }


    // STL iterators

    iterator begin() const{return this->m_data.get()+this->m_offset;}
    const_iterator cbegin() const{return begin();}

    iterator end() const{return this->m_data.get()+this->m_offset+this->m_count;}
    const_iterator cend() const{return end();}

    reverse_iterator rbegin() const{return reverse_iterator(end());}
    const_reverse_iterator crbegin() const{return rbegin();}

    reverse_iterator rend() const{return reverse_iterator(begin());}
    const_reverse_iterator crend() const{return rend();}

    reference front() const{return (*this)[0];}
    reference back() const{return (*this)[this->m_count-1];}

    // Modifications

    void push_back(param_type v)
    {
        resize(this->size()+1);
        back() = v;
    }

    void pop_back()
    {
        this->slice(0, this->size()-1);
    }

    // data access

    pointer data() const{return this->m_data.get()+this->m_offset;}

    reference operator[](size_t i) const {return this->m_data.get()[this->m_offset+i];}

    reference at(size_t i) const
    {
        if(i>this->m_count)
            throw std::out_of_range("Index out of bounds");
        return (*this)[i];
    }

};

//! Specialization for storing untyped pointers
//! Does not allow access or iteration of contents
template<>
class shared_vector<void> : public detail::shared_vector_base<void> {
    typedef detail::shared_vector_base<void> base_t;
public:
    typedef void* pointer;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    typedef std::tr1::shared_ptr<void> shared_pointer_type;

    shared_vector() :base_t() {}

    template<typename A>
    shared_vector(A v, size_t o, size_t c) :base_t(v,o,c) {}

    template<typename E1>
    shared_vector(const std::tr1::shared_ptr<E1>& d, size_t o, size_t c)
        :base_t(d,o,c) {}

    template<typename A, typename B>
    shared_vector(A d, B b, size_t o, size_t c)
        :base_t(d,b,o,c) {}

    template<typename E1>
    shared_vector(const shared_vector<E1>& o) :base_t(o) {}

    pointer data() const{
        return (pointer)(((char*)this->m_data.get())+this->m_offset);
    }
};

//! Specialization for storing constant untyped pointers
//! Does not allow access or iteration of contents
template<>
class shared_vector<const void> : public detail::shared_vector_base<const void> {
    typedef detail::shared_vector_base<const void> base_t;
public:
    typedef const void* pointer;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    typedef std::tr1::shared_ptr<void> shared_pointer_type;

    shared_vector() :base_t() {}

    template<typename A>
    shared_vector(A v, size_t o, size_t c) :base_t(v,o,c) {}

    template<typename E1>
    shared_vector(const std::tr1::shared_ptr<E1>& d, size_t o, size_t c)
        :base_t(d,o,c) {}

    template<typename A, typename B>
    shared_vector(A d, B b, size_t o, size_t c)
        :base_t(d,b,o,c) {}

    template<typename E1>
    shared_vector(const shared_vector<E1>& o) :base_t(o) {}

    pointer data() const{
        return (pointer)(((char*)this->m_data.get())+this->m_offset);
    }
};

template<typename E>
class weak_vector {
    std::tr1::weak_ptr<E> m_data;
    //! Offset in the data array of first element
    size_t m_offset;
    //! Number of elements between m_offset and end of data
    size_t m_count;

public:
    typedef E element_type;

    weak_vector()
        :m_data()
        ,m_offset(0)
        ,m_count(0)
    {}
    weak_vector(const weak_vector& o)
        :m_data(o.m_data)
        ,m_offset(o.m_offset)
        ,m_count(o.m_count)
    {}
    weak_vector(const shared_vector<E>& o)
        :m_data(o.dataPtr())
        ,m_offset(o.dataOffset())
        ,m_count(o.dataCount())
    {}

    bool expired() const{return m_data.expired();}
    shared_vector<E> lock() const
    {
        return shared_vector<E>(m_data.lock(), m_offset, m_count);
    }

    void reset()
    {
        m_data.reset();
        m_offset = m_count = 0;
    }

    void swap(weak_vector& o)
    {
        m_data.swap(o);
        std::swap(m_offset, o.m_offset);
        std::swap(m_count, o.m_count);
    }
};

namespace detail {
    template<typename TO, typename FROM> struct shared_vector_caster {};

    template<typename FROM>
    struct shared_vector_caster<void,FROM> {
        static inline shared_vector<void> op(const shared_vector<FROM>& src) {
            return shared_vector<void>(
                    std::tr1::static_pointer_cast<void>(src.dataPtr()),
                    src.dataOffset()*sizeof(FROM),
                    src.dataCount()*sizeof(FROM));
        }
    };

    template<typename TO>
    struct shared_vector_caster<TO,void> {
        static inline shared_vector<TO> op(const shared_vector<void>& src) {
            return shared_vector<TO>(
                    std::tr1::static_pointer_cast<TO>(src.dataPtr()),
                    src.dataOffset()/sizeof(TO),
                    src.dataCount()/sizeof(TO));
        }
    };
}

/** @brief Allow casting of shared_vector between types
 *
 * Currently only to/from void is implemented.
 *
 @warning Casting from void is undefined unless the offset and length
 *        are integer multiples of the size of the destination type.
 */
template<typename TO, typename FROM>
static inline
shared_vector<TO>
static_shared_vector_cast(const shared_vector<FROM>& src)
{
    return detail::shared_vector_caster<TO,FROM>::op(src);
}

//! Allows casting from const TYPE -> TYPE.
template<typename TYPE>
static inline
shared_vector<TYPE>
const_shared_vector_cast(const shared_vector<const TYPE>& src)
{
    return shared_vector<TYPE>(
            std::tr1::const_pointer_cast<TYPE>(src.dataPtr()),
            src.dataOffset(),
            src.dataCount());
}


}} // namespace epics::pvData

// Global operators for shared_vector

template<typename A, typename B>
bool operator==(const epics::pvData::shared_vector<A>& a,
                const epics::pvData::shared_vector<B>& b)
{
    if(a.size() != b.size())
        return false;
    if(a.dataOffset()==b.dataOffset() && a.dataPtr().get()==b.dataPtr().get())
        return true;
    return std::equal(a.begin(), a.end(), b.begin());
}

template<typename A, typename B>
bool operator!=(const epics::pvData::shared_vector<A>& a,
                const epics::pvData::shared_vector<B>& b)
{
    return !(a==b);
}

template<typename E>
std::ostream& operator<<(std::ostream& strm, const epics::pvData::shared_vector<E>& arr)
{
    strm<<'{'<<arr.size()<<"}[";
    for(size_t i=0; i<arr.size(); i++) {
        if(i>10) {
            strm<<"...";
            break;
        }
        strm<<arr[i];
        if(i+1<arr.size())
            strm<<", ";
    }
    strm<<']';
    return strm;
}


#endif // SHAREDVECTOR_H

/** @page vectordiff Differences between std::vector and shared_vector
 *
 * @section diffbehave Differences in behavior
 *
 * shared_vector models const-ness like shared_ptr.  A equivalent of
 * 'const std::vector<E>' is 'const shared_vector<const E>'.  However,
 * it is also possible to have 'const shared_vector<E>' analogous to
 * 'E* const' and 'shared_vector<const E>' which is analogous to
 * 'const E*'.
 *
 * Copying a shared_vector, by construction or assignment, does
 * not copy its contents.  Modifications to one such "copy" effect
 * all associated shared_vector instances.
 *
 * std::vector::reserve(N) has no effect if N<=std::vector::capacity().
 * However, like resize(), shared_vector<E>::reserve() has the side
 * effect of always calling make_unique().
 *
 * @section notimpl Parts of std::vector interface not implemented
 *
 * Mutating methods insert(), erase(), shrink_to_fit(),
 * emplace(), and emplace_back() are not implemented.
 *
 * shared_vector does not model an allocator which is bound to the object.
 * Therefore the get_allocator() method and the allocator_type typedef are
 * not provided.
 *
 * The assign() method and the related constructor are not implemented
 * at this time.
 *
 * The comparison operators '>', '>=', '<=', and '<' are not implemented
 * at this time.
 *
 * @section newstuff Parts not found in std::vector
 *
 * shared_vector has additional constructors from raw pointers
 * and shared_ptr s.
 *
 * The copy constructor and assignment operator allow implicit
 * casting from type 'shared_vector<T>' to 'shared_vector<const T>'.
 *
 * To faciliate safe modification the methods unique() and
 * make_unique() are provided.
 *
 * The slice() method selects a sub-set of the shared_vector.
 *
 * The low level accessors dataPtr(), dataOffset(), dataCount(),
 * and dataTotal().
 *
 */

/** @page vectormem Memory Management with shared_vector
 *
 * The @link epics::pvData::shared_vector shared_vector class @endlink
 * is a std::vector like class which implements sharing data by reference counting.
 *
 * Internally memory is tracked with the shared_ptr reference counting smart pointer.
 * This allows a custom destructor to be specified.  This allows a vector to borrow
 * memory allocated by 3rd party libraries which require special cleanup.
 *
 * In place element modification is allowed.  It is left to user code to ensure
 * that such modification is safe, either from application specific knowledge, or by
 * calling
 * @link paramTable::string_data::make_unique make_unique @endlink
 * explicitly, or implicitly by calling
 * @link paramTable::shared_vector::resize resize @endlink
 * prior to making modifications.
 *
 @code
   extern "C" {
     // array embedded in C structure
     struct composite {
       int other, stuff;
       char buf[42];
     }

     // Unknown relation between array and handle
     typedef void* handle_type;
     handle_type mylib_alloc(void);
     char *mylib_mem(handle_type);
     void mylib_free(handle_type);
   }

   // Note that mylibcleaner must be copy constructable
   struct mylibcleaner {
     handle_type handle;
     mylibcleaner(handle_type h) :handle(h) {}
     void operator()(char*){ mylib_free(handle);}
   };

   struct compcleaner {
     void operator()(char* c){ free(c-offsetof(composite,buf)); }
   };

   void main() {

     unsigned char* buf=calloc(42,1);

     shared_vector<epicsUInt8> a(buf, &free);

     a.clear(); // calls free(ptr)


     composite *c=malloc(sizeof(*c));
     assert(c!=NULL);

     shared_vector<char> d(c->buf, compcleaner());

     d.clear(); // calls free(ptr-offsetof(composite,buf))


     void *handle=mylib_alloc();
     char *hmem=mylib_mem(handle);
     assert(hmem!=NULL);

     shared_vector<epicsUInt8> b(hmem, mylibcleaner(handle));

     b.clear(); // calls mylib_free(handleptr)
   }
 @endcode
 */
