/* sharedVector.h */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvData is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef SHAREDVECTOR_H
#define SHAREDVECTOR_H

#ifdef _WIN32
#define NOMINMAX
#endif

#include <ostream>
#include <algorithm>
#include <stdexcept>
#include <iterator>

#include <cassert>

#include "pv/sharedPtr.h"
#include "pv/pvIntrospect.h"
#include "pv/typeCast.h"
#include "pv/templateMeta.h"

namespace epics { namespace pvData {

template<typename E, class Enable = void> class shared_vector;

template<typename TO, typename FROM>
static FORCE_INLINE
shared_vector<TO>
const_shared_vector_cast(shared_vector<FROM>& src);

namespace detail {
    template<typename E>
    struct default_array_deleter {void operator()(E a){delete[] a;}};

    // How values should be passed as arguments to shared_vector methods
    // really should use boost::call_traits
    template<typename T> struct call_with { typedef T type; };
    template<typename T> struct call_with<std::tr1::shared_ptr<T> >
    { typedef const std::tr1::shared_ptr<T>& type; };
    template<> struct call_with<std::string> { typedef const std::string& type; };

    struct _shared_vector_freeze_tag {};
    struct _shared_vector_thaw_tag {};
    struct _shared_vector_cast_tag {};

    /* All the parts of shared_vector which
     * don't need special handling for E=void
     */
    template<typename E>
    class shared_vector_base
    {
        // allow specialization for all E to be friends
        template<typename E1> friend class shared_vector_base;
    protected:
        // NOTE: Do no use m_data, since VxWorks has a 'm_data' macro defined
        std::tr1::shared_ptr<E> m_sdata;
        //! Offset in the data array of first visible element
        size_t m_offset;
        //! Number of visible elements between m_offset and end of data
        size_t m_count;
        //! Total number of elements between m_offset and the end of data
        size_t m_total;

        /* invariants
         *  m_count <= m_total (enforced)
         *  m_offset + m_total <= (size_t)-1 (checked w/ assert())
         */

    public:

        //! @brief Empty vector (not very interesting)
        shared_vector_base()
            :m_sdata(), m_offset(0), m_count(0), m_total(0)
        {}

    protected:
        // helper for constructors
        // Ensure that offset and size are zero when we are constructed with NULL
        void _null_input()
        {
            if(!m_sdata.get()) {
                m_offset = m_total = m_count = 0;
            } else {
                // ensure we won't have integer overflows later
                assert( m_offset <= ((size_t)-1) - m_total);
            }
        }
    public:

#ifdef _WIN32
        template<typename A>
        shared_vector_base(A* v, size_t o, size_t c)
            :m_sdata(v, detail::default_array_deleter<A*>())
            ,m_offset(o), m_count(c), m_total(c)
        {_null_input();}
#else
        template<typename A>
        shared_vector_base(A v, size_t o, size_t c)
            :m_sdata(v, detail::default_array_deleter<A>())
            ,m_offset(o), m_count(c), m_total(c)
        {_null_input();}
#endif
        shared_vector_base(const std::tr1::shared_ptr<E>& d, size_t o, size_t c)
            :m_sdata(d), m_offset(o), m_count(c), m_total(c)
        {_null_input();}


        template<typename A, typename B>
        shared_vector_base(A d, B b, size_t o, size_t c)
            :m_sdata(d,b), m_offset(o), m_count(c), m_total(c)
        {_null_input();}

        shared_vector_base(const shared_vector_base& O)
            :m_sdata(O.m_sdata), m_offset(O.m_offset)
            ,m_count(O.m_count), m_total(O.m_total)
        {}

    protected:
        typedef typename meta::strip_const<E>::type _E_non_const;
    public:
        //! Constructor used to implement freeze().
        //! Should not be called directly.
        shared_vector_base(shared_vector_base<_E_non_const>& O,
                           _shared_vector_freeze_tag)
            :m_sdata()
            ,m_offset(O.m_offset)
            ,m_count(O.m_count)
            ,m_total(O.m_total)
        {
            if(!O.unique())
                throw std::runtime_error("Can't freeze non-unique vector");
            m_sdata = O.m_sdata;
            O.clear();
        }

        //! Constructor used to implement thaw().
        //! Should not be called directly.
        shared_vector_base(shared_vector<const E>& O,
                           _shared_vector_thaw_tag)
            :m_sdata()
            ,m_offset(O.m_offset)
            ,m_count(O.m_count)
            ,m_total(O.m_total)
        {
            O.make_unique();
            m_sdata = std::tr1::const_pointer_cast<E>(O.m_sdata);
            O.clear();
        }

        //! @brief Copy an existing vector
        shared_vector_base& operator=(const shared_vector_base& o)
        {
            if(&o!=this) {
                m_sdata=o.m_sdata;
                m_offset=o.m_offset;
                m_count=o.m_count;
                m_total=o.m_total;
            }
            return *this;
        }

        //! @brief Swap the contents of this vector with another
        void swap(shared_vector_base& o) {
            if(&o!=this) {
                m_sdata.swap(o.m_sdata);
                std::swap(m_count, o.m_count);
                std::swap(m_offset, o.m_offset);
                std::swap(m_total, o.m_total);
            }
        }

        //! @brief Clear contents.
        //! size() becomes 0
        void clear() {
            m_sdata.reset();
            m_offset = m_total = m_count = 0;
        }

        //! @brief Data is not shared?
        bool unique() const {return !m_sdata || m_sdata.unique();}


        //! @brief Number of elements visible through this vector
        size_t size() const{return m_count;}
        bool empty() const{return !m_count;}


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
            if(offset>m_count)
                offset = m_count; // will slice down to zero length

            const size_t max_count = m_count - offset;

            m_offset += offset;

            m_total -= offset;

            m_count = std::min(length, max_count);
        }

        // Access to members.
        const std::tr1::shared_ptr<E>& dataPtr() const { return m_sdata; }
        size_t dataOffset() const { return m_offset; }
        size_t dataCount() const { return m_count; }
        size_t dataTotal() const { return m_total; }
    };
}

/** @brief A holder for a contiguous piece of memory.
 *
 * Data is shared, but offset and length are not.
 * This allows one vector to have access to only a
 * subset of a piece of memory.
 *
 * The ways in which shared_vector is intended to differ from
 * std::vector are outlined in @ref vectordiff .
 *
 * Also see @ref vectormem and @ref vectorconst
 *
 * @warning Due to the implementation of std::tr1::shared_ptr, use of
 * shared_vector should not be combined with use of weak_ptr.
 * shared_ptr::unique() and shared_ptr::use_count() do @b not
 * include weak_ptr instances.  This breaks the assumption made
 * by make_unique() that unique()==true implies exclusive
 * ownership.
 */
template<typename E, class Enable>
class shared_vector : public detail::shared_vector_base<E>
{
    typedef detail::shared_vector_base<E> base_t;
    typedef typename detail::call_with<E>::type param_type;
    typedef typename meta::strip_const<E>::type _E_non_const;
public:
    typedef E value_type;
    typedef E& reference;
    typedef typename meta::decorate_const<E>::type& const_reference;
    typedef E* pointer;
    typedef typename meta::decorate_const<E>::type* const_pointer;
    typedef E* iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef typename meta::decorate_const<E>::type* const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    typedef E element_type;
    typedef std::tr1::shared_ptr<E> shared_pointer_type;

    // allow specialization for all E to be friends
    template<typename E1, class Enable1> friend class shared_vector;


    //! @brief Empty vector (not very interesting)
    shared_vector() :base_t() {}

    //! @brief Allocate (with new[]) a new vector of size c
    explicit shared_vector(size_t c)
        :base_t(new _E_non_const[c], 0, c)
    {}

    //! @brief Allocate (with new[]) a new vector of size c and fill with value e
    shared_vector(size_t c, param_type e)
        :base_t(new _E_non_const[c], 0, c)
    {
        std::fill_n((_E_non_const*)this->m_sdata.get(), this->m_count, e);
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

    //! @internal
    //! Internal for static_shared_vector_cast
    template<typename FROM>
    shared_vector(const shared_vector<FROM> &src,
                  typename meta::is_void<FROM, detail::_shared_vector_cast_tag>::type)
        :base_t(std::tr1::static_pointer_cast<E>(src.dataPtr()),
                src.dataOffset()/sizeof(E),
                src.dataCount()/sizeof(E))
    {}


    shared_vector(shared_vector<typename base_t::_E_non_const>& O,
                  detail::_shared_vector_freeze_tag t)
        :base_t(O,t)
    {}

    shared_vector(shared_vector<const E>& O,
                  detail::_shared_vector_thaw_tag t)
        :base_t(O,t)
    {}

    size_t max_size() const{return ((size_t)-1)/sizeof(E);}

    size_t capacity() const { return this->m_total; }

    /** @brief Set array capacity
     *
     * A side effect is that array data will be uniquely owned by this instance
     * as if make_unique() was called.  This holds even if the capacity
     * does not increase.
     *
     * For notes on copying see docs for make_unique().
     */
    void reserve(size_t i) {
        if(this->unique() && i<=this->m_total)
            return;
        size_t new_count = std::min(this->m_count, i);
        _E_non_const* temp=new _E_non_const[i];
        try{
            std::copy(begin(), begin()+new_count, temp);
            this->m_sdata.reset(temp, detail::default_array_deleter<E*>());
        }catch(...){
            delete[] temp;
            throw;
        }
        this->m_offset = 0;
        this->m_count = new_count;
        this->m_total = i;
    }

    /** @brief Grow or shrink array
     *
     * A side effect is that array data will be uniquely owned by this instance
     * as if make_unique() were called.  This holds even if the size does not change.
     *
     * For notes on copying see docs for make_unique().
     */
    void resize(size_t i) {
        if(i==this->m_count) {
            make_unique();
            return;
        }
        if(this->m_sdata && this->m_sdata.unique()) {
            // we have data and exclusive ownership of it
            if(i<=this->m_total) {
                // We have room to grow (or shrink)!
                this->m_count = i;
                return;
            }
        }
        // must re-allocate :(
        size_t new_total = std::max(this->m_total, i);
        _E_non_const* temp=new _E_non_const[new_total];
        try{
            // Copy as much as possible from old,
            // remaining elements are uninitialized.
            std::copy(begin(),
                      begin()+std::min(i,this->size()),
                      temp);
            this->m_sdata.reset(temp, detail::default_array_deleter<pointer>());
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
     * not desirable then do something like the following.
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
        _E_non_const *d = new _E_non_const[this->m_total];
        try {
            std::copy(this->m_sdata.get()+this->m_offset,
                      this->m_sdata.get()+this->m_offset+this->m_count,
                      d);
        }catch(...){
            delete[] d;
            throw;
        }
        this->m_sdata.reset(d, detail::default_array_deleter<E*>());
        this->m_offset=0;
    }


    // STL iterators

    iterator begin() const{return this->m_sdata.get()+this->m_offset;}
    const_iterator cbegin() const{return begin();}

    iterator end() const{return this->m_sdata.get()+this->m_offset+this->m_count;}
    const_iterator cend() const{return end();}

    reverse_iterator rbegin() const{return reverse_iterator(end());}
    const_reverse_iterator crbegin() const{return rbegin();}

    reverse_iterator rend() const{return reverse_iterator(begin());}
    const_reverse_iterator crend() const{return rend();}

    reference front() const{return (*this)[0];}
    reference back() const{return (*this)[this->m_count-1];}

    // Modifications

private:
    void _push_resize() {
        if(this->m_count==this->m_total || !this->unique()) {
            size_t next;
            if(this->m_total<1024) {
                // round m_total+1 up to the next power of 2
                next = this->m_total;
                next |= next >> 1;
                next |= next >> 2;
                next |= next >> 4;
                next |= next >> 8;
                next++;
            } else {
                // pad m_total up to the next multiple of 1024
                next = this->m_total+1024;
                next &= ~0x3ff;
            }
            assert(next > this->m_total);
            reserve(next);
        }
        resize(this->size()+1);
    }

public:

    void push_back(param_type v)
    {
        _push_resize();
        back() = v;
    }

    void pop_back()
    {
        this->slice(0, this->size()-1);
    }

    // data access

    pointer data() const{return this->m_sdata.get()+this->m_offset;}

    reference operator[](size_t i) const {return this->m_sdata.get()[this->m_offset+i];}

    reference at(size_t i) const
    {
        if(i>this->m_count)
            throw std::out_of_range("Index out of bounds");
        return (*this)[i];
    }

};

/**
 * @brief Specialization for storing untyped pointers.
 *
 * Does not allow access or iteration of contents
 * other than as void* or const void*
 */
template<typename E>
class shared_vector<E, typename meta::is_void<E>::type >
    : public detail::shared_vector_base<E>
{
    typedef detail::shared_vector_base<E> base_t;
    ScalarType m_vtype;
public:
    typedef E* pointer;
    typedef ptrdiff_t difference_type;
    typedef size_t size_type;

    typedef std::tr1::shared_ptr<E> shared_pointer_type;

    shared_vector() :base_t(), m_vtype((ScalarType)-1) {}

    shared_vector(pointer v, size_t o, size_t c)
        :base_t(v,o,c), m_vtype((ScalarType)-1) {}

    template<typename B>
    shared_vector(pointer d, B b, size_t o, size_t c)
        :base_t(d,b,o,c), m_vtype((ScalarType)-1) {}

    template<typename E1>
    shared_vector(const std::tr1::shared_ptr<E1>& d, size_t o, size_t c)
        :base_t(d,o,c), m_vtype((ScalarType)-1) {}

    shared_vector(const shared_vector& o)
        :base_t(o), m_vtype(o.m_vtype) {}

    //! @internal
    //! Internal for static_shared_vector_cast
    template<typename FROM>
    shared_vector(const shared_vector<FROM> &src,
                  typename meta::is_not_void<FROM, detail::_shared_vector_cast_tag>::type)
        :base_t(std::tr1::static_pointer_cast<E>(src.dataPtr()),
                src.dataOffset()*sizeof(FROM),
                src.dataCount()*sizeof(FROM))
        ,m_vtype((ScalarType)ScalarTypeID<FROM>::value)
    {}

    shared_vector(shared_vector<void>& O,
                  detail::_shared_vector_freeze_tag t)
        :base_t(O,t)
    {}

    shared_vector(shared_vector<const void>& O,
                  detail::_shared_vector_thaw_tag t)
        :base_t(O,t)
    {}

    shared_vector& operator=(const shared_vector& o)
    {
        if(&o!=this) {
            this->base_t::operator=(o);
            m_vtype = o.m_vtype;
        }
        return *this;
    }

    size_t max_size() const{return (size_t)-1;}

    pointer data() const{
        return (pointer)(((char*)this->m_sdata.get())+this->m_offset);
    }

    shared_vector& set_original_type(ScalarType t) { m_vtype=t; return *this; }
    ScalarType original_type() const {return m_vtype;}
};

namespace detail {

    // Default to type conversion using castUnsafe (C++ type casting) on each element
    template<typename TO, typename FROM, class Enable = void>
    struct shared_vector_converter {
        static inline shared_vector<TO> op(const shared_vector<FROM>& src)
        {
            shared_vector<TO> ret(src.size());
            std::transform(src.begin(), src.end(), ret.begin(), castUnsafe<TO,FROM>);
            return ret;
        }
    };

    // copy reference when types are the same (excluding const qualifiers)
    template<typename TO, typename FROM>
    struct shared_vector_converter<TO,FROM, typename meta::same_root<TO,FROM>::type > {
        static FORCE_INLINE shared_vector<TO> op(const shared_vector<FROM>& src) {
            return src;
        }
    };

    // "convert" to 'void' or 'const void from non-void
    // is an alias for shared_vector_cast<void>()
    template<typename TO, typename FROM>
    struct shared_vector_converter<TO,FROM,
            typename meta::_and<meta::is_void<TO>, meta::is_not_void<FROM> >::type
            >
    {
        static FORCE_INLINE shared_vector<TO> op(const shared_vector<FROM>& src) {
            return shared_vector<TO>(src, detail::_shared_vector_cast_tag());
        }
    };
    
    // convert from void uses original type or throws an exception.
    template<typename TO, typename FROM>
    struct shared_vector_converter<TO,FROM,
            typename meta::_and<meta::is_not_void<TO>, meta::is_void<FROM> >::type
            >
    {
        static shared_vector<TO> op(const shared_vector<FROM>& src) {
            typedef typename meta::strip_const<TO>::type to_t;
            ScalarType stype = src.original_type(),
                       dtype = (ScalarType)ScalarTypeID<TO>::value;
            if(stype==dtype) {
                // no convert needed
                return shared_vector<TO>(src, detail::_shared_vector_cast_tag());
            } else {
                // alloc and convert
                shared_vector<to_t> ret(src.size()/ScalarTypeFunc::elementSize(stype));
                castUnsafeV(ret.size(),
                            dtype,
                            static_cast<void*>(ret.data()),
                            stype,
                            static_cast<const void*>(src.data()));
                return const_shared_vector_cast<TO>(ret);
            }
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
static FORCE_INLINE
shared_vector<TO>
static_shared_vector_cast(const shared_vector<FROM>& src,
                          typename meta::same_const<TO,FROM,int>::type = 0)
{
    return shared_vector<TO>(src, detail::_shared_vector_cast_tag());
}

/** @brief Allow converting of shared_vector between types
 *
 * Conversion utilizes castUnsafe<TO,FROM>().
 *
 * Converting to/from void is supported.  Convert to void
 * is an alias for static_shared_vector_cast<void>().
 * Convert from void utilizes shared_vector<void>::original_type()
 * and throws std::runtime_error if this is not valid.
 */
template<typename TO, typename FROM>
static FORCE_INLINE
shared_vector<TO>
shared_vector_convert(const shared_vector<FROM>& src)
{
    return detail::shared_vector_converter<TO,FROM>::op(src);
}

/** @brief transform a shared_vector<T> to shared_vector<const T>
 *
 * Transform a reference to mutable data into a reference to read-only data.
 * Throws an exception unless the reference to mutable data is unique.
 * On success the reference to mutable data is cleared.
 */
template<typename SRC>
static FORCE_INLINE
shared_vector<typename meta::decorate_const<typename SRC::value_type>::type>
freeze(SRC& src)
{
    typedef typename meta::decorate_const<typename SRC::value_type>::type const_value;
    return shared_vector<const_value>(src, detail::_shared_vector_freeze_tag());
}

/** @brief transform a shared_vector<const T> to shared_vector<T>
 *
 * Transform a reference to read-only data into a unique reference to mutable data.
 *
 * The reference to read-only data is cleared.
 */
template<typename SRC>
static FORCE_INLINE
shared_vector<typename meta::strip_const<typename SRC::value_type>::type>
thaw(SRC& src)
{
    typedef typename meta::strip_const<typename SRC::value_type>::type value;
    return shared_vector<value>(src, detail::_shared_vector_thaw_tag());
}

namespace detail {
    template<typename TO, typename FROM, class Enable = void>
    struct const_caster {};

    template<typename TYPE>
    struct const_caster<TYPE,const TYPE> {
        static FORCE_INLINE shared_vector<TYPE> op(shared_vector<const TYPE>& src)
        {
            return thaw(src);
        }
    };

    template<typename TYPE>
    struct const_caster<const TYPE,TYPE> {
        static FORCE_INLINE shared_vector<const TYPE> op(shared_vector<TYPE>& src)
        {
            return freeze(src);
        }
    };

    template<typename TYPE>
    struct const_caster<TYPE,TYPE> {
        static FORCE_INLINE shared_vector<TYPE> op(shared_vector<TYPE>& src)
        {
            shared_vector<TYPE> ret(src);
            src.clear();
            return ret;
        }
    };
}

//! Allows casting from const TYPE -> TYPE.
template<typename TO, typename FROM>
static FORCE_INLINE
shared_vector<TO>
const_shared_vector_cast(shared_vector<FROM>& src)
{
    return detail::const_caster<TO,FROM>::op(src);
}


namespace ScalarTypeFunc {
    //! Allocate an untyped array based on ScalarType
    shared_vector<void> allocArray(ScalarType id, size_t len);

    //! Allocate an untyped array based on ScalarType
    template<ScalarType ID>
    inline
    shared_vector<typename ScalarTypeTraits<ID>::type>
    allocArray(size_t len)
    {
        shared_vector<void> raw(allocArray(ID, len));
        return static_shared_vector_cast<typename ScalarTypeTraits<ID>::type>(raw);
    }
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
 * To facilitate safe modification the methods unique() and
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

/** @page vectorconst Value const-ness and shared_vector

The type 'shared_vector<T>' can be thought of as 'T*'.
Like the T pointer there are three related constant types:

@code
  shared_vector<int> v_mutable; // 1
  const shared_vector<int> v_const_ref; // 2
  shared_vector<const int> v_const_data; // 3
  const shared_vector<const int> v_const_ref_data; // 4
@endcode

The distinction between these types is what "part" of the type is constant,
the "reference" (pointer) or the "value" (location being pointed to).

Type #2 is constant reference to a mutable value.
Type #3 is a mutable reference to a constant value.
Type #4 is a constant reference to a constant value.

Casting between const and non-const references of the same value type
is governed by the normal C++ casting rules.

Casting between const and non-const values does @b not follow the normal
C++ casting rules.

For casting between shared_vector<T> and shared_vector<const T>
explicit casting operations are required.  These operations are
@b freeze() (non-const to const) and @b thaw() (const to non-const).

A shared_vector<const T> is "frozen" as its value can not be modified.

These functions are defined like:

@code
namespace epics{namespace pvData{
  template<typename T>
  shared_vector<const T> freeze(shared_vector<T>&);

  template<typename T>
  shared_vector<T> thaw(shared_vector<const T>&);
}}
@endcode

So each consumes a shared_vector with a certain value
const-ness, and returns one with the other.

The following guarantees are provided by both functions:

# The returned reference points to a value which is equal to the value referenced
 by the argument.
# The returned reference points to a value which is only referenced by
 shared_vectors with the same value const-ness as the returned reference.

Please note that the argument of both freeze and thaw is a non-const
reference which will always be cleared.

@section vfreeze Freezing

The act of freezing a shared_vector requires that the shared_vector
passed in must be unique() or an exception is thrown.  This is
done to reduce the possibility of accidental copying.

This possibility can be avoided by calling the make_unique() on a
shared_vector before passing it to freeze().

@section vthaw Thawing

The act of thawing a shared_vector may make a copy of the value
referenced by its argument if this reference is not unique().

*/
