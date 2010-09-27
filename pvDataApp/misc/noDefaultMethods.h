/* noDefaultMethods.h */
#ifndef NO_DEFAULT_METHODS_H
#define NO_DEFAULT_METHODS_H
namespace epics { namespace pvData { 
/* This is based on Item 6 of 
 * Effective C++, Third Edition, Scott Meyers
 */


    class NoDefaultMethods {
    protected:
    // allow by derived objects
    NoDefaultMethods(){};
    ~NoDefaultMethods(){}
    private:
    // do not implment
    NoDefaultMethods(const NoDefaultMethods&);
    NoDefaultMethods & operator=(const NoDefaultMethods &);
    };

}}
#endif  /* NO_DEFAULT_METHODS_H */
