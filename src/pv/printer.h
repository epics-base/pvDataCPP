#ifndef PRINTER_H
#define PRINTER_H

#include <ostream>
#include <shareLib.h>

#include "pvData.h"

namespace epics { namespace pvData {

class PrinterBase
{
public:
    virtual void setStream(std::ostream&);
    virtual void clearStream();

    virtual void print(const PVField&);

protected:
    PrinterBase();
    virtual ~PrinterBase()=0;

    virtual void beginStructure(const PVStructure&);
    virtual void endStructure(const PVStructure&);

    virtual void beginStructureArray(const PVStructureArray&);
    virtual void endStructureArray(const PVStructureArray&);

    virtual void beginUnion(const PVUnion&);
    virtual void endUnion(const PVUnion&);

    virtual void beginUnionArray(const PVUnionArray&);
    virtual void endUnionArray(const PVUnionArray&);

    virtual void encodeScalar(const PVScalar&);
    virtual void encodeArray(const PVScalarArray&);
    virtual void encodeNull();

    inline std::ostream& S() { return *strm; }

    void impl_print(const PVField&);
private:
    std::ostream *strm;
};

class PrinterPlain : public PrinterBase
{
    size_t ilvl;
protected:
    virtual void beginStructure(const PVStructure&);
    virtual void endStructure(const PVStructure&);

    virtual void beginStructureArray(const PVStructureArray&);
    virtual void endStructureArray(const PVStructureArray&);

    virtual void beginUnion(const PVUnion&);
    virtual void endUnion(const PVUnion&);

    virtual void beginUnionArray(const PVUnionArray&);
    virtual void endUnionArray(const PVUnionArray&);

    virtual void encodeScalar(const PVScalar&);
    virtual void encodeArray(const PVScalarArray&);
    virtual void encodeNull();

public:
    PrinterPlain();
    virtual ~PrinterPlain();
};

}}

#endif // PRINTER_H
